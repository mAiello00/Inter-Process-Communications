#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

//Marco Aiello
//251 047 101
//Due: October 8,2021

int main(int argc, char **argv)
{
	int port[2];
	pid_t pid;

	if(pipe(port) < 0)//create the pipe and check if it failed
	{
		perror("pipe error\n");
		exit(0);
	}

	pid = fork();

	if(pid < 0)//if the fork failed
	{
		perror("fork error\n");
		exit(-1);
	}

	if(pid > 0)//if we are in the parent)
	{
		printf("parent PID(%d) created child PID(%d)\n",getpid(),pid );

		char* str1 = argv[1];//get the first command-line argument
		int len1 = strlen(str1);//get the length of the comman-line argument
		printf("parent PID(%d) receives X = '%s' from the user\n",getpid(),str1);

		write(port[1],&len1, sizeof(len1));//write the length of the first command-line argument string to pipe
		write(port[1],str1, len1);//write the passed argument to the pipe
		printf("parent PID(%d) writes X = '%s' to the pipe\n",getpid(),str1);

		wait(NULL);

		int totalLength;
		read(port[0],&totalLength,sizeof(totalLength));
		totalLength = totalLength + 1;

		char completePhrase[totalLength];
		read(port[0],&completePhrase,totalLength);

		printf("parent PID(%d) reads concatenated result from the pipe(Z' = '%s')\n",getpid(),completePhrase);
	}

	if(pid ==0)//if we are in the child
	{
		//get the remaining 2 arguments from the command-line and calculate their length
		char* str2 = argv[2];
		int len2 = strlen(str2);
		char* str3 = argv[3];
		int len3 = strlen(str3);
		printf("child PID(%d) receives Y = '%s' and Z = '%s' from the user\n",getpid(), str2, str3 );

		//get the length of arguments 2 and 3 and create an array that is 1 larger than that (for the space between the two phrases)
		//then concatenate them
		int catLen = len2+len3;
		char conc[catLen + 1];
		strcpy(conc, str2);
		strcat(conc," ");
		strcat(conc, str3);
		printf("child PID(%d) concatenates Y and Z to generate Y' = '%s'\n",getpid(),conc);

		int len1;
		//get the length of the first command-line argument in the pipe
		read(port[0],&len1,sizeof(len1));
		//printf("%d\n",len1);

		//get the first command-line argument from the pipe
		char str1[len1];
		read(port[0], &str1,len1);
		printf("child PID(%d) reads X from the pipe = '%s'\n",getpid(),str1);

		//create a char array large anough for the phrase with the spacing and concatenate the command-line arguments
		int totalLen = len1+catLen+1;
		char phrase[totalLen];
		strcpy(phrase, str1);
		strcat(phrase," ");
		strcat(phrase,conc);
		printf("child PID(%d) concatenates X and Y' to generate Z' = '%s'\n",getpid(),phrase);

		//write the total length into the pipe so the parent knows how long the string is
		write(port[1],&totalLen,sizeof(totalLen));

		//write the phrase in the port
		int phraseLen = strlen(phrase);
		write(port[1],phrase,phraseLen);
		printf("child PID(%d) writes Z' into the pipe\n",getpid());
	}
}
