#ifndef DLUSETTINGS_H_INCLUDED
#define DLUSETTINGS_H_INCLUDED

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#define DLUSETTINGS_CURRENT_VERSION "0.55"

#include <unordered_map>
#include <fstream>
#include <sstream>
#include <random>
#include <Python.h>
#include <numpy/arrayobject.h>

#include "DLUErrorLog.h"

namespace cdlu {

    class DataChunk {
    public:
        DataChunk(void); //���巨�򣬶�������֮һ�ͱ���ȫ���ֶ�����
        ~DataChunk(void);
        DataChunk(const DataChunk &ref);
        DataChunk& operator=(const DataChunk &ref);
        DataChunk(DataChunk &&ref) noexcept;
        DataChunk& operator=(DataChunk &&ref) noexcept;
        DataChunk(char *ref, size_t ref_size);
        // ���������
        bool operator<(const DataChunk& ref) const; // �Ƚ�����������ڹ����ֵ�
        bool operator==(const DataChunk& ref) const;
        size_t hash_func() const;
        friend std::ostream & operator<<(std::ostream & out, const DataChunk & self_class); // ������ʾ
        // ���ܲ���
        void set(char *ref, size_t ref_size); // ��һ��������������
        void set_unsafe(char *ref, size_t ref_size); // ��һ���������ƶ���ֵ���������ݣ�ע������һ�ֲ����յ�����
        char* get_copied(char *dest); // ��������������Լ����ڴ�η���һ�����飬������λ�ƺ������ָ��
        void get_copied(char **dataptr); // �����������ΪNumpy_Iter��Ƶİ汾
        size_t size;
    private:
        char *mem;
        
    };

    class Projector{ /*��Ч��ͨ��ͶӰװ��*/
    public:
        Projector(void);                                      //���캯��
        // ���²��־��Ǵ�˵�е����巨�򣬶�������֮һ�ͱ���ȫ���ֶ�����
        ~Projector(void);                                     //��������
        Projector(const Projector &ref);                      //�������캯��
        Projector& operator=(const Projector &ref);           //������ֵ����
        Projector(Projector &&ref) noexcept;                  //�ƶ����캯��
        Projector& operator=(Projector &&ref) noexcept;       //�ƶ���ֵ����
        // ���������
        friend std::ostream & operator<<(std::ostream & out, const Projector & self_class); // ������ʾ
        void clear();
        size_t size() const;
        bool register_map(PyObject *PyList);
        PyObject* action(PyObject *PyPicture, bool inversed=false);
    private:
        size_t in_size;
        size_t out_size;
        struct _cmp_hash {
            size_t operator()(const DataChunk &a) const {
                return a.hash_func();
            }
        };
        struct _cmp_eq {
            bool operator()(const DataChunk &a, const DataChunk &b) const {
                return a == b;
            }
        };
        std::unordered_map<DataChunk, DataChunk, _cmp_hash, _cmp_eq> map_forward;
        std::unordered_map<DataChunk, DataChunk, _cmp_hash, _cmp_eq> map_inverse;
        PyObject *__npyintpToPyList(npy_intp *in_list, int size);
        npy_intp __prodNpy(npy_intp *in_list, int size);
    };

    class IO_Abstract {
    public:
        IO_Abstract(void);                                        //���캯��
        // ���²��־��Ǵ�˵�е����巨�򣬶�������֮һ�ͱ���ȫ���ֶ�����
        virtual ~IO_Abstract(void);                               //��������
        IO_Abstract(const IO_Abstract &ref) = delete;             //���ÿ������캯��
        IO_Abstract& operator=(const IO_Abstract &ref) = delete;  //���ÿ�����ֵ����
        IO_Abstract(IO_Abstract &&ref) noexcept;                  //�ƶ����캯��
        IO_Abstract& operator=(IO_Abstract &&ref) noexcept;       //�ƶ���ֵ����
        // ���������
        friend std::ostream & operator<<(std::ostream & out, const IO_Abstract & self_class); // ������ʾ
        virtual void clear();
        virtual PyObject *size() const;
        virtual bool load(string filename) = 0;
        virtual bool save(string filename) = 0;
        virtual void close() = 0;
        virtual PyObject *read(size_t s_num); // Read a single data
        virtual PyObject *read(PyObject *s_numPyList); // Read a series of data, arranged by channels
        virtual PyObject *read(int batchNum, PyObject *batchShape); // Read random batch, arranged by batchNum
        virtual int64_t write(PyObject *data); // Write a single/multiple data
    protected:
        std::ifstream __h;
        std::ofstream __oh;
        // Эͬ������������麯��
        virtual std::ostream & __print(std::ostream & out) const;
    };

    class IO_Sesmic : public IO_Abstract { /**/
    public:
        IO_Sesmic(void);                                      //���캯��
                                                              // ���²��־��Ǵ�˵�е����巨�򣬶�������֮һ�ͱ���ȫ���ֶ�����
        ~IO_Sesmic(void);                                     //��������
        IO_Sesmic(const IO_Sesmic &ref);                      //�������캯��
        IO_Sesmic& operator=(const IO_Sesmic &ref);           //������ֵ����
        IO_Sesmic(IO_Sesmic &&ref) noexcept;                  //�ƶ����캯��
        IO_Sesmic& operator=(IO_Sesmic &&ref) noexcept;       //�ƶ���ֵ����
        // ���������
        friend std::ostream & operator<<(std::ostream & out, const IO_Sesmic & self_class); // ������ʾ
        void clear() override;
        PyObject *size() const override;
        bool load(string filename) override;
        bool save(string filename) override;
        void close() override;
        PyObject *read(size_t s_num) override;
        PyObject *read(PyObject *s_numPyList) override;
        PyObject *read(int batchNum, PyObject *batchShape) override;
        int64_t write(PyObject *data) override;
    private:
        string __filename; //������������͸�ֵ����ʱ����
        string __folderpath;
        string __ofilename;
        string __ofolderpath;
        size_t num_shot;
        size_t num_rec;
        size_t num_time;
        size_t onum_shot;
        size_t onum_rec;
        size_t onum_time;
        bool __read_log_info();
        bool __write_log_info();
        string __full_path(bool write=false);
    protected:
        std::ostream & __print(std::ostream & out) const override;
    };
}

#endif