#ifndef DLUTILITY_H_INCLUDED
#define DLUTILITY_H_INCLUDED

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <iostream>
#include <Python.h>
#include <numpy/arrayobject.h>
#include <structmember.h>
#include <string>
#include <sstream>

#include "DLUSettings.h"

PyObject *str2PyStr(string Str) {
    //��ԭʼ���תΪUnicode
    int wlen = mbstowcs(nullptr, Str.c_str(), 0);
    wchar_t* wszString = new wchar_t[wlen + 1];
    mbstowcs(wszString, Str.c_str(), wlen);
    wszString[wlen] = 0;
    PyObject* res = PyUnicode_FromUnicode((const Py_UNICODE*)wszString, wlen);
    delete[] wszString;
    return res;
}

/*****************************************************************************
* ��/�ṹ�Ķ���:
* ֱ������CMISS_Handle����
* ����֮�⣬���ڸ����������κ�Python�Կڵ����ݣ�
* ��Ϊ��Ӧ�������Ѿ���װ��CMISS_Handle��
*****************************************************************************/
// DLU_Projector
typedef struct _C_DLU_Projector
{
    PyObject_HEAD             // == PyObject ob_base;  ����һ��PyObject����.
                              //////////////////////////////////////////////////////////////////////////
                              // ��/�ṹ��������Ա����.
                              //
    cdlu::Projector *_in_Handle;
}C_DLU_Projector;

static PyMemberDef C_DLU_Proj_DataMembers[] =        //ע����/�ṹ�����ݳ�Ա.
{ //��ע���κ����ݣ���Ϊ������CMpegDecoder���ϲ��ǲ��ɼ���
  //{"m_dEnglish", T_FLOAT,  offsetof(CScore, m_dEnglish), 0, "The English score of instance."},
    { "hAddress",   T_ULONGLONG, offsetof(C_DLU_Projector, _in_Handle),   READONLY, "The address of the handle in memory." },
    { nullptr, 0, 0, 0, nullptr }
};

// DLU_DataIO
typedef struct _C_DLU_DataIO
{
    PyObject_HEAD             // == PyObject ob_base;  ����һ��PyObject����.
                              //////////////////////////////////////////////////////////////////////////
                              // ��/�ṹ��������Ա����.
                              //
    cdlu::IO_Abstract *_in_Handle;
}C_DLU_DataIO;

static PyMemberDef C_DLU_DtIO_DataMembers[] =        //ע����/�ṹ�����ݳ�Ա.
{ //��ע���κ����ݣ���Ϊ������CMpegDecoder���ϲ��ǲ��ɼ���
  //{"m_dEnglish", T_FLOAT,  offsetof(CScore, m_dEnglish), 0, "The English score of instance."},
    { "hAddress",   T_ULONGLONG, offsetof(C_DLU_DataIO, _in_Handle),   READONLY, "The address of the handle in memory." },
    { nullptr, 0, 0, 0, nullptr }
};

/*****************************************************************************
* ȫ��������:
* Ϊ�����ĺ���ע��׼���������Ķ���
*****************************************************************************/

/*static PyObject* C_MPC_Global(PyObject* Self, PyObject *args, PyObject *kwargs) {
    char dumpLevel = -1;
    static char *kwlist[] = { "dumpLevel", nullptr };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|B", kwlist, &dumpLevel)) {
        cerr << "Error.GlobalSettings: invalid keyword'" << endl;
        return nullptr;
    }
    if (dumpLevel != -1) {
        cmpc::__dumpControl = static_cast<int8_t>(dumpLevel);
    }
    Py_RETURN_NONE;
}*/

static PyObject* C_DLU_Help(PyObject* Self) {
    cout << R"(================================================================================
 __, _,    __, _, _ _,_  _, _, _  _, _ _, _  _,   ___  _,  _, _,   _,
 | \ |     |_  |\ | |_| /_\ |\ | / ` | |\ | / _    |  / \ / \ |   (_ 
 |_/ | ,   |   | \| | | | | | \| \ , | | \| \ /    |  \ / \ / | , , )
 ~   ~~~   ~~~ ~  ~ ~ ~ ~ ~ ~  ~  ~  ~ ~  ~  ~     ~   ~   ~  ~~~  ~ 
================================================================================
Yuchen's Deep Learning Enhancing Tools - Readme
    This is a collection of deep learning utilities. You could use it to pre-
        process some data and do something on numpy arrays efficiently.
    For more instructions, you could tap help(dlUtilities). 
================================================================================
V0.7 update report:
    1. Fix some bugs that may cause memory leaking.
    2. Improve the code quality by using try blocks to tackle errors.
    3. Add the mode 'fwm180602' to the 'DataIO.load()'.
    4. Arrange the format of DLUError.
V0.6 update report:
    1. Add the 'save' & 'write' methods for 'DataIO' tool.
V0.55 update report:
    1. Add the 'batchRead' method for 'DataIO' tool.
V0.5 update report:
    1. Provide the 'Projector' tool and 'DataIO' tool.
)";
    Py_RETURN_NONE;
}

/*****************************************************************************
* ����C_DLU������������á����췽����
*****************************************************************************/

// DLU_Projector
static int C_DLU_Proj_init(C_DLU_Projector* Self, PyObject* args, PyObject *kwargs) { //���ء����췽��.
    PyObject *lmap = nullptr;
    static char *kwlist[] = { "labelMap", nullptr };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", kwlist, &lmap)) {
        PyErr_SetString(PyExc_TypeError, "need 'labelMap(list)'");
        return -1;
    }
    if (lmap) {
        if (!PyList_Check(lmap)) {
            PyErr_SetString(PyExc_TypeError, "need 'labelMap(list)'");
            return -1;
        }
        Self->_in_Handle = new cdlu::Projector;
        Self->_in_Handle->register_map(lmap);
    }
    else {
        Self->_in_Handle = new cdlu::Projector;
    }
    return 0;
}

static void C_DLU_Proj_Destruct(C_DLU_Projector* Self) { //���ء���������
    delete Self->_in_Handle;
    Py_TYPE(Self)->tp_free((PyObject*)Self); //�ͷŶ���/ʵ��.
}

static PyObject* C_DLU_Proj_Str(C_DLU_Projector* Self) { //���ء�����str/printʱ�Զ����ô˺���.
    ostringstream OStr;
    OStr << *(Self->_in_Handle);
    return str2PyStr(OStr.str());
}

static PyObject* C_DLU_Proj_Repr(C_DLU_Projector* Self) { //���ء�����repr���ú���ʱ�Զ�����.
    return C_DLU_Proj_Str(Self);
}

// DLU_DataIO
static int C_DLU_DtIO_init(C_DLU_DataIO* Self) { //���ء����췽��.
    Self->_in_Handle = nullptr;
    return 0;
}

static void C_DLU_DtIO_Destruct(C_DLU_DataIO* Self) { //���ء���������
    if (Self->_in_Handle)
        delete Self->_in_Handle;
    Py_TYPE(Self)->tp_free((PyObject*)Self); //�ͷŶ���/ʵ��.
}

static PyObject* C_DLU_DtIO_Str(C_DLU_DataIO* Self) { //���ء�����str/printʱ�Զ����ô˺���.
    if (Self->_in_Handle) {
        ostringstream OStr;
        OStr << *(Self->_in_Handle);
        return str2PyStr(OStr.str());
    }
    else {
        return str2PyStr(string("<IOHandle - Unallocated>"));
    }
}

static PyObject* C_DLU_DtIO_Repr(C_DLU_DataIO* Self) { //���ء�����repr���ú���ʱ�Զ�����.
    return C_DLU_DtIO_Str(Self);
}

/*****************************************************************************
* ����C_DLU���������Python�ӿڡ�
*****************************************************************************/
// Projector
static PyObject* C_DLU_Proj_Register(C_DLU_Projector* Self, PyObject *args, PyObject *kwargs) {
    /* ��װ(bool)C_DLU_Proj_Register��������������Ϊ:
    *   labelMap [dict->dict]: �ؼ���ת���ֵ�
    */
    PyObject *lmap = nullptr;
    static char *kwlist[] = { "labelMap", nullptr };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|", kwlist, &lmap)) {
        PyErr_SetString(PyExc_TypeError, "need 'labelMap(list)'");
        return nullptr;
    }
    if (!PyList_Check(lmap)) {
        PyErr_SetString(PyExc_TypeError, "need 'labelMap(list)'");
        return nullptr;
    }
    auto res = Self->_in_Handle->register_map(lmap);
    if (res)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyObject* C_DLU_Proj_Action(C_DLU_Projector* Self, PyObject *args, PyObject *kwargs) {
    /* ��װ(PyArrayObject)C_DLU_Proj_Action��������������Ϊ:
    *   labelMap [dict->dict]: �ؼ���ת���ֵ�
    */
    if (PyArray_API == nullptr) {
        import_array();
    }
    PyObject *inpic = nullptr;
    int inversed = 0;
    static char *kwlist[] = { "PyPicture", "inversed", nullptr };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|p", kwlist, &inpic, &inversed)) {
        PyErr_SetString(PyExc_TypeError, "need 'PyPicture(np.ndarray)', and optional argument 'inversed(bool)'");
        return nullptr;
    }
    if (!PyArray_Check(inpic)) {
        cerr << cdlu::DLU_error_log(0x203) << endl;
        return nullptr;
    }
    bool b_inversed = (inversed != 0);
    auto res = Self->_in_Handle->action(inpic, b_inversed);
    if (res == nullptr) {
        PyErr_SetString(PyExc_RuntimeError, "fail to execute action, please check the DLUError log.");
    }
    return res;
}

static PyObject* C_DLU_Proj_Clear(C_DLU_Projector* Self) {
    /* ��װ(void)clear���������������� */
    Self->_in_Handle->clear();
    Py_RETURN_NONE;
}

// DataIO
static PyObject* C_DLU_DtIO_Clear(C_DLU_DataIO* Self) {
    /* ��װ(void)clear���������������� */
    if (Self->_in_Handle) {
        Self->_in_Handle->clear();
        delete Self->_in_Handle;
        Self->_in_Handle = nullptr;
    }
    Py_RETURN_NONE;
}

static PyObject* C_DLU_DtIO_Close(C_DLU_DataIO* Self) {
    /* ��װ(void)close���������������� */
    if (Self->_in_Handle) {
        Self->_in_Handle->close();
        delete Self->_in_Handle;
        Self->_in_Handle = nullptr;
    }
    else {
        PyErr_SetString(PyExc_IOError, "Should not close a file without loading/saving it.");
        return nullptr;
    }
    Py_RETURN_NONE;
}

static PyObject* C_DLU_DtIO_Size(C_DLU_DataIO* Self) {
    /* ��װ(?)size���������������� */
    if (Self->_in_Handle) {
        auto sizeobj = Self->_in_Handle->size();
        if (!sizeobj) {
            PyErr_SetString(PyExc_NotImplementedError, "The current mode does not provide this function.");
            return nullptr;
        }
        else {
            return sizeobj;
        }
    }
    else {
        PyErr_SetString(PyExc_IOError, "Should not detect size without loading/saving file.");
        return nullptr;
    }
    Py_RETURN_NONE;
}

static PyObject* C_DLU_DtIO_Load(C_DLU_DataIO* Self, PyObject *args, PyObject *kwargs) {
    /* ��װ(bool)load��������������Ϊ:
    *   filepath [bytes->string]: �ļ�·��
    *   mode     [bytes->string]: ����ģʽ
    */
    if (Self->_in_Handle) {
        PyErr_SetString(PyExc_IOError, "Should close/clear the IO handle before loading a new file.");
        return nullptr;
    }
    Py_buffer py_fpath = { 0 };
    Py_buffer py_mode = { 0 };
    static char *kwlist[] = { "filePath", "mode", nullptr };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "y*|y*", kwlist, &py_fpath, &py_mode)) {
        PyErr_SetString(PyExc_TypeError, "need 'filePath(bytes)', and optional argument 'mode(bytes)'");
        return nullptr;
    }
    string in_fpath;
    if (py_fpath.buf)
        in_fpath.assign(reinterpret_cast<char *>(py_fpath.buf));
    else {
        PyErr_SetString(PyExc_TypeError, "Must specify a valid path.");
        if (py_mode.buf)
            PyBuffer_Release(&py_mode);
        return nullptr;
    }
    PyBuffer_Release(&py_fpath);
    string in_mode;
    if (py_mode.buf)
        in_mode.assign(reinterpret_cast<char *>(py_mode.buf));
    else
        in_mode.assign("seismic");
    PyBuffer_Release(&py_mode);
    if (in_mode.compare("seismic") == 0) {
        Self->_in_Handle = new cdlu::IO_Sesmic;
    }
    else if (in_mode.compare("fwm180602") == 0) {
        Self->_in_Handle = new cdlu::IO_FWM180602;
    }
    else {
        PyErr_SetString(PyExc_TypeError, "The assigned mode is not valid.");
        return nullptr;
    }
    auto success = Self->_in_Handle->load(in_fpath);
    if (success) {
        Py_RETURN_TRUE;
    }
    else {
        PyErr_SetString(PyExc_IOError, "Unable to load the specified file, please check the DLU-Error for details.");
        delete Self->_in_Handle;
        Self->_in_Handle = nullptr;
        return nullptr;
    }
}

static PyObject* C_DLU_DtIO_Save(C_DLU_DataIO* Self, PyObject *args, PyObject *kwargs) {
    /* ��װ(bool)save��������������Ϊ:
    *   filepath [bytes->string]: �ļ�·��
    *   mode     [bytes->string]: ����ģʽ
    */
    if (Self->_in_Handle) {
        PyErr_SetString(PyExc_IOError, "Should close/clear the IO handle before saving a new file.");
        return nullptr;
    }
    Py_buffer py_fpath = { 0 };
    Py_buffer py_mode = { 0 };
    static char *kwlist[] = { "filePath", "mode", nullptr };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "y*|y*", kwlist, &py_fpath, &py_mode)) {
        PyErr_SetString(PyExc_TypeError, "need 'filePath(bytes)', and optional argument 'mode(bytes)'");
        return nullptr;
    }
    string in_fpath;
    if (py_fpath.buf)
        in_fpath.assign(reinterpret_cast<char *>(py_fpath.buf));
    else {
        PyErr_SetString(PyExc_TypeError, "Must specify a valid path.");
        if (py_mode.buf)
            PyBuffer_Release(&py_mode);
        return nullptr;
    }
    PyBuffer_Release(&py_fpath);
    string in_mode;
    if (py_mode.buf)
        in_mode.assign(reinterpret_cast<char *>(py_mode.buf));
    else
        in_mode.assign("seismic");
    PyBuffer_Release(&py_mode);
    if (in_mode.compare("seismic") == 0) {
        Self->_in_Handle = new cdlu::IO_Sesmic;
    }
    else {
        PyErr_SetString(PyExc_TypeError, "The assigned mode is not valid.");
        return nullptr;
    }
    auto success = Self->_in_Handle->save(in_fpath);
    if (success) {
        Py_RETURN_TRUE;
    }
    else {
        PyErr_SetString(PyExc_IOError, "Unable to save the specified file, please check the DLU-Error for details.");
        delete Self->_in_Handle;
        Self->_in_Handle = nullptr;
        return nullptr;
    }
}

static PyObject* C_DLU_DtIO_Read(C_DLU_DataIO* Self, PyObject *args, PyObject *kwargs) {
    /* ��װ(ndarray)read��������������Ϊ:
    *   indices [int/tuple]: �±�/���±�
    */
    if (!Self->_in_Handle) {
        PyErr_SetString(PyExc_IOError, "Should not read without loading file.");
        return nullptr;
    }
    PyObject *indices = nullptr;
    static char *kwlist[] = { "indicies", nullptr };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|", kwlist, &indices)) {
        PyErr_SetString(PyExc_TypeError, "need 'indices(int/sequence)'");
        return nullptr;
    }
    if (PyLong_Check(indices)) {
        auto index = PyLong_AsLong(indices);
        auto res = Self->_in_Handle->read(index);
        if (!res) {
            PyErr_SetString(PyExc_NotImplementedError, "Meet a fatal error, or the current mode does not provide this function.");
            return nullptr;
        }
        else {
            return res;
        }
    }
    else {
        auto res = Self->_in_Handle->read(indices);
        if (!res) {
            PyErr_SetString(PyExc_NotImplementedError, "Meet a fatal error, or the current mode does not provide this function.");
            return nullptr;
        }
        else {
            return res;
        }
    }
    Py_RETURN_NONE;
}

static PyObject* C_DLU_DtIO_BatchRead(C_DLU_DataIO* Self, PyObject *args, PyObject *kwargs) {
    /* ��װ(ndarray)batchread��������������Ϊ:
    *   batchnum     [int]: batch��Ŀ
    *   shape      [tuple]: �ߴ�
    */
    if (!Self->_in_Handle) {
        PyErr_SetString(PyExc_IOError, "Should not read without loading file.");
        return nullptr;
    }
    int batchnum = 0;
    PyObject *shape = nullptr;
    static char *kwlist[] = { "batchNum", "shape", nullptr };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "iO|", kwlist, &batchnum, &shape)) {
        PyErr_SetString(PyExc_TypeError, "need 'batchNum(int)', 'shape(sequence)' and an optional 'multithread(bool)'");
        return nullptr;
    }
    auto res = Self->_in_Handle->read(batchnum, shape);
    if (!res) {
        PyErr_SetString(PyExc_NotImplementedError, "Meet a fatal error, or the current mode does not provide this function.");
        return nullptr;
    }
    else {
        return res;
    }
    Py_RETURN_NONE;
}

static PyObject* C_DLU_DtIO_Write(C_DLU_DataIO* Self, PyObject *args, PyObject *kwargs) {
    /* ��װ(size_t)write��������������Ϊ:
    *   indices [int/tuple]: �±�/���±�
    */
    if (PyArray_API == nullptr) {
        import_array();
    }
    if (!Self->_in_Handle) {
        PyErr_SetString(PyExc_IOError, "Should not write without saving file.");
        return nullptr;
    }
    PyObject *data = nullptr;
    static char *kwlist[] = { "data", nullptr };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|", kwlist, &data)) {
        PyErr_SetString(PyExc_TypeError, "need 'data(ndarray)'");
        return nullptr;
    }
    if (PyArray_Check(data)) {
        auto res = Self->_in_Handle->write(data);
        if (res == -1) {
            PyErr_SetString(PyExc_NotImplementedError, "Meet errors while writing, or the current mode does not provide this function.");
            return nullptr;
        }
        else {
            return Py_BuildValue("i", res);
        }
    }
    else {
        PyErr_SetString(PyExc_TypeError, "need 'data(ndarray)'");
        return nullptr;
    }
    Py_RETURN_NONE;
}

//ע����������������Ϊʲô���ǲ���ҪPy_IN/DECREF�أ���Ϊδ������ʱ������Ҳû��
//ʹ������None�������ֳɷ��ض���
/*static PyObject* FreePyArray(PyArrayObject *PyArray) {
    if (PyArray_API == nullptr) {
        import_array();
    }
    uint8_t * out_dataptr = (uint8_t *)PyArray_DATA(PyArray);
    delete [] out_dataptr;
    return nullptr;
}
void FreePyList(PyObject *PyList) {
    Py_ssize_t getlen = PyList_Size(PyList);
    for (Py_ssize_t i = 0; i < getlen; i++) {
        PyObject *Item = PyList_GetItem(PyList, i);
        FreePyArray((PyArrayObject*)Item);
    }
    Py_DECREF(PyList);
    int x = PyList_ClearFreeList();
    //cout << "Freed: " << x << " items" << endl;
}*/

/*****************************************************************************
* ����ģ���¼��ע��
*****************************************************************************/
static PyMethodDef C_DLU_MethodMembers[] =      //ע��ȫ�ֺ����б�
{
    { "readme",          (PyCFunction)C_DLU_Help,               METH_NOARGS, \
    "Use it to see readme and some useful instructions." },
    { nullptr, nullptr, 0, nullptr }
};

static PyMethodDef C_DLU_Proj_MethodMembers[] =      //ע��������г�Ա�����ṹ�б�.
{ //�ò������弴Ϊ��һ����װCMISS_Handle��Ϊ���ṩ����Python�Ľӿ�
    { "clear",              (PyCFunction)C_DLU_Proj_Clear,                METH_NOARGS, \
    "clear the settings of this projector." },
    { "registerMap",        (PyCFunction)C_DLU_Proj_Register,             METH_VARARGS | METH_KEYWORDS, \
    "Use a numpy-based dict to setup this projector.\n - labelMap: [list] an n*2 list which defines the axes of projecting method." },
    { "action",             (PyCFunction)C_DLU_Proj_Action,               METH_VARARGS | METH_KEYWORDS, \
    "Project the input ndarray into another axis space.\n - PyPicture: [ndarray] the input picture which needs to be projected.\n - inversed: [bool](o) whether the projecting direction is inversed." },
    { nullptr, nullptr, 0, nullptr }
};

static PyMethodDef C_DLU_DtIO_MethodMembers[] =      //ע��������г�Ա�����ṹ�б�.
{
    { "clear",              (PyCFunction)C_DLU_DtIO_Clear,                METH_NOARGS, \
    "clear the settings of this IO Handle, always avaliable." },
    { "close",              (PyCFunction)C_DLU_DtIO_Close,                METH_NOARGS, \
    "close a loaded/saved IO handle, avaliable only after we have called load/save method." },
    { "size",               (PyCFunction)C_DLU_DtIO_Size,                 METH_NOARGS, \
    "Return the size of file in current handle. Sometimes it has other meanings and sometimes this\n method is not avaliable. It depends on the mode where the handle works." },
    { "load",               (PyCFunction)C_DLU_DtIO_Load,                 METH_VARARGS | METH_KEYWORDS, \
    "Load a resource file.\n - filePath: [bytes] a path which defines where the file stored.\n - mode: [bytes] the assigned mode of the IN handle (default: 'seismic')." },
    { "save",               (PyCFunction)C_DLU_DtIO_Save,                 METH_VARARGS | METH_KEYWORDS, \
    "Create a destination file for saving data.\n - filePath: [bytes] a path which defines where the file stored.\n - mode: [bytes] the assigned mode of the OUT handle (default: 'seismic')." },
    { "read",               (PyCFunction)C_DLU_DtIO_Read,                 METH_VARARGS | METH_KEYWORDS, \
    "Read a data chunk.\n - indices: [int/sequence] the indices (or index) of fetched data." },
    { "write",              (PyCFunction)C_DLU_DtIO_Write,                METH_VARARGS | METH_KEYWORDS, \
    "Write an array as a data chunk to the destination.\n - data: [ndarray] the data that needs to be written." },
    { "batchRead",          (PyCFunction)C_DLU_DtIO_BatchRead,            METH_VARARGS | METH_KEYWORDS, \
    "Use a random strategy to read a data batch.\n - batchNum: [int] the number of fetched sample data.\n - shape: [sequence] the shape of returned data." },
    { nullptr, nullptr, 0, nullptr }
};

/*****************************************************************************
* ��/�ṹ�����г�Ա���������Ե�˵����Ϣ..
* ΪPython���ṩ����ķ�װ
*****************************************************************************/
static PyTypeObject C_DLU_Proj_ClassInfo =
{
    PyVarObject_HEAD_INIT(nullptr, 0)"dlUtilities.Projector",  //����ͨ��__class__�������ַ���. CPP��������.__name__��ȡ.
    sizeof(C_DLU_Projector),                 //��/�ṹ�ĳ���.����PyObject_Newʱ��Ҫ֪�����С.
    0,
    (destructor)C_DLU_Proj_Destruct,    //�����������.
    0,
    0,
    0,
    0,
    (reprfunc)C_DLU_Proj_Repr,
    0,
    0,
    0,
    0,
    0,
    (reprfunc)C_DLU_Proj_Str,         //Str/print���ú�������.
    0,
    0,
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,     //���û���ṩ�����Ļ���ΪPy_TPFLAGS_DEFAULE
    "This class is used to project a numpy array to another form based on a key map.",   //__doc__,��/�ṹ��DocString.
    0,
    0,
    0,
    0,
    0,
    0,
    C_DLU_Proj_MethodMembers,       //������з�������.
    C_DLU_Proj_DataMembers,         //����������ݳ�Ա����.
    0,
    0,
    0,
    0,
    0,
    0,
    (initproc)C_DLU_Proj_init,      //��Ĺ��캯��.
    0,
};

static PyTypeObject C_DLU_DtIO_ClassInfo =
{
    PyVarObject_HEAD_INIT(nullptr, 0)"dlUtilities.DataIO",  //����ͨ��__class__�������ַ���. CPP��������.__name__��ȡ.
    sizeof(C_DLU_DataIO),                 //��/�ṹ�ĳ���.����PyObject_Newʱ��Ҫ֪�����С.
    0,
    (destructor)C_DLU_DtIO_Destruct,    //�����������.
    0,
    0,
    0,
    0,
    (reprfunc)C_DLU_DtIO_Repr,
    0,
    0,
    0,
    0,
    0,
    (reprfunc)C_DLU_DtIO_Str,         //Str/print���ú�������.
    0,
    0,
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,     //���û���ṩ�����Ļ���ΪPy_TPFLAGS_DEFAULE
    "This class is used as a high-level API for low-level fast reading/writing data.\nNoted that not always is every method provided. It depends on the mode where you let this instance work.",   //__doc__,��/�ṹ��DocString.
    0,
    0,
    0,
    0,
    0,
    0,
    C_DLU_DtIO_MethodMembers,       //������з�������.
    C_DLU_DtIO_DataMembers,         //����������ݳ�Ա����.
    0,
    0,
    0,
    0,
    0,
    0,
    (initproc)C_DLU_DtIO_init,      //��Ĺ��캯��.
    0,
};

/*****************************************************************************
* ��ģ��˵����Ϣ..
* ΪPythonģ���ṩ����ķ�װ
*****************************************************************************/
static PyModuleDef ModuleInfo =
{
    PyModuleDef_HEAD_INIT,
    "dlUtilities",               //ģ���������--__name__.
    "A collection of deep learning enhancing tools.",  //ģ���DocString.__doc__
    -1,
    nullptr, nullptr, nullptr, nullptr, nullptr
};

#endif