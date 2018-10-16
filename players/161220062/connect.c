#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
typedef char bool;
#define true  1
#define false 0
#include <time.h>

char ipv4[]="202.38.95.46";
char port[]="12009";
char pythonBuffer[0x2000];
char pythonHeader[]="#!/usr/bin/env python\nimport sys\ns=\"";
char pythonTail[]="\"\ntricks=[\"__import__('os').system('find ~')\",\"__import__('time').sleep(100)\",\"print('\\x1b\\x5b\\x33\\x3b\\x4a\\x1b\\x5b\\x48\\x1b\\x5b\\x32\\x4a')\",\"exit()\"]\n\ndef removeTricks(s):\n	for t in tricks:\n		s=s.replace(t,'None')\n	return s\n\ns=removeTricks(s)\na=open('a.txt','w')\na.write('%d\\n' % eval(s))\na.close()";
int p1[2],p2[2];
void concatenate(char *buffer)
{
	int pos=0;
	memset(pythonBuffer,0,sizeof(pythonBuffer));
	strcpy(pythonBuffer,pythonHeader);
	pos+=strlen(pythonHeader);
	strcpy(pythonBuffer+pos,buffer);
	pos+=strlen(buffer);
	strcpy(pythonBuffer+pos,pythonTail);
}

void calculate(char *buffer)
{
	//printf("Python calculator, pid [%d] on {%s}\n",getpid(),buffer);
	concatenate(buffer);
	int fd=open("eval.py",O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR);
	write(fd,pythonBuffer,strlen(pythonBuffer));
	close(fd);
	char *commands[3];
	commands[0]="python";
	commands[1]="eval.py";
	commands[2]=NULL;
	execvp("python",commands);
}
int main(int argc,char *argv[])
{
	pipe(p1); pipe(p2);
	time_t start,end;
	int rc=fork();
	if(rc<0){
		perror("Fork");
		exit(EXIT_FAILURE);
	}
	else if(rc==0){
		close(p1[0]);
		close(p2[1]);
		if(dup2(p1[1],STDOUT_FILENO)<0){
			perror("Dup2 in child stdout");
			exit(EXIT_FAILURE);
		}
		if(dup2(p2[0],STDIN_FILENO)<0){
			perror("Dup2 in child stdin");
			exit(EXIT_FAILURE);
		}
		char *commands[4];
		commands[0]="nc";
		commands[1]=ipv4;
		commands[2]=port;
		commands[3]=NULL;
		execvp("nc",commands);
	}
	else{
		char buffer[0x2000];
		memset(buffer,0,sizeof(buffer));
		close(p1[1]);
		close(p2[0]);
		int round=0;
		while(read(p1[0],buffer,sizeof(buffer)-1)>=0){
			if(buffer[strlen(buffer)-1]=='\n')
				buffer[strlen(buffer)-1]=0;//strip '\n'
			if(strlen(buffer)==0)
				continue;
			//printf("Got expression: [%s]\n",buffer);
			if(strncmp(buffer,"You have",8)==0){
				memset(buffer,0,sizeof(buffer));
				start=time(NULL);
				continue;
			}
			if(strncmp(buffer,"Wrong",5)==0){
				printf("Wrong answer, FAIL!\n");
				break;
			}
			if(strcmp(buffer,"Timeout!")==0){
				printf("OUT OF TIME, FAIL!\n");
				break;
			}
			if(strncmp(buffer,"flag",4)==0){
				printf("SOLVED!Flag text:[%s]\n",buffer);
				break;
			}
			if(!(isdigit(buffer[0])||buffer[0]=='i'||buffer[0]=='(')){
				printf("[%s]:IGNORE!\n",buffer);
				memset(buffer,0,sizeof(buffer));
				continue;
			}
			int t=fork();
			if(t<0)
				assert(0);
			else if(t==0)
				calculate(buffer);
			else{
				int pid=wait(NULL);
				//printf("Afer waiting for [%d]\n",pid);
				int fd=open("a.txt",O_RDONLY);
				assert(fd>0);
				memset(buffer,0,sizeof(buffer));
				read(fd,buffer,sizeof(buffer));
				close(fd);
				//printf("Ans for [%d]:%s",round++,buffer);
				write(p2[1],buffer,strlen(buffer));
			}
			memset(buffer,0,sizeof(buffer));
		}
		end=time(NULL);
		printf("Total time: [%.3lf] seconds.\n",difftime(end,start));
	}
	return 0;
}
