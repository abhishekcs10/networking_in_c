#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>
#include<signal.h>
#include<fcntl.h>
#include<semaphore.h>
#include <sys/stat.h>
using namespace std;

#define PORT_NO 15000
#define BUFF_SIZE 1024
#define REC_SIZE 15

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

volatile sig_atomic_t flag=0;
int sockfd;
char sem_fn1[] = "my_sem1";
char sem_fn2[]="my_sem2";

sem_t  *semdes1,*semdes2;
void handler(int sig_num)
{
		if(flag==0)
		{
				flag=1;
				int n=write(sockfd,"1",1);
				printf(RED "\nTerminating Connection..!!\n" RESET);
				fflush(stdout);
				sem_close(semdes1);
				sem_close(semdes2);

		}
		exit(0);
}
int main()
{

		//semaphore
		semdes1 = sem_open(sem_fn1, O_CREAT|O_EXCL, 0644, 0);

		if(semdes1 == (void*)-1){
				perror("sem_open failure");
				exit(1);
		}
		semdes2 = sem_open(sem_fn2, O_CREAT|O_EXCL, 0644, 1);

		if(semdes2 == (void*)-1){
				perror("sem_open failure");
				exit(1);
		}




		int port_no=PORT_NO,n;
		//client side socket creation
		sockfd=socket(AF_INET,SOCK_STREAM,0);
		struct hostent *server;
		server=gethostbyname("localhost");
		struct sockaddr_in serv_addr;
		bzero((char *) &serv_addr,sizeof(serv_addr));

		serv_addr.sin_family=AF_INET;
		bcopy((char *)server -> h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
		serv_addr.sin_port=htons(port_no);
		//estalish connection with server
		if(connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))<0){
				perror(RED "connecting error\nExiting...\n" RESET) ;
				return 1;
		}
		//read the input string and pass it to the server
		char buffer[BUFF_SIZE];



		bzero(buffer,sizeof(buffer));
		n=read(sockfd,buffer,sizeof(buffer));
		if(n<0)
		{
				printf(RED "Error Connecting\nTerminating..." RESET);
				return 1;
		}
		printf("%s\n",buffer);
		bzero(buffer,BUFF_SIZE);


		/*
		//working code
		sprintf(buffer,"provide client info\n");
		n=write(sockfd,buffer,sizeof(buffer));
		if(n<0)
		perror("error transmitting msg...\n");

		bzero(buffer,sizeof(buffer));
		n=read(sockfd,buffer,sizeof(buffer));
		printf("%s",buffer);
		 */

		signal(SIGCHLD, SIG_IGN);
		//ended
		char receiver[REC_SIZE];

		pid_t async=fork();
		if(async<0)
				perror(RED "Error forking\n" RESET);
		else if(async!=0)
		{	//handle signal
				struct sigaction sa;
				sa.sa_handler=handler;
				sa.sa_flags=0;
				sigemptyset(&sa.sa_mask);
				if(sigaction(SIGINT,&sa,NULL)==-1){
						perror("sigaction\n");
						exit(1);

				}
				//read
				while(1)
				{		

						//	if(!sem_wait(semdes2))
						//	{
						//		if(!sem_wait(semdes2))
						//		{


						//printf("here");
						bzero(buffer,sizeof(buffer));
						//sleep(2);
						n=read(sockfd,buffer,sizeof(buffer));
						if(strlen(buffer)==1 && buffer[0]=='0')
						{
								printf("Connection Limit exceeded !!\n");
								fflush(stdout);				
						}
						if(strlen(buffer)>2)
						{		//	fcntl(0,F_SETFL,fcntl(0, F_GETFL)|(O_NONBLOCK)); 
								//		fcntl(1,F_SETFL,fcntl(1, F_GETFL)|(O_NONBLOCK)); 

								sprintf(buffer,"%s\n" BLUE" Press I to query available client or enter client Id or name to which you want to send message: " RESET ,buffer);
								write(1,buffer,strlen(buffer));
								//printf("%s",buffer);
								fflush(stdout);

								//fcntl(0,F_SETFL,fcntl(0, F_GETFL) & (O_NONBLOCK)); 
						}

						//	n=read(sockfd,buffer,sizeof(buffer));
						//		printf("%s\n",buffer);

						//				sem_post(semdes1);
						//		}
						//			sem_post(semdes2);
						//	}

				}





				exit(0);

		}
		else
		{



				//write
				int read_stdin=1;
				char line[256];
				//size_t len=50;
				bzero(buffer,sizeof(buffer));
				strcpy(buffer, BLUE "Press I to query available client or enter client Id or name to which you want to send message: " RESET);

				write(1,buffer,strlen(buffer));

				while(1)
				{
						//if(!sem_wait(semdes1))
						//	{
						//		if(!sem_wait(semdes1))
						//		{

						bzero(receiver,sizeof(receiver));
						bzero(buffer,sizeof(buffer));


						if(read_stdin==1)
						{
								bzero(buffer,sizeof(buffer));
								read_stdin=0;
						}	

						//fcntl(0,F_SETFL,fcntl(0, F_GETFL) & ~(O_NONBLOCK)); 
						//sleep(1);
						fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
						//		fcntl(1,F_SETFL,fcntl(1, F_GETFL)|(O_NONBLOCK)); 

						//printf("here\n");
						sleep(1);
						n=read(0,receiver,sizeof(receiver));
						//sleep();
						//printf("here2\n");
						if(strlen(receiver)==0||strlen(receiver)==1)
						{
								sleep(1);
								write(sockfd,"\n",1);
								continue;
						}
						if(strlen(receiver)>2)
						{
								fcntl(0,F_SETFL,fcntl(0, F_GETFL) & ~(O_NONBLOCK)); 
								strcpy(buffer, MAGENTA "Enter the message:\n" MAGENTA);
								write(1,buffer,strlen(buffer));
								bzero(buffer,sizeof(buffer));							

								n=read(0,buffer,sizeof(buffer));


								//sleep(2);
								buffer[strlen(buffer)-1]='\0';
								strcat(buffer,":");
								strcat(buffer,receiver);
								//		printf("%s",buffer);
								//		fflush(stdout);
						}
						else
								strcpy(buffer,receiver);
						//printf("%s",buffer);
						//fflush(stdout);
						n=write(sockfd,buffer,sizeof(buffer));
						read_stdin=1;
						sleep(1);
						//if(n<0)
						//		perror("\nerror occured\n");
						//				sem_post(semdes1);
						//		}
						//			sem_post(semdes1);
						//		}



				}
				exit(0);
		}

		close(sockfd);
		return 0;

}
