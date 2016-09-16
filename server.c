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

unsigned short calc_checksum(unsigned size, const char *buf, int verify);
void data_processing(unsigned char op, unsigned char shift, 
						unsigned int length, char * src, char * dst);

int main(int argc, char **argv)
{
	int s_sock;
	int c_sock;
	int state=0;
	int client_len;
	struct sockaddr_in s_addr;
	struct sockaddr_in c_addr;

	char *input_msg;  // received packet from client
	char *msg;        // will be sent to client (header+ciphertext)

	int i=0;
	unsigned char op=0;
	unsigned char shift=0;
	unsigned short checksum=0;
	unsigned int length=0;
	unsigned int newlen=0;


	client_len=sizeof(c_addr);

	s_sock=socket(AF_INET, SOCK_STREAM, 0); //make server socket
	if ( s_sock<0 ){
		printf("============Socket Error\n");
		exit(0);
	}
	memset(&s_addr, 0, sizeof(s_addr)); 
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_port = htons(atoi(argv[1]));
	state = bind(s_sock, (struct sockaddr *)&s_addr, sizeof(s_addr));
	if (state == -1){
		printf("============Bind Error\n");
		exit(0);
	}
	while(1){
		state = listen(s_sock,10000000);  //wait approach from client socket
		if (state == -1){
			printf("============Listen Error\n");
			exit(0);
		}
		fork();
		c_sock = accept(s_sock, (struct sockaddr *)&c_addr, &client_len); //make other socket to communicate with client
		if (c_sock == -1){
			printf("============Accept Error\n");
			exit(0);
		}
		
		input_msg=calloc(10*1000*1000, sizeof(char)); // calloc max size of expected packet

		while(1){
			if (recv(c_sock, input_msg, 10*1000*1000, 0) <= 0) // if server receives EOF, quit receiving packet
				break;

			msg=calloc(10*1000*1000,sizeof(char)); // allocate memory for encrypted/decrypted message

			memcpy(&op, input_msg, 1);        //////////////////////////////////////
			memcpy(&shift, input_msg+1, 1);   // get op, shift, length from packet//
			memcpy(&newlen, input_msg+4, 4);  //////////////////////////////////////
			length=ntohl(newlen);

			if (calc_checksum(length, input_msg, 1)!=0xffff){ //if sanity check fails, close socket
				printf("server checksum error!!\n");
				free(input_msg);
				free(msg);
				close(c_sock);
				exit(0);
			}

			// encrypt/decrypt text and store it to proper spot of msg
			data_processing(op, shift, length, input_msg+8, msg+8);

			// store op, shift, length to header
			memcpy(msg, &op, 1);
			memcpy(msg+1, &shift, 1);
			memcpy(msg+4, &newlen, 4);

			checksum=calc_checksum(length, msg, 0);
			memcpy(msg+2, &checksum, 2);  //checksum

			write(c_sock, msg, length);
			free(msg);

		}
		free(input_msg);
		close(c_sock);
	}
	close(s_sock);
	exit(0);
}

void data_processing
(unsigned char op, unsigned char shift_, unsigned int length, char * src, char * dst){
	int i=0;
	short shift = shift_%26;
	short tmp = 0;
	
	if(op)
		shift=-shift;

	for(i=0;i<length-8;i++){
		tmp = (short)src[i];
		if (tmp>64 && tmp<91) tmp+=32;
		if (tmp>96 && tmp<123){
			tmp+=shift;
			if (tmp<=96) tmp+=26;
			else if (tmp>=123) tmp-=26;
			
		}
		dst[i] = (char)tmp;
	}
}

unsigned short calc_checksum(unsigned size, const char *buf, int verify)
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
	if(verify)
		return sum;
	else return ~sum;
}

