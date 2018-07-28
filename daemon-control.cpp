#include <dirent.h>
#include <iterator>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>
#include <vector>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iomanip>

int PINSIGNAL=29;
int CONTROLPIN=28;
int PINLIGHT=27;
bool State1=1;
bool State2=0;
bool State3=0;
bool SWITCH=0;
int t=0;


void do_heartbeat()
{// TODO: implement processing code to be performed on each heartbeat

    if(wiringPiSetup()==-1)
        exit(EXIT_FAILURE);
////Проверка инициализации библиотеки.


    //State1=1;       //Простой
    State2=0;       //До выдержки времени
    State3=0;       //После выдержки времени
    t=0;
    State2=digitalRead(CONTROLPIN);
    if (State2)		{
	while (t<5)	{
	State3=digitalRead(CONTROLPIN);
	//State1=State1*State3;
        State3*=State3;
        //digitalWrite(PINSIGNAL,HIGH);
        digitalWrite(PINSIGNAL,State3);
	//digitalWrite(PINLIGHT,State1);
        digitalWrite(PINLIGHT,State3);
	t=t+1;
	usleep(100);
	}
        //digitalWrite(PINSIGNAL,0);
        }
	else {
        digitalWrite(PINSIGNAL,0);
    };
	if(State2*State3){      //Если была нажата кнопка, была выдержана выдержка, совершить переключение
	SWITCH=!SWITCH;
        digitalWrite(PINLIGHT,SWITCH);
	};
};


// For security purposes, we don't allow any arguments to be passed into the daemon
int main(void)
{
  wiringPiSetup();
////   
   pinMode(CONTROLPIN,INPUT);
   pinMode(PINLIGHT,OUTPUT);
   pinMode(PINSIGNAL,OUTPUT);
   digitalWrite(PINLIGHT,0);
   digitalWrite(PINSIGNAL,0);


   // Define variables
   pid_t pid, sid;
   // Fork the current process
   pid = fork();
   // The parent process continues with a process ID greater than 0
   if (pid > 0)
   {
      exit(EXIT_SUCCESS);
   }
   // A process ID lower than 0 indicates a failure in either process
   else if(pid < 0)
   {
      exit(EXIT_FAILURE);
   }
   // The parent process has now terminated, and the forked child process will continue
   // (the pid of the child process was 0)
   // Since the child process is a daemon, the umask needs to be set so files and logs can be written
   umask(0);
   // Open system logs for the child process
   openlog("daemon-control", LOG_NOWAIT | LOG_PID, LOG_USER);
   syslog(LOG_NOTICE, "Successfully started daemon-control");
   // Generate a session ID for the child process
   sid = setsid();
   // Ensure a valid SID for the child process
   if(sid < 0)
   {
      // Log failure and exit
      syslog(LOG_ERR, "Could not generate session ID for child process");
      // If a new session ID could not be generated, we must terminate the child process
      // or it will be orphaned
      exit(EXIT_FAILURE);
   }
   // Change the current working directory to a directory guaranteed to exist
   if((chdir("/")) < 0)
   {
      // Log failure and exit
      syslog(LOG_ERR, "Could not change working directory to /");
      // If our guaranteed directory does not exist, terminate the child process to ensure
      // the daemon has not been hijacked
      exit(EXIT_FAILURE);
   }
   // A daemon cannot use the terminal, so close standard file descriptors for security reasons
   close(STDIN_FILENO);
   close(STDOUT_FILENO);
   close(STDERR_FILENO);
   // Daemon-specific intialization should go here
   const int SLEEP_INTERVAL = 3;
   //        
   //
   // Enter daemon loop
   while(1)
   {
      // Execute daemon heartbeat, where your recurring activity occurs
      do_heartbeat();
      //printf("Привет");
      // Sleep for a period of time
      sleep(SLEEP_INTERVAL);
   }

   // Close system logs for the child process
   syslog(LOG_NOTICE, "Stopping daemon-control");
   closelog();
   // Terminate the child process when the daemon completes
   exit(EXIT_SUCCESS);
}
