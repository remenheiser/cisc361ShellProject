
#include "get_path.h"

int pid;
int sh( int argc, char **argv, char **envp);
char **stringToArray(char *input);
char readInput(char* buffer);
char *which(char *command, struct pathelement *pathlist);
char *where(char *command, struct pathelement *pathlist);
void list ( char *dir );
void printenv(char **envp);

#define PROMPTMAX 32
#define MAXARGS 10
#define BUFFERSIZE 128
