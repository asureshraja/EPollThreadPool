#include <iostream>
#include "concurrentqueue.h"
moodycamel::ConcurrentQueue<struct task *> q(10000000);
extern "C" void enqueue(struct task *arg){
        while(q.try_enqueue(arg)==false) {
                std::cout << "failing in q elem allocation" << std::endl;
        }

}
extern "C" struct task * dequeue(){
        struct task *item;
        bool retval = q.try_dequeue(item);
        if(!retval) {return NULL; }
        return item;
}
