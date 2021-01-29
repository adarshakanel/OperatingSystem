//Refrence used : https://gitlab.com/cpsc457/public/longest-int-my-getchar/-/blob/master/fast-int.cpp
#include <unistd.h>
#include <stdio.h>
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <iostream>
#include <list>

// split string p_line into a vector of strings (words)
// the delimiters are 1 or more whitespaces
std::vector<std::string>
split( const std::string & p_line)
{
  auto line = p_line + " ";
  std::vector<std::string> res;
  bool in_str = false;
  std::string curr_word = "";
  for( auto c : line) {
    if( isspace(c)) {
      if( in_str)
        res.push_back(curr_word);
      in_str = false;
      curr_word = "";
    }
    else {
      curr_word.push_back(c);
      in_str = true;
    }
  }
  return res;
}

// global variable used in stdin_readline()
// 
// reads in a line from STDIN
// reads until EOF and result includes \n if present
// returns empty string on EOF
char buffer[1024*1024]; // 1MB sotrage to hold results of read()
int buff_size = 0;      // stores how many characters are stored in buffer
int buff_pos = 0;       // position in bufffer[] where we extract next char

char
fast_read_one_character_from_stdin()
{
  // check if buffer[] is empty
  if( buff_pos >= buff_size) {   

    // buffer is empty, let's replenish it
    buff_size = read( STDIN_FILENO, buffer, sizeof(buffer));
    // detect EOF, send char version of -1
    if(buff_size <= 0) return -1;
    // reset position from where we'll return characters
    buff_pos = 0;
  }
  // return the next character from the buffer and update position
  return buffer[buff_pos++];
}

// returns the next char from stdin, or -1 on EOF
// returns characters stored in buffer[] until it is empty
// when buffer[] gets empty, it is replenished by calling fast_read_one_character_from_Stdin()
std::string
stdin_readline()
{
  // initialize result as a string to return 
  std::string result;
  // run until we return result or break
  while( true) {
	  // c will contain the individual chars returns from fast_Read_one_chracter_from_stdin
    char c = fast_read_one_character_from_stdin();
    // -1 signifies an end of file, and if c  = -1, then we reset buff_pos, buff_size abd return result
    if( c == -1){
	// buff_pos = 0 and vuff_size = 0, to prevent broken pipes
	buff_pos = 0;
	buff_size = 0;
	return result;
    }
    // if c returns \n and if result has atleast 1 char in it, break
    // done to prevent cases where we get 2 \n and the algorithm is ended prematurely 
    if(c == '\n' && result.size() > 0) break;
     // if none of the break or return statemnts are triggered, collect the chars c in result
    result.push_back(c);
  }
  return result;
	
}
// returns true if a word is palindrome
// palindrome is a string that reads the same forward and backward
//    after converting all characters to lower case
bool
is_palindrome( const std::string & s)
{
  for( size_t i = 0 ; i < s.size() / 2 ; i ++)
    if( tolower(s[i]) != tolower(s[s.size()-i-1]))
      return false;
  return true;
}

// returns the longest palindrome on standard input
// in case of ties for length, returns the first palindrome
// all input is broken into words, and each word is checked
// word is a sequence of characters separated by white space
// white space is whatever isspace() says it is
//    i.e. ' ', '\n', '\r', '\t', '\n', '\f'
std::string
get_longest_palindrome()
{
  std::string max_pali;
  //infinite loop till we gear a breal
  while(1) {
    // string line will be the string value returned by stdin_readline
    std::string line = stdin_readline();
    // if no string was returned break as we may have reached EOF
    if( line.size() == 0) break;
    // the return value of stdini_readline was not "", so we split up the string into individual words
    auto words = split( line);
    // for all each word in words, do
    for( auto word : words) {
      // if the word size is less than the current max palindrome size, then conditnue otherwise, no point in continuing 
      if( word.size() <= max_pali.size()) continue;
      // check if the word is a palindrome
      if( is_palindrome(word))
	// we know the word is a palindome and the length is more or equal to the length of current longest palindrome, so we keep the new word as max_pali
        max_pali = word;
    }
  }
  // at the end of the loop, return the max_pali found
  return max_pali;
}

int
main()
{
  std::string max_pali = get_longest_palindrome();
  printf("Longest palindrome: %s\n", max_pali.c_str());
  return 0;
}

