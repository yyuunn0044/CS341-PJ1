#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	int s_sock;
	int c_sock;
	int state = 0;
	int pid;
	int filesize=0;
	int readed, total=0;
	int client_len;
	struct sockaddr_in s_addr;
	struct sockaddr_in c_addr;
	FILE *fp;

	char buf[5000];
	char filename[255];
	char path[255];


	int i=0;
	DIR *dp;
	struct dirent *dir;


//	client_len=sizeof(clientaddr);
//	if (argc>1){
//		printf("============Server need not argument, Anyway..\n");  //fs_server gave argument?
//	}

	printf("============Server Start\n");
	s_sock=socket(AF_INET, SOCK_STREAM, 0); //make socket
	if ( s_sock<0 ){
		printf("============Socket Error\n");
		exit(0);
	
	}

	memset(&s_addr, 0, sizeof(s_addr));       //assignments to characters of serveraddr
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_port = htons(atoi(argv[1]));

	state = bind(s_sock, (struct sockaddr *)&s_addr, sizeof(s_addr));

	if (state == -1){
		printf("============Bind Error\n");
		exit(0);
	}




	state = listen(s_sock, 30);               //wait approach from client socket
	if (state == -1){
		printf("============Listen Error\n");
		exit(0);
	}
	printf("============OK Server\n");

	c_sock = accept(s_sock, (struct sockaddr *)&c_addr, &client_len); //make other socket to communicate with client

	if (c_sock == -1){
		printf("============Accept Error\n");
		exit(0);
	}



	while(i<4){
		memset(buf, 0, 4096);
		if (read(c_sock, buf, 4096) <= 0){
			exit(0);
		}
		printf("%s",buf);
		i++;
	}





/*
	
	while(1){
		state = listen(ssock, 10);               //wait approach from client socket
		if (state == -1){
			printf("============Listen Error\n");
			exit(0);
		}
		printf("============OK Server\n");
		fork();  //fork process -> server will not die  
		csock = accept(ssock, (struct sockaddr *)&clientaddr, &client_len); //make other socket to communicate with client

		if (csock == -1){
			printf("============Accept Error\n");
			exit(0);
		}


		memset(buf, '0', 255);
		if (read(csock, buf, 255) <= 0){
			break;
		}


		if (strncmp(buf, "-l",2) == 0){
			dp=opendir("./ftpData");    //open directory to read elements
			if(dp==NULL){               //there isn't directory?
				printf("Received Query: 1, no file\n");
				printf("List called!\n");
				printf("No directory, create new one. . .\n");
				mkdir("./ftpData", 0777);    //make the directory "ftpData"
				send(csock, "end\n", 4, 0);  //send string "end" to represent end of precess
				continue;
			}
			else{
				printf("Received Query: 1\n"); //there is directory
				printf("List called!\n");
				while(1){
					memset(buf, 0, 10000);
					dir=readdir(dp);
					if (dir==NULL)  //read all the elements -> dir become NULL
					{
						close(csock);
						break;
					}
					sprintf(buf, dir->d_name);  //copy the name of element to buffer
					printf("%s\n",buf);
					send(csock, buf, strlen(dir->d_name),0); //send the name
					send(csock, "\n", 1, 0);  // send enter to represent end of name
				}
				closedir(dp);
				continue;
			}	   
		}
		else if(strncmp(buf, "-u",2)==0){
			read(csock, buf, 255);         //receive argv[4], the name of file
			printf("Received Query: 2, %s\n", buf);
			printf("Upload called!\n");
			memset(buf, 0, sizeof(buf));
			memset(path, 0, sizeof(path));
			memset(filename, 0, sizeof(filename));
			if (recv((csock), filename, 255, 0)<0){
				close(csock);
				continue;
			}
			sprintf(path, "./ftpData/");
			strncat(path,filename,strlen(filename)); //make path of file by adding "./ftpData/"

			fp=fopen(path, "wb");
			if (recv(csock, (char *)&filesize, sizeof(filesize),0)<0){ 
				close(csock);
				continue;
			}
			while(filesize>0){
				readed=recv(csock, buf, filesize, 0); //open file and read until read characters == size of file
				if(readed<0){
					close(csock);
					exit(-1);
				}
				else
					fwrite(buf, 1, readed, fp); //write characters to file, to make copy of origin file
				filesize-=readed;
			}
			
			fclose(fp);
			continue;

		}

		else if(strncmp(buf, "-d",2)==0){
			read(csock, buf, 255);                     //receive the name of file, argv[4]
			printf("Received Query: 3, %s\n",buf);
			printf("Download called!\n");
			memset(buf, 0, 10000);
			memset(path, 0, 255);
			memset(filename, 0,255);
			if (recv(csock, filename, 255, 0)<0){
				close(csock);
				continue;
			}
			sprintf(path, "./ftpData/");
			strncat(path, filename, strlen(filename));
			fp=fopen(path, "rb");
			if(fp==NULL){
				close(csock);
				continue;
			}
			else{
				fseek(fp, 0, SEEK_END);  //check the size of file
				filesize=ftell(fp);
			}
			if (send(csock, (char *)&filesize, sizeof(filesize), 0)<0){
				close(csock);
				continue;
			}
			rewind(fp); //rewind file pointer to start point
			while(1){
				readed=fread(buf, 1, filesize-total, fp); //read from file and send the characters
				if (readed>0){
					if(send(csock, buf, readed, 0)<0){
						close(csock);
						break;
					}
					memset(buf, 0, 10000);
					total+=readed;
				}
				else if(total==filesize)
					break;
				else
					break;
			}
			fclose(fp);
			continue;
		}
		break;

	}
*/	close(c_sock);
	close(s_sock);
	exit(0);
}


