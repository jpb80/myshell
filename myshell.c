/**project5
 * jpb80@pitt.edu
 * cs449
 *  **/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <signal.h>
#include <stdlib.h>
#define SIZE 100

char line[SIZE];
char bkupline[SIZE];
char *input;
clock_t cutime;
clock_t utime;
clock_t stime;
clock_t cstime;
clock_t st_time;
clock_t en_time;
struct tms st_tms;
struct tms en_tms;
int flag;
int counter;


void sh_exit() {
   pid_t shell_pid = getpid();
   kill(shell_pid, SIGTERM);
}// end sh_exit


void sh_chdir(char *tok) {

   char *input;
   input = tok;
   int status;

   tok = strtok(NULL, " \n\t()<>|&;");    

   while (tok != NULL) {
	 status = chdir(tok);
	 if (status < 0) {
	    perror("cd error");
	    return; 
	 }
	 tok = strtok(NULL, " \n\t()<>|&;");     
   }
} //end sh_chdir


void sh_unix(char* tok) {
   
   char *args[25];
   input = tok;
   int status;
   counter = 0;
   int ticks;
   int flag2;
   args[0] = input;
   pid_t pid;

   if (strcmp(tok, "time") == 0) {
      times(&st_tms);      
      cutime = st_tms.tms_cutime; 
      utime = st_tms.tms_utime; 
      stime  = st_tms.tms_stime; 
      cstime = st_tms.tms_cstime; 
      counter = -1;
   }
	 	
   tok = strtok(NULL, " \n\t()<|&;");    
   while (tok != NULL) {  
      if (strcmp(tok, ">") == 0) {
	 flag = 1; 
      } else if (strcmp(tok, ">>") == 0 ) {
	 flag2 = 1;
      }
      counter++;
      args[counter] = tok; 
      tok = strtok(NULL, " \n\t()<|&;");    
   }
   
   counter++;
   args[counter] = NULL;
 
   int execvp_status;
   int close_status;
   pid = fork();
   
   if (pid < (pid_t)0) {
      perror("Child Process Failed");
      _Exit(0); 
   }

   int wait_status;
   FILE *stream;

   //child process
   if (pid == (pid_t)0) { 
      if (flag == 1 || flag2 == 1) {
	    stream = freopen(args[counter-1], "w+", stdout); 
	    if (stream == NULL) {
	       	perror("Parent Stream Error");
	       _Exit(0);
	    }
	    printf("flag= %s\n", args[counter-1]);
	    int i;
	    i = 0;
	    while (args[i] != NULL) {
	    fprintf(stream, args[i]);
	    i++;
	    }

	    close_status = fclose(stream);
	    if (close_status != 0) {
	       perror("Stream fclose error");
	       exit(EXIT_FAILURE);
	    }
	    flag = 0;
	    flag2 = 0;
	    _Exit(0);
      }  
	 execvp_status = execvp(args[0], args);
	 if (execvp_status < 0 ) {
	    perror("Execvp Error");
	    exit(EXIT_FAILURE); 
	 }
	 _Exit(0);
      } else {
	 // parent process
	 wait_status = wait(&status);
	 if (wait_status < 0) {
	    perror("Wait Unsuccessful");
	    exit(EXIT_FAILURE);
	 }
	 flag = 0;
	 flag2 = 0;
      } 
}// end sh_unix

int main(int argc, char *argv[]) {
   clock_t time1,time2,time3,time4;
   int i;   
   char *tok; //used for return value in which is a pointer 
   int result;
   flag = 0;

   while (1) {
      fgets(line, sizeof(line), stdin);
      tok = strtok(line, " \n\t()<>|&;"); 
      if (tok == NULL) {
	 continue;
      } else if (strcmp(tok, "exit") == 0) {
	 sh_exit();
      } else if (strcmp(tok, "cd") == 0) {
	 sh_chdir(tok);
      }  else { 
	 sh_unix(tok);  
	 if (strcmp(input, "time") == 0) {
	    times(&en_tms);
	    time1 = en_tms.tms_cutime - cutime;
	    time2 = en_tms.tms_utime - utime;
	    time3 = en_tms.tms_stime - stime;
	    time4 = en_tms.tms_cstime - cstime;
	    printf("cutime: %f\n", (double)time1);
	    printf("utime: %f\n", (double)time2);
	    printf("stime: %f\n", (double)time3);
	    printf("cstime: %f\n", (double)time4);	   
	 } 
      }
   }     
   return 0;
}//end main
