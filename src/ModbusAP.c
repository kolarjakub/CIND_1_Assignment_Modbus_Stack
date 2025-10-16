#include "ModbusTCP.h"    // používáš Send_Modbus_request z této vrstvy
#include "ModbusAP.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


// 0x10 Write multiple registers
int Write_multiple_regs (struct in_addr server_add, uint16_t port, uint32_t st_r, uint16_t n_r, char* val)
{
    // check consistency of parameters
    // st_r in range 0x0000 to 0xFFFF
    if (n_r<1 || n_r>123)
    {
        #if DEBUG_AP
            printf("Invalid number of registers...\n");
        #endif
        return -1;
    }
    if (st_r<1 || st_r>65536)
    {
        #if DEBUG_AP
            printf("Starting adress out of range...\n");
        #endif
        return -1;
    }
    if (st_r + (uint32_t)n_r - 1 > 65536)
    {
        #if DEBUG_AP
            printf("Invalid address range: exceeds Modbus limit 65536.\n");
        #endif
        return -1;
    }
    if (val == NULL)
    {
        #if DEBUG_AP
            printf("Pointer to values is NULL...\n");
        #endif
        return -2;
    }


    // assembles APDU (MODBUS PDU)
    uint16_t APDUlen=APDU_W_M_R_HEADER_LEN+2*n_r;
    char* APDU = malloc(APDUlen);
    if (APDU ==NULL)
    {
        #if DEBUG_AP
            printf("Error in APDU allocation...\n");
        #endif
        return -3;
    }
    APDU[0]=0x10;   // function code
    APDU[1]=((st_r-1) >> 8) & 0xFF;
    APDU[2]= st_r-1 & 0xFF;
    APDU[3]=(n_r  >> 8) & 0xFF;
    APDU[4]= n_r  & 0xFF;
    APDU[5]=2*n_r;      // number of bztes
    memcpy(APDU+APDU_W_M_R_HEADER_LEN,val,n_r*2);


    //Send_Modbus_request (server_add, port, APDU, APDUlen, APDU_R);
    // response muye mit 5 Bytu, Error 3 Byty
    char *APDU_R=malloc(5*sizeof(char));
    if (APDU_R ==NULL)
    {
        #if DEBUG_AP
            printf("Error in APDU_R allocation...\n");
        #endif
        free(APDU);
        return -4;
    }
    int req=Send_Modbus_request (server_add, port, APDU, APDUlen, APDU_R);
    free(APDU);
    if (req <0)
    {
        #if DEBUG_AP
            printf("Invalid Modbus request...\n");
        #endif
        free(APDU_R);
        return -5;
    }


    // checks the response (APDU_R or error_code)
    // returns: number of written registers - ok, <0 - error
    if (APDU_R[0]==0x10)
    {
        #if DEBUG_AP
             printf("Written registers: %u\n", (APDU_R[3]<<8)+APDU_R[4]);
        #endif
        free(APDU_R);
        return 0;
    //}else if (APDU_R[0]==0x90){
    }else if (((uint8_t)APDU_R[0]>> 7) == 1){
        #if DEBUG_AP
            printf("Exception code: 0x%02X\n", (uint8_t)APDU_R[1]);
        #endif
        free(APDU_R);
        return APDU_R[1];   // returning error code of the exception
    }else {
        #if DEBUG_AP
            printf("Unknown response\n");
        #endif
        free(APDU_R);
        return -7;
    }
}







// 0x03 Read holding register
int Read_h_regs(struct in_addr server_add, uint16_t port, uint32_t st_r, uint16_t n_r, char* val){
    // check consistency of parameters
    // st_r in range 0x0000 to 0xFFFF
    if (n_r<1 || n_r>125)
    {
        #if DEBUG_AP
            printf("Invalid number of registers...\n");
        #endif
        return -1;
    }
    if (st_r<1 || st_r>65536)
    {
        #if DEBUG_AP
            printf("Starting adress out of range...\n");
        #endif
        return -1;
    }
    if (st_r + (uint32_t)n_r - 1 > 65536)
    {
        #if DEBUG_AP
            printf("Invalid address range: exceeds Modbus limit 65536...\n");
        #endif
        return -1;
    }
    if (val == NULL)
    {
        #if DEBUG_AP
            printf("Pointer to values is NULL...\n");
        #endif
        return -2;
    }


    // assembles APDU (MODBUS PDU)
    uint16_t APDUlen=APDU_R_M_R_HEADER_LEN;
    char* APDU = malloc(APDUlen);
    if (APDU ==NULL)
    {
        #if DEBUG_AP
            printf("Error in APDU allocation...\n");
        #endif
        return -3;
    }
    APDU[0]=0x03;
    APDU[1]=((st_r-1) >> 8) & 0xFF;
    APDU[2]= (st_r-1) & 0xFF;
    APDU[3]=(n_r  >> 8) & 0xFF;
    APDU[4]= n_r  & 0xFF;

    char *APDU_R=malloc(2*n_r+2);   // function code (1B) + Byte count (1B) + 2* number_of_registers / OR exception with 2 Bytes
    if (APDU_R ==NULL)
    {
        #if DEBUG_AP
            printf("Error in APDU_R allocation...\n");
        #endif
        free(APDU);
        return -4;
    }
    int req=Send_Modbus_request (server_add, port, APDU, APDUlen, APDU_R);
    free(APDU);
    if (req <0)
    {
        #if DEBUG_AP
            printf("Invalid Modbus request...\n");
        #endif
        free(APDU_R);
        return -5;
    }


    // checks the response (APDU_R or error_code)
    // returns: number of written registers - ok, <0 - error
    if (APDU_R[0]==0x03)
    {
        #if DEBUG_AP
            printf("Read Bytes: %u (%u Bytes)\n", APDU_R[1]/2, APDU_R[1]);
        #endif
        memcpy(val,APDU_R+2,n_r*2);
        free(APDU_R);
        return 0;
    //}else if (APDU_R[0]==0x83){
    }else if (((uint8_t)APDU_R[0]>> 7) == 1){
        #if DEBUG_AP
            printf("Exception code: 0x%02X\n", (uint8_t)APDU_R[1]);
        #endif
        free(APDU_R);
        return APDU_R[1];   // returning error code of exception
    }else {
        #if DEBUG_AP
            printf("Unknown response\n");
        #endif
        free(APDU_R);
        return -7;
    }
}