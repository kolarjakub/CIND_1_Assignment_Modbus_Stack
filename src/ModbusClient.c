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
    int wr = Write_multiple_regs(server_add, SERVER_PORT, 65530, 2, write_buf);
    if (wr>=0)
    {
        printf("--------------------\nWrite registers succesful...\n");
        printf("Register values:\n");
        for (int i = 0; i < 2; i++) {
            uint16_t value = ((uint8_t)write_buf[i*2] << 8) | (uint8_t)write_buf[1 + i*2];
            printf("Reg[%d] = %u (0x%04X)\n", 65530 + i, value, value);
        }
        printf("--------------------\n\n");
    }else{
        printf("--------------------\nWrite registers unsuccesful...\n--------------------\n\n");
    }


    char read_buf[10]={};
    int rr=Read_h_regs(server_add, SERVER_PORT, 65529, 5, read_buf);
    if (rr>=0)
    {
        printf("--------------------\nRead registers succesful...\n");
        printf("Register values:\n");
        for (int i = 0; i < 5; i++) {
            uint16_t value = ((uint8_t)read_buf[i*2] << 8) | (uint8_t)read_buf[1 + i*2];
            printf("Reg[%d] = %u (0x%04X)\n", 65529 + i, value, value);
        }
        printf("--------------------\n\n");
    }else{
        printf("--------------------\nRead registers succesful...\n--------------------\n\n");
    }
    return 0;
}