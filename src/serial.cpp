#include <wiringSerial.h>
#include <string.h>
#include <iostream>

#include "plog/Log.h"
#include "serial.h"

Serial::Serial() {
    _baud = 9600;
    memset(_dev,0,40);
    _fd = 0;
}

Serial::Serial(const char *_device,const int _bauRate) {
    memset(_dev,0,40);
    strcpy(_dev,_device);
    _baud = _bauRate;
}

Serial::~Serial() {
    if (_fd > 0)
        serialClose(_fd);
}

void Serial::setConfigValues(const char *_device,const int _bauRate) {
    memset(_dev,0,40);
    strcpy(_dev,_device);
    _baud = _bauRate;
}

uint8_t Serial::open() {
    _fd = serialOpen(_dev,_baud);
    if ( _fd < 0) {
        PLOG_ERROR << "Cannot open serial port: " << _dev;
        return 0;
    }
    return 1;
}

uint8_t Serial::open(const char *_device,const int _bauRate) {
    setConfigValues(_device,_bauRate);
    return this->open();
}

void Serial::close() {
    serialClose(_fd);
}

void Serial::flush() {
    serialFlush(_fd);
}

void Serial::putchar(const unsigned char c) {
    serialPutchar(_fd,c);
}

void Serial::puts(const char *s) {
    serialPuts(_fd,s);
}

int Serial::dataAvail()  {
    return serialDataAvail(_fd);
}

int Serial::getchar()  {
    return serialGetchar(_fd);
}

std::string Serial::read() {
    char tmpbuff[100];
    memset(tmpbuff,0,100);
    
    uint8_t index = 0;
    while (serialDataAvail(_fd) > 0 && index < 100) {
        tmpbuff[index++] = (char)serialGetchar(_fd);
    }

    return std::string(tmpbuff);
}

void Serial::write(const unsigned char *s,unsigned int size) {
    for (unsigned int i = 0; i < size; i++)
        serialPutchar(_fd, s[i]);
}

std::string Serial::getDevice() {
    return std::string(this->_dev);
}