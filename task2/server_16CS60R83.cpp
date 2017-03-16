#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<netdb.h>
#include<string.h>
#include<sys/types.h>
#include<ctime>
#include<random>
#include<queue>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<signal.h>
#include<sys/sem.h>
#include<semaphore.h>
using namespace std;

#define MAX_CONNECT 5
#define BUFF_SIZE 1024
#define PORT_NO 15000
#define BUFF_MSG 20

//from stackoverflow printing coloured output

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

typedef struct client_info{
		char connect_time[80];
		int unique_id;
		char unique_name[11];
		int isonline[5];
}client_info;
int *front,*rear;//first insert element then increment rear , for deletion increment the front  and if front==rear reset
union semun{
		int val;
		struct semid_ds *buf;
		ushort *array;
};

typedef struct message_send{

		int from;
		int to;
		char body[256];
		int sent;

}message_send;
//client_info watch_clients[MAX_CONNECT];
//int online_clients=0;

int semid=-1,shmid=-1,shmid_cli=-1;//semid is semaphore id and shmid is shared memory id


void handler(int num){
		printf("Destroying shared variables\n");
		//free shared memory and semaphores
		if(shmid!=-1)
				shmctl(shmid, IPC_RMID, NULL);
		if(semid!=-1)
				semctl(semid,0,IPC_RMID);
		if(shmid_cli!=-1)
				shmctl(shmid,IPC_RMID,NULL);

		exit(0);
}

int  semaphore_lock(int semid,int num){
		struct sembuf sb;
		sb.sem_num=num;//which semaphore in the semaphore array
		sb.sem_op=-1;//what operation to perform here it add the value 1 to the semaphore
		sb.sem_flg=0;//set flag to 0 to wait for operation to get successful otherwise set the semaphore to IPC_NOWAIT to continue operation
		int retval=semop(semid,&sb,1);
		if(retval==0)//successfull
				return 1;
		else
				return 0;

}
int semaphore_unlock(int semid,int num){
		struct sembuf sb;
		sb.sem_num=num;
		sb.sem_op=1;
		sb.sem_flg=0;
		int retval=semop(semid,&sb,1);
		if(retval==0)
				return 1;
		else 
				return 0;
}

int initsem(key_t key,int nsems){
		int i;
		union semun arg;
		struct semid_ds buf;
		int semid;
		semid=semget(key,nsems,IPC_CREAT | 0666);
		if(semid==-1){
				perror("semget failed");
				exit(1);

		}
		arg.val=1;//this is the value to initialize the semaphore variable

		if(semctl(semid,0,SETVAL,arg) == -1){
				perror("could not initialize semaphore");
		}


}

void message_handler_process(message_send *mem){
		//set lock
		int n;
		//put message in queue
		//dequeue message


}




/*
   In this function the required query is performed.
   It takes input the file descripter of socket to which client has established connection.

 */


void runclient(int newsockfd,int client_no, message_send *mem, client_info *watch_clients,FILE *fp){
		char buffer[BUFF_SIZE],client1[BUFF_SIZE/6]="",msg[BUFF_SIZE],send_msg[BUFF_SIZE],disconnect_id[11];
		int n,notify=0;
		//message_send *get_msg=(message_send *)msg_str;
		bzero(buffer,sizeof(buffer));


		/*		n=read(newsockfd,buffer,sizeof(buffer));
				printf("Query received: %s\n",buffer);
				bzero(buffer,sizeof(buffer));
				for(int i=0;i<client_no+1;i++)
				{
				if(i!=client_no && watch_clients[i].isonline)
				{
				notify++;
				bzero(client1,sizeof(client1));
				sprintf(client1,"Id: %d\nName: %s\n",watch_clients[i].unique_id,watch_clients[i].unique_name);
				strcat(buffer,client1);
				}
				}

				if(notify==0)
				sprintf(buffer,"No other client is available\n");
				n=write(newsockfd,buffer,sizeof(buffer));
				printf("Query processed...:)\n");

		 */
		while(1)
		{				
				//check if queue front has got some message for the given id
				bzero(buffer,sizeof(buffer));
				bzero(msg,sizeof(msg));
				notify=0;
				int new_dis=0;
				//check if some client is offline and display message
				for(int i=0;i<MAX_CONNECT;i++)
				{
						if(((watch_clients+client_no)->isonline[i] == 2))
						{
								if(!new_dis)
										sprintf(disconnect_id,RED "\n%d " RESET ,(watch_clients+i)->unique_id);
								else
										sprintf(disconnect_id,RED "%d " RESET ,(watch_clients+i)->unique_id);
								strcat(msg,disconnect_id);
								(watch_clients+client_no)->isonline[i]=-1;
								new_dis=1;
						}

				}
				if(new_dis)
						strcat(msg,RED "disconnected\n" RED);
				n=write(newsockfd,msg,sizeof(msg));

				bzero(msg,sizeof(msg));


				if((mem+*front)->to==(watch_clients+client_no)->unique_id)
				{	int res=semaphore_lock(semid,0);

						printf("Message Transferred\n");
						(mem+*front)->sent=1;
						(mem+*front)->to=0;
						sprintf(buffer,GREEN "\nFrom: " RESET "%d\n " GREEN "Message: " RESET "%s\n",(mem+*front)->from,(mem+*front)->body);
						(*front)++;
						if(*front==*rear)
						{*front=0;*rear=0;}
						fflush(stdout);
						res=semaphore_unlock(semid,0);

				}

				else{

						sprintf(buffer,"\n");	
						//write null	
				}
				write(newsockfd,buffer,sizeof(buffer));
				bzero(buffer,sizeof(buffer));
				bzero(msg,sizeof(msg));
				bzero(client1,sizeof(client1));
				bzero(buffer,sizeof(buffer));
				n=read(newsockfd,msg,sizeof(msg));

				if(strlen(msg)==0||(strlen(msg)==1 && msg[0]=='\n'))
						continue;

				printf("Query received: %s %d\n",msg,(int)strlen(msg));
				if(strlen(msg)>2)
				{
						char *token;
						token=strtok(msg,":");
						strcpy(send_msg,token);

						printf("%s %d\n",send_msg,(int)strlen(send_msg));
						token=strtok(NULL,":");
						if(token!=NULL)
						{
								strcpy(buffer,token);
								printf("%s %d\n",buffer,(int)strlen(buffer));
						}						
						fflush(stdout);

				}
				else
						strcpy(buffer,msg);

				bzero(msg,sizeof(msg));
				if(strlen(buffer)==2 && (buffer[0]=='i' || buffer[0]=='I')&&buffer[1]=='\n')
				{

						for(int i=0;i<MAX_CONNECT;i++)
						{
								if(i!=client_no && ((watch_clients+i)->isonline[i]==1))
								{
										notify++;
										bzero(client1,sizeof(client1));
										sprintf(client1,GREEN "\nId: " RESET "%d\n" GREEN "Name: " RESET "%s\n",(watch_clients+i)->unique_id,(watch_clients+i)->unique_name);
										strcat(buffer,client1);
								}
						}

						if(notify==0)
								sprintf(buffer,RED "\nNo other client is available\n" RESET);
						n=write(newsockfd,buffer,sizeof(buffer));
						printf("Query processed...:)\n");


				}

				else if(strlen(buffer)==1 && buffer[0]=='1'){
						printf("Connection closed\n");
						fflush(stdout);
						//(watch_clients+client_no)->isonline=0;
						//				online_clients--;
						//notify other clients
						for(int i=0;i<MAX_CONNECT;i++)
								(watch_clients+i)->isonline[client_no]=2;

						break;
				}
				else if (strlen(buffer)==6)
				{
						buffer[5]='\0';
						int loop_again=0;
						for(int i=0;i<strlen(buffer);i++)
						{
								if(buffer[i]>='0'&& buffer[i]<='9')
								{}
								else{
										sprintf(msg,"\nInvalid id\n");
										loop_again=1;
										n=write(newsockfd,msg,sizeof(msg));
										break;
								}
						}
						if(loop_again)
						{
								loop_again=0;
								continue;
						}
						int id=atoi(buffer);
						bzero(buffer,sizeof(buffer));
						printf("%d\n",id);
						if(id==(watch_clients+client_no)->unique_id){
								sprintf(buffer,"\nLoopback message not allowed...\n");
								n=write(newsockfd,buffer,sizeof(buffer));
						}
						else{
								int ispresent=0;
								for(int i=0;i<MAX_CONNECT;i++){

										if((watch_clients+i)->unique_id==id && ((watch_clients+i)->isonline[i]==1) ){

												//acquire lock
												int res=semaphore_lock(semid,0);
												//add message in queue
												(mem+*rear)->from=(watch_clients+client_no)->unique_id;
												(mem+*rear)->to=id;
												//	(mem+rear)->body=(char *)malloc(sizeof(char)*256);
												strcpy((mem+*rear)->body,send_msg);
												(mem+*rear)->sent=0;
												ispresent=1;
												printf("At memory location: %p\nFrom client %d\nTo client %d\nMsg: %s\n",(mem+*rear),(mem+*rear)->from,(mem+*rear)->to,(mem+*rear)->body);
												fprintf(fp,"%d,\t%d,\t%s\n",(mem+*rear)->from,(mem+*rear)->to,(mem+*rear)->body);
												(*rear)++;

												//*rear++;
												if(*rear==BUFF_MSG)
														printf("\nMemory limit exceeded\n");

												res=semaphore_unlock(semid,0);

												//read message from queue
												bzero(buffer,sizeof(buffer));
												sprintf(buffer,"\nMessage Queued and will be delivered\n");
												n=write(newsockfd,buffer,strlen(buffer));
												bzero(buffer,sizeof(buffer));	
										}

								}
								if(!ispresent)
								{
										sprintf(buffer,"\nClient not available\n");
										n=write(newsockfd,buffer,sizeof(buffer));
										fflush(stdout);
								}


						}


				}
				else if (strlen(buffer)==11){

						//compare names
						bzero(msg,sizeof(msg));
						buffer[strlen(buffer)-1]='\0';
						int ispresent=1;
						if(strcmp(buffer,(watch_clients+client_no)->unique_name)==0){
								sprintf(msg,"\nLoopback message not allowed...\n");
								n=write(newsockfd,msg,strlen(msg));

						}
						else{
								int ispresent=0;
								for(int i=0;i<MAX_CONNECT;i++){

										if(strcmp((watch_clients+i)->unique_name,buffer)==0 && ((watch_clients+i)->isonline[i]==1) ){

												//acquire lock
												int res=semaphore_lock(semid,0);
												//add message in queue
												(mem+*rear)->from=(watch_clients+client_no)->unique_id;
												(mem+*rear)->to=(watch_clients+i)->unique_id;
												//	(mem+rear)->body=(char *)malloc(sizeof(char)*256);
												strcpy((mem+*rear)->body,send_msg);
												(mem+*rear)->sent=0;
												ispresent=1;
												printf("At memory location: %p\nFrom client %d\nTo client %d\nMsg: %s\n",(mem+*rear),(mem+*rear)->from,(mem+*rear)->to,(mem+*rear)->body);
												fprintf(fp,"%d,%d,%s\n",(mem+*rear)->from,(mem+*rear)->to,(mem+*rear)->body);
												*rear++;

											//	*rear++;
												if(*rear==BUFF_MSG)
														printf("\nMemory limit exceeded\n");

												res=semaphore_unlock(semid,0);
												//read message from queue
												bzero(buffer,sizeof(buffer));
												sprintf(buffer,"\nMessage Queued and will be delivered\n");
												n=write(newsockfd,buffer,strlen(buffer));
												bzero(buffer,sizeof(buffer));	

										}

								}
								if(!ispresent)
								{
										sprintf(msg,"\nClient not available\n");
										n=write(newsockfd,msg,sizeof(msg));
										fflush(stdout);
								}



						}

				}
				else{
						sprintf(buffer,"\nInvalid input");
						n=write(newsockfd,buffer,sizeof(buffer));

				}	

		}

		return;
}


int main()
{
		FILE *fp;
		fp=fopen("./log.txt","w+");
		//fprintf(fp,"From,\tTo,\tMsg\n");
		int port_no=PORT_NO;
		default_random_engine id;
		uniform_int_distribution<int> distribution(10000,99999);
		static const char alphanum[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		uniform_int_distribution<int> gen_index(0,strlen(alphanum));


		//client info shared memory
		key_t key=ftok(".",'b');
		client_info *watch_clients;
		size_t allocate_memory=sizeof(client_info)*MAX_CONNECT+ sizeof(message_send)*BUFF_MSG+sizeof(int)*2;

		//shared memory variable
		message_send  *str;
		char *data;

		if((shmid=shmget(key,allocate_memory,IPC_CREAT | 0666))<0)
		{
				perror("SHMGET: \n");
				exit(1);
		}				
		if((data=(char *)shmat(shmid,(void *)0,0))==(char *)(-1)){
				perror("shmat22: ");
		}

		watch_clients=(client_info *)data;

		for(int i=0;i<MAX_CONNECT;i++){

				(watch_clients+i)->connect_time[0]='\0';
				(watch_clients+i)->unique_id=0;
				(watch_clients+i)->unique_name[0]='\0';
				(watch_clients+i)->isonline[0]=0;
				(watch_clients+i)->isonline[1]=0;
				(watch_clients+i)->isonline[2]=0;
				(watch_clients+i)->isonline[3]=0;
				(watch_clients+i)->isonline[4]=0;

		}

		str=(message_send *)(watch_clients+MAX_CONNECT);
		str->to=0;
		str->from=0;
		front=(int *)(str+BUFF_MSG);
		rear=front+1;
		*front=0;*rear=0;

		//share data using str character pointer

		//end shared memory




		//get lock on shared memory single lock required
		key_t sem_key=ftok(".",'z');
		semid=initsem(sem_key,1);//1 is number of semaphore



		struct sockaddr_in server;
		int sockfd,newsockfd,connection_made=0;
		char buffer[BUFF_SIZE];
		bzero(buffer,sizeof(buffer));
		bzero((char *) &server, sizeof(server));
		server.sin_family=AF_INET;
		server.sin_addr.s_addr=INADDR_ANY;
		server.sin_port=htons(port_no);
		//a socket creation requires domain which tells what protocol to use to connect; second arguement as whether to use sock_stream or sock_dgram which symbolise to use either TCP connection or UDP and the third arguement defines which protocol to use
		// when a socket is created it exists in address family but is not provided a logical address to get access
		sockfd=socket(AF_INET,SOCK_STREAM,0);
		//binding the server is required to attach address to the socket and access the socket through file descripter

		if(bind(sockfd,(struct sockaddr*) &server,sizeof(server))<0){
				perror("error on binding\n");
				return 1;

		}
		else
		{
				if(listen(sockfd,5)<0){
						perror("Error listening port\n");
						return 0;
				}
		}


		//handle signal
		signal(SIGCHLD,SIG_IGN);
		//silently reap child
		//handle ctrl+c

		//create other process to handle message passing 		

		pid_t handle_msg_proc=fork();
		if(handle_msg_proc<0){
				perror("error forking\n");
				exit(1);
		}
		else if(handle_msg_proc==0){
				while(1){
						message_handler_process(str);
				}
				exit(0);
		}

		else{

				struct sigaction sa;
				sa.sa_handler=handler;
				sa.sa_flags=0;
				sigemptyset(&sa.sa_mask);
				if(sigaction(SIGINT,&sa,NULL)==-1){

						perror("Sigaction\n");
						exit(1);

				}


				while(1)
				{
						struct sockaddr_in client;
						socklen_t cli=sizeof(client);
						bzero(buffer,sizeof(buffer));
						newsockfd=accept(sockfd,(struct sockaddr*) &client, &cli);//accept returns -1 on failure and non negative socket discriptor on success. This discriptor is used to handle client, while sockfd is closed in child to keep it free on parent process to listen.
						if(newsockfd<0)
						{
								perror("connection not accepted");
						}
						else{

								//limit accept connection
								if(connection_made<5){
										time_t rawtime;
										struct tm *timeinfo;
										time(&rawtime);
										timeinfo=localtime(&rawtime);
										strftime((watch_clients+connection_made)->connect_time,80,"%d-%m-%Y %T",timeinfo);
										(watch_clients+connection_made)->unique_id=distribution(id);
										(watch_clients+connection_made)->unique_name[0]='\0';
										int len=0;
										while((len=strlen((watch_clients+connection_made)->unique_name))<10)
										{
												(watch_clients+connection_made)->unique_name[len]=alphanum[gen_index(id)];
												(watch_clients+connection_made)->unique_name[len+1]='\0';
										}	

										(watch_clients+connection_made)->unique_name[10]='\0';

										sprintf(buffer,"Connection Successful..:)\nTime: %s\nUnique Id: %d\nNickname: %s\n",(watch_clients+connection_made)->connect_time,(watch_clients+connection_made)->unique_id,(watch_clients+connection_made)->unique_name);

										for(int i=0;i<MAX_CONNECT;i++)
										{
												(watch_clients+i)->isonline[connection_made]=1;	
										}
										int n=write(newsockfd,buffer,sizeof(buffer));
										if(n<0)
										{
												perror("Error witing\n");
										}	

										connection_made++;					
										//online_clients++;
								}
								else{

										sprintf(buffer,"Connection limit exceeded..!!\n");
										int n=write(newsockfd,buffer,sizeof(buffer));
										close(newsockfd);
										continue;
								}


						}
						pid_t pid=fork();
						if(pid<0){
								perror("Could not fork process");

						}

						if(pid==0)
						{

								close(sockfd);//in child process content is written on newsockfd so sockfd is closed.
								runclient(newsockfd,connection_made-1,str,watch_clients,fp);
								exit(0);
						}
						else
						{
								close(newsockfd);//the server needs to listen connection so newsockfd is closed in parent. I think if we remove this line the server will accept five connection.
						}

				}
		}
		return 0;
}
