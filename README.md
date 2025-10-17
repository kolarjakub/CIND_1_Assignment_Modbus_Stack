Modbus stack in CIND course

compile: $ gcc clienteA.c ModbusTCP.c ModbusAP.c -o clienteA

```c
Example test code:
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
    if (wr==0)
    {
        #if DEBUG_CLIENT
            printf("--------------------\nWrite registers succesful...\n");
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
    if (rr==0)
    {
        #if DEBUG_CLIENT
            printf("--------------------\nRead registers succesful...\n");
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

/*
Cliente Modbus TCP – úkol 1

1. Zapíše 0x41 do HR[121] (127.0.0.1:502)
2. Čte A[0..3] z HR[122..125] (127.0.0.1:502)
3. Čte B z HR[126] (127.0.0.1:502)
4. C = (B == 0) ? 9999 : A[0] + A[3]
5. Zapíše C do HR[127] (127.0.0.1:502)
6. Zapíše C do HR[128] (10.227.113.1:502)
7. Výstup (printf):
     result = 0  → OK
     result = -1 → chyba Modbus
     result = X>0 → kód chyby Modbus
Unit ID = 51
*/

// build:
// gcc clientA.c ModbusAP.c ModbusTCP.c -o clientA

int main(){
    struct sockaddr_in server;
    //Prepare the sockaddr_in structure
    struct in_addr server_add;
    inet_aton("127.0.0.1",&server_add);

    struct in_addr server_add2;
    inet_aton("10.227.113.1",&server_add2);


    // each register consists of 2 Bytes -> uint16_t
    char write_buf[2]={0x00,0x41};
    int r = Write_multiple_regs(server_add, SERVER_PORT, 122, 1, write_buf);
    if(r<0){
        printf("result = -1\n");
        return 0;
    }else if(r>0){
        printf("result = %d\n", r);
        return 0;
    }

    char A[8]={};
    r=Read_h_regs(server_add, SERVER_PORT,123, 4, A);
    if(r<0){
        printf("result = -1\n");
        return 0;
    }else if(r>0){
        printf("result = %d\n", r);
        return 0;
    }

    char B[2]={};
    r=Read_h_regs(server_add, SERVER_PORT,127, 1, B);
    if(r<0){
        printf("result = -1\n");
        return 0;
    }else if(r>0){
        printf("result = %d\n", r);
        return 0;
    }

    char C[2]={};
    if(B[1]==0 && B[0]==0){
        C[0]=(uint16_t)9999>>8 & 0xFF;
        C[1]=(uint16_t)9999 & 0xFF;
    }else{
        C[0]=(uint16_t)(A[0]+A[3])>>8 & 0xFF;
        C[1]=(uint16_t)(A[0]+A[3]) & 0xFF;
    }
    r=Write_multiple_regs(server_add, SERVER_PORT,128, 1, C);
    if(r<0){
        printf("result = -1\n");
        return 0;
    }else if(r>0){
        printf("result = %d\n", r);
        return 0;
    }

    r=Write_multiple_regs(server_add, SERVER_PORT,129, 1, C);
    if(r<0){
        printf("result = -1\n");
        return 0;
    }else if(r>0){
        printf("result = %d\n", r);
        return 0;
    }else{printf("result = 0\n");}

    return 0;
}
