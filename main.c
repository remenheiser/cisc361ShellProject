#include "sh.h"
#include <signal.h>
#include <stdio.h>

void sig_handler(int signal); 

int main( int argc, char **argv, char **envp )
{
  /*TODO put signal set up stuff here */

  return sh(argc, argv, envp);
}

void sig_handler(int signal)
{
  /*TODO define your signal handler */
}

