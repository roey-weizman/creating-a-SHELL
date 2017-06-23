#include "LineParser.h"
#include <unistd.h>
#include <linux/limits.h>
#include <stdlib.h> 
#include <string.h>
#include <signal.h>

int  main(int argc,char** argv){
     char  myLine[2048];       int debugMode=0;  
     char  currDir[PATH_MAX];            
     cmdLine* myCmd;
     for(int i=0;i<argc;i++){
          if(strcmp(argv[i], "-d") == 0)
               debugMode=1;
     }
     while (1) { 
          getcwd(currDir, sizeof(currDir));                  /* repeat until done ....         */
          printf("#");
          printf(currDir);     /*   display a prompt             */
          printf("# ");
          gets(myLine);  
          printf("\n");
          myCmd=parseCmdLines(myLine);            /*   read in the command line     */
          if (strcmp(myCmd->arguments[0], "quit") == 0){  /* is it an "exit"?     */
               if(debugMode==1){
                int pid= getpid();
                debugPrint(pid,"quit");
                  }
                exit(0); 
               }       /*   exit if it is                */
          execute(myCmd,debugMode); 
          freeCmdLines(myCmd);          /* otherwise, execute the command */
     }
     return 0;
}

void  execute(cmdLine *pCmdLine,int debugMode){
     pid_t  pidNum;
     int    status;


     if ((pidNum = fork()) < 0) {     /* fork a child process           */
          perror("**fork() method failed\n");
          exit(1);

     }
     else if (pidNum == 0) {          /* for the child process:         */
          if (execvp (pCmdLine->arguments[0], pCmdLine->arguments) < 0) {     /* execute the command  */
               perror("** execv failed\n");
               exit(-1);
          }
          if(debugMode==1)
               debugPrint(pidNum,pCmdLine->arguments[0]);

     }
     else {     
             if(debugMode==1)
               debugPrint(pidNum,pCmdLine->arguments[0]);                     /* for the parent:      */
          while (wait(&status) != pidNum)       /* wait for completion  */
               ;
     }
}

void signalHandler( int gotSignal){
    const char *signal_name;

    // Find out which signal we're handling
    switch (gotSignal) {
        case SIGQUIT:
            signal_name = "SIGQUIT";
            break;
        case SIGTSTP:
            signal_name = "SIGTSTP";
            break;
        case SIGCHLD:
          signal_name = "SIGCHLD";
            break;
        default:
            fprintf(stderr, "Caught wrong signal: %d\n", signal);
            
      printf("ignoring  %s signal \n", signal_name);
     }
}
void debugPrint(int pid,char * exeCmd){
      char   buf[100];
     sprintf(buf, "Procces ID: %d\nExecuting Command: %s\n\n", pid, exeCmd);
     write(2, buf, strlen(buf));
   }
