#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#include "lab2_sync_types.h"

void input_data(Car* w, int total_car) {
	srand(time(NULL));
	int total = total_car;
	int weight = total/10;
    int randTotal[NUM];
    int randAriv[NUM];
    int selected[NUM];
    int next;
    randAriv[0] = 0;

    for (int i=0; i<NUM; i++) {
        if (i+1 == NUM) randTotal[i] = total;
        else {
            randTotal[i]= rand()%(total/3) + (total/5);
            total -= randTotal[i];
        }
        selected[i] = 1;
    }

    for (int i=1; i<NUM; i++)
        randAriv[i] = rand()%weight;

	for (int i=0; i<NUM; i++) {
        while(1) {
            next = rand()%NUM;
            if(selected[next]) break;
        }
		w[i].carId = i;
		w[i].start_time = randAriv[next];
		w[i].product_num = randTotal[next];
        selected[next]=0;
    }
}

/* Queue methods */
void qinit(Queue *q, int qsize) {
    q->front = q->rear = 0; 
    q->data = (int*)malloc(sizeof(int)*(qsize + 1));
    q->size = qsize;
    q->balance = 0;
    q->balance_enqueue = 0;
    q->balance_dequeue = 0;
}
void enqueue(Queue *q, int data) {
    assert(!qfull(q));
    q->rear = (q->rear + 1) % (q->size + 1);    
    q->data[q->rear] = data;
    q->balance++;
}
int dequeue(Queue *q) {
    assert(!qempty(q));
    q->front = (q->front + 1) % (q->size + 1);
    q->balance--;
    return q->data[q->front];
}
void enqueue_fine(Queue *q, int data) {
    assert(!qfull(q));
    q->rear = (q->rear + 1) % (q->size + 1);    
    q->data[q->rear] = data;
    q->balance_enqueue++;
}
int dequeue_fine(Queue *q) {
    assert(!qempty(q));
    q->front = (q->front + 1) % (q->size + 1);
    q->balance_dequeue--;
    return q->data[q->front];
}
int qpeek(Queue *q) {
    if (qempty(queue))
        return -1;
    int temp;
    temp = (q->front + 1) % (q->size + 1);
    return q->data[temp];
}
int qempty(Queue *q) {
    return q->front == q->rear;
}
int qfull(Queue *q) {
    return ((q->rear + 1) % (q->size + 1)) == q->front;
}

/* Round Robin */
// order은 스케쥴링 순서.
// order[0]에는 total_car 값이 들어있음.
void RR(int c, int q) {
    Queue *tempq = malloc(sizeof(Queue));     // RR에서 사용할 Queue.
    qinit(tempq, 5);
    int remain[NUM];
    int start[NUM];
    int running = -1;
    int next = -1;

    for (int i=0; i<NUM; i++) {
    	remain[i] = workload[i].product_num;
        start[i] = workload[i].start_time;
    }

    for (int run_time=0; run_time<c; run_time++) {
        for (int i=0; i<NUM; i++)
            if (start[i] == run_time && !qfull(tempq))
                enqueue(tempq, i);
        if (running<0) {
            if(next>=0 && !qfull(tempq))
                enqueue(tempq, next);
	        if (!qempty(tempq)) {
                next = dequeue(tempq);
                running = 0;
            }
        }
        if (running>=0) {
            order[run_time + 1] = next;     // order[0] = total_car
            remain[next]--;
            running++;
            if (running==q)
		        running = -1;
	        if (remain[next] == 0) {
                running = -1;
		        next = -1;
            }
        }
    }
}

/* No Lock */
void *NoLock_producer(void *order) {
    int total_car = *((int *)order);
    int *carId = ((int *) order + 1);

    for (int i=0; i<total_car; i++) {
        while(1)
            if (!qfull(queue)) {
                enqueue(queue, carId[i]);
                break;
            }
    }
    finish = 1;
}
void *NoLock_consumer(void *Id) {
    int id = *(int *)Id;
    while(!finish || !qempty(queue)) {
        if (qpeek(queue) == id)
            dequeue(queue);
    }
}

/* Coarse-Grained Lock */
void *Coarse_producer(void *order) {
    int total_car = *((int *)order);
    int *carId = ((int *) order + 1);

    for (int i=0; i<total_car; i++) {
        pthread_mutex_lock(&mutex);
        while(qfull(queue))
            pthread_cond_wait(&empty, &mutex);
        enqueue(queue, carId[i]);
        pthread_cond_broadcast(&fill);      // using broadcast()
        pthread_mutex_unlock(&mutex);
    }
    while(1) pthread_cond_broadcast(&fill);
}
void *Coarse_consumer(void *Id) {
    int id = *(int *)Id;
    int quota = workload[id].product_num;

    for (int i=0; i<quota; i++) {
        pthread_mutex_lock(&mutex);
        while(qpeek(queue) != id)
            pthread_cond_wait(&fill, &mutex);
        dequeue(queue);
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
    }
}

/* Fine-Grained Lock */
void *Fine_producer(void *order) {
    int total_car = *((int *)order);
    int *carId = ((int *) order + 1);

    for (int i=0; i<total_car; i++) {
        pthread_mutex_lock(&mutex_producer);
        while(qfull(queue))
            pthread_cond_wait(&empty, &mutex_producer);
        enqueue(queue, carId[i]);
        pthread_mutex_unlock(&mutex_producer);

        pthread_cond_broadcast(&fill);      // using broadcast()
    }
    while(1) pthread_cond_broadcast(&fill);
}
void *Fine_consumer(void *Id) {
    int id = *(int *)Id;
    int quota = workload[id].product_num;

    for (int i=0; i<quota; i++) {
        pthread_mutex_lock(&mutex);
        while(qpeek(queue) != id)
            pthread_cond_wait(&fill, &mutex);
        dequeue(queue);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&empty);
    }
}

/* Experiment methods*/
int experimet(void* producer, void* consumer) {
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex_producer, NULL);
    pthread_cond_init(&empty, NULL);
    pthread_cond_init(&fill, NULL);
    qinit(queue, MAX_QUEUE);
    pthread_t p, c[NUM];
    finish = 0;

    pthread_create(&p, NULL, producer, order);
    for (int i=0; i<NUM; i++) 
        pthread_create(&c[i], NULL, consumer, cunsId+i);

	//pthread_join(p, NULL); 
    for (int i=0; i<NUM; i++)
        pthread_join(c[i], NULL);

    return (queue->balance_enqueue)-(queue->balance_dequeue);
}