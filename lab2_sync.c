/*
*   DKU Operating System Lab
*           Lab2 (Vehicle production Problem)
*           Student id : 32170078
*           Student name : 강정현
*
*   lab2_sync.c :
*       - lab2 main file.
*       - must contains Vehicle production Problem function's declations.
*
*/

#include <aio.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#include "lab2_sync_types.h"

/*
 * you need to implement Vehicle production Problem. 
 */



void lab2_sync_usage(char *cmd) {
	printf("\n Usage for %s : \n",cmd);
    printf("    -c: Total number of vehicles produced, must be bigger than 0 ( e.g. 100 )\n");
    printf("    -q: RoundRobin Time Quantum, must be bigger than 0 ( e.g. 1, 4 ) \n");
}

void lab2_sync_example(char *cmd) {
	printf("\n Example : \n");
    printf("    #sudo %s -c=100 -q=1 \n", cmd);
    printf("    #sudo %s -c=10000 -q=4 \n", cmd);
}

int main(int argc, char* argv[]) {
	char op;
	int n; char junk;
	int time_quantum = 0;
	int total_car = 0;

	if (argc <= 1) {
		lab2_sync_usage(argv[0]);
		lab2_sync_example(argv[0]);
		exit(0);
	}

	for (int i = 1; i < argc; i++) {
		if (sscanf(argv[i], "-c=%d%c", &n, &junk) == 1) {
			total_car = n;
		}
		else if (sscanf(argv[i], "-q=%d%c", &n, &junk) == 1) {
			time_quantum = n;
		}
		else {
			lab2_sync_usage(argv[0]);
			lab2_sync_example(argv[0]);
			exit(0);
		}
	}

	/*
	   * You need to implement Vehicle production Problem 
	   * If you want to use other header file, it's ok!
	   * If you have any question for this task, please contact me (ghwls03s@gmail.com)
	   * You may be need to use pthread_create function
	*/

	// 변수 선언
	struct timeval start, end;
	double result_T[3];
	int balance[3] = {0,0,0};

	// 전역 변수 초기화
	queue = malloc(sizeof(Queue));
	order = malloc(sizeof(int)*total_car + 1);
	*order = total_car;    // order[0] = total_car
	for (int i=0; i<NUM; i++) cunsId[i] = i;

	input_data(workload, total_car);
  
	// Round-Robin 결과가 order 에 저장됨.
	RR(total_car, time_quantum);

	// No Lock Experiment
	gettimeofday(&start, NULL);
	//balance[0] = experimet(NoLock_producer, NoLock_consumer);
	gettimeofday(&end, NULL);
	result_T[0] = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)/1000000);
	
	// Coarse-grained Lock Experiment
	gettimeofday(&start, NULL);
	balance[1] = experimet(Coarse_producer, Coarse_consumer);
	gettimeofday(&end, NULL);
	result_T[1] = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)/1000000);
	
	// Fine-grained Lock Experiment
	gettimeofday(&start, NULL);
	balance[2] = experimet(Fine_producer, Fine_consumer);
	gettimeofday(&end, NULL);
	result_T[2] = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)/1000000);

	// 문자열 출력 양식 지정
	char str[3][35] = {
		"(1) No Lock Experiment",
		"(2) Coarse-grained Lock Experiment",
		"(3) Fine-grained Lock Experiment"
	};

	// 결과 출력
	for(int i=0; i<3; i++)
		print_result(str[i], total_car, balance[i], result_T[i]);
}

void print_result(char* str, int total_car, int balance, double execution_time) {
	printf("==== Vehicle Production Problem ====\n");
	printf("%s\n", str);
	printf("Experiment Info\n");
	printf("\tTotal Produce Number = %d\n", total_car);
	printf("\tFinal Balance Value = %d\n", balance);
	printf("\tExecution time = %f\n", execution_time);
}

void test() {
	qinit(queue,2);
	enqueue(queue, 1);
	enqueue(queue, 1);
	printf("%d\n", qfull(queue));
}