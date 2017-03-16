#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>

int main()
{
int sockfd,port_no=15000,n;
char *line;
size_t len;
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
perror("connecting error\nExiting...\n");
return 1;
}
//printf("eher\n");
//read the input string and pass it to the server
char buffer[256];
printf("\nenter the string terminated by ; and maximum 30 charater allowed\n");
line=(char *)malloc(sizeof(char)*256);
ssize_t get_read=getline(&line, &len, stdin);

if(line[strlen(line)-1]=='\n')
{
line[strlen(line)-1]='\0';
}

int last=strlen(line)-1,start=0,lind=0;
int save=last;
while(line[start]==' ')
start++;
while(line[last]==' ')
last--;
for(int z=start;z<=last;z++)
line[lind++]=line[z];

for(int i=lind;i<=save;i++)
line[i]='\0';


if(line[strlen(line)-1]!=';')
{
printf("The line must be terminated with ;\nExiting client\n");
return 1;
}

for(int i=0;i<strlen(line)-1;i++)
{
if(!((line[i]>='a'&&line[i]<='z')||(line[i]>='A' && line[i]<='Z')||line[i]==' '))
{
printf("Invalid Characters found\n Exiting....\n");
return(1);
}

}
if(strlen(line)==1)
{
printf("No string provided to search keyword\nTerminating client program\n");
return 1;
}
if(strlen(line)>31)
{
printf("Length greater than 30 not allowed\nTerminating client program\n");
return 1;
}
//printf("This is %s",line);
n=write(sockfd, line, len);
if(n<0)
{
printf("Error sending message\nTerminating...");
return 1;
}
printf("Enter the keyword to be searched (max 5 character)\n");

get_read=getline(&line,&len,stdin);
int read_len=strlen(line);

if(line[strlen(line)-1]=='\n')
{
line[strlen(line)-1]='\0';
}
last=strlen(line)-1,start=0,lind=0;
save=last;
while(line[start]==' ')
start++;
while(line[last]==' ')
last--;
for(int z=start;z<=last;z++)
line[lind++]=line[z];

for(int i=lind;i<=save;i++)
line[i]='\0';



if(strlen(line)==0)
{
printf("No valid keyword found\n Exiting....\n");
return(1);
}
for(int i=0;i<strlen(line)-1;i++)
{
if(!((line[i]>='a'&&line[i]<='z')||(line[i]>='A' && line[i]<='Z')))
{
printf("Invalid Characters found\n Exiting....\n");
return(1);
}

}
if(read_len>5)
{
printf("Length greater than 5 not allowed\nTerminating client program\n");
return 1;
}

n=write(sockfd,line,len);
if(n<0)
{
printf("Error sending message\nTerminating...");
return 1;

}
bzero(buffer,256);
n=read(sockfd,buffer,255);
if(n<0)
{
printf("Error sending message\nTerminating...");
return 1;
}
printf("%s\n",buffer);
close(sockfd);
return 0;
}
