#pragma once

#include<cstddef>

template<typename T>
class Memory_Pool
{
    public:
        Memory_Pool(){

        }

        T* acquire(){return nullptr;}
};