/*
Adarsha Kanel
30049820
Refrences used:	lecture slides
				hints provided in: https://d2l.ucalgary.ca/d2l/le/content/327860/Home
				Anisha's tut recording : https://d2l.ucalgary.ca/d2l/le/content/327860/viewContent/4354326/View
*/
#include "common.h"
#include "deadlock_detector.h"
#include "common.h"
#include "deadlock_detector.h"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <set>
#include <vector>
/// this is the function you need to implement
/// parameter edges[] contains a list of request and assignment edges
///   example of a request edge, process "p1" resource "r1"
///     "p1 -> r1"
///   example of an assignment edge, process "XYz" resource "XYz"
///     "XYz <- XYz"
/// Parameters edge_index and cycle[] are used to return
/// results back to the caller.
///
/// You need to process edges[] one edge at a time, and run a deadlock
/// detection after each edge.
///
/// As soon as you detecte a deadlock, you need to set edge_index to the index of the edge
/// in edges[] that caused the deadlock. For example, edges[7] caused the deadlock,
/// then set edge_index=7. You must also populete cycle[] with the names of
/// processes that are in a deadlock. You can then return from the function without
/// processing any other edges.
///
/// To indicate deadlock was detected after processing all edges, you must
/// set edge_index = -1 and clear cycle[] e. by calling cycle.clear()
///
void detect_deadlock(
    const std::vector<std::string> & edges,
    int & edge_index,
    std::vector<std::string> & cycle)
{
	// adj list is a vecotr of vector of int, that have ints that have an outgoing edge towards the index (node)
	std::vector<std::vector<int>> adj_list;
	// outcount is a vector of int that contains the amount of outgoing edges by each index (node)
	std::vector<int> out_counts;
	// word to int is a vector of string that contain the string represntation of each indexx ( node)
	std::vector<std::string> word_to_int;
	// initializing word2int
	Word2Int x;
	edge_index = -1;
	int index = -1;
	std::vector<int> zero;
	int max_range = -1;
	cycle.clear();
	std::vector<int> out;
	std::string first_string;	
	std::string second_string;
	int first;
	int second;
	int out_size = 0;
	// for loop for topoloigcal sorting
	for(auto result: edges){
		// index is incremented by 1, so that we can go to the next node in the next sorting ( if one is needed)
		index ++;
		// split the result to be used to sort
		auto word = split(result);
		// add a string "process" at the end of process node, to make it easier to recognize
		first_string = word[0] + " process";
		// convert first_string into a int and store in first
		first = x.get(first_string);
		// add a string "resource" at the end of resource node, to make it easier to recognize
		second_string = word[2] + " resource";
		// convert second_string to an int and store in second
		second = x.get(second_string);
		// find the max of first and second
		int max_find = std::max(first, second);
		// if max_find is more than max range we make max range = max find
		if( max_find >= (max_range)){
			max_range = max_find;
        }
		// since max range is changed, we have to increase the size of each vertex
		if ( max_range >= out_size){
			// resize the following vertex to make indexes and prevent seg errors
			out_size = 2*max_range;
			out_counts.resize(out_size);
			adj_list.resize(out_size);
			word_to_int.resize(out_size);	
		}	
		// in index first, store first_string for easy retrival later
		word_to_int[first] = (first_string);
		// in index second, store second string for easy retrival later
		word_to_int[second] = (second_string);
		
		// if we have process -> resource that do the following
		if(word[1] == "->"){
			// add first as haveing an outgoing edge to second
			adj_list[second].push_back(first);
			// increment the outgoing edges of first by 1
			out_counts[first]++; 
		}else{		// we have process <- resource
			// add second as having a outgoing edge towards first
			adj_list[first].push_back(second);
			// increment the num of outgoing edge of second by 1
			out_counts[second]++;
		}
		// make a substitue vector out with the contents of out_count to be modified
		out = out_counts;	
		// find all the nodes with outgoing edges of 0 and push in zero
		for(unsigned long int i =0; (int) i<= max_range;i++){
			if(out[i] == 0){
				zero.push_back(i);
			}
		}
		// if zero is not empty continue
		while (!zero.empty()){
			// get the last element of zero as q
			int q = zero.back();
			// pop that element
			zero.pop_back();
			// for all nodes that have outgoing edge towards q, do 
			for( auto bb:adj_list[q]){
				// decrement the outgoing edge of each of them by 1
				out[bb]--;
				// if some of them have no more outgoing edge, add them to zero
				if(out[bb] == 0) zero.push_back(bb);
			}
		}
		//check if there are any elements in out, if there is, there must be a cycle
		for(unsigned long int i =0; (int) i<=max_range;i++) {
			// found a cycle, get the processes in that cycle
			if(out[i] > 0) {
				auto res = split(word_to_int[i]);
				// add the process nodes to cycle, without the "process" in the end
				if( res[1] == "process") cycle.push_back(res[0]);
			}
		}
		// if cycle is not empty, that means a cycle was found, so add index as the edge index and stop the loop
		if(!(cycle).empty()){
			edge_index = index;
			break;
		}
	}
}