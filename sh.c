#include "sh.h"
#include <dirent.h>
#include <limits.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int sh(int argc, char **argv, char **envp) {
    char *prompt = calloc(PROMPTMAX, sizeof(char));
    char *commandline = calloc(MAX_CANON, sizeof(char));
    char *command, *arg, *commandpath, *p, *pwd, *owd;
    char **args = calloc(MAXARGS, sizeof(char *));
    int uid, i, status, argsct, go = 1;
    struct passwd *password_entry;
    char *homedir;
    struct pathelement *pathlist;

    uid = getuid();
    password_entry = getpwuid(uid);   /* get passwd info */
    homedir = password_entry->pw_dir; /* Home directory to start
						  out with*/

    if ((pwd = getcwd(NULL, PATH_MAX + 1)) == NULL) {
        perror("getcwd");
        exit(2);
    }
    owd = calloc(strlen(pwd) + 1, sizeof(char));
    memcpy(owd, pwd, strlen(pwd));
    prompt[0] = ' ';
    prompt[1] = '\0';

    /* Put PATH into a linked list */
    pathlist = get_path();

    printf("\nWelcome to the remenheiser shell!!!\n");
    while (go) {  //ALL TODO
        /* print your prompt */
        printf("\n[%s] > ", pwd);
        /* get command line and process */
        readInput(commandline);
        args = stringToArray(commandline);
        //-----------EXIT---------------------------------------------------------------------------------
        if (strcmp(args[0], "exit") == 0) {
            go = 0;  //run = false
            printf("\nExiting remenheiser shell...\n");
            /* check for each built in command and implement */
        }
        //-----------PWD----------------------------------------------------------------------------------
        else if (strcmp(args[0], "pwd") == 0) {  // Print working directory
            pwd = getcwd(*args, BUFFERSIZE);
            printf("%s\n", pwd);
            //free(pwd);
        }
        //-----------CD-----------------------------------------------------------------------------------
        else if (strcmp(args[0], "cd") == 0) {
            //break into cases based on args[1]
            if (strcmp(args[1], "/") == 0) {  //Works
                chdir("/");
                pwd = getcwd(args[1], BUFFERSIZE);
                //free(pwd);
            } else if (strcmp(args[1], ".") == 0) {  //Works
                chdir(".");
                pwd = getcwd(*args, 2 * BUFFERSIZE);
                //free(pwd);
            } else if (strcmp(args[1], "..") == 0) {  //Works
                chdir("..");
                pwd = getcwd(args[1], 2 * BUFFERSIZE);
                //free(pwd);
            } else if (strcmp(args[1], "home") == 0) {  //Why can't this work with ""? Ask silber, space leads to seg fault
                chdir("/Users");
                pwd = getcwd(args[1], BUFFERSIZE);
                //free(pwd);
            } else if (strcmp(args[1], "") == 0) {  //Does not work yet
                printf(
                    "\ntype "
                    "cd home"
                    " to go to /Users\n");
            } else if (chdir(args[1]) == 0) {  //Works
                pwd = getcwd(*args, 2 * BUFFERSIZE);
                //free(pwd);
            } else {
                printf("\nNo such directory exists\n");
            }
        }
        //-----------LS----------------------------------------------------------------------------------
        else if (strcmp(args[0], "list") == 0) {  //Almost works
            if (sizeof(args[1]) != 0) {
                chdir(args[1]);
                pwd = getcwd(args[1], 2 * BUFFERSIZE);
                list(pwd);
                //free(pwd);
            } else {
                list(pwd);
            }
        }
        //----------PID----------------------------------------------------------------------------------
        else if (strcmp(args[0], "pid") == 0) {
            printf("\nPID of remenheiser shell: %ld", (long)getpid());
        }
        //----------WHERE--------------------------------------------------------------------------------
        else if (strcmp(args[0], "where") == 0) {  //does not work yet
            if (args[1] != NULL) {
                where(args[1], pathlist);
            }
        }
        //----------WHICH--------------------------------------------------------------------------------
        else if (strcmp(args[0], "which") == 0) {  //does not work yet
            which(args[1], pathlist);
        }

        //check for built in commands like exit, use extra if elses
        else {
            char *absPath = where(args[0], pathlist);
            if (absPath == NULL) {
                printf("Command not found: %s\n", args[0]);
            } else {
                int pid = fork();
                if (pid == 0) {  //child process
                                 /*  else  program to exec */
                    execve(absPath, args, envp);
                    printf("ERROR");  //Code should never reach here! If it does there is a problem!
                    exit(0);
                } else {  //parent process
                    waitpid(pid, NULL, 0);
                }
            }

            /* find it */
            /* do fork(), execve() and waitpid() */

            /* else */
            /* fprintf(stderr, "%s: Command not found.\n", args[0]); */
        }
    }
    freeList(pathlist);
    //free(args); not freed properly
    for (int i = 0; i < sizeof(args); i += 8) {
        free(args[i]);
    }
    free(args);
    free(owd);
    free(pwd);
    free(commandline);
    free(prompt);
    return 0;
} /* sh() */

void freeList(struct pathelement *pathlist) {
    struct pathelement *head;

    while (pathlist != NULL) {
        head = pathlist;
        pathlist = pathlist->next;
        free(head);
    }
    free(pathlist);
}

//readInput() reads standard input and puts it into a buffer. Helps allow strings with spaces
char readInput(char *buffer) {
    char length = 0;
    if (fgets(buffer, BUFFERSIZE, stdin) != 0) {
        length = strlen(buffer);
        buffer[length - 1] = '\0';
    }
    return length;
}

char **stringToArray(char *input) {
    // make a copy of array
    char buff[BUFFERSIZE] = "";
    strcpy(buff, input);
    char *t = strtok(buff, " ");  // this returns the first word (if empty string it returns NULL)
    int count = 0;

    // call strtok over and over until you determine how long it is
    while (strtok(NULL, " ")) {
        count++;
    }

    char **argv = malloc((count + 1) * sizeof(char *));
    argv[count] = NULL;

    count = 0;
    strcpy(buff, input);
    t = strtok(buff, " ");

    while (t) {
        int len = strlen(t);
        argv[count] = (char *)malloc((len + 1) * sizeof(char *));
        strcpy(argv[count], t);
        count++;
        t = strtok(NULL, " ");
    }
    return argv;
}

char *which(char *command, struct pathelement *pathlist) {  //TODO
  /* loop through pathlist until finding command and return it.  Return
   NULL when not found. */
    printf("Executing Built-In command: [which]\n");
    while (pathlist) {  // WHICH
        sprintf(command, "%s/gcc", pathlist->element);
        if (access(command, X_OK) == 0) {
            printf("[%s]\n", command);
            break;
        }
        pathlist = pathlist->next;
    }
    return NULL;
} /* which() */

char *where(char *command, struct pathelement *pathlist) {  //TODO
    // /* similarly loop through finding all locations of command */
    printf("Executing Built-In command: [where]\n");
    while (pathlist) {  // WHERE
        sprintf(command, "%s/gcc", pathlist->element);
        if (access(command, F_OK) == 0) {
            //printf("%s", pathlist->element);
            printf("[%s]\n", command);
        }
        pathlist = pathlist->next;
    }
    return NULL;
} /* where() */

void list(char *dir) {  //TODO
    /* see man page for opendir() and readdir() and print out filenames for
  the directory passed */
    DIR *directory = opendir(dir);
    struct dirent *direntp;

    if (directory == NULL) {
        printf("\nDirectory does not exist\n");
    } else {
        for (;;) {
            direntp = readdir(directory);
            if (direntp == NULL)
                break;

            printf("\n%s", direntp->d_name);
        }
        closedir(directory);
    }
} /* list() */
