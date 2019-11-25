#ifndef UDPMODULE_H
#define UDPMODULE_H


#include "Python.h"
#include "udpwrapper.h"
#include <thread>
#include <vector>
#include <memory>

#define UN_USED(x) (void)x

#ifdef __cplusplus
extern "C"{
#endif

typedef struct MemPool{
    std::mutex _mut;
    std::vector<bool> index;
    std::vector<uint8_t*> _memory_;
    size_t buff_size;

    MemPool(size_t b_size){
        buff_size = b_size;
    }

    ~MemPool(){
        for(size_t i=0; i<_memory_.size(); i++) delete[] _memory_[i];
    }

    int GetFreeIndex(){
        int ret = -1;
        _mut.lock();
        for(size_t i=0; i<index.size(); i++){
            if(index[i]){
                ret = static_cast<int>(i);
                index[i] = false;
                break;
            }
        }
        if(ret == -1){
            ret = static_cast<int>(index.size());
            index.push_back(false);
            uint8_t* buf = new uint8_t[buff_size]();
            _memory_.push_back(buf);
        }
        _mut.unlock();
        return ret;
    }

    uint8_t* GetBuff(int _index){
        _mut.lock();
        uint8_t* ret = _memory_[static_cast<size_t>(_index)];
        _mut.unlock();
        return ret;
    }

    void FreeIndex(int _index){
        _mut.lock();
        index[static_cast<size_t>(_index)] = true;
        _mut.unlock();
    }

} StaticPool;

#ifdef __cplusplus
}
#endif

#endif // UDPMODULE_H
