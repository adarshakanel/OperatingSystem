/*
Adarsha Kanel
30049820
Refrences used: https://repl.it/@michelleeeeeeee/List-and-Set-Examples-A5#main.cpp
				anisha tut: Decemeber 3 tutorial
				Gabriela : Dec 8 tutorial and December 3 tutorial
				https://www.geeksforgeeks.org/find-maximum-and-minimum-element-in-a-set-in-c-stl/
				https://www.geeksforgeeks.org/data-structures/linked-list/
*/
#include "memsim.h"
#include <cstdio>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <list>
#include <unordered_map>
#include <set>
#include <vector>
#include <stdio.h>
#include <math.h>

struct Partition {
	long tag ;
	long size ;
	int64_t addr ;
}; 
typedef std::list<Partition>::iterator PartitionRef;
struct scmp {  
	bool operator()( const PartitionRef & c1, const PartitionRef & c2) const {
		if( c1-> size == c2-> size) return c1-> addr < c2-> addr; 
		else return c1-> size < c2-> size;  
	} 
};

std::list<Partition> partitions;
// sorted partitions by size/address
std::set<PartitionRef,scmp> free_blocks;
// quick access to all tagged partitions
std::unordered_map<long, std::vector<PartitionRef>> tagged_blocks; 
struct Simulator {
	//intitialize pages_requested as 0
	int64_t pages_requested=0;
	//initialize pages_size_
	int64_t page_size_;
	Simulator(int64_t page_size) { page_size_ = page_size; }
	
	// a derivation of clear_all_blocks shown by prof in class
	void deallocate(int tag){
		// for all partitions in tagged_blocks
		for(auto &p : tagged_blocks[tag]){
			// mark them as empty
			p->tag = -1;
			// check to see if the previous partition is also empty
			if(p!= partitions.begin()&&std::prev(p)->tag==(-1)){
				// deallocate the current partition and add the size of the pervious empty partition
				p->size += std::prev(p)->size;
				// update the address of current partition to previous
				p->addr = std::prev(p)->addr;
				// remove prev of p from free_blocks as it has been merged with p
				free_blocks.erase(std::prev(p));
				// remove prev of p from partitions as it has been merged with p
				partitions.erase(std::prev(p));
			}
			// check to see if the next partition is also empty
			if(std::next(p) != partitions.end() && std::next(p)->tag==(-1)){
				// if the next partition is also empty, then add the size of both the current and next partition
				p->size +=std::next(p)->size;
				// remvoe next p from the free_blocks as it has been merged with p
				free_blocks.erase(std::next(p));
				// remove the next of p from partitions as it has been merged with p
				partitions.erase(std::next(p));
			}
			// reinsert p into free blocks
			free_blocks.insert(p);
			
		}
		// clear tagged blocks at the end
		tagged_blocks[tag].clear();
	}
	// derivation of ALLOCATED shown by prof in class
	void allocate(int tag, int size){
		// if the size of parititon is lesd than 0 return
		if(size < 0) return;
		int64_t size_request;
		int64_t prev_node_addr;
		int64_t size_requester;
		//partition a will be a partition that is empty with size "size"
		Partition a;
		a.tag = -1;
		a.size = size;
		a.addr= 0;
		PartitionRef best_partition;
		//generate dummy iterator and find the best match if one exists
		std::list<Partition> dummy {a};
		auto sbesti = free_blocks.lower_bound( dummy.begin());
		PartitionRef best_free_block_iter = partitions.end();
		// partition aa will represent the partitison got, with aa.tag being tag, aa.size being size and the address to be found
		Partition aa;
		aa.tag = tag;
		aa.size = size;
		if( sbesti != free_blocks.end()){
			best_free_block_iter = * sbesti;
			//best paritiion will be best_free_block_iter to avoid confusion
			best_partition = *sbesti;
			prev_node_addr = ((*std::prev(best_partition)).addr +(*std::prev(best_partition)).size);
			// the address of aa will be the address of previous node + size of previous node
			aa.addr = prev_node_addr;
			// insert aa to partition
			partitions.insert(best_partition, aa);
			// push back previous of best partition to tageed block of tag
			tagged_blocks[tag].push_back(std::prev(best_partition));
		}
		// if best_free_block_iter is the end of partition then a best partition is not found
		if( best_free_block_iter == partitions.end()){
			// if partition is not empty 
			if(!partitions.empty()){
				// best_partition is the prev of best_free_block_iter
				best_partition = --best_free_block_iter;
				// erase best_free_block_iter from free blocks to modify it 
				free_blocks.erase(best_partition);
				// find the size of memory needed to be requested
				size_requester = ( size - (*best_partition).size );
				// find if the size needed to be requested is a modular of page_size, otherwise use the ceil
				if(size_requester%page_size_ == 0) size_request = ceil(size_requester/page_size_);
				else size_request = floor(size_requester/page_size_)+1;
				// increase pages requested by the amount calculated above
				pages_requested += size_request;
				// increase the size of best_partition by (size_request*page_size_), to represent the meomory increase
				(*best_partition).size += (size_request*page_size_);
				// insert best_partition into free blocks
				free_blocks.insert(best_partition);
				prev_node_addr = ((*std::prev(best_partition)).addr + (*std::prev(best_partition)).size);
				// the address of aa will be the address of previous node + size of previous node
				aa.addr = prev_node_addr;
				// insert aa to partition
				partitions.insert(best_partition, aa);
				// push back previous of best partition to tageed block of tag
				tagged_blocks[tag].push_back(std::prev(best_partition));
			}else{
				// make a new paritition beginning which is free, has size and addr 0, and push into paritition, to avoid seg faults
				Partition beginning;
				beginning.tag = -1;
				beginning.size = 0;
				beginning.addr= 0;
				partitions.push_back(beginning);
				// best_partition is the pervious parttition of best_free_block_iter
				best_partition = --best_free_block_iter;
				// find the size of memory needed to be requested
				size_requester = ( size - (*best_partition).size );
				// find if the size needed to be requested is a modular of page_size, otherwise use the ceil
				if(size_requester%page_size_ == 0) size_request = ceil(size_requester/page_size_);
				else size_request = floor(size_requester/page_size_)+1;
				// increase pages requested by the amount calculated above
				pages_requested += size_request;
				// increase the size of best_partition by (size_request*page_size_), to represent the meomory increase
				(*best_partition).size += (size_request*page_size_);
				free_blocks.insert(best_partition);
				prev_node_addr = 0;
				// the address of aa will be the address of previous node + size of previous node
				aa.addr = prev_node_addr;
				// insert aa to partition
				partitions.insert(best_partition, aa);
				// push back previous of best partition to tageed block of tag
				tagged_blocks[tag].push_back(std::prev(best_partition));
			}
		}
		//make a new partition push that represents the memory left after aa has been placed
		Partition push;
		// the ta of push is -1 to show that it is empty
		push.tag = -1;
		// the address of push will be the prev_node_addr + its size
		push.addr = prev_node_addr + size;
		// if the size of (*best_partition).size != size
		if( (*best_partition).size != size){
			// the size of push is that deifference between size of best_partition and size
			push.size = (*best_partition).size - size;	
			// insert this into parititons
			partitions.insert(best_partition,push);
		} else if (std::prev(partitions.end()) == best_partition) {	// if the size of both is the same, then push is size 0, since all have been used to allocate aa
			// so push size is 0
			push.size = 0;
			// insert push into partitions in the place of best_partition
			partitions.insert(best_partition, push);	
		}			
		// re insert the previous of best_partition into free block
		free_blocks.insert(std::prev(best_partition));
		// erase best_partition from free_blocks
		free_blocks.erase(best_partition);
		// erase best_partition from partitions
		partitions.erase(best_partition);
	}
	void getStats(MemSimResult & result)
	{
		// if free_blocks is not empty
		if(!free_blocks.empty()){
			// find the partition of the max val among free block
			std::list<Partition>::iterator max_val = *((free_blocks).rbegin());
			result.max_free_partition_size = (*max_val).size;
		}else result.max_free_partition_size = 0;	// if free blocks is empty, the largest size must be 0
		// put pages_requested in result.n_pages_requested
		result.n_pages_requested = pages_requested;
	}
};



// re-implement the following function
// ===================================
// input parameters:
//    page_size: integer in range [1..1,000,000]
//    requests: array of requests, each with tag and size
// output parameters:
//    result: populate with correct values before returning
void mem_sim(int64_t page_size, const std::vector<Request> & requests, MemSimResult & result)
{
	Simulator sim(page_size);
	for (const auto & req : requests) {
		if (req.tag < 0) {
		sim.deallocate(-req.tag);
		} else {
		  sim.allocate(req.tag, req.size);
		}
	}

	sim.getStats(result);
}