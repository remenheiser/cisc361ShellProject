#include "sh.h"
#include <signal.h>
#include <stdio.h>

void sig_handler(int signal); 

int main( int argc, char **argv, char **envp )
{
  /*TODO put signal set up stuff here */
  int result = sh(argc, argv, envp);
  //  for (int i = 0; i < sizeof(argv); i+=8) {
  //  free(argv[i]);
  // }
  return result;
}

void sig_handler(int signal)
{
  /*TODO define your signal handler */
}

