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
        printf("%s[%s] > ", prompt, pwd);
        /* get command line and process */
        readInput(commandline);
        if (commandline == NULL) {
            continue;
            printf("\n");
        } 
    
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
            freeList(pathlist);
            free(commandline);
            free(prompt);
            /* check for each built in command and implement */
        }
        //-----------PWD----------------------------------------------------------------------------------
        else if (strcmp(args[0], "pwd") == 0) {  // Print working directory
            printf("Executing Built-In command: [pwd]\n");
            printf("[%s]\n", pwd);
            free(args[0]);
            free(args);
        }
        //-----------CD-----------------------------------------------------------------------------------
        else if (strcmp(args[0], "cd") == 0) {
            char buff[BUFFERSIZE] = "";
            char *tempPwd = pwd;
            printf("Executing Built-In command: [cd]\n");
            //break into cases based on args[1]
            if (argsct == 1) {
                chdir("/usa");
                tempPwd = getcwd(buff, BUFFERSIZE);
                memcpy(pwd, tempPwd, strlen(tempPwd) + 1);
                free(args[0]);
            } else if (strcmp(args[1], "/") == 0) {  //Works
                chdir("/");
                tempPwd = getcwd(buff, BUFFERSIZE);
                memcpy(pwd, tempPwd, strlen(tempPwd) + 1);
                free(args[1]);
                free(args[0]);
            } else if (strcmp(args[1], ".") == 0) {  //Works
                chdir(".");
                free(args[1]);
                free(args[0]);
            } else if (strcmp(args[1], "..") == 0) {  //Works
                chdir("..");
                tempPwd = getcwd(buff, BUFFERSIZE);
                memcpy(pwd, tempPwd, strlen(tempPwd) + 1);
                free(args[1]);
                free(args[0]);
            } else if (chdir(args[1]) == 0) {  //Works
                tempPwd = getcwd(buff, BUFFERSIZE);
                memcpy(pwd, tempPwd, strlen(tempPwd) + 1);
                free(args[1]);
                free(args[0]);
            } else {
                printf("\nNo such directory exists\n");
                free(args[1]);
                free(args[0]);
            }
            free(args);
        }
        //-----------LS----------------------------------------------------------------------------------
        else if (strcmp(args[0], "list") == 0) {  //Almost works
            printf("Executing Built-In command: [list]\n");
            if (argsct == 1) {
                list(pwd);
                free(args[0]);
                free(args);
            } else {
                char *tempPwd = pwd;
                char buff[BUFFERSIZE] = "";
                if (chdir(args[1]) == 0) {
                    tempPwd = getcwd(buff, 2 * BUFFERSIZE);
                    list(tempPwd);
                    free(args[1]);
                } else {
                    printf("No such directory exists");
                    free(args[1]);
                }
                free(args[0]);
                free(args);
            }
        }
        //----------PID----------------------------------------------------------------------------------
        else if (strcmp(args[0], "pid") == 0) {
            printf("Executing Built-In command: [pid]\n");
            printf("PID of remenheiser shell: %ld\n", (long)getpid());
            free(args[0]);
            free(args);
        }
        //----------WHERE--------------------------------------------------------------------------------
        else if (strcmp(args[0], "where") == 0) {  //does not work yet
            if (argsct == 2) {
                where(args[1], pathlist);
                free(args[1]);
            }
            free(args[0]);
            free(args);
        }
        //----------WHICH--------------------------------------------------------------------------------
        else if (strcmp(args[0], "which") == 0) {  //does not work yet
            if (argsct == 2) {
                which(args[1], pathlist);
                free(args[1]);
            }
            free(args[0]);
            free(args);
        }
        //----------KILL----------------------------------------------------------------------------------
        else if (strcmp(args[0], "kill") == 0) {
            printf("Executing Built-In command: [kill]\n");
            if (argsct == 1) {
                free(args[0]);
                free(args);
                free(pwd);
                freeList(pathlist);
                free(owd);
                free(commandline);
                free(prompt);
                kill(pid, SIGTERM);
            } else {
                pid_t temp = (long)args[1];
                kill(temp, SIGTERM);
                free(args[1]);
                free(args[0]);
                free(args);
            }
        }
        //---------PROMPT--------------------------------------------------------------------------------
        else if (strcmp(args[0], "prompt") == 0) {
            printf("Executing Built-In command: [prompt]\n");
            if (argsct == 2) {
                memcpy(prompt, args[1], strlen(args[1]) + 1);
                free(args[1]);
            } else {
                char temp[BUFFERSIZE] = "";
                printf("Input prompt prefix: ");
                readInput(temp);
                memcpy(prompt, temp, strlen(temp) + 1);
                free(args[1]);
            }
            free(args[0]);
            free(args);
        }
        //--------RUN-PROGRAMS--------------------------------------------------------------------------

        //check for built in commands like exit, use extra if elses
        else {
            char *absPath = where(args[0], pathlist);
            // for (int i = 0; i < argsct; i++) {
            //     free(args[i]);
            // }
            //printf("ARGS COUNT: %d\n ", argsct);
            if (absPath == NULL) {
                printf("Command not found: %s\n", args[0]);
            } else {
                pid_t pid = fork();
                if (pid == 0) {  //child process
                    /*  else  program to exec */
                    execve(absPath, args, envp);
                    printf("ERROR\n");  //Code should never reach here! If it does there is a problem!
                    exit(0);
                } else {  //parent process
                    waitpid(pid, NULL, 0);
                }
            }
            for (int i = 0; i < argsct; i++) {
                free(args[i]);
            }
            free(args);
        }
    }
    // freeList(pathlist);
    // free(commandline);
    // free(prompt);
    return 0;
} /* sh() */

// void intHandler(int sig) {
//     signal(sig, SIGINT);

// }

void freeList(struct pathelement *pathlist) {
    struct pathelement *head;
    free(pathlist->element);
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
    char *cmd = malloc(1024 * sizeof(char *));
    struct pathelement *tempPath = pathlist;
    strcpy(cmd, command);
    while (pathlist) {  // WHICH, doing the same as where here for now
        if (command != NULL) {
            sprintf(cmd, "%s/%s", tempPath->element, command);
            if (access(cmd, F_OK) == 0) {
                printf("%s\n", cmd);
                char *temp = cmd;
                free(cmd);
                return temp;
            } else if(access(cmd, F_OK) != 0 && tempPath->next == NULL) {
                free(cmd);
                break;
            }
            tempPath = tempPath->next;
        } 
    }
    return NULL;
} /* which() */

char *where(char *command, struct pathelement *pathlist) {
    printf("Executing Built-In command: [where]\n");
    char *cmd = malloc(1024 * sizeof(char *));
    struct pathelement *tempPath = pathlist;
    strcpy(cmd, command);
    while (pathlist) {  // WHERE
        if (command != NULL) {
            sprintf(cmd, "%s/%s", tempPath->element, command);
            if (access(cmd, F_OK) == 0) {
                printf("%s\n", cmd);
                char *temp = cmd;
                free(cmd);
                return temp;
            } else if(access(cmd, F_OK) != 0 && tempPath->next == NULL) {
                free(cmd);
                break;
            }
             tempPath = tempPath->next;
        }
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
        printf("\n");
    }
} /* list() */
