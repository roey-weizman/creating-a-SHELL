#include "LineParser.h"
#include <unistd.h>
#include <linux/limits.h>
#include <stdlib.h> 
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "JobControl.h"

int  main(int argc,char** argv){
        
     signal(SIGTTIN, SIG_IGN); //signals ignoring
     signal(SIGTTOU, SIG_IGN);
     signal(SIGQUIT,signalHandler);
     signal(SIGCHLD,signalHandler);
     signal(SIGTSTP,signalHandler);
     setpgid(getpid(),getpid());
     
     struct termios termAttributes;
     if (tcgetattr(STDOUT_FILENO, &termAttributes) != 0)
           perror("tcgetattr error");
     
     char  myLine[2048];       int debugMode=0;  
     char  currDir[PATH_MAX];          
     cmdLine* myCmd; 
     char slashBuf[1]={'/'};
     char errBuf[28];
     for(int i=0;i<argc;i++){
          if(strcmp(argv[i], "-d") == 0)
               debugMode=1;
     }
     
     job** myJobList=malloc(sizeof(job));

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
         if (strcmp(myCmd->arguments[0], "jobs") == 0){  /* is it an "exit"?     */
                if(myJobList!=NULL)
                  printJobs(myJobList);
                if(debugMode==1){
                  int pid= getpid();
                  debugPrint(pid,"jobs");
                }
                continue;
               } 
                     /*   exit if it is                */
                int blocking=(int)(myCmd->blocking);
          execute(myCmd,debugMode,blocking,myJobList); 
          freeCmdLines(myCmd);          /* otherwise, execute the command */
     }
     return 0;
}

void  execute(cmdLine *pCmdLine,int debugMode,int blocking,job** myJobList){
     pid_t  pidNum;
     int    status;
     int died=0;
    
     if ((pidNum = fork()) < 0) {     /* fork a child process           */
          perror("**fork() method failed\n");
          exit(1);
     }

     else if (pidNum == 0) {          /* for the child process:         */
          
          signal(SIGTTIN, SIG_DFL);// restore default settings for signals
          signal(SIGTTOU, SIG_DFL);
          signal(SIGQUIT, SIG_DFL);
          signal(SIGCHLD, SIG_DFL);
          signal(SIGTSTP , SIG_DFL);
          
          setpgid(pidNum,pidNum);
          
          
          if (execvp (pCmdLine->arguments[0], pCmdLine->arguments) < 0) {     /* execute the command  */
               perror("** execv failed\n");
               exit(-1);
          }
          if(debugMode==1)
               debugPrint(pidNum,pCmdLine->arguments[0]);
        }

     else {    
            setpgid(pidNum,pidNum);
             if(debugMode==1) 
               debugPrint(pidNum,pCmdLine->arguments[0]); 
             
            if(blocking==1)
                while (wait(&status) != pidNum);
             else {
                  waitpid(-1, &status, WNOHANG);
                  job* currJob=addJob(myJobList,pCmdLine->arguments[0]);
                  currJob->status=RUNNING;




             }

        }
}

void signalHandler(int gotSignal){
    const char *signal_name;

    // Find out which signal we're handling
   /* switch (gotSignal) {
        
        case SIGQUIT:
            signal_name = "SIGQUIT";
            signal(SIGQUIT, SIG_IGN);
            break;
        
        case SIGTSTP:
            signal_name = "SIGTSTP";
            signal(SIGTSTP, SIG_IGN);
            break;
       
        case SIGCHLD:
          signal_name = "SIGCHLD";
          signal(SIGCHLD, SIG_IGN);
          break;
        default:
            fprintf(stderr, "Caught wrong signal: %d\n", signal);
            */
      printf("ignoring  %s signal \n", signal_name);
     }
}

  void debugPrint(int pid,char * exeCmd){
        char   buf[100];
       sprintf(buf, "Procces ID: %d\nExecuting Command: %s\n\n", pid, exeCmd);
       write(2, buf, strlen(buf));
     }
