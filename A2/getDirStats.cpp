/*
Adarsha Kanel
UCID: 30049820

Refrences Used: 

https://stackoverflow.com/questions/19795341/how-can-i-sort-a-vector-containing-pairint-int-elements-sorting-is-done-as-pe
http://www.cplusplus.com/reference/unordered_map/unordered_map/find/
https://pubs.opengroup.org/onlinepubs/009695399/functions/popen.html
https://gitlab.com/cpsc457/public/find-empty-directories/-/blob/master/myfind.cpp

Michelle: https://d2l.ucalgary.ca/d2l/le/content/327860/viewContent/4295456/View    (2pmRecording-A2Discussion-PleaseSeeOtherVideoAsWell)
			Tutorial5Demos(FIXED).zip
			Tutorial10ExercisesWORKEDOUT.zip

Anisha: W05D02_code.cpp

Michelle: dirsearch_dirent.cpp
*/
#include "getDirStats.h"
#include "digester.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <utility>
#include <algorithm>
#include <unordered_map>
#include <numeric>
//is_dir will take in a path and return true if the path given results in a directory
static bool
is_dir( const std::string & path)
{
  struct stat buff;
  if( 0 != stat( path.c_str(), & buff))
    return false;
  return S_ISDIR(buff.st_mode);
}

// global variable that will keep the path for the largest file
std::string largest_file_path;	

// global variable that will keep track of the largest file size, returns -1 if no files in dir
long largest_file_size = -1;

// global variable that will keep track of the number of files in the given directory(dir_name)
long n_files ;

// global variable that will keep track of the file size for every file in dir_name ( except directories)
long all_files_size ;

// global variable that will keep track of all the sub directories in dir_name
long n_dirs;

// global variable that will keep track of the types of each file and how many times they occur
std::unordered_map<std::string,int> most_common_types;

// global variable that will take most_common_types and swap the position of the pairs such that we can sort them
std::vector<std::pair< int, std::string>> common_types;

// global variable that will contain a vector that containts vector of string such that they contain duplicate content of each file in descending order
std::unordered_map<std::string,std::vector<std::string>> duplicate_files_map;

// global variable that contain a pair (int, std::string), where the string is the result of shah256_from_file and int is the number of duplicate files with the same hash code
std::vector<std::pair<int, std::string>> duplicate;

// global variable that is a vector contain vector of string that is a result of shah256_from_file such that each string are ordered from most occured to least
std::vector<std::vector<std::string>> duplicate_files;



//recursion will take in a dir_name and computes stats about directory dir_name
//if successful it will return true, otherwise return false
bool recursion(const std::string & dir_name){
	DIR *dir;
	struct dirent *entry;
	struct stat fileStat;
	std::vector<std::string> stack;
	// push back the path of the current dir_name in the stack to be used recursively
	stack.push_back(dir_name.c_str());
	FILE *fp;

	// recursively keep reading the path fo each directories until stack is empty
	while( ! stack.empty()) {
		// dir_name is the path of a directory to be read recursively
		auto dir_name = stack.back();
		// pop the back of the stack, so that we remove the current directory that is being read from stack
		stack.pop_back();
		dir = opendir(dir_name.c_str());

		// if the current directory can be opened, then continue, otheriwse move on to next
		if (dir){
			std:: string path;
			// multiple will keep track if there are multiple hidden files in the dir
			//int multiple = 0;
			// command is the const char *command for popen, all will be used to give the type of all the functions ( including hidden files )  in a directory
			std::string command = ("file -b " + dir_name + "/{*,.*}") ;
			// fp = the popen(command)
			fp = popen(command.c_str(), "r");
			
			//buffer is 4 KIB
			int BUFFER = 1024 * 4;
			//reset or initialize pather
			char pather[BUFFER] = "";
			// if the popen returns null, stop and return false, as we cannot open it
			if(fp == NULL){
				return false;
			}
			// while the fgets != null continue.
			while (fgets(pather, BUFFER, fp) != NULL){
				// turn pather into a string to be used
				auto line = std::string (pather) + " ";
				std::string result;	
				// result will contain the substring of pather before "," or "\n" is encoutered
				for( auto c : line) {
					// ',' or '\n' encountered, break loop and return result
					 if((c == ',') || (c == '\n')){
						break;
					 }
					else {
						// while c != ',' or c != '\n' continue pushing the characters to result
					  result.push_back(c);
					}
				}

				// if we still have result as "cannot open" that means its an empty directory, so continues
				if(result.find("cannot open") != std::string::npos){
					continue;
				}
				//	 if result = directory, then ignore as we only want file typles
				if(result == "directory") continue;
				// if the file type cannot be found in most_common_types, then add a new pair with pair (result, 1)
				if (most_common_types.find (result) == most_common_types.end() ) {
					most_common_types.emplace(result, 1);
				// if the file type was found, increement the number of times it occurs by 1
				}else {
					most_common_types[result]++;		
				}
			}
			// once the loop is done, we do not neeed to use popen anymore for this directory, so pclose(fp)
			pclose(fp);
			// continue reading the directory, until there is no more files or subdirectory left
			dir = opendir(dir_name.c_str());
			while(1){
				entry = readdir(dir);
				// if there is no more files or subdirectory, end the while loop 
				if(!entry) break;
				// ignore if the d_name is "." or ".."
				if(strcmp(entry ->d_name, ".") == 0 || strcmp(entry ->d_name, "..") == 0){
					continue;
				}
				// path = the directory we are in + '/' + file name
				path = dir_name+'/'+entry-> d_name;
				// if the path of the current file is actully a direcotry do this
				if (is_dir(path) == true ){
					// the number of subdirectory is incremenet by 1
					n_dirs += 1;
					// push this directory path in stack to be recursively read later on
					stack.push_back( path);
					// donot do the rest of the while loop, and continue to the next file in directory
					continue;
				}
				// get a hash code for the file by calling shah256_from_file and giving it the object as path
				std::string hash = sha256_from_file( path);
				// push this file as once that has the hash code hash in duplicate_files_map
				duplicate_files_map[hash].push_back(path);
				// increase the number of files in directory by 1
				n_files += 1;
				// if 0 is not returned by stat, then permission is denied and we cannot read the size of this file
				if(stat(path.c_str(),&fileStat) < 0){
					return false;
				}
				// all_files_size incremenets by the current file size
				all_files_size += fileStat.st_size;
				// if the largest_file_size < the current file size, then continue
				if(largest_file_size < fileStat.st_size){
					// replace the previous largest file size and replace it with the current file size
					largest_file_size = fileStat.st_size;
					// the path of the largest file is updated to this file
					largest_file_path = path;
				}
			}
		}
		// closefir (dir) once the while loop ends
		closedir(dir);
	}	
		// switch the placement of the pairs in most_common_types and push them in common_types, in order to be sorted
		for(auto& map: most_common_types){
			common_types.push_back(std::make_pair(map.second, map.first));

		}
		// sort the first pair in common_types, which will give us the number of times each types occur, in descending order
		std::sort(common_types.begin(), common_types.end(), std::greater<std::pair< int, std::string>>());
		
		for(auto& map: duplicate_files_map){
			// if the num, of duplicate files in duplicate_files_map[map.first] is >1 then continue
			if((duplicate_files_map[map.first].size()) > 1){
				// push back the size of the duplicate file as the first pair and the has code as the second pair ( because sort, sorts by first pair)
				duplicate.push_back(std::make_pair((duplicate_files_map[map.first].size()), map.first));
			}
		}
		// sort the duplicate by the first pair in descending order
		std::sort(duplicate.begin(), duplicate.end(), std::greater<std::pair<int, std::string>>());
		
		int count = 0;
		for(auto& map: duplicate){
			// push back the first 5 pairs of duplicate in duplicate_files to get the 5 largest no of duplicate files
			duplicate_files.push_back(duplicate_files_map[map.second]);
			count++;
			if (count == 5) break;
		}
	return true;
}

// getDirStats() computes stats about directory dir_name
// if successful, it return true and stores the results in 'res'
// on failure, it returns false, and res is in undefined state
//
bool
getDirStats(const std::string & dir_name, Results & res)
{
	// call the recursion function to set all the global variables and check if it returns false
	if(recursion(dir_name) == false){
		// if it returns false, we could not read the dir, so return false
		return false;
	}
	//recursion returned true, so so have out information to print
	// set all the res.files to equal to the correct variable
	res.largest_file_path = largest_file_path;
	res.largest_file_size = largest_file_size;
	res.n_files = n_files;
	res.n_dirs = n_dirs;
	res.all_files_size = all_files_size;
	int num = 0;
	std::vector< std::string> name;
	// only the top 5 common files are picked and given to res.common_types
	for(auto myPair : common_types){
		num ++;
		name.push_back(myPair.second);
		if (num ==5) break;
	}
  res.most_common_types = name;
  res.duplicate_files = duplicate_files;
return true;
}
