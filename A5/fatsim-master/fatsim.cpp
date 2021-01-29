/*
Adarsha Kanel
30049820
*/
#include "fatsim.h"
#include <cstdio>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <set>
#include <vector>
std::vector<long> visited;
std::vector<long> out_loop;
std::vector<long> n;
long size = 0;
void fat_sim(const std::vector<long> & fat, long & longest_file_blocks, long & unused_blocks)
{
	//out_loop will contain the nodes that have been visited
	out_loop.resize((int) fat.size());
	// visited will contain the nodes visited at each iteration
	visited.resize((int) fat.size());
	longest_file_blocks = 0;;
	unused_blocks = 0;
	for(int i = 0; i < (int) fat.size(); i++ ) {
		// if the node hasnt been visited yet continue
		if(out_loop[i] != 0) continue;
		long current = i;
		long block;
		//start traversing the adjecent edges of that node
		while(1){
			//block becomes the adjacent node of the current node
			block = fat[current];
			// if this node has not been visited in the current iteration
			if(visited[current] == 0){		
				// push that adjacent node to n
				n.push_back(current);
				//found a path that is not a loop
				if(block == -1){	
					//reset visited
					for(int i = 0; i < (int) n.size(); i++){
						visited[n[i]] = 0;
						size++;
					}
					break;
				}else{	// if the block is not end continue iteration
					//mark the node as being visited
					visited[current] = 1;
					// start iteration again with the adjact node as the root node
					out_loop[current] = -1;
					current = block;
				}
			}else{// loop found
				// mark the nodes as visited in out loop and reset cisited
				for (int i = 0; i < (int) n.size(); i++){
					out_loop[n[i]] = n[i]+1;
					visited[n[i]] = 0;
				}
				break;
			}
		}
		//clear n for next index
		n.clear();

		// check if the path of the current index was greater than pervious one
		if(size > longest_file_blocks) longest_file_blocks = size;
		size = 0;
	}
	// find all the vertices greater than 0 in outloop to find the unsued blocks

	for (int i = 0; i < (int) out_loop.size(); i++){
		if(out_loop[i] > 0){
			unused_blocks ++;
		}
	}		
}
