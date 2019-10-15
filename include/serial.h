#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdint.h>
#include <iostream>

class Serial
{
private:
    int _fd;
    int _baud;
    char _dev[40];

public:
    Serial();
    Serial(const char *_device,const int _bauRate);
    ~Serial();
    void setConfigValues(const char *_device,const int _bauRate);
    uint8_t open();
    uint8_t open(const char *_device,const int _bauRate);
    void close();
    void flush();
    void putchar(const unsigned char c);
    void puts(const char *s);
    int dataAvail() ;
    int getchar() ;

    void write(const unsigned char *s,unsigned int size);
    std::string read();
    std::string getDevice();
};


#endif //SERIAL_H_