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

	// Workload 지정
/* workload1 */
	input_data(workload);
/* workload2 */
	// input_data2(workload);
/* workload3 */
	// input_data3(workload);
/* workload4 */
	// input_data4(workload);
  
	// Round-Robin 결과가 order 에 저장됨.
	RR(total_car, time_quantum);

	// No Lock Experiment
	// !!!!! 세 번째와 네 번째 workload 사용시 balance[0] 줄은 주석처리해야 함. !!!!!
	gettimeofday(&start, NULL);
	balance[0] = experimet(NoLock_producer, NoLock_consumer);
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

void input_data(Car* w) {
    w[0].carId = 0;
    w[0].start_time = 0;
    w[0].product_num = 15;

    w[1].carId = 1;
    w[1].start_time = 2;
    w[1].product_num = 30;

    w[2].carId = 2;
    w[2].start_time = 4;
    w[2].product_num = 20;

    w[3].carId = 3;
    w[3].start_time = 6;
    w[3].product_num = 25;

    w[4].carId = 4;
    w[4].start_time = 8;
    w[4].product_num = 10;
}

void input_data2(Car* w) {
    w[0].carId = 0;
    w[0].start_time = 30;
    w[0].product_num = 200;

    w[1].carId = 1;
    w[1].start_time = 0;
    w[1].product_num = 350;

    w[2].carId = 2;
    w[2].start_time = 60;
    w[2].product_num = 100;

    w[3].carId = 3;
    w[3].start_time = 40;
    w[3].product_num = 50;

    w[4].carId = 4;
    w[4].start_time = 10;
    w[4].product_num = 300;
}

void input_data3(Car* w) {
    w[0].carId = 0;
    w[0].start_time = 300;
    w[0].product_num = 2500;

    w[1].carId = 1;
    w[1].start_time = 0;
    w[1].product_num = 1000;

    w[2].carId = 2;
    w[2].start_time = 40;
    w[2].product_num = 3000;

    w[3].carId = 3;
    w[3].start_time = 150;
    w[3].product_num = 1500;

    w[4].carId = 4;
    w[4].start_time = 1000;
    w[4].product_num = 2000;
}

void input_data4(Car* w) {
    w[0].carId = 0;
    w[0].start_time = 0;
    w[0].product_num = 15000;

    w[1].carId = 1;
    w[1].start_time = 2000;
    w[1].product_num = 30000;

    w[2].carId = 2;
    w[2].start_time = 4000;
    w[2].product_num = 20000;

    w[3].carId = 3;
    w[3].start_time = 6000;
    w[3].product_num = 25000;

    w[4].carId = 4;
    w[4].start_time = 8000;
    w[4].product_num = 10000;
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