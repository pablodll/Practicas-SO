#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
	pid_t pid;
	pid = fork();
	int stat;

	if (argc!=2){
		fprintf(stderr, "Usage: %s <command>\n", argv[0]);
		exit(1);
	}

	if(pid == 0){
		// Child
		execl("/bin/bash", "/bin/bash", "-c", argv[1], NULL);
		exit(-1);
	}
	else{
		// Parent
		while(pid != wait(&stat));
	}

	exit(0);
	//return system(argv[1]);
}

