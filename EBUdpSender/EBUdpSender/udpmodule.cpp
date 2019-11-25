#include "udpmodule.h"



static PyObject* WrappCreateUdpSocketList(PyObject* self, PyObject* args)
{
   UN_USED(self);
   uint16_t group;
   char *host;
   uint16_t port;
   int socket_num;
   if(!PyArg_ParseTuple(args, "HsHi", &group, &host, &port, &socket_num)){
       return nullptr;
   }
   return Py_BuildValue("i",  CreateUdpSocketList(group, host, port, socket_num));
}

static PyObject* WrapUnRefUdpSocketList(PyObject* self, PyObject* args)
{
    UN_USED(self);
    int index;
    if(!PyArg_ParseTuple(args, "i", &index)){
        return Py_BuildValue("i", -1);
    }
    UnRefUdpSocketList(index);
     return Py_BuildValue("i", 0);
}

static PyObject* WrappSendDataOut(PyObject* self, PyObject* args){
   UN_USED(self);
   int sks_index, u_sec;
   Py_buffer buff;
   uint16_t pic_id;
   int width, height, sample;
   if(!PyArg_ParseTuple(args, "iy*Hiiii", &sks_index, &buff, &pic_id, &width, &height, &sample, &u_sec)){
       return Py_BuildValue("i", -1);
   }
   SendDataOut(sks_index, reinterpret_cast<uint8_t*>(buff.buf), pic_id, width, height, sample, u_sec);
   return Py_BuildValue("i", 0);
}


static bool thread_hold=false;
static std::thread* thPtr=nullptr;


static PyObject* WrapStartListener(PyObject* self, PyObject* args){
    UN_USED(self);
    if(thread_hold){
      printf("a listener is still running, only one listener allowed\r\n");
      return Py_BuildValue("i", -2);
    }
    uint16_t port;
    printf("start listen...\r\n");
    if(!PyArg_ParseTuple(args, "H", &port)){
        return Py_BuildValue("i", -1);
    }
    printf("parse success,  port= %d \r\n", port);
    if(thPtr) delete thPtr;
    thPtr = new std::thread( [port](){
                      thread_hold = true;
                      StartListener(port, nullptr);
                      thread_hold=false;
              });
    return Py_BuildValue("i", 0);
}

static PyObject* WrapStopListen(PyObject* self, PyObject* args){
    UN_USED(self);
    int port;
    if(PyArg_ParseTuple(args, "i", &port)){
        StopListener(port);
    }
    return Py_BuildValue("");
}

static size_t pic_size = 2000*2000*4;

static std::shared_ptr<StaticPool> static_mempool= std::shared_ptr<StaticPool>(new StaticPool(pic_size));

static PyObject* WrapGetPic(PyObject* self, PyObject* args){
    UN_USED(self);
    uint16_t group;
    if(!PyArg_ParseTuple(args, "H", &group)){
        return Py_BuildValue("HHHs#", 0, 0, 0, nullptr, 0);
    }
    uint16_t _width=0,  _height=0,  _channel=0;
    int index = static_mempool->GetFreeIndex();
    uint8_t* dst = static_mempool->GetBuff(index);
    bool rt = GetCurrentPic(group, &_width,  &_height,  &_channel,  dst, pic_size);
    PyObject* ret=nullptr;
    if(!rt) ret = Py_BuildValue("HHHs#", 0, 0, 0, nullptr, 0);
    else {
        const char* str = reinterpret_cast<const char*>(dst);
        int str_len = _width * _height * _channel;
        ret = Py_BuildValue("HHHy#", _width, _height, _channel, str, str_len);
    }
    static_mempool->FreeIndex(index);
    return ret;

}

static PyObject* WrapGetCurGroup(PyObject* self, PyObject* args){
    UN_USED(self);
    UN_USED(args);
    int gsize=0;
    uint16_t* groups = GetCurrentAllPicGroup(&gsize);
    PyObject* list =  PyList_New(gsize);
    Py_ssize_t size = static_cast<Py_ssize_t>(gsize);
    for(Py_ssize_t i=0; i<size; i++){
        PyList_SetItem(list, i, Py_BuildValue("H", groups[i]));
    }
    if(gsize > 0) delete [] groups;
    return list;
}

static PyMethodDef EBUdpWrapMethods[] = {
    { "CreateUdpSocketList", WrappCreateUdpSocketList, METH_VARARGS, "apply client sockets" },
    { "SendDataOut", WrappSendDataOut, METH_VARARGS, "Send Pic" },
    { "StartListener", WrapStartListener, METH_VARARGS, "StartServer" },
    { "GetCurGroups", WrapGetCurGroup, METH_VARARGS, "GetPicGroups"},
    { "GetPic", WrapGetPic, METH_VARARGS, "GetPic"},
    {"StopListen", WrapStopListen, METH_VARARGS, "stop port listen"},
    {"ReleaseSocket", WrapUnRefUdpSocketList, METH_VARARGS, "close sockets"},
    { nullptr, nullptr, 0, nullptr }
};

static PyModuleDef CreateModule(){
    PyModuleDef def;
    memset(&def, 0, sizeof(PyModuleDef));
    def.m_base = PyModuleDef_HEAD_INIT;
    def.m_name = "EBUdpWrap";
    def.m_doc = "udp Client and server apis";
    def.m_size = -1;
    def.m_methods = EBUdpWrapMethods;
    return def;
}

static PyModuleDef  EBUdpWrapModule = CreateModule();


PyMODINIT_FUNC PyInit_EBUdpWrap(void)
{
    return  PyModule_Create(&EBUdpWrapModule);
}


