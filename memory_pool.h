#pragma once

#include<cstddef>
#include "item.h"
#include <stack>

template<typename T>
class Memory_Pool
{
    private:
        //std::unique_ptr<T> _resource;
        size_t capacity;

        T* p_resourcePool;

        std::stack<T*> positions;

    public:
        Memory_Pool(size_t N)
        {
            capacity = N;
            //_resource = new std::make_unique<T[]>(N); - this manages ownership, not pool, not recommended for mem pools
            
            // Allocate raw, properly aligned storage
            // No contr called
            // proper alignment
            // correct for lock-free/ pool use
            void* raw = ::operator new(sizeof(T) * N, std::align_val_t(alignof(T))); 


            p_resourcePool = (static_cast<T*>(raw));

            for(T* index = p_resourcePool; index < (p_resourcePool + N); index++)
            {
                positions.push(index);
            }
        }

        ~Memory_Pool()
        {
            ::operator delete(p_resourcePool, std::align_val_t(alignof(T)));      
        }

        template<typename... Args>
        T* acquire(Args&&... args)
        {
            if(positions.empty()) {
                std::cout << "memory full \n";
                return nullptr;
            }

            auto slot = positions.top();
            positions.pop();

            T* obj = new (slot) T(std::forward<Args>(args)...); // new (&p_resourcePool[index]) T();

            return obj;
        
        }

        void release(T* obj)
        {
            positions.push(obj);
            obj->~T();
        }

        uint64_t size() {
            return capacity;
        }

        uint64_t available() {
            return positions.size();
        }
};