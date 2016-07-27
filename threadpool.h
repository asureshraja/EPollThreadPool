#include <sys/eventfd.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>
struct threadpool{
        int num_of_threads;
        int worker_epfd;
        struct lfq *q;
        int *msg_passer;
};

struct task{
    void (*function)();
    void * arg;
};
void worker_thread_work(struct threadpool *thpool);
void init(struct threadpool *thpool);
void submit_to_worker_thread(struct threadpool *thpool,struct task *arg);
