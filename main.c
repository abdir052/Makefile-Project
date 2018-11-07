/* CSci4061 Fall2018 Assignment 1
* login: alixx577
* date: 10.04.18
* name: Ali Adam, Abdirahman Abdirahman, Hassan Ali
* id: 5330610, 5330419, 4570634 */



// This is the main file for the code

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "util.h"

/*-------------------------------------------------------HELPER FUNCTIONS PROTOTYPES---------------------------------*/

void show_error_message(char * ExecName);
void Build(char * T);
void show_targets(target_t targets[], int nTargetCount);
void fork_exec(int x);

/*-------------------------------------------------------END OF HELPER FUNCTIONS PROTOTYPES--------------------------*/

target_t targets[MAX_NODES];

int nTargetCount = 0;

/* Variables you'll want to use */

char Makefile[64] = "Makefile";
char TargetName[64];
char* args [ARG_MAX];
int y;
char* s = " ";
int upToDate = TRUE;
int timeStamp;
int fileExist;

/*-------------------------------------------------------HELPER FUNCTIONS--------------------------------------------*/



//This is the function for writing an error to the stream

//It prints the same message in all the cases

void show_error_message(char * ExecName)
{
    fprintf(stderr, "Usage: %s [options] [target] : only single target is allowed.\n", ExecName);
    fprintf(stderr, "-f FILE\t\tRead FILE as a makefile.\n");
    fprintf(stderr, "-h\t\tPrint this message and exit.\n");
    exit(0);
}



//Write your functions here



//Phase1: Warmup phase for parsing the structure here. Do it as per the PDF (Writeup)

void show_targets(target_t targets[], int nTargetCount)
{
    //Write your warmup code here
    int i = 0;
    for(i=0;i<nTargetCount;i++){
        printf("TargetName: %s\n",targets[i].TargetName);
        printf("DependencyCount: %d\n",targets[i].DependencyCount);
        int j;

        printf("DependencyNames: ");
        for(j=0;j<targets[i].DependencyCount;j++){
            printf("%s ",targets[i].DependencyNames[j]);
        }
        printf("\n");
        printf("Command: %s\n",targets[i].Command);\
        printf("Status: %d\n",targets[i].Status);
        printf("\n");
    }
}

//Phase2:

void Build(char   * T){
    int x = find_target(T,targets,nTargetCount);             // Sets x to return value of find target
    if(x == -1 ){
        fileExist = does_file_exist(targets[x].TargetName); // functions returns -1 if there is an error
        if(fileExist == -1) return;
    }
    if (targets[x].DependencyCount == 0){             // If target has no children, will change flag to finished

        if(targets[x].Status == UNFINISHED){
            targets[x].Status = FINISHED;

            fork_exec(x);                              // forks and executes target
        }
        return;
    }

    int i;

    for(i=0;i<targets[x].DependencyCount;i++){        // for each dependent target Ti that needs to be built

        Build(targets[x].DependencyNames[i]);         // Recursive call of build

        fileExist = does_file_exist(targets[x].TargetName); // functions returns -1 if there is an error

        int z = find_target(targets[x].DependencyNames[i+1],targets,nTargetCount); // Sets z to return value of find target

        if(fileExist == -1 && z != -1){

            Build(targets[x].DependencyNames[i+1]);   // Recursive call build if file exists and target exists
            continue;
        }

        if(fileExist==-1)
            break;

        timeStamp = compare_modification_time(targets[x].TargetName, targets[x].DependencyNames[i]);

        if (NEEDS_BUILDING==timeStamp){                // checks to see if target is modified earlier than dependency
            break;
        }else{
            continue;
        }
    }



    if(timeStamp == NEEDS_BUILDING && targets[x].Status == UNFINISHED  || fileExist == -1){  // checks if Target is modified later then Dependency

        targets[x].Status = FINISHED;

        fork_exec(x); // forks and executes target

    }

    return;

}



void fork_exec(int x){ // forks and executes target takes argument x which is the return value of find target

    upToDate=FALSE;
    pid_t pid;
    int wstatus;

    pid = fork();

    if (pid>0){    //parent

        pid = wait(&wstatus);
        if (WEXITSTATUS(wstatus) != 0){
            printf("child exited with error code=%d\n", WEXITSTATUS(wstatus));
            exit(-1);
        }

    }else if (pid==0){  // child

        printf("%s\n", targets[x].Command);

        y = parse_into_tokens(targets[x].Command,args,s);
        targets[x].Status= FINISHED;

        execvp(args[0],args);        //execute the command for T
        fprintf(stderr, "Error, did not execute");
        exit(-1);

    }else{            //error handling
        perror("Fork problem");
        exit(-1);
    }
}

/*-------------------------------------------------------END OF HELPER FUNCTIONS-------------------------------------*/


/*-------------------------------------------------------MAIN PROGRAM------------------------------------------------*/

//Main commencement
int main(int argc, char *argv[])
{
    /* Declarations for getopt. For better understanding of the function use the man command i.e. "man getopt" */

    extern int optind;           // It is the index of the next element of the argv[] that is going to be processed
    extern char * optarg;        // It points to the option argument
    int ch;
    char *format = "f:h";
    char *temp;

    //Getopt function is used to access the command line arguments. However there can be arguments which may or may not need the parameters after the command
    //Example -f <filename> needs a finename, and therefore we need to give a colon after that sort of argument
    //Ex. f: for h there won't be any argument hence we are not going to do the same for h, hence "f:h"

    while((ch = getopt(argc, argv, format)) != -1)
    {
        switch(ch)
        {
            case 'f':
                temp = strdup(optarg);
                strcpy(Makefile, temp);  // here the strdup returns a string and that is later copied using the strcpy
                free(temp);    //need to manually free the pointer
                break;

            case 'h':
            default:
                show_error_message(argv[0]);
                exit(1);
        }
    }

    argc -= optind;
    if(argc > 1)   //Means that we are giving more than 1 target which is not accepted
    {
        show_error_message(argv[0]);
        return -1;   //This line is not needed
    }

    /* Init Targets */
    memset(targets, 0, sizeof(targets));   //initialize all the nodes first, just to avoid the valgrind checks

    /* Parse graph file or die, This is the main function to perform the toplogical sort and hence populate the structure */
    if((nTargetCount = parse(Makefile, targets)) == -1)  //here the parser returns the starting address of the array of the structure. Here we gave the makefile and then it just does the parsing of the makefile and then it has created array of the nodes
        return -1;

    //Phase1: Warmup-----------------------------------------------------------------------------------------------------
    //Parse the structure elements and print them as mentioned in the Project Writeup
    /* Comment out the following line before Phase2 */
    //show_targets(targets, nTargetCount);
    //End of Warmup------------------------------------------------------------------------------------------------------

    /*
     * Set Targetname
     * If target is not set, set it to default (first target from makefile)
     */

    if(argc == 1)

        strcpy(TargetName, argv[optind]);    // here we have the given target, this acts as a method to begin the building
    else

        strcpy(TargetName, targets[0].TargetName);  // default part is the first target

    /*

     * Now, the file has been parsed and the targets have been named.
     * You'll now want to check all dependencies (whether they are
     * available targets or files) and then execute the target that
     * was specified on the command line, along with their dependencies,
     * etc. Else if no target is mentioned then build the first target
     * found in Makefile.
     */

    //Phase2: Begins ----------------------------------------------------------------------------------------------------
    /*Your code begins here*/

    Build(TargetName);
    if (upToDate == TRUE){
        printf("'%s' is up to date\n",TargetName);
    }

    /*End of your code*/
    //End of Phase2------------------------------------------------------------------------------------------------------
    return 0;
}
/*-------------------------------------------------------END OF MAIN PROGRAM------------------------------------------*/
