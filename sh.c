#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "sh.h"

int sh( int argc, char **argv, char **envp )
{
  char *prompt = calloc(PROMPTMAX, sizeof(char));
  char *commandline = calloc(MAX_CANON, sizeof(char));
  char *command, *arg, *commandpath, *p, *pwd, *owd;
  char **args = calloc(MAXARGS, sizeof(char*));
  int uid, i, status, argsct, go = 1;
  struct passwd *password_entry;
  char *homedir;
  struct pathelement *pathlist;

  uid = getuid();
  password_entry = getpwuid(uid);               /* get passwd info */
  homedir = password_entry->pw_dir;		/* Home directory to start
						  out with*/
     
  if ( (pwd = getcwd(NULL, PATH_MAX+1)) == NULL )
  {
    perror("getcwd");
    exit(2);
  }
  owd = calloc(strlen(pwd) + 1, sizeof(char));
  memcpy(owd, pwd, strlen(pwd));
  prompt[0] = ' '; prompt[1] = '\0';

  /* Put PATH into a linked list */
  pathlist = get_path();

  while ( go )
  { //ALL TODO
  
    printf("\n[%s]", prompt);                         /* print your prompt */
    readInput(commandline);
    stringToArray(commandline);
    
                          /* get command line and process */

                          /* check for each built in command and implement */

                          /*  else  program to exec */
    {
                          /* find it */
                          /* do fork(), execve() and waitpid() */

                          /* else */
                            /* fprintf(stderr, "%s: Command not found.\n", args[0]); */
    }
  }
  return 0;
} /* sh() */

//readInput() reads standard input and puts it into a buffer. Helps allow strings with spaces
char readInput(char* buffer) {
    char length = 0;
    if (fgets(buffer, BUFFERSIZE, stdin) != 0) {
        length = strlen(buffer);
        buffer[length - 1] = '\0';
    }
    return length;
}


char **stringToArray(char *input)
{
  // make a copy of array
  char buff[BUFFERSIZE];
  strcpy(buff, input);
  char *t = strtok(buff, " "); // this returns the first word (if empty string it returns NULL)
  int count;

  // call strtok over and over until you determine how long it is
  while (strtok(NULL, " "))
  {
    count++;
  }

  char **argv = malloc((count + 1) * sizeof(char *));
  argv[count] = NULL;

  count = 0;
  strcpy(buff, input);
  t = strtok(buff, " ");

  while (t)
  {
    int len = strlen(t);
    argv[count] = (char*)malloc((len + 1) * sizeof(char*));
    strcpy(argv[count], t);
    count++;
    t = strtok(NULL, " ");
  }

  return argv;
}


char *which(char *command, struct pathelement *pathlist )
{ //TODO
   /* loop through pathlist until finding command and return it.  Return
   NULL when not found. */
  printf("Executing Built-In command: [which]\n");
  pathlist = get_path();
  while (pathlist->next != NULL) {
    if (strcmp(pathlist->element, command) == 0) {
      return command;
    } 
    pathlist = pathlist->next;
  }
  return NULL;
} /* which() */

char *where(char *command, struct pathelement *pathlist )
{ //TODO
  /* similarly loop through finding all locations of command */
  printf("Executing Built-In command: [where]\n");
  pathlist = get_path();
  char* result = "";
  while (pathlist->next != NULL) {
    if (strcmp(pathlist->element, command) == 0) {
       result = strcat(result, pathlist->element);
    } 
    pathlist = pathlist->next;
  }
  return result;
} /* where() */

void list ( char *dir )
{ //TODO
  /* see man page for opendir() and readdir() and print out filenames for
  the directory passed */
  
} /* list() */

