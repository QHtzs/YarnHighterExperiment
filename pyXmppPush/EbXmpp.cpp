#include "apis.h"
#include <iostream>
#include <Python.h>

static PyObject* PyIgnoreSslError(PyObject*, PyObject*)
{
    SetIgnoreSslError();
    return Py_BuildValue("");
}

static PyObject* PyConnectToServer(PyObject*, PyObject* args)
{
    char* arg1, *arg2;
    if(PyArg_ParseTuple(args, "yy", &arg1, &arg2)){
        ConnectToServer(arg1, arg2);
        return Py_BuildValue("i", 0);
    }else{
        return Py_BuildValue("i", -1);
    }
}

static PyObject* PySendMessage(PyObject*, PyObject* args)
{
    char* arg1, *arg2;
    if(PyArg_ParseTuple(args, "yy", &arg1, &arg2)){
        SendMessage(arg1, arg2);
        return Py_BuildValue("i", 0);

    }else{
        return Py_BuildValue("i", -1);
    }
}

static PyObject* PySendImage(PyObject*, PyObject* args)
{
    char *arg1, *arg2;
    if(PyArg_ParseTuple(args, "yy", &arg1, &arg2)){
        SendImage(arg1, arg2);
        return Py_BuildValue("i", 0);

    }else{
        return Py_BuildValue("i", -1);
    }
}

static PyObject* PyAddCaCert(PyObject*, PyObject* args)
{
    char * ca;
    if(!PyArg_ParseTuple(args, "y", &ca)){
        AddCaCertificate(ca);
        return Py_BuildValue("i", -1);
    }
    return Py_BuildValue("i", 0);
}

static PyMethodDef EbXmppMethods[] = {
    {"pSetIgnoreSslError", PyIgnoreSslError, METH_VARARGS, "Ignore SSL ERROR"},
    {"pAddCaCertificate", PyAddCaCert, METH_VARARGS, "add trust Certificates"},
    {"pConnectToServer", PyConnectToServer, METH_VARARGS, "Connect to Server"},
    {"pSendMessage", PySendMessage, METH_VARARGS, "send text"},
    {"pSendImage", PySendImage, METH_VARARGS, "send picture"},
    {nullptr, nullptr, 0, nullptr},
};

static PyModuleDef InitModuleFunc(){
    PyModuleDef def;
    memset(&def, 0, sizeof(PyModuleDef));
    def.m_base = PyModuleDef_HEAD_INIT;
    def.m_name = "EbXmpp";
    def.m_doc = "A Client of Xmpp protocol(QXmpp)";
    def.m_size = -1;
    def.m_methods = EbXmppMethods;
    return def;
}

static PyModuleDef EbXmppModule = InitModuleFunc();

PyMODINIT_FUNC PyInit_EbXmpp(void)
{
    return PyModule_Create2(&EbXmppModule, PYTHON_API_VERSION);
}

