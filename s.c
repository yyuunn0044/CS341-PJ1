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
#include <ctype.h>
unsigned short verify_checksum(){
	return 0xffff;
}

unsigned short calc_checksum(unsigned size, const char *buf);
void data_processing(unsigned char op, unsigned char shift, 
						unsigned int length, char * src, char * dst);

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

	char *input_msg;
	char *msg;
	char data[255];
	char path[255];


	int i=0;
	unsigned char op=0;
	unsigned char shift=0;
	unsigned short checksum=0;
	unsigned int length=0;
	unsigned int newlen=0;
	

	client_len=sizeof(c_addr);
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
	input_msg=calloc(10*1000*1000, sizeof(char));

while(1){
	if (recv(c_sock, input_msg, 10*1000*1000, 0) <= 0){
		printf("server read fail\n");
		break;
		free(input_msg);
		exit(0);
	}

	
//	if (verify_checksum()){
//		printf("server checksum error!!\n");
//		free(input_msg);
//		exit(0);
//	}
	
	while(i<8){ printf("%x ", input_msg[i]); i++;} printf("\n");

	while(i<16){ printf("%x ", input_msg[i]); i++;} printf("\n");

	while(i<24){ printf("%x ", input_msg[i]); i++;} printf("\n");

	while(i<32){ printf("%x ", input_msg[i]); i++;} printf("\n");

	while(i<40){ printf("%x ", input_msg[i]); i++;} printf("\n");

	while(i<48){ printf("%x ", input_msg[i]); i++;} printf("\n");

	while(i<56){ printf("%x ", input_msg[i]); i++;} printf("\n");

	while(i<64){ printf("%x ", input_msg[i]); i++;} printf("\n");

	msg=calloc(10*1000*1000,sizeof(char));
	
	memcpy(&op, input_msg, 1);
	memcpy(&shift, input_msg+1, 1);
	memcpy(&newlen, input_msg+4, 4);
	length=ntohl(newlen);
	
	data_processing(op, shift, length, input_msg+8, msg+8);
	
	memcpy(msg, &op, 1);
	memcpy(msg+1, &shift, 1);
	memcpy(msg+4, &length, 4);
//	memcpy(msg+8, input_msg+8, length); //data
	checksum=calc_checksum(length, msg);
	memcpy(msg+2, &checksum, 2);  //checksum

	write(c_sock, msg, length);

}
	free(msg);
	free(input_msg);

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

void data_processing
(unsigned char op, unsigned char shift_, unsigned int length, char * src, char * dst){
	int i=0;
	short shift = shift_%26;
	short tmp = 0;
	printf("op: %d\n", op);
	printf("shift: %d\n", shift);

	printf("length: %d\n", length);

	printf("src: %c\n", src);

		
	
	if(op)
		shift=-shift;

	for(i=0;i<length-8;i++){
		tmp = (short)src[i];
		if (tmp>64 && tmp<91) tmp+=32;
		if (tmp>96 && tmp<123){
			tmp+=shift;
			if (tmp<=96) tmp+=26;
			else if (src[i]>=123) tmp-=26;
			
		}
		dst[i] = tmp;
		printf("%d, %d\n", src[i], dst[i]);
	}
}




unsigned short calc_checksum(unsigned size, const char *buf)
{
	unsigned long long sum = 0;
	const unsigned long long *b = (unsigned long long *) buf;

	unsigned t1, t2;
	unsigned short t3, t4;

	/* Main loop - 8 bytes at a time */
	while (size >= sizeof(unsigned long long))
	{
		unsigned long long s = *b++;
		sum += s;
		if (sum < s) sum++;
		size -= 8;
	}

	/* Handle tail less than 8-bytes long */
	buf = (const char *) b;
	if (size & 4)
	{
		unsigned s = *(unsigned *)buf;
		sum += s;
		if (sum < s) sum++;
		buf += 4;
	}

	if (size & 2)
	{
		unsigned short s = *(unsigned short *) buf;
		sum += s;
		if (sum < s) sum++;
		buf += 2;
	}

	if (size)
	{
		unsigned char s = *(unsigned char *) buf;
		sum += s;
		if (sum < s) sum++;
	}

	/* Fold down to 16 bits */
	t1 = sum;
	t2 = sum >> 32;
	t1 += t2;
	if (t1 < t2) t1++;
	t3 = t1;
	t4 = t1 >> 16;
	t3 += t4;
	if (t3 < t4) t3++;

	return ~t3;
}
