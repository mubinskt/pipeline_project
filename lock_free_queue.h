#pragma once

#include <cstddef>

template<typename T>
class LockFreeQueue
{
    std::size_t unit_size;
    public:
        explicit LockFreeQueue(std::size_t size_): unit_size(size_){

        }

        bool push(const T&);
        bool pop(T&);

        std::size_t get_unit_size() {
            return unit_size;
        }

};