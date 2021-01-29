/// =========================================================================
/// Written by pfederl@ucalgary.ca in 2020, for CPSC457.
/// =========================================================================
/// DO NOT EDIT THIS FILE. DO NOT SUBMIT THIS FILE FOR GRADING.

#include "getDirStats.h"
#include <cstdio>
#include <cstdlib>

void usage( const std::string & pname, int exit_code)
{
  printf( "Usage: %s directory_name\n", pname.c_str());
  exit( exit_code);
}

std::string vs2str( const std::vector<std::string> & arr)
{
  std::string res;
  bool first = true;
  for( const auto & s : arr) {
    if( ! first) res += " ";
    first = false;
    res += s;
  }
  return res;
}

int main( int argc, char ** argv)
{
  if( argc != 2) usage( argv[0], -1);

  Results res;
  auto success = getDirStats( argv[1], res);
  if( ! success) {
    printf("Could not get dir stats.\n");
  }
  else {
    printf("--------------------------------------------------------------\n");
    printf("Largest file:      %s\n", res.largest_file_path.c_str());
    printf("Largest file size: %ld\n", res.largest_file_size);
    printf("Number of files:   %ld\n", res.n_files);
    printf("Number of dirs:    %ld\n", res.n_dirs);
    printf("Total file size:   %ld\n", res.all_files_size);
    printf("Most common file types:\n");
    for( auto & type : res.most_common_types)
      printf("  - %s\n", type.c_str());
    int gcount = 1;
    for( auto & group : res.duplicate_files) {
      printf("Duplicate files - group %d:\n", gcount ++);
      for( auto & f : group) {
        printf("  - %s\n", f.c_str());
      }
    }
    printf("--------------------------------------------------------------\n");
  }

  return 0;
}
