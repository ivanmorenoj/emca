#include "stdio.h"

#include "MCP3424.h"

MCP3424Config _cnf[8] = {
  { .address   = 0x69,
    .bitrate    = 18,
    .gain       = 1,
    .conv_mode  = 1,
    .channel    = 1},
  { .address   = 0x69,
    .bitrate    = 18,
    .gain       = 1,
    .conv_mode  = 1,
    .channel    = 2},
  { .address   = 0x69,
    .bitrate    = 18,
    .gain       = 1,
    .conv_mode  = 1,
    .channel    = 3},
  { .address   = 0x69,
    .bitrate    = 18,
    .gain       = 1,
    .conv_mode  = 1,
    .channel    = 4},
  { .address   = 0x6A,
    .bitrate    = 18,
    .gain       = 1,
    .conv_mode  = 1,
    .channel    = 1},
  { .address   = 0x6A,
    .bitrate    = 18,
    .gain       = 1,
    .conv_mode  = 1,
    .channel    = 2},
  { .address   = 0x6A,
    .bitrate    = 18,
    .gain       = 1,
    .conv_mode  = 1,
    .channel    = 3},
  { .address   = 0x6A,
    .bitrate    = 18,
    .gain       = 1,
    .conv_mode  = 1,
    .channel    = 4}
};

int main(int argc, char const *argv[])
{
    MCP3424 _adc[8];

    for(uint8_t i = 0; i < 8; ++i)
        _adc[i].setConfigValues(&_cnf[i]);
    
    for(uint8_t i = 0; i < 8; ++i)
        printf("The voltage [%d] : %.4f\n",i,_adc[i].readVoltage());

    printf("end of the program\n");
    return 0;
}
