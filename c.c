#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned short calc_checksum(unsigned length, const char *buf);

int main(int argc, char **argv){
	int s_sock, c_sock;
	struct sockaddr_in s_addr;
	
	char buf[1009];
	char *msg;
	char data[1001];
	int i=0;
	unsigned char op=0;
	unsigned char shift=3;
	unsigned long length=0;
	unsigned long newlen=0;
	unsigned short checksum=0;

	c_sock=socket(AF_INET, SOCK_STREAM, 0); //make client socket
	if (c_sock<0){
		printf("Socket Error\n");
		exit(0);
	}
	s_addr.sin_addr.s_addr=inet_addr("143.248.56.16");
	s_addr.sin_family=AF_INET;
	s_addr.sin_port=htons(3000);
	s_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	s_addr.sin_family=AF_INET;
	s_addr.sin_port=htons(atoi(argv[1]));


	if ( connect(c_sock, (struct sockaddr *)&s_addr, sizeof(s_addr)) <0 ){
		close(c_sock);
		exit(0);
	}
	
	while(fgets(data, 1000, stdin)){ //read data with 'null '

				
		length=strlen(data)+9; //include null!!!!!!!!!
		msg=calloc(1009, sizeof(char));
		memset(msg, 0, sizeof(msg));

		memcpy(msg, &op, 1);


		memcpy(msg+1, &shift, 1);
		newlen=htonl(length);
		memcpy(msg+4, &newlen, 4);

		memcpy(msg+8, data, strlen(data)+1);

		checksum=calc_checksum(length, msg);
//		printf("half checksum: %6hx\n", checksum);

		memcpy(msg+2, &checksum, 2);
/*			while(i<8){ printf("%x ", msg[i]); i++;} printf("\n");
		
		while(i<16){ printf("%x ", msg[i]); i++;} printf("\n");
		
		while(i<24){ printf("%x ", msg[i]); i++;} printf("\n");
		
		while(i<32){ printf("%x ", msg[i]); i++;} printf("\n");
					
		while(i<40){ printf("%x ", msg[i]); i++;} printf("\n");
		
		while(i<48){ printf("%x ", msg[i]); i++;} printf("\n");
		
		while(i<56){ printf("%x ", msg[i]); i++;} printf("\n");
		
		while(i<64){ printf("%x ", msg[i]); i++;} printf("\n");
		
		i=0;		
		
*/	
		checksum=calc_checksum(length, msg);

//		printf("whole checksum: %6hx\n", checksum);	
		write(c_sock, msg, length);
		read(c_sock, buf, length);
	//	recv(c_sock, buf, sizeof(buf), 0);
/*		while(i<8){ printf("%x ", buf[i]); i++;} printf("\n");
		
		while(i<16){ printf("%x ", buf[i]); i++;} printf("\n");
		
		while(i<24){ printf("%x ", buf[i]); i++;} printf("\n");
		
		while(i<32){ printf("%x ", buf[i]); i++;} printf("\n");
			while(i<40){ printf("%x ", buf[i]); i++;} printf("\n");
		
		while(i<48){ printf("%x ", buf[i]); i++;} printf("\n");
		
		while(i<56){ printf("%x ", buf[i]); i++;} printf("\n");
		
		while(i<64){ printf("%x ", buf[i]); i++;} printf("\n");
*/	
		memcpy(&length, buf+4, 4);
		newlen=ntohl(length);

		checksum=calc_checksum((unsigned)18, buf);
//		printf("\nchecksum verify(have to ffff): %x\n", checksum);

		buf[2]=0;
		buf[3]=0;
		checksum=calc_checksum(18, buf);
//		printf("calc half checksum of encrypted: %x\n", checksum);

//printf("\n");
//		
//if (~checksum){
//			printf("checksum error!!!\n");
//			close(c_sock);
//			exit(0);
//		}

		printf("%s", buf+8);
		free(msg);
	}


/*

	write(c_sock, "filename is...", strlen("filename is...")+1);
	while(fgets(filename, 4096, stdin)){
		write(c_sock, filename, strlen(filename)+1); 
		i++;
	}*/
/*
	else if(strncmp(argv[3], "-u", 2)==0){
		write(csock, strcat(argv[4],"\0"),strlen(argv[4])+1); // receive name of upload file
		printf("============This is the upload function\n");
		printf("Sent command: 2\n");
		printf("Sent filename: %s\n",argv[4]);
		memset(buf, 0, 10000);
		memset(filename, 0, 30);
		if (send(csock, argv[4], strlen(argv[4]), 0)<0)
			exit(0);


		sprintf(filename, "./");
		strncat(filename,argv[4], strlen(argv[4]));
		
		fp=fopen(filename, "rb");


		fseek(fp, 0, SEEK_END);   //save size of file
		filesize=ftell(fp);		
		
		if (send(csock, (char *)&filesize, sizeof(filesize),0)<0)
			exit(0);

		rewind(fp); //rewind file pointer to start
		while(1){
			readed = fread(buf, 1, filesize-total, fp); //read the file and send the characters

			if (readed>0){
				if(send(csock, buf, readed, 0)<0)
					exit(0);
				memset(buf, 0, 10000);
				total+=readed;
			}
			else if(total==filesize){
				fclose(fp);
				printf("============End Upload!\n");
				exit(0);
			}

		}
		printf("============End upload!\n");

	}
	else if(strncmp(argv[3], "-d", 2)==0){
		write(csock, strcat(argv[4], "\0"), strlen(argv[4])+1);
		printf("============This is the download function\n");
		printf("Sent command: 3\n");
		printf("Sent filename: %s\n",argv[4]);

		total=0;
		memset(buf, 0, 255);
		memset(path, 0, 255);
		memset(filename, 0,255);
		if (send(csock, argv[4], strlen(argv[4]), 0)<0){
			close(csock);
			exit(0);
		}
		
		sprintf(filename, "./");
		strncat(filename,argv[4], strlen(argv[4]));


		fp=fopen(filename, "wb");
		if(fp==NULL){
			close(csock);
			exit(0);
		}
		if (recv(csock, (char *)&filesize, sizeof(filesize),0)<0){
			close(csock);
			exit(0);
		}

		while(filesize>0){
			readed=recv(csock, buf, filesize, 0); //receive characters and write to new file
			if(readed<0){
				close(csock);
				exit(0);
			}
			else{
				fwrite(buf, 1, readed, fp);
			}
			filesize-=readed;
		}
		fclose(fp);
		printf("============End Download!\n");

	}

	else
		printf("Unproper Command\n");
*/
	close(c_sock);
	
	exit(0);
}
unsigned short calc_checksum(unsigned length, const char *buf)
{
	unsigned long long sum = 0;
	const unsigned long long *b = (unsigned long long *) buf;

	unsigned t1, t2;
	unsigned short t3, t4;

	/* Main loop - 8 bytes at a time */
	while (length >= sizeof(unsigned long long))
	{
		unsigned long long s = *b++;
		sum += s;
		if (sum < s) sum++;
		length -= 8;
	}

	/* Handle tail less than 8-bytes long */
	buf = (const char *) b;
	if (length & 4)
	{
		unsigned s = *(unsigned *)buf;
		sum += s;
		if (sum < s) sum++;
		buf += 4;
	}

	if (length & 2)
	{
		unsigned short s = *(unsigned short *) buf;
		sum += s;
		if (sum < s) sum++;
		buf += 2;
	}

	if (length)
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
unsigned short calc_checksum2(unsigned size, const char *buf)
{
	unsigned sum = 0;
	int i;

	/* Accumulate checksum */
	for (i = 0; i < size - 1; i += 2)
	{
		unsigned short word16 = *(unsigned short *) &buf[i];
		sum += word16;
	}

	/* Handle odd-sized case */
	if (size & 1)
	{
		unsigned short word16 = (unsigned char) buf[i];
		sum += word16;
	}

	/* Fold to get the ones-complement result */
	while (sum >> 16) sum = (sum & 0xFFFF)+(sum >> 16);

	/* Invert to get the negative in ones-complement arithmetic */
	return ~sum;
}
