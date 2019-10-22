#include "sh.h"
#include <dirent.h>
#include <limits.h>
#include <pwd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wordexp.h>

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
    while (go) {
        printf("%s[%s] > ", prompt, pwd);
        /* get command line and process */
        readInput(commandline);
        if (commandline == "\0") {
            //Do nothing
            printf("\n");
            continue;
        }

        args = stringToArray(commandline, argv, &argsct);

        //-----------RETURN-------------------------------------------------------------------------------
        //Catches if user presses enter
        if (commandline[0] == '\0') {
            //Do Nothing
            free(args);
        }
        //-----------EXIT---------------------------------------------------------------------------------
        else if (strcmp(args[0], "exit") == 0) {  //frees allocated memory
            go = 0;                               //run = false
            printf("\nExiting remenheiser shell...\n");
            free(args[0]);
            free(args);
            free(owd);
            free(pwd);
            freeList(pathlist);
            free(commandline);
            free(prompt);
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
        //-----------LIST--------------------------------------------------------------------------------
        else if (strcmp(args[0], "list") == 0) {  //works
            printf("Executing Built-In command: [list]\n");
            if (argsct == 1) {  //list is only argument
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
        else if (strcmp(args[0], "where") == 0) {  //works
            if (argsct == 2) {
                char *result = where(args[1], pathlist);
                free(result);
                free(args[1]);
            }
            free(args[0]);
            free(args);
        }
        //----------WHICH--------------------------------------------------------------------------------
        else if (strcmp(args[0], "which") == 0) {  //works
            if (argsct == 2) {
                char *result = which(args[1], pathlist);
                free(result);
                free(args[1]);
            }
            free(args[0]);
            free(args);
        }
        //----------KILL----------------------------------------------------------------------------------
        else if (strcmp(args[0], "kill") == 0) {
            printf("Executing Built-In command: [kill]\n");
            if (argsct < 2) {  //kill is either only element or kill without a signal or command following it
                               //Do Nothing
                free(args[1]);
                free(args[0]);
                free(args);
                printf("kill: Too few arguments.\n");
            } else {
                int shellPid = getpid();
                int signal = SIGTERM;
                int startIndex = 1;

                if (args[1][0] == '-') {
                    startIndex = 2;
                    signal = atoi(args[1]) * -1;
                }

                for (startIndex; startIndex < argsct; startIndex++) {
                    int tempPid = atoi(args[startIndex]);

                    if (shellPid == tempPid) {
                        go = 0;
                        free(args[2]);
                        free(args[1]);
                        free(args[0]);
                        free(args);
                        free(pwd);
                        freeList(pathlist);
                        free(owd);
                        free(commandline);
                        free(prompt);
                        kill(shellPid, signal);
                    }
                    kill(tempPid, signal);
                }
                free(args[2]);
                free(args[1]);
                free(args[0]);
                free(args);
            }
        }
        //---------PROMPT--------------------------------------------------------------------------------
        else if (strcmp(args[0], "prompt") == 0) {
            printf("Executing Built-In command: [prompt]\n");
            if (argsct == 2) {  //if prompt then a prefix follows as the second argument
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
        //--------PRINTENV------------------------------------------------------------------------------
        else if (strcmp(args[0], "printenv") == 0) {
            printf("Executing Built-In command: [printenv]\n");
            if (argsct == 1) {
                while (*envp) {
                    printf("%s\n", *envp++);
                }
            } else if (argsct == 2 && getenv(args[1]) != NULL) {
                char *result = getenv(args[1]);
                free(args[1]);
                printf("%s\n", result);
            } else {
                printf("Not a valid envp variable\n");
                free(args[1]);
            }
            free(args[0]);
            free(args);
        }
        //--------SETENV--------------------------------------------------------------------------------
        else if (strcmp(args[0], "setenv") == 0) {
            printf("Executing Built-In command: [setenv]\n");
            if (argsct == 1) {
                while (*envp) {
                    printf("%s\n", *envp++);
                }
            } else if (argsct == 2) {
                setenv(args[1], "", 1);
                free(args[1]);
            } else if (argsct == 3) {
                setenv(args[1], args[0], 1);
                free(args[1]);
            } else {
                printf("setenv: Too many arugments\n");
                for (int i = 1; i < argsct; i++) {
                    free(args[i]);
                }
            }
            free(args[0]);
            free(args);
        }

        //--------RUN-PROGRAMS--------------------------------------------------------------------------
        //Implements wordexp to catch wildcards * and ?. Side affect is that after the command with wildcards is run, it will say after the command was not found but it still works.
        else {
            char *absPath = where(args[0], pathlist);
            for (int i = 0; i < argsct; i++) {
                if (strchr(args[i], '*') != NULL || (strchr(args[i], '?') != NULL)) {
                    // printf("TEST: %s\n", strchr(*args, '*'));
                    int wildIndex = findWildIndex(args, argsct);
                    wordexp_t p;
                    char **word;
                    int position = 0;
                    wordexp(args[wildIndex], &p, 0);
                    word = p.we_wordv;
                    for (position = 0; position < p.we_wordc; position++) {
                        printf("%s\n", word[position]);
                    }
                    wordfree(&p);
                }
            }

            printf("Executing user entered command: [%s]\n ", args[0]);
            //printf("COUNT: %d\n", argsct);
            if (absPath == NULL) {
                printf("Command not found: %s\n", args[0]);
                free(absPath);
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
            free(absPath);
            for (int i = 0; i < argsct; i++) {
                free(args[i]);
            }
            free(args);
        }
    }
    return 0;
} /* sh() */

//findWildIndex() is a helper function used to determine the index where the wildcard appears in args.
int findWildIndex(char **args, int argsct) {
    for (int i = 1; i < argsct; i++) {
        for (int j = 0; j < strlen(args[i]); j++) {
            if ((args[i][j] == '*') || (args[i][j] == '?')) {
                return i;
            }
        }
    }
    return 0;
}

//freeList() is a helper function that loops through the pathlist in order to free each node to prevent memory leaks.
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

//stringToArray() is used to parse the commandline input and convert into **args which uses strtok to break up the string
//side affects involve allocating memory which needs to be freed elsewhere.
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
    argv = malloc((count + 2) * sizeof(char *));
    argv[count + 1] = NULL;

    count = 0;
    strcpy(buff, input);
    t = strtok(buff, " ");

    while (t) {
        int len = strlen(t);
        argv[count] = malloc((len + 1) * sizeof(char));
        strcpy(argv[count], t);
        count++;
        *argsCount = count;
        t = strtok(NULL, " ");
    }
    return argv;
}

//which() is used to show the first instance of a command found in the linked list pathlist.
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
                return cmd;
            } else if (access(cmd, F_OK) != 0 && tempPath->next == NULL) {
                free(cmd);
                break;
            }
            tempPath = tempPath->next;
        }
    }
    return NULL;
} /* which() */

//where() is used to show every instance of a command found in the linked list pathlist. For the sake of this shell,
//which and where do the same thing.
char *where(char *command, struct pathelement *pathlist) {
    printf("Executing Built-In command: [where]\n");
    char *cmd = malloc(1024 * sizeof(char *));
    struct pathelement *tempPath = pathlist;
    strcpy(cmd, command);
    while (pathlist) {  // WHERE
        if (command != NULL) {
            if (*command == '/' || *command == '.') {
                if (access(command, X_OK) == 0) {
                   // char *returnStr = calloc(strlen(command) + 1, sizeof(char));
                    strncpy(cmd, command, strlen(command));
                    return cmd;
                } else {
                    free(cmd);
                    return NULL;
                }
            } else {
                sprintf(cmd, "%s/%s", tempPath->element, command);
                if (access(cmd, F_OK) == 0) {
                    printf("%s\n", cmd);
                    return cmd;
                } else if (access(cmd, F_OK) != 0 && tempPath->next == NULL) {
                    free(cmd);
                    break;
                }
                tempPath = tempPath->next;
            }
        }
    }
    return NULL;
} /* where() */

//list() is a function which is meant to clone ls.
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
