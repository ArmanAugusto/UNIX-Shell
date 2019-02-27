/*
 * Author:          Arman Augusto
 * ID:              #004588816
 * Course:          CS 433 - Operating Systems (Tue/Thur 9:00am-10:15am)
 * Instructor:      Dr. Xiaoyu Zhang
 * Assignment:      Programming Assignment 2 - UNIX Shell
 * Description:     osh> UNIX shell with an included history feature.
 *                  Enter '!!' command for history feature
 *                  Enter 'exit' or CTRL + c to exit the program
 * 
 * Created on Tuesday, February 27, 2019
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 80 /* The maximum length of a command */
#define BUFFER_SIZE 50
#define buffer "\nShell Command History:\n"

//declarations
char history[10][BUFFER_SIZE]; // array to store history commands
int count = 0;

/*
 * Functions utilized in the program 
 */
void displayHist(); //Displays the command history
int formatCmd(char inputBuffer[], char *args[],int *flag); // formats command

/******************************************************************************/
/* main driver ****************************************************************/
int main(void) {
    printf("Author:         Arman Augusto\n");
    printf("ID:             #004588816\n");
    printf("Course:         CS 433 - Operating Systems (Tue/Thur 9:00am-10:15am)\n");
    printf("Instructor:     Dr. Xiaoyu Zhang\n");
    printf("Assignment:     Programming Assignment 2 - UNIX Shell\n");
    printf("\nCreated on Tuesday, February 27, 2019\n");
    
    char inputBuffer[MAX_LINE]; /* buffer to hold the input command */
    int flag; // equals 1 if a command is followed by "&"
    char *args[MAX_LINE/2 + 1]; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    
    pid_t pid, tpid;
    int i;
   
    while (should_run) { //infinite loop for shell prompt while should_run = 1
        flag = 0; // default value for flag is 0
        printf("osh>"); // prompt
        fflush(stdout);
        if(-1!=formatCmd(inputBuffer,args,&flag)) { // get next command
            pid = fork();
            if (pid < 0) { // if pid is less than 0, forking fails
                printf("Fork failed.\n");
            	exit (1);
            }
            else if (pid == 0) { // if pid == 0
                // indicate that the command was not executed
            	if (execvp(args[0], args) == -1) {
                    printf("Error executing command\n");
            	}
            } else {
                i++;
           	if (flag == 0) { // if flag == 0, the parent will wait
                    i++;
                    wait(NULL);
           	}
            }
   	}
    }
    return 0;
} /* end main */
/******************************************************************************/
/******************************************************************************/

/* Function Definitions */

/* 
 * displayHist function
 * 
 * Displays the command history
 * 
 * parameters: none
 */
void displayHist() {
    printf("Shell command history:\n");
    
    int i;
    int j = 0;
    int histCount = count;
    
    // Iterate through commands
    for (i = 0; i < 10; i++) {
        // command index
        printf("%d.  ", histCount);
        while (history[i][j] != '\n' && history[i][j] != '\0') {
            // print command(s) in history
            printf("%c", history[i][j]);
            j++;
        }
        printf("\n");
        j = 0;
        histCount--;
        if (histCount ==  0)
            break;
    }
    printf("\n");
} /* end displayHist */

/* 
 * formatCmd function
 * 
 * Gets the command from the shell and tokenizes it,
 * then sets the args parameter.
 * 
 * parameters: char inputBuffer[], char *args[], int *flag
 */
int formatCmd(char inputBuffer[], char *args[],int *flag) {
    int length; // # of chars in command line
    int i;     // loop index for inputBuffer
    int start;  // index of beginning of next command
    int ct = 0; // index of where to place the next parameter into args[]
    int hist;
    	
    // Read user input on command line and check whether the command is !!
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);	
   
    start = -1;
    if (length == 0)
        exit(0);   //end of command
    if (length < 0)
    {
        printf("Command not read\n");
        exit(-1);  //terminate
    }
    
   // check each character
    for (i = 0; i < length; i++) {
        switch (inputBuffer[i]) {
            case ' ':
            case '\t': 
                if(start != -1) {
                    args[ct] = &inputBuffer[start];    
                    ct++;
                }
                inputBuffer[i] = '\0'; // adds a null char at the end
                start = -1;
                break;
            case '\n':
                if (start != -1) {
                    args[ct] = &inputBuffer[start];
                    ct++;
                }
                inputBuffer[i] = '\0';
                args[ct] = NULL;
                break;          
            default :           
                if (start == -1)
                    start = i;
                if (inputBuffer[i] == '&') {
                    *flag  = 1; // flag indicating the child process invoked in the background
                    inputBuffer[i] = '\0';
                }
        }
    }
    
    args[ct] = NULL; //if the input line was > 80
    
    if(strcmp(args[0],"exit")==0) {
    exit(0);
    }

    if(strcmp(args[0],"!!") == 0) {
        if(count>0) {
            displayHist();
	} else {
            printf("\nNo Commands in the history\n");
	}
	return -1;
    } 
    else if (args[0][0]-'!' ==0) {
        int x = args[0][1]- '0'; 
	int z = args[0][2]- '0'; 
		
	if(x>count) { //second letter check
            printf("\nNo Such Command in the history\n");
            strcpy(inputBuffer,"Wrong command");
	} 
	else if (z!=-48) { //third letter check 
            printf("\nNo Such Command in the history. Enter <=!9 (buffer size is 10 along with current command)\n");
            strcpy(inputBuffer,"Wrong command");
	} else {
            if(x==-15) { //Checking for '!!',ascii value of '!' is 33.
                strcpy(inputBuffer,history[0]);  // this will be your 10 th(last) command
            }
            else if(x==0) { //Checking for '!0'
                printf("Enter proper command");
		strcpy(inputBuffer,"Wrong command");
            }
			
            else if(x>=1) { //Checking for '!n', n >=1
                strcpy(inputBuffer,history[count-x]);
            }		
	}
    }
    for (i = 9;i>0; i--) { //Moving the history elements one step higher
       	strcpy(history[i], history[i-1]);
    }
    strcpy(history[0],inputBuffer); //Updating the history array with input buffer
    count++;
	if(count>10) {
            count=10;
	}
} /* end formatCmd */
