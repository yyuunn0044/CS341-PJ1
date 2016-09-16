#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned short calc_checksum(unsigned length, const char *buf, int verify);
void print64(char * input_msg);

int main(int argc, char **argv){

	int s_sock, c_sock;
	struct sockaddr_in s_addr;
	
	unsigned char buf[1009];  // for receiving packet from server
	unsigned char *msg;       // for sending message to server (include header and text)
	unsigned char data[1001]; // temporary buffer for 1000 bytes text from stdin
	
	int i=0;
	unsigned char op=0;
	unsigned char shift=0;
	unsigned long length=0;
	unsigned long newlen=0; // network ordered 'length of total message'
	unsigned short checksum=0;

	c_sock=socket(AF_INET, SOCK_STREAM, 0); //make client socket
	if (c_sock<0){
		printf("Socket Error\n");
		exit(0);
	}
	s_addr.sin_addr.s_addr=inet_addr(argv[2]);
	s_addr.sin_family=AF_INET;
	s_addr.sin_port=htons(atoi(argv[4]));
	
	op=atoi(argv[6]);
	shift=atoi(argv[8]);

	if ( connect(c_sock, (struct sockaddr *)&s_addr, sizeof(s_addr)) <0 ){
		close(c_sock);
		exit(0);
	}

	while(fgets(data, 1000, stdin)){ //read 1000 bytes of data with 'null'
	
		msg=calloc(1009, sizeof(char));
						
		length=strlen(data)+9; //include header, data, null	
		newlen=htonl(length);	

		memcpy(msg, &op, 1);                 ///////////////////////////////////
		memcpy(msg+1, &shift, 1);            // store op, shift, length, text //
		memcpy(msg+4, &newlen, 4);           // in the right spot             //
		memcpy(msg+8, data, strlen(data)+1); ///////////////////////////////////

		checksum=calc_checksum(length, msg, 0); 
		memcpy(msg+2, &checksum, 2);
		
		write(c_sock, msg, length);    // send plaintext to server
		recv(c_sock, buf, length, 0);  // ...and receive encrypted/decrypted text

		memcpy(&newlen, buf+4, 4);
		length=ntohl(newlen);
		
		checksum=calc_checksum(length, buf, 1); //sanity check with checksum
		if (checksum!=0xffff){
			printf("checksum error!!!\n");			
			free(msg);
			close(c_sock);
			exit(0);
		}

		printf("%s", buf+8); // encrypted/decrypted data to stdout
		free(msg);
	}

	close(c_sock);
	
	exit(0);
}

// calc_checksum : if 'verify' is true, calculate checksum for header
//                 otherwise, check sanity of packet
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

