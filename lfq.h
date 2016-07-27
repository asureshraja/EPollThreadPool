#include <pthread.h>
struct lfq{
        int size;
        void **buffer;
        int direction;
        int tail;
        int head;
};
struct lfq * make_new_lfq(int size){
        struct lfq *temp;
        temp=malloc(sizeof(struct lfq));
        temp->size=size;
        temp->buffer=malloc(sizeof(void *) * size);
        temp->direction=1;
        temp->tail=-1;
        temp->head=0;
        return temp;
}

int dequeue(struct lfq *queue, void** data){
    int old_head_val=0;
    int new_head_val=0;


    //recursive force swap
    do{

                    old_head_val=queue->head;
                    new_head_val=old_head_val+1;
                    *data=queue->buffer[old_head_val];
                //     printf("%d \n",old_head_val );
                    if (new_head_val-1>=queue->size) {
                        new_head_val=0;
                        queue->direction=1;
                    }
                    if (queue->direction==1) {
                        if (old_head_val>=queue->tail+1) {
                                 printf("no element to dequeue\n");
                            return -1;
                        }
                    }


}while (!__sync_bool_compare_and_swap(&queue->head, old_head_val, new_head_val));

    return 1;
}

int enqueue(struct lfq *queue,void *data){
    int old_tail_val=0;
    int new_tail_val=0;
//recursive force swap
    do{
            old_tail_val=queue->tail;
            new_tail_val=old_tail_val+1;
            if (new_tail_val>=queue->size) {
                new_tail_val=0;
                queue->direction=-1;
            }
            if (queue->direction==1) {
                if (new_tail_val>=queue->size) {
                    return -1;
                }
            }else{
                if (new_tail_val>=queue->head) {
                        return -1;
                }
            }
}while ( !__sync_bool_compare_and_swap(&queue->tail, old_tail_val, new_tail_val));
    queue->buffer[new_tail_val]=data;
    return 1;
}
