#include "ModbusAP.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>
#include <stdint.h>

#define SERVER_ADDR "127.0.0.1"//"10.227.113.16"
#define SERVER_PORT 502


int main(){
    struct sockaddr_in server;
    char write_buf[4]={0x00,0x0A,0x01,0x02};

    //Prepare the sockaddr_in structure
    struct in_addr server_add;
    inet_aton("127.0.0.1",&server_add);

    //struct in_addr server_add;
    int wr = Write_multiple_regs(server_add, SERVER_PORT, 65536, 1, write_buf);
    if (wr<0)
    {
        printf("Write registers unsuccesful...\n");
        return -1;
    }else{
        printf("Write registers succesful...\n");
        return 0;
    }

}