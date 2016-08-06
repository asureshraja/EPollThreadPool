#include <stdio.h>
#include "threadpool.h"
void print_without_num(){
    printf("%s\n","hello");
}
void print(int *a){
    printf("%s %d\n","hello",*a);
}
void sleep_test(){
    while (1) {
        sleep(1);
    }
}
int main(){
        struct threadpool *thpool;
        thpool = make_new_threadpool(4);
        init(thpool);
        struct task *t1;
        t1=malloc(sizeof(struct task));
        int *a=malloc(sizeof(int));
        *a=5;
        printf("a= %d\n",a ,*a);
        t1->arg=NULL; //chk with variable 'a' also
        t1->function=print_without_num;
        int i;
        for (i = 0; i < 4; i++) {
            submit_to_worker_thread(thpool,t1);
        }
        sleep(6);
}
