#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<netdb.h>
#include<string.h>
#include<sys/types.h>
/*
In this function the required query is performed.
It takes input the file descripter of socket to which client has established connection.

*/

void runclient(int newsockfd){
		char buffer[256],keyword[256];// buffer stores the input string from the client which contains a single string containing ; separated text and keyword to be searched
		int n,occur=0;//occur stores the number of occurences and n is used to keep track of number of characters read from client
		bzero(buffer,256);//initializes the buffer with 0
		bzero(keyword,256);
		n=read(newsockfd,buffer,255);//read content from the socket given by client
		if(buffer[strlen(buffer)-1]=='\n') 
				buffer[strlen(buffer)-1]='\0';  //remove the trailing line change

		if(n<0){
				perror("cannot read\n");
		}

		printf("Here is msg received: %s\n",buffer); //for debug purpose

		n=read(newsockfd,keyword,255);//read content from the socket given by client
		if(keyword[strlen(keyword)-1]=='\n') 
				keyword[strlen(keyword)-1]='\0';  //remove the trailing line change

		if(n<0){
				perror("cannot read\n");
		}

		printf("Here is msg received: %s\n",keyword); //for debug purpose


		int i;
		for( i=0;i<strlen(buffer);i++)//check if the sentence has a ;
		{
				if(buffer[i]==';')
						break;
		}
		char msg[256];//this will contain the acknowledgement sent by server to client which will contain number of occurences

//the below condition checks if the keyword is empty;
		if(i==strlen(buffer))
				strcpy(msg,"No ; provided in sentence error\n");
	/*	else if(i==(strlen(buffer)-1)||(i==(strlen(buffer)-2)&&buffer[strlen(buffer)-1]=='\n'))
				strcpy(msg,"No search keyword provided\n");*/
		else
		{
				//this removes any trailing spaces
	/*			i+=1;				
				while(i<strlen(buffer)&&buffer[i]==' ')
						i++;
				int za=0;
				for(;i<strlen(buffer);i++)
				{
						if(buffer[i]==' '|| buffer[i]=='\n')
								break;
						keyword[za++]=buffer[i];

				}
				keyword[za]='\0';*/
				if(strlen(keyword)==0)
				{sprintf(msg,"Empty keyword\n");

				}
				/*
				   for(i=strlen(buffer)-1;i>0;i--)
				   {
				   if(buffer[i]==' ')
				   buffer[i]='\0';
				   else
				   break;
				   }

				 */				
				else
				{
					//query performed here
					printf("Processing..\n");
					/*	char *token=strtok(buffer,";");

						char text[256];
						strcpy(text,token);
						token=strtok(NULL,";");*/
						char text[256];
						strcpy(text,buffer);
						printf("##%s##%s##\n",text,keyword);
						//strcpy(keyword,token);
						for(int j=0;j<strlen(text);j++)
						{
								int k=j,matched=0;
								for(;matched<strlen(keyword);matched++)
								{
										if(text[k+matched]!=keyword[matched])
												break;

								}		
								if(matched==strlen(keyword))
										occur++;
						}
						int z=sprintf(msg,"The number of occurrences of #%s# are: %d\n",keyword,occur);
					
				}	
		}



		n=write(newsockfd,msg,strlen(msg));
		printf("Processing Complete....:)\n");
		if(n<0)
		{
				perror("cannot write message\n");

		}

		return;
}



int main()
{
		int port_no=15000;
		struct sockaddr_in server;
		int sockfd,newsockfd;
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

		while(1)
		{
				struct sockaddr_in client;
				socklen_t cli=sizeof(client);
				newsockfd=accept(sockfd,(struct sockaddr*) &client, &cli);
				if(newsockfd<0)
				{
						perror("connection not accepted");
				}
				pid_t pid=fork();
				if(pid<0){
						perror("COuld not fork process");

				}

				if(pid==0)
				{

						close(sockfd);
						runclient(newsockfd);
						exit(0);
				}
				else
				{
						close(newsockfd);
				}

		}
		return 0;
}
