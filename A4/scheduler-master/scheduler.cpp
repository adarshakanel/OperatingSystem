/*
Adarsha Kanel
30049820
Refrences used: lecture slides
				hints provided in: https://d2l.ucalgary.ca/d2l/le/content/327860/Home
				Anisha's tut recording : https://d2l.ucalgary.ca/d2l/le/content/327860/viewContent/4354326/View
				Gabriella tut A4 Hints https://d2l.ucalgary.ca/d2l/le/content/327860/viewContent/4363796/View
*/
#include "scheduler.h"
#include "common.h"
#include <queue>
#include <iostream>
#include <memory>
#include <numeric>
#include <set>
#include <vector>
#include <math.h>
// this is the function you should edit
//
// runs Round-Robin scheduling simulator
// input:
//   quantum = time slice
//   max_seq_len = maximum length of the reported executing sequence
//   processes[] = list of process with populated IDs, arrival_times, and bursts
// output:
//   seq[] - will contain the execution sequence but trimmed to max_seq_len size
//         - idle CPU will be denoted by -1
//         - other entries will be from processes[].id
//         - sequence will be compressed, i.e. no repeated consecutive numbers
//   processes[]
//         - adjust finish_time and start_time for each process
//         - do not adjust other fields
//
void simulate_rr(
    int64_t quantum, 
    int64_t max_seq_len,
    std::vector<Process> & processes,
    std::vector<int> & seq
) {
	// clear seq before proceeding
	seq.clear();
	int64_t index = 0;
	bool idle = true;
	bool push = false;
	int64_t curr_time = 0;
	int64_t previous = -2;
	int64_t actual_time = 0;
	bool begin = false;
	int64_t job_remaining = processes.size();
	std::queue<int64_t> q;
	std::vector<int64_t> remaining_time;
	bool repeat = false;
	// make sure the bursts from processes[index].burst is copied to remaining time
	for(unsigned long int i = 0; i < processes.size();i++) remaining_time.push_back(processes[i].burst);
	// infinity loop
	while(1){
		// if job remaining is 0 break
		if(job_remaining == 0) break;
		// if CPU is not idel
		if(!idle){
			// if bool push is true
			if (push == true){
				// reset idle totrue
				idle = true;
				// if the index remaining time is more than 0, re que it back in q
				if(remaining_time[index] > 0) {
					q.push((int) index);
				}
			}else {		// if push is false
				idle = true;
				// if remaining time of the index is 0, then mark it as finished job and mark the current time as the finish time
				if (remaining_time[index] == 0){
					processes[index].finish_time = actual_time;
					job_remaining--;

				}
			}
		}
		// check if any new process is arriving in this crrent time
		for (int i = index; i < (int)processes.size(); i++) {
			// if a new process arrived in curre time, then push it back in the queue
			if(curr_time == processes[i].arrival_time){
				q.push((int) processes[i].id);
			}
		}
		// if cpu is idle and q is empty
		if(idle == true && q.empty()){
			// if the job remaining > 0, then
			if(job_remaining > 0){
				// if the max seq length is > seq.size 
				if(max_seq_len > (int)seq.size()){
					// if the previous thing added to seq was not -1. add a -1 ( to prevent duplicate processes)
					if (previous != -1) seq.push_back(-1);
					previous = -1;
				}
				// if this was the first process, jump to the arrival time of the first process 
				if(begin == false) {
					curr_time = processes[0].arrival_time;
					actual_time = curr_time;
					// mark that we are no longer in the beginning
					begin = true;
					continue;
				}
			}
			// increase actual time and current time by 1
			actual_time++;
			curr_time++;
			// do not go further and continue
			continue;
		}
		// if idle was true and q was not empty
		if(idle == true && !q.empty()){
			// begin is no longer false
			begin = true;
			// get the first element in queue
			index = q.front();
			// pop that element
			q.pop();
			// if the burst in process and remaining time for the same index is the same, then this can be marked as the start for index
			if(processes[index].burst == remaining_time[index]){
				processes[index].start_time = curr_time;
			}
			// infinity loop
			while(1){
				int64_t jump;
				// if repeat is true do:
				if(repeat == true){
					// we will jump till the next index in processes arrives
					jump = floor((processes[index+1].arrival_time - actual_time )/quantum)*quantum;
					// subtract processes[index] by this jump
					remaining_time[index] -= jump;
				}
				// if the remaining time[index] > quantum
				if(remaining_time[index] > quantum){
					// decrease the remaining_time[index] by quantum
					remaining_time[index] -=  quantum;
					// if repeat was false just increase the actual time by quantum
					if(repeat == false) actual_time += quantum;	
					else actual_time += quantum + jump;		// if it was not false, then increase the time by quantum + jump
					// turn push to true, to indicate we can continue this process
					push = true;
					// increment current time by 1
					curr_time++;
				}else {
					// else just increase atual time by remaining_time
					actual_time += remaining_time[index];
					curr_time ++;
					// the processes of index is complete so make ts remaining time 0
					remaining_time[index] = 0;
					// make push false to indicate that we donot need to further push this
					push = false;
				}
				// makr that the CPU is no longer idle
				idle = false;
				// if the max_seq_len > seq.size(), then 
				if(max_seq_len > (int)seq.size()){
					// if the previous is not index, push it back into seq
					if (previous != index) seq.push_back(index);
					// make the previous this index, to prevent duplicates
					previous = index;
				}
				// if job remaining == 1
				if(job_remaining == 1){
					// jump to the end of the task and break
					processes[index].finish_time = actual_time + remaining_time[index];
					break;
				}	
				// check if we missed any new process that arrived when we jumped
				if(processes[index+1].arrival_time <= actual_time){
					for (int i = index; i < (int)processes.size(); i++) {
						if(processes[i].arrival_time >= curr_time && processes[i].arrival_time < actual_time){
							q.push((int) processes[i].id);
						}
					}
					// if we enter this loop, then repeat = false
					repeat = false;
				}else if(q.empty() && remaining_time[index] > 0) {	// if q is still empty and the remainng time of current inex > 0
					// make repeat true
					repeat = true;
				}else repeat = false;		// if none of the above match, make repeat false
				// make current time actual time to make up for the jump
				curr_time = actual_time;
				// if repeat is still false break this while loop
				if(repeat == false) break;
			}
			// if the job remaining is 1, break
			if(job_remaining == 1)break;
			continue;
		}
		// increment actual and current time by 1
		actual_time++;
		curr_time++;
	}
}