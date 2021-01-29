//refrence useD: https://repl.it/@emmynex2007/A3-interval-Calc#main.cpp
// Anisha tut: https://d2l.ucalgary.ca/d2l/le/content/327860/viewContent/4323218/View
//				https://d2l.ucalgary.ca/d2l/le/content/327860/viewContent/4323474/View
//hint provided: https://d2l.ucalgary.ca/d2l/le/content/327860/Home?itemIdentifier=D2L.LE.Content.ContentObject.ModuleCO-4323473
#include "sumFactors.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <atomic>
// calculates smallest divisor of n that is less than n
//
// returns 0 if n is prime or n < 2
//
// examples:
//    n =  1         --> result = 0
//    n =  2         --> result = 0
//    n =  9         --> result = 3
//    n = 35         --> result = 5
//    n = 8589934591 --> result = 7
//
// you may modify this function
//
pthread_mutex_t mutex;
pthread_barrier_t barrier;
pthread_barrier_t barrier2;
int64_t max;
int64_t n_threads;
std::atomic<bool> done, end, skip;;
std::atomic <int64_t> tracker;
struct Task {std::atomic<int64_t> f;};
Task task[256];
int64_t n;
int64_t min;
int64_t sum ;
int64_t min_index;
int64_t multiplication;

int64_t finder(int64_t index){
	if(task[index].f < min){
		min = task[index].f;
		// copy the index of this as min_index
		min_index = index;
	}
	int64_t count;
	//check if in task if any factors in the previous index thread were found
	for(int i = 0; i < index; i++){
		// if there is a task[index].f that isnt 1 or higher, it might still be the lowest factor, so break
		if(task[index].f == 0) {
			count = 0;
			break;
		}else count ++;		// otherwise keep counting
	}
	return count;
}
int64_t factor(void * ind){
	int64_t	index = (long) ind;
	// if done == false continue, or cancel the thread
	if(done == false){
		if( n <= 3) {
			min = 0;	// handle values less than 3
			return min;
		} // 2 and 3 are primes
		else if( n % 2 == 0){
			min = 2;	//handle value that is divisible by 2
			return min;
		}
		else if( n % 3 == 0) {		
			min = 3;	// handle value that is divisible by 3
			return min;
		}else{
			int64_t start_row, end_row;	
			// initialized multiplication depending on num
			multiplication = ((6* ((max-5)/(6*n_threads))));
			// increase start row by 5 + multiplication* index
			start_row = 5 + (multiplication*index);
			// endrow is start row + mltiplication
			end_row = start_row + multiplication;	
			// if the thread is the last thread, then end_row must be the max, to cover all the ints in num
			if(index == (n_threads - 1))  end_row = max;

			int64_t start_in_thread = start_row;
			// continue after syncronization
			pthread_barrier_wait(&barrier2);
			while(( start_in_thread <= end_row) && (start_in_thread <= max) ){
				// if done is made true when running this loop, stop and cancel the thread
				if(done == true	) break;
				// we found a factor of n
				if (n % start_in_thread == 0) {
					pthread_mutex_lock(&mutex);
					// put that int as the smallest factor of this thread in task
					task[index].f = start_in_thread;			
					// if the finder(index) returns index+1 that means it is the smallest factor
					if(finder(index) == index+1){
						// we can make done true and cancel all threads from working
						done = true;
					}
					pthread_mutex_unlock(&mutex);
					break;
				}
				if (n % (start_in_thread+2) == 0) {
					pthread_mutex_lock(&mutex);
					// put that int as the smallest factor of this thread in task
					task[index].f = start_in_thread+2;
					// if the finder(index) returns index+1 that means it is the smallest factor
					if(finder(index) == index+1){
						// we can make done true and cancel all threads from working
						done = true;
					}
					pthread_mutex_unlock(&mutex);
					break;
					
					}
				// incremenet start_in_thead by 6
				start_in_thread += 6;	
				}
			}
	}
	// make task[index].f = 1, to indicate that this index has been read completely
	task[index].f = 1;
	pthread_barrier_wait(&barrier2);
	// reset task[index].f to 0
	task[index].f = 0;
	return min;
}


// reads in a positive integers in range (2 .. 2^63-1) from standard input
// for each number on input:
//   if the number is prime, it is ignored
//   determines the smallest factor of the composite number
// prints out the sum of all smallest factors
//
// if no composite numbers are found, it returns 0
//
// please note: the current implementation is single-threades, so
// n_threads argument is ignored
//
// you may modify this function
//
void * get_smallest_divisor(void * ind)
{
	while(1) {
		//syncronize all threads
		int res = pthread_barrier_wait(&barrier2);
		if(res ==PTHREAD_BARRIER_SERIAL_THREAD){
			int64_t num;
			end = false;
			if( ! (std::cin >> num) ){ 
				end =true;
			}
			// put n as num so that other functions can use this constant
			n = num;

		}
		// reset skip and done as false
		skip = false;
		done = false;
		//syncronize all threads before continuing
		pthread_barrier_wait(&barrier2); 
		// if cin couldnt be read, break
		if(end == true){
			break;
		}
		// the min factor is initialized as n
		min = n;
		min_index = n;
		//tracker is reset
		tracker = 0;
		// max is square root of n
		max = sqrt(n);
		// if the thread isnt done continue doing factor
		if(!done) factor(ind);
		//only add min to sum once per num
		if(!skip){
			// skip is true, so no other thread will add min to sum
			skip = true;
			// if min is still n, then it is a prime number, so min = 0
			if(min == n) min = 0;
			sum += min;
		}
	}
	// wait till all the threads are syncronized before quitting them
	pthread_barrier_wait(&barrier); 
    pthread_exit(NULL);

}


int64_t sum_factors(int n_thread)
{
	//nthreads = n_thread
	n_threads = n_thread;
	//make n_threads number of threads
	pthread_t thread[n_threads];
	// initialize a barrier "barrier" with count n_threads +1
	pthread_barrier_init(&barrier, NULL, n_threads+1);
	//initialize a barrier "barrier2"
	pthread_barrier_init(&barrier2, NULL, n_threads);
	// initialize a mutex "mutex"
	pthread_mutex_init( &mutex, NULL);
	// reset sum to 0
	sum = 0;
	for(int i = 0; i < n_threads; i++){
		// create n_threads pthread "thread" once, to run on function get_smallest_divisor with arg i
		pthread_create(&thread[i],NULL, get_smallest_divisor, (void*)(intptr_t)i);
	}
	// wait till all the threads are done the function get_smallest_divisor before returning sum
	pthread_barrier_wait(&barrier);
	// join all threads 
	for(int i = 0; i< n_threads ; i++){
		pthread_join(thread[i], NULL);
	}
	// destroy mutex "mutex"
	pthread_mutex_destroy(&mutex);
	//destroy barrier "barrier"
	pthread_barrier_destroy(&barrier);
	//"destroy barrier "barrier2"
	pthread_barrier_destroy(&barrier2);
	return sum;
}