/**
 * @kchavali_assignment1
 * @author  Krishnakant Chavali <kchavali@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include <sys/poll.h>
#include<netdb.h>
#include <string.h>
#include <unistd.h>
#include<assert.h>
#include<errno.h>
#include "../include/global.h"

/**
 * main function
 * REFERENCE: http://beej.us/guide/bgnet/, http://vinodthebest.wordpress.com/category/c-programming/c-network-programming/, http://www.thegeekstuff.com/2011/12/c-socket-programming/
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
void creator();
void upload(int , char *);
int handleconnections(int ,int );
void updateList(char *);
void help();
char* myip(); // function to display the machines ip
int registerclient(char *, int , char *); // Function to register the client
void Listsend(fd_set , int , int ,int ); 
int connectclient(char *, char* , char *); // Function to connect to another client
void shift(int , int );
void terminate(int );
int times=0;
int ti=0;
int fdmax;        // maximum file descriptor number
int sender, senderfd;	//sender/client socket descriptor
char buffer1[1024];
fd_set master;    // master file descriptor list
int i, n, u, g, o;
int l=1;
char buffer2[1024];
int registered=0;
struct Serv_IP_List
	{
		int Host_id;
		int host_setno;
		char remoteIP[1024]; //IP address of the Client/server
		char Hostname[1024]; //Host name
		char portno[1024]; //Listening port
	} List[5], List1[5];

struct sockaddr_in remoteaddr; // client address

int main(int argc, char *argv[])
{
	int loop=0;
	int no=1;
	
	int yes=1;
	int flag=0;
	int trap=0;
	int listeningport;
	char buffer[1024], *token1, *token2, *token3;
	fd_set read_fds;  // temp file descriptor list for select()
	int listener;     // listening socket descriptor
	int newfd;        // newly accept()ed socket descriptor
	socklen_t addrlen;
	struct sockaddr_in serv_addr;
	if (argc < 3)
	{
		fprintf(stderr,"ERROR, no port provided\n");
		exit(0);
	}
	listeningport = atoi(argv[2]);
	listener = socket(AF_INET, SOCK_STREAM , 0); // Servers listening socket
	if(listener<0)
	{
		perror("Listener error\n");
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	inet_aton(myip(), &serv_addr.sin_addr);
	serv_addr.sin_port=htons(listeningport);
	
	//Reusing an already used socket
	if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR, &yes,sizeof(int)) == -1)
	{
		perror("setsockopt");
        	exit(1);
	}
		// bind it to the port we passed in to getaddrinfo():
	if(bind(listener, (struct sockaddr *)&serv_addr, sizeof serv_addr)<0)
	{
		perror("Error in bind\n");
	}		
	//Listening to incoming connections
	if(listen(listener,4)<0)
	{
		perror("Error in listening");
	}
	FD_ZERO(&master);//clearing the master file descriptor		
	FD_SET(listener,&master);// Adding the listener to the master set
	fdmax = listener;// Tracking the biggest file descriptor
	printf("$ ");
	fflush(stdout);
	while(loop!=1)
	{
		
		FD_ZERO(&read_fds); //clearing the read file descriptor
		read_fds = master; // copy it
		
		FD_SET(0,&read_fds);
		
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) < 0)
		{
			perror("ERROR in select");
		}
		memset(buffer, 0, sizeof buffer);
		if (FD_ISSET(0, &read_fds))
		{
			fgets(buffer, 1024, stdin); // Taking the input from the user from the commandline
			token1 = strtok(buffer," ");
			
			for(i=0; i<strlen(token1);i++)
			{
				if(strcmp(&token1[i],"\n")==0)
				{
					token1[strlen(token1) - 1] = 0;
				}	
			}
			token2 = strtok(NULL, " ");
			token3 = strtok(NULL, " ");
			if(strcmp(token1,"CREATOR")==0)
			{
				creator();
			}
			else if(strcmp(token1,"HELP")==0)
			{
			}
			else if(strcmp(token1,"MYIP")==0)
			{
				char *ips = myip();
				strcpy(List[0].remoteIP, ips);
				printf("IP address: %s\n", List[0].remoteIP);
			}
			else if(strcmp(token1,"MYPORT")==0)
			{
				printf("The listening port is: %d\n", listeningport);
			}
				
			else if(strcmp(token1,"REGISTER")==0)
			{
				if(strcmp(argv[1],"c")==0)
				{	
					if(token3!=NULL)
					{				
					token3[strlen(token3) - 1] = 0;
					int S_port=atoi(token3);
					senderfd=registerclient(token2, S_port, argv[2]);
					FD_SET(senderfd,&master);
					if(senderfd>fdmax)
					{
					fdmax=senderfd+1;
					}
					List[0].Host_id=1;
					strcpy(List[0].remoteIP,List1[0].remoteIP);
					strcpy(List[0].Hostname,List1[0].Hostname);
					strcpy(List[0].portno,List1[0].portno);
					List[0].host_setno=senderfd;
					}
					else{
					printf("Enter the right values\n");
					}
				}
				else
				{
					printf("Server doesnt have this functionality\n");
				}
			}
				
			else if(strcmp(token1,"CONNECT")==0)
			{
				if(strcmp(argv[1],"c")==0)
				{	
					printf("3\n");
					if(token3!=NULL)
					{				
					token3[strlen(token3) - 1] = 0;
					connectclient(token2, token3, argv[2]);
					}
					else
					{
						printf("Enter the right values\n");
					}
				}
				else
				{
					printf("Server doesnt have this functionality\n");
				}
				
			}
			else if(strcmp(token1,"UPLOAD")==0)
			{
				if(token3!=NULL)
				{
				token3[strlen(token3)-1]=0;
				upload(atoi(token2),token3);
				}
				else{
					printf("Enter the right values\n");
				}
			}		
			else if(strcmp(token1,"DOWNLOAD")==0)
			{
			}
			else if(strcmp(token1,"LIST")==0)
			{
				
				printf("%-5d%-35s%-20s%-8s\n",List[0].Host_id, List[0].Hostname, List[0].remoteIP, List[0].portno);	
				o=1;
				while(List[o].Host_id!=0)
				{
					printf("%-5d%-35s%-20s%-8s\n",List[o].Host_id, List[o].Hostname, List[o].remoteIP, List[o].portno);
					o++;	
				}
			}		
			else if(strcmp(token1,"TERMINATE")==0)
			{
				if(token2!=NULL){
				if(strcmp(argv[1], "c")==0)
				{
				token2[strlen(token2) - 1] = 0;
				int T_id = atoi(token2);
				if(T_id==0)
				{
					perror("Enter right ID value\n");
				}
				else
				{
				terminate(T_id);
				}
				}
				else
				{
					printf("server doesnt have this functionality\n");
				}
				}
				else{
					printf("Enter the right values\n");
				}				
			}
			else if(strcmp(token1,"EXIT")==0)
			{
				
				close(listener);
				exit(0);
				loop = 1;
			}
			else if(strcmp(token1,"STATISTICS")==0)
			{
			}
			else{
				printf("Enter valid command.\n");
			}	
			
		}
		else if (FD_ISSET(listener, &read_fds)) 
		{
			
			addrlen = sizeof remoteaddr;
			//Accepting connections
			newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
			if (newfd == -1)
			{
				perror("Error in accept\n");
			}
			
			if(strcmp(argv[1], "s")==0) //RUN THIS CODE IF ITS THE SERVER
			{
			//Initializing the server_IP_LIST
			List[0].Host_id=1;
			List[0].host_setno = listener;
			strcpy(List[0].portno, argv[2]);
			char *ips = myip();
			strcpy(List[0].remoteIP, ips);
			
			g = gethostname(List[0].Hostname, sizeof List[0].Hostname);
			if (g<0)
			{
			 perror("getnameinfo error:\n");
			 exit(0);
			}
						 
			if(List[no].Host_id!=6)
			{
			if (flag!=1) 
			{
				int ty;
				ty = handleconnections(newfd, fdmax);
				FD_SET(ty, &master); // add to master set
				if (ty > fdmax)
				{    // keep track of the max
					fdmax = ty+1;
				}
				n = send(ty, "Successfully registered\n", 1024, 0);
				
				times++;
				Listsend(master, listener,fdmax,times);
				no++;
			}
			else
			{
				n = send(newfd,"Client is already registered\n", 1024, 0);
				if(n<0)
				{
					perror("Error in sending client error\n");
				}			
				
			}
			}
			else
			{
				n = send(newfd,"Only 4 connections allowed", 1024, 0);
				if(n<0)
				{
					perror("Error in sending client request\n");
				}
			
			}
			}
		
			if(strcmp(argv[1],"c")==0)
			{
			
			int ty;
			int flag;
			ty = handleconnections(newfd, fdmax);
			int lol=0;
			while(List1[lol].Host_id!=0)
			{
				printf("List1: %s buffer: %s\n",List1[lol].portno, buffer2);
				if(strcmp(buffer2,List1[lol].portno)==0)
				{
					flag=1;
					break;
				}
				else
				{
					flag=0;
				}
			lol++;
			}	
			if(flag==1)
			{
			if(List[trap].Host_id!=5)
			{
				
				
				FD_SET(ty, &master); // add to master set
				printf("NEWFD: %d\n",ty);
				if (ty > fdmax)
				{    // keep track of the max
					fdmax = ty+1;
				}
				n = send(ty,"Successfully connected\n", 1024, 0);
				if(n<0)
				{
					perror("Error in sending client response\n");
				}
				
				//printf("trap:%d\n",trap);
				trap++;
			}
			else // If more than 3 clients are connecting they will get terminated
			{
				printf("Only 3 peers allowed\n");
				n = send(newfd,"Only 3 peers allowed", 1024, 0);
				if(n<0)
				{
					perror("Error in sending client response\n");
				}				
			}
			}
			else{
				printf("Client is not registered\n");
				n = send(newfd,"Client is not registered",30,0);
				if(n<0)
				{
					perror("Error in sending client response\n");
				}
			}
			
			}
		}
		else
		{
			int y;
			int nbytes;
			for(y=0;y<=fdmax;y++)
			{
			if(FD_ISSET(y,&read_fds))
			{
			memset(buffer1,0,sizeof buffer1);
			if ((nbytes = recv(y, buffer1, sizeof buffer1, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", y);
                        } else {
                            printf("Client ID: %d exited\n",y);
                        }
			close(y); // bye!
                        FD_CLR(y, &master); // remove from master set
			printf("REMOVED %d from master set\n", y);
			
			if(strcmp(argv[1], "s")==0)//server
			{
				shift(y,times);
				l--;
				times--;
				Listsend(master, listener,fdmax,times);
			}
			else
			{
			l--;
			times--;
			int max=0;
			while(List[max].Host_id!=0)
			{
				max++;
			}
			shift(y,max-1);
			}
                        
			}
			else{
				if(y==sender)
				{
				int end=0;
				while(List1[end].Host_id!=0)
				{
					if(List1[end+1].Host_id==0)
					{
					memset(&List1[end], 0, sizeof List1[end]);
					}
					end++;
				}
				int itr1=0;
				updateList(buffer1);
				while(List1[itr1].Host_id!=0)
				{
					printf("%-5d%-35s%-20s%-8s\n",List1[itr1].Host_id, List1[itr1].Hostname, List1[itr1].remoteIP, List1[itr1].portno);
				itr1++;
				}
				
				}
				
			}
			
		
			}
			}
		}
			
						
			
                    
		
		
	}
	
	return 0;
}

void creator()
{
	printf("I have read and inderstood the course academic integrity policy located at http://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity \n");
}

void help()
{

}


char* myip()
{
//Reference: http://stackoverflow.com/questions/212528/get-the-ip-address-of-the-machine
	struct addrinfo googleip, *goo;
	int sock;
	memset(&googleip, 0 , sizeof googleip);
	googleip.ai_family = AF_INET;
	googleip.ai_socktype= SOCK_DGRAM;
	getaddrinfo("8.8.8.8", "53", &googleip, &goo); // Connecting to a google dns server
	sock = socket(goo->ai_family, goo->ai_socktype, 0);
 			if(sock<0)
	{
		perror("socket error:");
	}
	int err = connect(sock, goo->ai_addr, goo->ai_addrlen);
	if(err<0)
	{
		perror("connect error:");
	}
	struct sockaddr_in name;
	socklen_t namelen = sizeof(name);
	int e = getsockname(sock, (struct sockaddr *) &name, &namelen); // Fetching the ip address of the machine
	if(e<0)
	{
		perror("sockname error:");
	}
	
	return inet_ntoa(name.sin_addr);
	close(sock);
} 
int handleconnections(int newfd, int fdmax)
{
// handle new connections
	struct sockaddr_in name1;
	 	int number=0;
				List[l].Host_id=l+1;
				List[l].host_setno=newfd;
				memset(buffer2, 0, sizeof buffer2);
				int n = recv(newfd, &buffer2, sizeof buffer2, 0);
				if (n < 0)
				{
					perror("ERROR reading from socket");
				}
				printf("Got portno: %s\n",buffer2);
				while(List[number].Host_id!=0)
				{
					printf("INNER List: %s buffer: %s\n",List[number].portno, buffer2);
					if(strcmp(List[number].portno, buffer2)==0)
					{
						send(newfd, "Already registered\n",1024,0);
						return 0;
					}
					number++;
				}
				strcpy(List[l].portno, buffer2);
				
				socklen_t namelen1 = sizeof(name1);
				int e = getpeername(newfd, (struct sockaddr *)&name1, &namelen1); 
				if(e<0)
				{
					perror("peername error:");
				}
				char *hj = inet_ntoa(name1.sin_addr);
				strcpy(List[l].remoteIP, hj);
				
				int u = getnameinfo((struct sockaddr *)&name1, sizeof(name1), List[l].Hostname, sizeof(List[l].Hostname), NULL, 0, 0);
				if (u<0)
				{
				 perror("getnameinfo error:\n");
				 exit(1);
				}
				printf("Successfully connected with hostname: %s ID: %d IPaddress: %s on port: %s\n", List[l].Hostname, List[l].Host_id, List[l].remoteIP, List[l].portno);
				l++;
	return newfd;
}
int registerclient(char *serverip, int Sport, char *Lport)
{
	int L_port=atoi(Lport);
	if(L_port!=Sport)
	{
	struct sockaddr_in cli_addr;
	sender = socket(AF_INET, SOCK_STREAM, 0); 
	if(sender<0)
	{
		perror("Error connecting to socket\n");
	}

	cli_addr.sin_family=AF_INET;
	inet_aton(serverip, &cli_addr.sin_addr);
	cli_addr.sin_port=htons(Sport);
	
	int a = connect(sender, (struct sockaddr *)&cli_addr, sizeof cli_addr);	
	if(a<0)
	{
		perror("Connect");
	}
	printf("Connecting....\n");
	int m = send(sender, Lport, sizeof Lport, 0);
	if(m<=0)
	{
		perror("Error in sending port");
	}
	printf("Registering....\n");
	int nm = recv(sender, buffer1, sizeof buffer1, 0);
	if (nm < 0)
	{
		perror("ERROR reading from socket");
	}
	printf("%s\n", buffer1);
	if(strcmp(buffer1, "Already registered\n")==0)
	{
		return sender;
	}
	int q = strcmp(buffer1,"Only 4 connections allowed");
	if(q==0)
	{
		exit(0);
	}
	memset(buffer1,0, sizeof buffer1);
	n = recv(sender, buffer1, sizeof buffer1, 0);
	if (n < 0)
	{
		perror("ERROR reading from socket");
	}
	int itr1=0;
	updateList(buffer1);
	while(List1[itr1].Host_id!=0)
	{
		printf("%-5d%-35s%-20s%-8s\n",List1[itr1].Host_id, List1[itr1].Hostname, List1[itr1].remoteIP, List1[itr1].portno);
		itr1++;
	}
	
	strcpy(buffer1, "");
	
	return sender;
	}
	else
	{
		printf("Self connection\n");
		return sender;
	}
}

void Listsend(fd_set master, int listener, int fdmax, int time)
{
	int j,q;
	fflush(stdout);
	printf("time value: %d\n",time);
	char string[1024];
	strcpy(string, "");
	for(q=0;q<=time;q++)
	{
		strcat(string, List[q].remoteIP);
		strcat(string, " ");
		strcat(string, List[q].Hostname);
		strcat(string, " ");
		strcat(string, List[q].portno);
		strcat(string, "/");
		
	}
	for(j = 0; j <= fdmax+1; j++) 
	{
		
		// send the list to everyone!
		if (FD_ISSET(j, &master))
		{
			// except the listener
			if (j != listener)
			{
				
				send(j, &string, sizeof string, 0);
				
			}
		}
	}
}


int connectclient(char *clientIP, char *clientport, char *Lport)
{
	
	int L_port= atoi(Lport);
	int clientport1=atoi(clientport);
	int clientsock;
	char buffer1[1024];
	int ag=0;
	int reg=0;
	printf("1\n");
	if(L_port!=clientport1)
	{
	while(List[ag].Host_id!=0)
	{	
		printf("List1IP: %s ClientIP: %d\n",List[ag].portno, clientport1);
		if(clientport1==atoi(List[ag].portno))
		{
			reg=1;
		}
		ag++;	
	}
	if(reg==0)
	{
	struct sockaddr_in client_addr;
	
	client_addr.sin_family=AF_INET;
	inet_aton(clientIP, &client_addr.sin_addr);
	client_addr.sin_port=htons(clientport1);
	printf("2\n");
	clientsock = socket(AF_INET, SOCK_STREAM, 0);
	if(sender<0)
	{
		perror("Error connecting to socket\n");
	}
	printf("Sender: %d\n", clientsock);
	int a = connect(clientsock, (struct sockaddr *)&client_addr, sizeof client_addr);
	if ( a< 0)
	{
		perror("ERROR on connect: \n");
	}
	printf("Connecting.....\n");

	List[l].Host_id=l+1;
	List[l].host_setno=clientsock;
	//printf("sender%d\n",sender);
	strcpy(List[l].portno, clientport);
	strcpy(List[l].remoteIP, clientIP);

	int u = getnameinfo((struct sockaddr *)&client_addr, sizeof(client_addr), List[l].Hostname, sizeof(List[l].Hostname), NULL, 0, 0);
	if (u<0)
	{
	 perror("getnameinfo error:\n");
	 exit(0);
	}
	l++;
	int m = send(clientsock, Lport, sizeof Lport, 0);
	if(m<=0)
	{
		perror("Error in sending port");
	}
	memset(buffer1,0,sizeof buffer1);
	int nn = recv(clientsock, buffer1, sizeof buffer1, 0);
	if (nn < 0)
	{
		perror("ERROR reading from socket");
	}
	printf("%s\n", buffer1);
	if(strcmp(buffer1, "Already registered\n")==0)
	{
		return sender;
	}
	memset(buffer1,0,sizeof buffer1);
	
	int q = strcmp(buffer1,"Only 3 peers allowed");
	
	int w = strcmp(buffer1,"Client is not registered");
		
	if(q==0 || w==0)
	{
		close(clientsock);
		exit(0);
	}
	
	}
	else
	{
		printf("Already registered\n");
	}
	}
	else
	{
		printf("Self connection\n");
	}
return 0;
}

void shift(int closing, int time)
{
	int i, j;
	
	for(i=0;i<5;i++)
	{
		if(List[i].host_setno==closing)
		{
			
			for(j=i;j<=time;j++)
			{
				
				if(List[j+1].host_setno==0)
				{
					
					memset(&List[j], 0, sizeof List[j]);
					
					break;
				}
				List[j].Host_id = j+1;
				List[j].host_setno=List[j+1].host_setno;
				strcpy(List[j].Hostname, List[j+1].Hostname);
				strcpy(List[j].remoteIP, List[j+1].remoteIP);
				strcpy(List[j].portno, List[j+1].portno);
				
				
			}
			
		}
	}
}
void updateList(char string1[1024])
{
	char **totalList=calloc(20,sizeof(char *));
	char *token1;
	int itr=0;
	int op=0;
	int x;
	token1 = strtok(string1, " /");
	printf("SERVER List of all the registered clients:\n");
	while(token1!=NULL)
	{
		totalList[itr] = token1;
		itr++;
		token1 = strtok(NULL, " /");
	}
	for(x=0;x<((itr+1)/3);x++)
	{
		List1[x].Host_id = x+1;
		strcpy(List1[x].remoteIP,totalList[op]);
		op++;
		strcpy(List1[x].Hostname, totalList[op]);
		op++;
		strcpy(List1[x].portno, totalList[op]);
		op++;
	}
	free(totalList);
}


void terminate(int id)
{
	if(id!=1)
	{
	int iter=0;
	int max=0;
	while(List[max].Host_id!=0)
	{
		max++;	
	}
	
	while(List[iter].Host_id!=0)
	{
		if(List[iter].Host_id==id)
		{
			int closed=close(List[iter].host_setno);
			if(closed==0)
			{
				printf("Terminated the connection no %d\n",id);
				shift(List[iter].host_setno,max-1);
				l--;
			}
		}
			
		iter++;
	}
	}
	else
	{
		printf("Cant terminate the server.\n");
	}
}








void upload(int ID, char *filename)// Reference: http://www.linuxquestions.org/questions/programming-9/tcp-file-transfer-in-c-with-socket-server-client-on-linux-help-with-code-4175413995/
{
		
	char sdbuf[512]; 
	printf("[Client] Sending %s to the Client1... ", filename);
	FILE *fs = fopen(filename, "r");
	if(fs == NULL)
	{
		printf("ERROR: File %s not found.\n", filename);
		exit(1);
	}
	memset(sdbuf, 0 , sizeof sdbuf); 
	int fs_block_sz; 
	while((fs_block_sz = fread(sdbuf, sizeof(char), sizeof sdbuf, fs)) > 0)
	{
	    if(send(ID, sdbuf, fs_block_sz, 0) < 0)
	    {
	        perror("ERROR: Failed to send file . \n");
	        break;
	    }
	    memset(sdbuf, 0 , sizeof sdbuf);
	}
	printf("Ok File %s from Client2 was Sent!\n", filename);
}

//FILE *fr = fopen(token3, "a");
		//if(fr == NULL)
		//	printf("File %s Cannot be opened file on server.\n", token3);
		//else
		//{
		///	char revbuf[512]; 
		//	memset(revbuf, 0, sizeof revbuf); 
		//	int fr_block_sz = 0;
		//	while((fr_block_sz = recv(nsockfd, revbuf, LENGTH, 0)) > 0) 
		//	{
		//	    int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
		//		if(write_sz < fr_block_sz)
		//	    {
		//	        error("File write failed on server.\n");
		//	    }
		//		bzero(revbuf, LENGTH);
		//		if (fr_block_sz == 0 || fr_block_sz != 512) 
		//		{
		//			break;
		//		}
		//	}
		//	if(fr_block_sz < 0)
		  //  {
		    //    if (errno == EAGAIN)
	        	//{
	        //        printf("recv() timed out.\n");
	          //  }
	         //   else
	         //   {
	          //      fprintf(stderr, "recv() failed due to errno = %d\n", errno);
		//			exit(1);
	         //   }
        	//}
		//	printf("Ok received from client!\n");
		//	fclose(fr); 
		//}
				
		//}
