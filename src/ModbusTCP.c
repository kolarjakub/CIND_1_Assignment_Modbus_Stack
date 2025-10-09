#include "ModbusTCP.h"
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>   // pro uint8_t, uint16_t
#include <stdio.h>   // pro uint8_t, uint16_t


static uint16_t transaction_id = 0;

int Send_Modbus_request (struct in_addr server_add, uint16_t port, const char *APDU, uint16_t APDUlen, char *APDU_R){
    // generates TI (trans.ID →sequence number)
    transaction_id++;


    // assembles PDU = APDU(SDU) + MBAP
    char *PDU=malloc((MBAP_HEADER_LEN+APDUlen)*sizeof(char));
    if(PDU == NULL){
        printf("PDU allocation unsuccesful...\n");
        return -1;
    }
    PDU[0] = (char)(transaction_id >> 8) & 0xFF; // high byte
    PDU[1] = (char)transaction_id & 0xFF;        // low byte
    PDU[2] = (char)(MODBUS_IDENTIFIER >> 8) & 0xFF;
    PDU[3] = (char)MODBUS_IDENTIFIER & 0xFF;
    PDU[4] = ((APDUlen+1) >> 8) & 0xFF;;
    PDU[5] = (APDUlen+1) & 0xFF;    // +1 kvuli tomu, ze se tam pocita i ten Unit Identifier
    PDU[6] = SERVER_UNIT_ID;      // Unit ID - has to be the same as the server
    memcpy(PDU+MBAP_HEADER_LEN, APDU, APDUlen);


    // opens TCP client socket and connects to server (*)
    int socket_descriptor = socket(PF_INET , SOCK_STREAM , IPPROTO_TCP);
    if (socket_descriptor == -1)
    {
        printf("Socket  with descriptor %i creation failed...\n",socket_descriptor);
        free(PDU);
        return -2;
    }
    else
        printf("Socket successfully created...\n");
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr = server_add;
    server.sin_port = htons(port);
    if (connect(socket_descriptor , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("Connection with the server failed...\n");
        free(PDU);
        return -3;
    }
    else
        printf("Connected to the server at address (%s) port (%d)...\n", inet_ntoa(server_add), port);


    //write (fd, PDU, PDUlen); // sends Modbus TCP PDU
    int out = send(socket_descriptor , PDU , MBAP_HEADER_LEN + APDUlen , 0);
    if(out < 0)
    {
        printf("Send failed..\n");
        free(PDU);
        close(socket_descriptor);
        return -4;
    }else{
        printf("Sent data (%d bytes): ", out);
        for (int i = 0; i < MBAP_HEADER_LEN + APDUlen; i++) {
            printf("%02X ", (unsigned char)PDU[i]);
        }
        printf("\n");
    }
    free(PDU);


    //read (fd, PDU_R, PDU_Rlen); // response o timeout
    struct timeval tv;  // set timeout interval
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    //receive PDU_R
    char *MBAP_R=malloc(MBAP_HEADER_LEN*sizeof(char));
    bzero(MBAP_R, MBAP_HEADER_LEN);
    int in = recv(socket_descriptor, MBAP_R, MBAP_HEADER_LEN, 0); //blokne se to tady dokud to neprijme ten buffer
    if( in < 0)
    {
        printf("Recv failed..\n");
        free(MBAP_R);
        close(socket_descriptor);
        return -5;
    }
    // check transaction ID
    else if (((MBAP_R[0]<<8)|MBAP_R[1]) != transaction_id){
        printf("Recv invalid transaction ID..\n");
        free(MBAP_R);
        close(socket_descriptor);
        return -6;
    }
    else {
        printf("Received MBAP header (%d bytes): ", in);
        for (int i = 0; i < in; i++) {
            printf("%02X ", (unsigned char)MBAP_R[i]);
        }
        printf("\n");
    }

    const uint16_t APDU_R_LEN=(MBAP_R[4]<<8) + MBAP_R[5] -0x0001;    // -1 protože je to i s tím 1 Bytem od Unit Identifier
    free(MBAP_R);

    // receive APDU_R
    char *APDU_R_tmp=malloc(APDU_R_LEN*sizeof(char));
    bzero(APDU_R_tmp,APDU_R_LEN);
    in = recv(socket_descriptor, APDU_R_tmp, APDU_R_LEN, 0); //blokne se to tady dokud to neprijme ten buffer
    if( in < 0)
    {
        printf("Recv failed..\n");
        close(socket_descriptor);
        free(APDU_R_tmp);
        return -7;
    }
    else {
        printf("Received data (%d bytes): ", in);
        for (int i = 0; i < in; i++) {
            printf("%02X ", (unsigned char)APDU_R_tmp[i]);
        }
        printf("\n");
        memcpy(APDU_R, APDU_R_tmp, APDU_R_LEN);
        free(APDU_R_tmp);
    }

    // if response, remove MBAP, PDU_R → APDU_R


    // closes TCP client socket with server (*)
    close(socket_descriptor);
    printf("Connection succesfuly closed ...\n");


    // returns: APDU_R and 0 – ok, <0 – error (timeout)
    return 0;
}
