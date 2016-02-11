#include <sys/eventfd.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <stdio.h>
#include "thpool.h"
//g++ queueapi.cpp -o queueapi.o -std=c++11 -c -fPIC -Wall
//g++ -shared -o libq.so queueapi.o
extern void enqueue(struct task *arg);
extern struct task * dequeue();
#define MAX_MESSAGE_PASSERS 100
void worker_thread_work();void submit_to_worker_thread(struct task *arg);
int msg_passer[MAX_MESSAGE_PASSERS];
int worker_epfd;int num_of_threads;
void init(int no_of_threads){
    num_of_threads=no_of_threads;
    worker_epfd = epoll_create(num_of_threads);
    pthread_t threads[num_of_threads];int i;
        for (i = 0; i < num_of_threads; i++) {
                pthread_create(&threads[i], NULL, &worker_thread_work, NULL);
                msg_passer[i] = eventfd(0, EFD_NONBLOCK);
                struct epoll_event msg_passer_evnt;
                msg_passer_evnt.data.fd = msg_passer[i];
                msg_passer_evnt.events = EPOLLIN | EPOLLET;
                epoll_ctl(worker_epfd, EPOLL_CTL_ADD, msg_passer[i], &msg_passer_evnt);

        }
}
void worker_thread_work() {
        int number_of_ready_events; //epoll collected events counts
        struct epoll_event *events; //epoll events
        //maximum  events epoll should collect before returning in batch
        int max_events_to_stop_waiting = num_of_threads / 2;
        //maximum  time epoll should collect before returning in batch
        int epoll_time_wait = 1; //time wait for message passing
        events = malloc(sizeof(struct epoll_event) * max_events_to_stop_waiting); //epoll event allocation for multiplexing
        int i;
        struct task *arg;
        while (1) {
                number_of_ready_events = epoll_wait(worker_epfd, events, max_events_to_stop_waiting, epoll_time_wait);
                for (i = 0; i < number_of_ready_events; i++) {
                        eventfd_t val;
                        eventfd_read(events[i].data.fd, &val);
                        while ((arg = dequeue()) != NULL) {
                                arg->function(arg->arg);
                                arg = NULL;
                        }
                }
        }
}
void submit_to_worker_thread(struct task *arg) {
        static int counter = 0;
        enqueue(arg);
        counter++;
        eventfd_write(msg_passer[counter % num_of_threads], 1);
}


//test code starting
void sleep_test(){
    while (1) {
        sleep(1);
    }
}
void print(){
    printf("%s\n","hello");
}
int main(int argc, char const *argv[]) {
    printf("%s\n","Starting testing");
    init(5);
    struct task t1;
    t1.arg=NULL;
    t1.function=print;

    pthread_t threads[num_of_threads];int i;
    for (i = 0; i < num_of_threads; i++) {
        submit_to_worker_thread(&t1);
    }
    for (i = 0; i < num_of_threads; i++) {
            pthread_create(&threads[i], NULL, &sleep_test, NULL);

    }
    return 0;
}
