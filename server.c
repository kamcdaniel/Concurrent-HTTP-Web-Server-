#define _GNU_SOURCE
#include "net.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

#define PORT 22809

void limit_fork(rlim_t max_procs)
{
	struct rlimit rl;
	if (getrlimit(RLIMIT_NPROC, &rl))
	{
		perror("getrlimit");
		exit(-1);
	}
	rl.rlim_cur = max_procs;
	if (setrlimit(RLIMIT_NPROC, &rl))
	{
		perror("setrlimit");
		exit(-1);
	}
}

// SIGCHLD handler function - use signals to ensure no zombie processes
void sigchld_handler(int signo) {
	while (waitpid(-1, NULL, WNOHANG) > 0) {
	}
}

void handle_request(int nfd)
{
	FILE *network = fdopen(nfd, "rw");
   char *line = NULL;
   size_t size;
   ssize_t num;

   if (network == NULL)
   {
      perror("fdopen");
      close(nfd);
      return;
   }
	// until client gets to EOF
   while ((num = getline(&line, &size, network)) >= 0)
   {
	char *buffer = NULL;
	char *file = strtok(line, " ");
	file = strtok(NULL, "\n");
	// isolate file name from stdin, check to make sure valid 
	FILE *fp = fopen(file, "r");
	if (fp == NULL) {
		buffer = "Could not open file\n";
		write(nfd, buffer, strlen(buffer));
	} else {
		// if valid, fork to create child process to handle reading file contents
		pid_t pid = fork();
		if (pid < 0) {
			perror("fork() failed\n");
			exit(1);
		} 
		if (pid == 0) {
			size_t buffer_size = 0;
			ssize_t chars_read = 0;
			// while there is still content to read from the file	
			while((chars_read = getline(&buffer, &buffer_size, fp)) != -1) {
				// write file content to the client
				write(nfd, buffer, strlen(buffer));
			}	
			// next fork should only apply to parent
			exit(0);
		}
		// no need for "else", since signal handler function waits appropriately for children to end 
	}
	free(buffer);
	fclose(fp);
   }
   free(line);
   fclose(network);
}

void run_service(int fd)
{
   while (1)
   {
      int nfd = accept_connection(fd);
      if (nfd != -1)
      {
         printf("Connection established\n");
         handle_request(nfd);
         printf("Connection closed\n");
      }
   }
}

int main(void)
{
	signal(SIGCHLD, sigchld_handler);
	limit_fork(50);
   int fd = create_service(PORT);

   if (fd == -1)
   {
      perror(0);
      exit(1);
   }

   printf("listening on port: %d\n", PORT);
   run_service(fd);
   close(fd);

   return 0;
}
