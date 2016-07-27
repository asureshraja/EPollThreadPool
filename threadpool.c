#include "threadpool.h"
#include "lfq.h"
struct threadpool* make_new_threadpool(int no_of_threads){
        struct threadpool *thpool;
        thpool=malloc(sizeof(struct threadpool));
        thpool->num_of_threads=no_of_threads;
        thpool->msg_passer = malloc(sizeof(int)*no_of_threads);
        thpool->q=make_new_lfq(100);
}
void init(struct threadpool *thpool){
        thpool->worker_epfd = epoll_create(thpool->num_of_threads);
        pthread_t threads[thpool->num_of_threads];
        int i;
        for (i = 0; i < thpool->num_of_threads; i++) {
                pthread_create(&threads[i], NULL, &worker_thread_work, thpool);
                thpool->msg_passer[i] = eventfd(0, EFD_NONBLOCK);
                struct epoll_event msg_passer_evnt;
                msg_passer_evnt.data.fd = thpool->msg_passer[i];
                msg_passer_evnt.events = EPOLLIN | EPOLLET;
                epoll_ctl(thpool->worker_epfd, EPOLL_CTL_ADD, thpool->msg_passer[i], &msg_passer_evnt);
        }
}
void worker_thread_work(struct threadpool *thpool) {
        int number_of_ready_events; //epoll collected events counts
        struct epoll_event *events; //epoll events
        //maximum  events epoll should collect before returning in batch
        int max_events_to_stop_waiting = thpool->num_of_threads / 2;
        //maximum  time epoll should collect before returning in batch
        int epoll_time_wait = 10; //time wait for message passing
        events = malloc(sizeof(struct epoll_event) * max_events_to_stop_waiting); //epoll event allocation for multiplexing
        int i,k=-1;
        struct task *t1;
        while (1) {
                number_of_ready_events = epoll_wait(thpool->worker_epfd, events, max_events_to_stop_waiting, epoll_time_wait);
                for (i = 0; i < number_of_ready_events; i++) {
                        eventfd_t val;
                        eventfd_read(events[i].data.fd, &val);
                        while ((k = dequeue(thpool->q,&t1)) != -1) {
                                if (t1->arg==NULL) {
                                        t1->function();
                                }else{
                                        // printf("val%d\n",arg->arg);
                                        t1->function(t1->arg);
                                }
                        }
                }
        }
}
void submit_to_worker_thread(struct threadpool *thpool,struct task *arg) {
        static int counter = 0;
        enqueue(thpool->q, arg);
        counter++;
        eventfd_write(thpool->msg_passer[counter % thpool->num_of_threads], 1);
}
