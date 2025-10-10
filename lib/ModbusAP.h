#ifndef MODBUSAP_H
#define MODBUSAP_H

#include <netinet/in.h>   // pro struct in_addr

#define APDU_W_M_R_HEADER_LEN 6
#define APDU_R_M_R_HEADER_LEN 5

// Funkce pro zápis více registrů (funkční kód 0x10)
int Write_multiple_regs(struct in_addr server_add,
                         uint16_t port,
                         uint32_t st_r,
                         uint16_t n_r,
                         char* val);

// Funkce pro čtení holding registrů (funkční kód 0x03)
int Read_h_regs(struct in_addr server_add,
                 uint16_t port,
                 uint32_t st_r,
                 uint16_t n_r,
                 char* val);

#endif // MODBUSAP_H
