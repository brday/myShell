//
//  lab1.cpp
//  lab1_3453
//
//  Created by Brad Day on 9/18/13.
//  Copyright (c) 2013 Brad Day. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>    //need for fork()
#include <sys/wait.h>
#include <stdlib.h>
#include <iostream>
//#include <errno.h>

#define MAX_LINE 80         // The maximum length command


// Reads in the next command line, seperating it into distinct tokens using whitespace 
// as delimiters. Sets the args parameter as a null-terminated string.
void setupCmdLine(char[], char*[], int*);
int changeDir(char* );

void  executeProcess(char*[]);

int main(int argc, char *argv[]) {
    char inputBuffer[MAX_LINE];     // Buffer to hold the command entered
    char* args[MAX_LINE/2+1];       // Command line argruments
    int should_run = 1;             // Flag to determine when to exit program
    int background;                 // Equals 1 if a command is followed by '&'
    
    while (1) {
        background = 0;
        printf("myShell>");
        fflush(stdout);
        setupCmdLine(inputBuffer, args, &background);      // Get next command
        
        executeProcess(args);
        
        // After reading user input, steps are:
        // (1) fork a child process using fork()
        // (2) the child process will invoke ececvp()
        // (3) if command inluded &, parent will invoke wait()
        
        
    }
    
    return 0;
}

void setupCmdLine(char inputBuffer[], char* args[], int* background)
{
    int length, start, index;
    
    // read what the user enters on the command line into inputBuffer
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
    start = -1;
    index = 0;

    if (length == 0) {
        exit(0);                // ^d was entered, end of user command stream
    }
    
    if (length < 0) {
        perror("error reading the command");
        exit(-1);               // Terminate with error code of -1
    }
    
    // Examine every character in the inputBuffer
    for (int i = 0; i < length; i++) {
        switch (inputBuffer[i]) {
            case ' ':
            case '\t':                              // Argument seperators
                if (start != -1) {
                    args[index] = &inputBuffer[start];     // Set up pointer
                    index++;
                }
                inputBuffer[i] = '\0';      // Add null char; make a C string
                start = -1;
                break;
                
            case '\n':                      // Should be the final char examined
                if (start != -1) {
                    args[index] = &inputBuffer[start];
                    index++;
                }
                inputBuffer [i] = '\0';
                args[index] = NULL;            // No more arguments to this command
                break;
                
            case '&':
                *background = 1;
                inputBuffer[i] = '\0';
                break;
                
                
            default:                    // Some other character
                if (start == -1) {
                    start = i;
                }
                
        } // End switch
    } // End for loop
    args[index] = NULL;                    // Just incase input line was > 80
}

void  executeProcess(char **args)
{
    pid_t  pid;
    int    status;
    
    if (strcmp(args[0], "exit") == 0) { // Exit the program if user input exit
        exit(0);
    }
    
    /*  fork a child process  */
    pid = fork();
    
    if (pid < 0) {
        printf("*** ERROR: forking child process failed\n");
        exit(1);
    }
    else if (pid == 0) {          // for the child process:
        
        if (strcmp(args[0], "cd") == 0) {
            changeDir(args[1]);
        }
        printf("im in child\n");            // Test
        if (execvp(*args, args) < 0) {     /* execute the command  */
            printf("*** ERROR: exec failed\n");
            exit(1);
        }
    }
    else {                                  /* for the parent:      */
        while (wait(&status) != pid)       /* wait for completion  */
            ;
    }
}
int changeDir(char* path)
{
    long size = pathconf(".", _PC_PATH_MAX);
    char* buf = (char*)malloc((size_t) size);
    
    char* ptr = getcwd(buf, (size_t)size);
    
    
    chdir(path);
}