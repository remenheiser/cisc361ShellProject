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
    char **args;
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
        args = stringToArray(commandline, argv, &argsct);
        //-----------RETURN-------------------------------------------------------------------------------
        if (args[0] == '\0') {
            //Do Nothing
            free(args);
        }
        //-----------EXIT---------------------------------------------------------------------------------
        else if (strcmp(args[0], "exit") == 0) {
            go = 0;  //run = false
            printf("\nExiting remenheiser shell...\n");
            free(args[0]);
            free(args);
            free(owd);
            free(pwd);
            /* check for each built in command and implement */
        }
        //-----------PWD----------------------------------------------------------------------------------
        else if (strcmp(args[0], "pwd") == 0) {  // Print working directory
            printf("[%s]\n", pwd);
            free(args[0]);
            free(args);
        }
        //-----------CD-----------------------------------------------------------------------------------
        else if (strcmp(args[0], "cd") == 0) {
            //break into cases based on args[1]
            if (args[1] == NULL) {
                chdir("/usa");
                free(pwd);
                pwd = getcwd(args[1], BUFFERSIZE);
                free(args[0]);
            } else if (strcmp(args[1], "/") == 0) {  //Works
                chdir("/");
                free(pwd);
                pwd = getcwd(args[1], BUFFERSIZE);
                free(args[0]);
            } else if (strcmp(args[1], ".") == 0) {  //Works
                chdir(".");
                free(args[1]);
                free(args[0]);
            } else if (strcmp(args[1], "..") == 0) {  //Works
                chdir("..");
                free(pwd);
                pwd = getcwd(args[1], 2 * BUFFERSIZE);
                free(args[0]);
            } else if (chdir(args[1]) == 0) {  //Works
                free(pwd);
                pwd = getcwd(*args, 2 * BUFFERSIZE);
                free(args[1]);
            } else {
                printf("\nNo such directory exists\n");
                free(args[1]);
            }
            free(args);
        }
        //-----------LS----------------------------------------------------------------------------------
        else if (strcmp(args[0], "list") == 0) {  //Almost works
            if (args[1] == NULL) {
                list(pwd);
                free(args[0]);
                free(args);
            } else {
                free(pwd);
                char *tempPwd = pwd;
                if (chdir(args[1]) == 0) {
                    pwd = getcwd(args[1], 2 * BUFFERSIZE);
                    list(pwd);
                    chdir(tempPwd);
                    pwd = getcwd(args[1], 2 * BUFFERSIZE);
                } else {
                    list(tempPwd);
                }
                free(args[0]);
                free(args);
            }
        }
        //----------PID----------------------------------------------------------------------------------
        else if (strcmp(args[0], "pid") == 0) {
            printf("\nPID of remenheiser shell: %ld", (long)getpid());
            free(args[0]);
            free(args);
        }
        //----------WHERE--------------------------------------------------------------------------------
        else if (strcmp(args[0], "where") == 0) {  //does not work yet
            if (args[1] == NULL) {
                where(NULL, pathlist);
                free(args[0]);
                free(args);
            } else {
                where(args[1], pathlist);
                free(args[1]);
                free(args[0]);
                free(args);
            }
        }
        //----------WHICH--------------------------------------------------------------------------------
        else if (strcmp(args[0], "which") == 0) {  //does not work yet
            if (args[1] == NULL) {
                which(NULL, pathlist);
                free(args[0]);
                free(args);
            } else {
                which(args[1], pathlist);
                free(args[1]);
                free(args[0]);
                free(args);
            }
        }
        //----------KILL----------------------------------------------------------------------------------
        else if (strcmp(args[0], "kill") == 0) {
            if (args[1] == NULL) {
                free(args[0]);
                free(args);
                free(pwd);
                freeList(pathlist);
                free(owd);
                free(commandline);
                free(prompt);
                kill(pid, SIGTERM);
            } else {
                pid_t temp = (long) args[1];
                kill(temp, SIGTERM);
                free(args[1]);
                free(args[0]);
                free(args);
            }
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
                    pid = waitpid(pid, NULL, 0);
                }
            }
        }
    }
    freeList(pathlist);
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

char **stringToArray(char *input, char **argv, int *argsCount) {
    // make a copy of array
    char buff[BUFFERSIZE] = "";
    strcpy(buff, input);
    char *t = strtok(buff, " ");  // this returns the first word (if empty string it returns NULL)
    int count = 0;

    // call strtok over and over until you determine how long it is
    while (strtok(NULL, " ")) {
        count++;
    }
    argv = malloc((count + 1) * sizeof(char *));
    argv[count] = NULL;

    count = 0;
    strcpy(buff, input);
    t = strtok(buff, " ");

    while (t) {
        int len = strlen(t);
        argv[count] = (char *)malloc((len + 1) * sizeof(char *));
        strcpy(argv[count], t);
        count++;
        *argsCount = count;
        t = strtok(NULL, " ");
    }
    return argv;
}

char *which(char *command, struct pathelement *pathlist) {                                         
    printf("Executing Built-In command: [which]\n");
    while (pathlist) {  
        if (command != NULL) {
            sprintf(command, "%s/gcc", pathlist->element);
        }
        if (access(command, X_OK) == 0) {
            printf("[%s]\n", command);
            break;
        }
        pathlist = pathlist->next;
    }
    return NULL;
} /* which() */

char *where(char *command, struct pathelement *pathlist) {  
    printf("Executing Built-In command: [where]\n");
    while (pathlist) {  // WHERE
        if (command != NULL) {
            sprintf(command, "%s/gcc", pathlist->element);
        }
        if (access(command, F_OK) == 0) {
            //printf("%s", pathlist->element);
            printf("[%s]\n", command);
        }
        pathlist = pathlist->next;
    }
    return NULL;
} /* where() */

void list(char *dir) { 
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
