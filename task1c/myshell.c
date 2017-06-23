#include "LineParser.h"
#include <unistd.h>
#include <linux/limits.h>
#include <stdlib.h> 
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

int  main(int argc,char** argv){
     char  myLine[2048];       int debugMode=0;  
     char  currDir[PATH_MAX];            
     cmdLine* myCmd; 
     char slashBuf[1]={'/'};
     char errBuf[28];
     for(int i=0;i<argc;i++){
          if(strcmp(argv[i], "-d") == 0)
               debugMode=1;
     }
     while (1) { 
          getcwd(currDir, sizeof(currDir));                  /* repeat until done ....         */
          do{ 
          printf("#");
          printf(currDir);     /*   display a prompt             */
          printf("# ");
          gets(myLine);  
          printf("\n");}
          while(strcmp(myLine,"") == 0);

          myCmd=parseCmdLines(myLine);            /*   read in the command line     */
          if (strcmp(myCmd->arguments[0], "quit") == 0){  /* is it an "exit"?     */
               if(debugMode==1){
                int pid= getpid();
                debugPrint(pid,"quit");
                  }
                exit(0); 
               } 
               if (strcmp(myCmd->arguments[0], "cd") == 0){  /* is it an "exit"?     */
                    char dest[sizeof(currDir)+1+sizeof(myCmd->arguments[1])];
                    sprintf(dest, "%s%s", currDir, slashBuf);

                    sprintf(dest, "%s%s", dest, myCmd->arguments[1]);
                    if(chdir(dest)<0){
                      sprintf(errBuf, "Error executing cd command\n\n");
                     write(2, errBuf, strlen(errBuf));
                     }
               if(debugMode==1){
                int pid= getpid();
                debugPrint(pid,"cd");
                  }
                  continue;
               }
                     /*   exit if it is                */
                int blocking=(int)(myCmd->blocking);
          execute(myCmd,debugMode,blocking); 
          freeCmdLines(myCmd);          /* otherwise, execute the command */
     }
     return 0;
}

void  execute(cmdLine *pCmdLine,int debugMode,int blocking){
     pid_t  pidNum;
     int    status;
     int died=0;
    
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
               debugPrint(pidNum,pCmdLine->arguments[0]); 
             
            if(blocking==1)
                while (wait(&status) != pidNum);
             else {

              kill(pidNum, SIGTERM);

              
              for (int loop=0 ;!died && loop < 5 ; ++loop){
                        sleep(1);
                  if (waitpid(pidNum, &status, WNOHANG) == pidNum) 
                    died = 1;
              }

              if (!died) 
                kill(pidNum, SIGKILL);



             }

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
