//====================================================================
//Adarsha Kanel
// 30049820
//refrence useD: https://repl.it/@emmynex2007/A3-interval-Calc#main.cpp
// Anisha tut: https://d2l.ucalgary.ca/d2l/le/content/327860/viewContent/4323218/View
//				https://d2l.ucalgary.ca/d2l/le/content/327860/viewContent/4323474/View
//hint provided: https://d2l.ucalgary.ca/d2l/le/content/327860/Home?itemIdentifier=D2L.LE.Content.ContentObject.ModuleCO-4323473
// Xining tut: https://d2l.ucalgary.ca/d2l/le/content/327860/viewContent/4320829/View (code_11_4_6.zip)
// ======================================================================
//
// count_pi() calculates the number of pixels that fall into a circle
// using the algorithm explained here:
//
// https://en.wikipedia.org/wiki/Approximations_of_%CF%80
//
// count_pi() takes 2 paramters:
//  r         =  the radius of the circle
//  n_threads =  the number of threads you should create
//
// Currently the function ignores the n_threads parameter. Your job is to
// parallelize the function so that it uses n_threads threads to do
// the computation.

#include "calcpi.h"
#include <pthread.h>
#include <iostream>
#include "calcpi.h"
#include <stdio.h>
#include <math.h>
#define number_threads 256

int radius;
struct Task
{int start_row, end_row;
double partial_count;};
Task task[256];

void * threads(void* i){
	// count is initialized at 0
	uint64_t count = 0;
	//index will be the index used to access the items in Task task
	int index = (long) i;
	// req = double(radius)*radius
	double rsq = double(radius) * radius;
	// paralleize by starting loop from task[index].start_row to task[index].end_row
	for( double x = task[index].start_row ; x <= task[index].end_row ; x ++){
		for( double y = -radius ; y <= radius ; y ++){
			if( x*x + y*y <= rsq) {
				// incremenet count by 1
				count ++;
			}
		}
	}
	// store the total count for this start row and end row in task[index].partial_count
	task[index].partial_count = count;
	return NULL;
}


uint64_t count_pi(int r, int n_threads)
{ 
	// radius will be r
	radius = r;
	// if n_threads is more than r, then dont make extra thread, make r number of threads
	if(n_threads > r) n_threads = r;
	//initialize the threads
	pthread_t thread[n_threads];
	// the start will be -radius
	int start = -radius;
	// number of ints in radius will be len(-radius) + len(radius) + len(0)
	int number_of_radius = (radius *2)+1 ;

	for (int i = 0; i < n_threads; i++){
		// the start row for this index will be start
		task[i].start_row = start;
		// if this is the last pthread, then make the endrow for this index be radius
		if( i == (n_threads-1)){
			task[i].end_row = radius;
		}else{
			// start will be number of radius / the number of threads
			start+= floor(number_of_radius / (n_threads));
			// endrow increased by the updated start, since end starts at 0
			task[i].end_row += start;
			// start will be incremenet by 1, so its end +1
			start++;
		}
		// create the pthread and give it the arg i so that it can access the Task task
		pthread_create(&thread[i],NULL, threads, (void*)(intptr_t)i);
	  } 
	  // once all the pthreads are done, join them
	 for(int i = 0; i< n_threads ; i++){
		pthread_join(thread[i], NULL);
	 }
	// total count will be the final count that adds all the counts parallized by pthread
	uint64_t total_count = 0;
	for (int i = 0; i < n_threads; i++) {
		// add all the counts produced by pthreads
		total_count += task[i].partial_count;
	}
	// return the total count as the count
	return total_count;
}



