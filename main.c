#include "sh.h"
#include <signal.h>
#include <stdio.h>

void sig_handler(int signal); 

int main( int argc, char **argv, char **envp )
{
  if ((int)getpid() != 0) {
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGTSTP, sig_handler);
    signal(EOF, sig_handler);
    }  
    return sh(argc, argv, envp);
}

void sig_handler(int signal)
{
  /*TODO define your signal handler */
  if (signal == SIGINT || signal == SIGTERM || signal == SIGTSTP || signal == EOF) {
    return;
  }
}

