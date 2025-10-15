#ifndef MODBUS_TCP_H
#define MODBUS_TCP_H

#include <arpa/inet.h> // pro struct in_addr


#define MBAP_HEADER_LEN 7

#define PROTOCOL_IDENTIFIER 0x0000  // specific number for Modbus

#define DEBUG_TCP 0

int Send_Modbus_request(struct in_addr server_add,
                        uint16_t port,
                        const char *APDU,
                        uint16_t APDUlen,
                        char *APDU_R);

#endif // MODBUS_TCP_H
