#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include<ctype.h>

char *com;
char* commandcheck(char*, int,int);

void udpserver(int portno)
{
	printf("in server\n");
	int listenfd = 0;

	struct sockaddr_in serv_addr,client_addr;

	char sendBuff[1025]="";
	char recvBuff[1025]="";

	if((listenfd = socket(AF_INET, SOCK_DGRAM, 0))<0)
	{
		printf("Connection not established\n");
		return;
	}
	printf("socket retrieve success\n");

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if(bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(struct sockaddr))<0)
	{
		printf("Unable to bind\n");
		return;
	}

	int size;
	char otp[1000];
	fflush(stdout);
	while(1)
	{
		size=sizeof(struct sockaddr_in);
		socklen_t *l=(socklen_t*)&size;
		int num;
		while(1)
		{
			//char *reply;
			if((num = recvfrom(listenfd, recvBuff, 1024, 0,(struct sockaddr*)&serv_addr,l))==-1)
			{
				perror("recv");
			}
			else if (num ==0)
			{
				printf(" closed \n");
			}

			recvBuff[num] = '\0';
			printf("*%s*\n", recvBuff);
			strcpy(otp,commandcheck(recvBuff, listenfd,1));
			if(strcmp(otp,"ERR")==0)
			{
				strcpy(otp,"File does not exist\n");
			}
			recvBuff[0] = '\0';
			if((sendto(listenfd,otp,strlen(otp),0,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr)))==-1)
			{
				fprintf(stderr,"failure sending command output\n");
			}
			else
			{
				printf("output sent\n");
			}
			fflush(stdout);
			sleep(1);
		}
	}
	close(listenfd);

}
void udpclient(int portno)
{
	printf("in client\n");
	int connfd;
	char sendBuff[1024];
	char recvBuff[1024];
	int sockfd;
	struct sockaddr_in serv_addr;
	if((connfd = socket(AF_INET, SOCK_DGRAM, 0))< 0)
	{
		printf("\n Error : Could not create socket \n");
		return;
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int size=sizeof(struct sockaddr_in);
	socklen_t *l=(socklen_t*)&size;
	int num;
	while(1)
	{
		gets(sendBuff);
		puts(sendBuff);
		
		if ((sendto(connfd, sendBuff, strlen(sendBuff), 0,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr)))==-1)
		{
			fprintf(stderr, "Failure Sending Message\n");
		}
		else
		{
			printf("Message sent ");
		}

		if((num = recvfrom(connfd, recvBuff, 1024, 0,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr))) == -1)
		{
			perror(" recv error ");
		}
		else if (num==0)
		{
			printf("closed\n");
		}

		recvBuff[num] = '\0';
		char *to;
		printf("\n%s\n", recvBuff);
		if(strstr(sendBuff,"FileDownload"))
		{
			to=strtok(sendBuff," ");
			to=strtok(NULL," ");
			printf("yes\n");
			puts(to);
			if(strcmp(recvBuff,"ERR")==0)
			{
				printf("No such file exists\n");
			}
			else
			{
				FILE *f=fopen(to,"w");
				int n=strlen(recvBuff);
				int i;
				for(i=0;i<n;i++)
				{
					fprintf(f,"%c",recvBuff[i]);
					printf("%c",recvBuff[i]);
				}
				printf("Successfully downloaded file\n");
				fclose(f);
			}
		}
		recvBuff[0] = '\0';
	}
}
void tcpserver(int portno)
{
	printf("in server\n");
	int listenfd = 0,connfd = 0;

	struct sockaddr_in serv_addr,client_addr;

	char sendBuff[1025]="";
	char recvBuff[1025]="";

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		printf("Connection not established\n");
		return;
	}
	printf("socket retrieve success\n");

	memset(&serv_addr, '0', sizeof(serv_addr));
//	memset(sendBuff, '0', sizeof(sendBuff));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if(bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(struct sockaddr))<0)
	{
		printf("Unable to bind\n");
		return;
	}

	if(listen(listenfd, 10) == -1){
		printf("Failed to listen\n");
		return;
	}
	int size;
	fflush(stdout);
	while(1)
	{
		size=sizeof(struct sockaddr_in);
		socklen_t *l=(socklen_t*)&size;
		connfd = accept(listenfd, (struct sockaddr*)&client_addr ,l);
		//	perror("not accepting");
		//else
		//	printf("random\n");
		//connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL); // accept awaiting request
		//write(connfd,"hello",6);
		//fflush(stdout);
		int num;
		while(1)
		{
			//char *reply;
			if((num = recv(connfd, recvBuff, 1024, 0))==-1)
			{
				perror("recv");
			}
			else if (num ==0)
			{
				printf(" closed \n");
			}

			recvBuff[num] = '\0';
			printf("*%s*\n", recvBuff);
			char otp[1024];
			strcpy(otp,commandcheck(recvBuff, connfd,0));
			recvBuff[0] = '\0';
			fflush(stdout);
			sleep(1);
		}
	}
	close(connfd);
}

void tcpclient(int portno)
{
	printf("in client\n");
	int connfd;
	char sendBuff[1024];
	char recvBuff[1024];
	int sockfd;
	struct sockaddr_in serv_addr;
	if((connfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
	{
		printf("\n Error : Could not create socket \n");
		return;
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int size=sizeof(struct sockaddr_in);
	socklen_t *l=(socklen_t*)&size;
	if(connect(connfd,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr))==-1)
	{
		//printf("No connection\n");
//		return;
	}
	//printf("Got connection\n");
		//printf("waiting for scan\n");
		//scanf("%[^\n]",sendBuff);
	int num;
	while(1)
	{
		gets(sendBuff);
	//	getchar();
		//printf("writing ");
		puts(sendBuff);
		
		if ((send(connfd, sendBuff, strlen(sendBuff), 0))==-1)
		{
			fprintf(stderr, "Failure Sending Message\n");
		}
		else
		{
			printf("Message sent ");
		}

		if((num = recv(connfd, recvBuff, 1024, 0)) == -1)
		{
			perror(" recv error ");
		}
		else if (num==0)
		{
			printf("closed\n");
		}
		char *to;
		recvBuff[num] = '\0';
		printf("\n%s\n", recvBuff);
		if(strstr(sendBuff,"FileDownload"))
		{
			to=strtok(sendBuff," ");
			to=strtok(NULL," ");
			printf("yes\n");
			puts(to);
			if(strcmp(recvBuff,"ERR")==0)
			{
				printf("No such file exists\n");
			}
			else
			{
				FILE *f=fopen(to,"w");
				int n=strlen(recvBuff);
				int i;
				printf("The length of the received buffer is %d\n",strlen(recvBuff));
				for(i=0;i<n;i++)
				{
					fprintf(f,"%c",recvBuff[i]);
					printf("%c",recvBuff[i]);
				}
				printf("Successfully downloaded file\n");
				fclose(f);
			}
		}
		/*if(strstr(sendBuff,"FileDownload"))
		{
			if(strcmp(recvBuff,"ERR")==0)
			{
				printf("No such file exists\n");
			}
			else
			{
				int f=open(sendBuff[1],"w");
				int n=strlen(recvBuff);
				int i;
				for(i=0;i<n;i++)
					fprintf(f,"%c",recvBuff[i]);
				printf("Successfully downloaded file\n");
			}
		}*/
		recvBuff[0] = '\0';



		//write(connfd,sendBuff,strlen(sendBuff));
		//printf("Write done\n");
		//read(connfd,recvBuff,strlen(recvBuff));
		//puts("received ");
		//puts(recvBuff);
	}
		//printf("reading ");
		//puts(recvBuff);
		//puts(recvBuff);
//	close(connfd);
}


char* commandcheck(char *recvBuff, int connfd,int type)
{
	char otp[1024];
	strcpy(otp,"");
	int x=0;
	char command[4][15];
	char tempo[100];
	com=strtok(recvBuff, " ");
	while(com!=NULL)
	{
		strcpy(command[x],com);
		puts(command[x]);
		x++;
		com=strtok(NULL, " ");
	}

	char *te=(char *)malloc(500*sizeof(char));;
	char *str=(char *)malloc(500*sizeof(char));
	if (strcmp(command[0], "IndexGet")==0)
	{
		system("ls -l client2/ --time-style=+%Y%m%d%H%M%S -t | cut -d' ' -f5,6,7,8,9 > server/ls");
		fflush(stdout);
		int x;
		x=fopen("server/ls","r");
		fgets(str, 500, x);
		if (strcmp(command[1], "ShortList")==0)
		{
			//command[2] starting timestamp
			//command[3] ending timestamp
			while(fgets(str,500,x)!=NULL)
			{
				strcat(otp, "Size ---- Last Modified ---- Filename\n");
				strcpy(te,str);
				com=strtok(str," ");
				com=strtok(NULL," ");
				if(atoll(com)>=atoll(command[2]) && atoll(com)<=atoll(command[3]))
				{
					puts(te);
					//strcat(otp, "Size ---- Last Modified ---- Filename");
					strcat(otp,te);
				}
			}
		}
		else if(strcmp(command[1], "LongList")==0)
		{
			strcat(otp, "Size ---- Last Modified ---- Filename\n");
			while(fgets(str,500,x)!=NULL)
			{
				puts(str);
				//strcat(otp, "Size ---- Last Modified ---- Filename");
				strcat(otp,str);
			}
		}
		else if(strcmp(command[1], "RegEx")==0)
		{
			int n=strlen(command[2]);
			int i;
			char temp[100];
			int count=0;
			char *t;
//			puts(command[2]);
			/*for(i=0;i<n;i++)
			{
				if(isalnum(command[2][i]))
					temp[count++]=command[2][i];
			}
			temp[count++]='\0';*/
			chdir("client2/");
			system("pwd");
			strcpy(temp,"ls -l --time-style=+%Y%m%d%H%M%S -t ");
			strcat(temp,command[2]);
			strcat(temp," | cut -d' ' -f5,6,7,8,9 > ../server/regexfile");
		//	strcpy(temp,"ls -l ");
		//	strcat(temp,command[2]);
	//		strcat(temp," > server/regexfile");
			system(temp);
			chdir("..");
			char reg[100];
			int y=fopen("server/regexfile","r");
			fflush(stdout);
			strcat(otp, "Size ---- Last Modified ---- Filename\n");
			while(fgets(reg,100,y)!=NULL)
			{
				strcat(otp,reg);
			}
		}
		else
		{
			printf(" Invalid Command for IndexGet\n ");
		}
	}
	else if(strcmp(command[0], "FileHash")==0)
	{
		system("ls -l client2/ --time-style=+%Y%m%d%H%M%S -t | cut -d' ' -f6,7,8,9 > server/ls2");
		fflush(stdout);
		int x;
		x=fopen("server/ls2","r");
		fgets(str, 500, x);
		strcpy(te,str);
		char *dates;
		char *co;
		dates=(char *)malloc(sizeof(char)*500);
		strcat(otp, "Filename ----  Last Modified ---- MD5 Hash ---- Location\n");
		while(fgets(str,500,x)!=NULL)
		{
		//	strcat(otp, "Filename ----  Last Modified ---- MD5 Hash\n");
			strcpy(te,str);
			co=strtok(str," ");
			strcpy(dates,co);
			co=strtok(NULL," ");
			char comm[100];
			strcpy(comm,"md5sum client2/");
			strcat(comm,strtok(co,"\n"));
			strcat(comm," > server/hashfile");
			system(comm);
			fflush(stdout);
			int y=fopen("server/hashfile","r");
			char hash[50];
			fgets(hash,50,y);
			if(strcmp(command[1],"Verify")==0)
			{
				//printf("1\n");
				if(strcmp(command[2],co)==0)
				{
					printf("%s\t%s\t%s\n",co,dates,hash);
					//strcat(otp, "Filename ---- Last Modified ---- MD5 Hash");
					strcat(otp,co);
					strcat(otp,"\t");
					strcat(otp,dates);
					strcat(otp,"\t");
					strcat(otp,hash);
					strcat(otp,"\t");
				}
			}
			else if(strcmp(command[1],"CheckAll")==0)
			{
			//	printf("2\n");
				printf("%s\t%s\t%s\n",co,dates,hash);
				//strcat(otp, "Filename ---- Last Modified ---- MD5 Hash");
				strcat(otp,co);
				strcat(otp,"\t");
				strcat(otp,dates);
				strcat(otp,"\t");
				strcat(otp,hash);
				strcat(otp,"\t");
			}
			else
			{
			//	printf("3\n");
				printf("Incorrect request\n");
				strcat(otp,"\nIncorrect request\n");
			}

		}
		puts(otp);
		
	}
	else if(strcmp(command[0], "FileDownload")==0)
	{
		printf("*%s*\n",command[1]);
		FILE *f;
		char line[200];
		char ch='a';
		char path[100];
		chdir("client2");
		system("pwd");
		system("ls");
		printf("*%s*",command[1]);
		if((f=fopen(command[1],O_RDONLY))==NULL)
			perror("can't open");
		int index=0;
		while((ch=fgetc(f))!=EOF)
			otp[index++]=ch;
		otp[index++]='\0';
		//command[1] name of file
/*		if((f=fopen(command[1],"r"))==NULL)
		{
			return "ERR";
		}
		while(fgets(line,200,f))
		{
			puts(line);
			strcat(otp,line);
		}*/
		//return otp;
		puts(otp);
		chdir("..");
	}
	else if(strcmp(command[0], "FileUpload")==0)
	{
		//command[1] name of file
	}
	else
	{
		printf("Incorrect command\n");
		return;
	}
	puts(otp);
	if(type==0)
	{
		if((send(connfd, otp, strlen(otp), 0))==-1)
		{
			fprintf(stderr, "Failure Sending Command Output\n");
		}
		else
		{
			printf("Output Sent\n");
		}
	}
	else
	{
		return otp;
	}


}


int main(int argc,char **argv)
{
	int flag=0;	//0 for tcp, 1 for udp
	char command2[25];
	if(argc==1)
	{
		printf("Enter arguments\n");
		return 0;
	}
	char s1[10],s2[10];
	strcpy(s1,"tcp");
	strcpy(s2,"udp");
	int sockfd = 0,n = 0;
	char recvBuff[1024];
	int pid;
	struct sockaddr_in serv_addr;
	memset(recvBuff, '0' ,sizeof(recvBuff));
	if(strcmp(argv[1],"tcp")==0)
	{
		pid=fork();
		if(pid==0)
		{
			printf("child\n");
			tcpserver(atoi(argv[2]));
		}
		else
		{
			printf("parent\n");
			tcpclient(atoi(argv[3]));
		}
	}
	else if(strcmp(argv[1],"udp")==0)
	{
		flag=1;
		pid=fork();
		if(pid==0)
			udpserver(atoi(argv[2]));
		else
			udpclient(atoi(argv[3]));
	}

	else
	{
		printf("Incorrect argument\n");
		return 0;
	}
	

	return 0;
}
