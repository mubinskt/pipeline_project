
#include <iostream>
#include "memory_pool.h"
#include "lock_free_queue.h"
#include "producer.h"
#include "consumer.h"
#include "metrics.h"

int main() {
    std::cout << "Pipeline skeleton OK\n";

    LockFreeQueue<int> l(sizeof(10));

    LockFreeQueue<int>* l2 = new LockFreeQueue<int>(1);

    l.get_unit_size(); 

    std::cout << "LFQ 1 size "  << l.get_unit_size() << "\n";
    std::cout << "LFQ 2 size " << l2->get_unit_size() << "\n";

    return 0;
    
}
