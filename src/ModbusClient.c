#include "ModbusAP.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>
#include <stdint.h>


#define SERVER_ADDR "127.0.0.1"//"10.227.113.16"
#define SERVER_PORT 502

#define DEBUG_CLIENT 0


void Write_multiple_regs_print (struct in_addr server_add, uint16_t port, uint32_t st_r, uint16_t n_r, char* val) {
    int wr = Write_multiple_regs(server_add, port, st_r, n_r, val);
    if (wr>=0)
    {
        printf("--------------------\nWrite registers succesful...\n");
        #if DEBUG_CLIENT
            printf("Register values:\n");
            for (int i = 0; i < n_r; i++) {
                uint16_t value = ((uint8_t)val[i*2] << 8) | (uint8_t)val[1 + i*2];
                printf("Reg[%d] = %u (0x%04X)\n", st_r + i, value, value);
            }
            printf("--------------------\n\n");
        #endif
    }else{
        #if DEBUG_CLIENT
            printf("--------------------\nWrite registers unsuccesful...\n--------------------\n\n");
        #endif
    }
}

void Read_h_regs_print(struct in_addr server_add, uint16_t port, uint32_t st_r, uint16_t n_r, char* val) {
    int rr=Read_h_regs(server_add, port, st_r, n_r, val);
    if (rr>=0)
    {
        printf("--------------------\nRead registers succesful...\n");
        #if DEBUG_CLIENT
            printf("Register values:\n");
            for (int i = 0; i < n_r; i++) {
                uint16_t value = ((uint8_t)val[i*2] << 8) | (uint8_t)val[1 + i*2];
                printf("Reg[%d] = %u (0x%04X)\n", st_r + i, value, value);
            }
            printf("--------------------\n\n");
        #endif
    }else{
        #if DEBUG_CLIENT
            printf("--------------------\nRead registers unsuccesful...\n--------------------\n\n");
        #endif
    }
}




int main(){
    struct sockaddr_in server;

    //Prepare the sockaddr_in structure
    struct in_addr server_add;
    inet_aton("127.0.0.1",&server_add);

    char write_buf[4]={0x00,0x0A,0x01,0x02};
    uint32_t start_add=65530;
    uint16_t number_of_regs=2;



    char read_buf[10]={};
    start_add=65529;
    number_of_regs=2;

    while (1) {
        for (int i=1;i<=65536;i=i+2) {
            Read_h_regs_print(server_add, SERVER_PORT, i, number_of_regs, read_buf);
            usleep(100000);  // 100 000 mikrosekund = 0.1 s
            Write_multiple_regs_print(server_add, SERVER_PORT, i, number_of_regs, write_buf);
            usleep(1000000);  // 1000 000 mikrosekund = 1 s
            printf("\n");
        }
    }

    return 0;
}