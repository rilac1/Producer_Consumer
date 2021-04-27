/*
*   DKU Operating System Lab
*           Lab2 (Vehicle Production Problem)
*           Student id : 32170078
*           Student name : 강정현
*
*   lab1_sync_types.h :
*       - lab2 header file.
*       - must contains Vehicle Production Problem's declations.
*
*/

#ifndef _LAB2_HEADER_H
#define _LAB2_HEADER_H

#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#define MAX_SIZE 100
#define MAX_QUEUE 10
#define NUM 5

/*
 * You need to Declare functions in  here
 */

typedef struct car_queue {
	int *data;
	int size;		// 큐 용량
	int balance;
	int rear, front;
}Queue;

typedef struct Car {
	int carId;
	int start_time;
	int product_num;
}Car;

#endif /* LAB2_HEADER_H*/

// 전역변수
pthread_mutex_t mutex;
pthread_mutex_t mutex_producer;
pthread_cond_t empty;
pthread_cond_t fill;

int finish;			// if (producer is finished) finish = 1 
int *order;			// Priority_order
int cunsId[NUM];	// CunsId = {0 ~ num}
Car workload[NUM];
Queue *queue;


// 함수 헤더
void input_data(Car* w);
void input_data2(Car* w);
void input_data3(Car* w);
void input_data4(Car* w);
void print_result(char* str, int total_car, int balance, double execution_time);

void qinit(Queue *q, int qsize);
void enqueue(Queue *q, int data);
int dequeue(Queue *q);
int qpeek(Queue *q);
int qempty(Queue *q);
int qfull(Queue *q);

void RR(int c, int q);

void *NoLock_producer(void *order);
void *NoLock_consumer(void *Id);

void *Coarse_producer(void *order);
void *Coarse_consumer(void *Id);

void *Fine_producer(void *order);
void *Fine_consumer(void *Id);

int experimet(void* producer, void* consumer);

void test();