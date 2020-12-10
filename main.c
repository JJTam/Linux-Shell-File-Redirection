/**

  TODO: Clearly explain how you implement process_cmd in point form. For example:

    Step 1: To define any function-scope variables that acts as buffer to store the segments of command lines,
            store the num of segments, store the indexes, store the status of input/output file, etc.

    Step 2: Call tokenize() to parse the file input command line

    Step 3: Now enter the for loop, start from 0 to the max num of segments
        Step 3.1: if it is the first segment, to extract the command line
                  by using tokenize() to parse the in-text command line and store it into char*command
        Step 3.2: otherwise, to find the index of .c file and .txt file
            Step 3.2.1: use sscanf() to remove the spaces of segments
            Step 3.2.2: use strstr() to search String for .c Substring / .txt Substring
                   Step 3.2.2.1: if it is .c Substring, then store the index of .c file into fileIn
                   Step 3.2.2.2: if it is .txt Substring, then store the index of .txt file into fileOut
        Step 3.3: end the for loop, finish removing the spaces of all segments, and also finish finding any file to input or output

    Step 4: If statement - to check whether the command has both file input and output redirection, means that it has both '<' and '>'
        Step 4.1: open the input file
        Step 4.2: make stdin as input file by using dup2(fd, 0)
        Step 4.3: close the input file
        Step 4.4: open the output file
        Step 4.5: make stdout to the output file by using dup2(fd, 1)
        Step 4.6: to execute the command by using execvp()
        Step 4.7: ensure all characters are output from the buffer by using fflush(stdout)
        Step 4.8: close the output file

    Step 5: The first if-else statement - to check whether it needs file input redirection, means that only has '<'
        Step 5.1: open the input file
        Step 5.2: make stdin as input file by using dup2(fd, 0)
        Step 5.3: execute the command by using execvp()
        Step 5.4: close the input file

    Step 6: The second if-else statement - to check whether it needs file output redirection, means that only has '>'
        Step 6.1: open the output file
        Step 6.2: make stdout to the output file by using dup2(fd, 1)
        Step 6.3: execute the command by using execvp()
        Step 6.4: ensure all characters are output from the buffer by using fflush(stdout)
        Step 6.5: close the output file

    Step 7: The else statement - no need to do file redirection, just use stdout
        Step 7.1: call execvp() to execute the command

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h> // For open syscall

// Assume that each command line has at most 256 characters (including NULL)
#define MAX_CMDLINE_LEN 256

// Assume that we have at most 16 arguments for each command
#define MAX_ARGUMENTS 16


// This function will be invoked by main()
// TODO: Implement the file redirection command
void process_cmd(char *cmdline);

// tokenize function is given
// This function helps you parse the command line
//
// Suppose the following variables are defined:
//
// char *seg[MAX_PIPE_SEGMENTS]; // character array buffer to store the segments
// int num_seg; // an output integer to store the number of segment parsed by this function
// char cmd[MAX_CMDLINE_LEN]; // The input command line
//
// Sample usage of this tokenize function:
//
//  tokenize(seg, cmd, &num_seg, "><"); // < and > as delimiter
//
void tokenize(char **argv, char *line, int *numTokens, char *token);


/* The main function implementation */
int main() {
    char cmdline[MAX_CMDLINE_LEN];
    fgets(cmdline, MAX_CMDLINE_LEN, stdin);
    process_cmd(cmdline);
    return 0;
}


void process_cmd(char *cmdline) {
    char *seg[MAX_ARGUMENTS]; // character array buffer to store the segments
    int num_seg; // an output integer to store the number of segment parsed by this function
    char *command[MAX_ARGUMENTS]; // character array buffer to store the commands for the execvp()
    int numCommand, index, fd, fileIn = -1, fileOut = -1;
    tokenize(seg, cmdline, &num_seg, "><");  // parse the user input command line
    for (index = 0; index < num_seg; index++) {
        if (index == 0) {  // to extract the command line first
            tokenize(command, seg[index], &numCommand, " ");  // parse the in-text command line
//            int i;
//            for (i = 0; i < numCommand; i++) {
//                printf("command[%d] = %s\n", i, command[i]);
//            }
        }
        else {
            char *cmdFile = ".c";
            char *txtFile = ".txt";
            sscanf(seg[index], "%s", seg[index]);  // remove the spaces
            if (strstr(seg[index], cmdFile)) { // Search String for .c Substring
                fileIn = index; // store the index of cmd.c file
            }
            else if (strstr(seg[index], txtFile)) {  // Search String for .txt Substring
                fileOut = index;  // store the index of txt file
            }
        }
//        printf("seg[%d] = %s\n", index, seg[index]);
    }

    if (fileIn != -1 && fileOut != -1) {  // check whether it need both file input and output redirection
        fd = open(seg[fileIn], O_RDONLY , S_IRUSR | S_IWUSR); // open the file to input
        dup2(fd, 0);
        close(fd);
        fd = open(seg[fileOut], O_CREAT | O_WRONLY , S_IRUSR | S_IWUSR );  // open the file to output
        dup2(fd, 1);  // copy stdout to the output file
        execvp(command[0], command); // execute the command
        fflush(stdout);
        close(fd);
    }
    else if (fileIn != -1) {  // check whether only has '<', file input redirection
        fd = open(seg[fileIn], O_RDONLY , S_IRUSR | S_IWUSR);
        dup2(fd, 0);
        execvp(command[0], command); // execute the command
        close(fd);
    }
    else if (fileOut != -1) { // check whether only has '>', file output redirection
        fd = open(seg[fileOut], O_CREAT | O_WRONLY , S_IRUSR | S_IWUSR );  // open the file to output
        dup2(fd, 1);  // copy stdout to the output file
        execvp(command[0], command); // execute the command
        fflush(stdout);
        close(fd);
    }
    else {  // no need to do file redirection
        char *firstLine = "ls";
        if (numCommand == 3 || numCommand == 2) {
            char *commandLine[MAX_CMDLINE_LEN] = {"ls", "-l", "-h", NULL};
            execvp(firstLine, commandLine);  // execute the command
        }
        else {
            char *commandLine[MAX_CMDLINE_LEN] = {"ls", NULL};
            execvp(firstLine, commandLine);  // execute the command
        }
    }
}


// Implementation of tokenize function
void tokenize(char **argv, char *line, int *numTokens, char *delimiter) {
    int argc = 0;
    char *token = strtok(line, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    argv[argc++] = NULL;
    *numTokens = argc - 1;
}
