#include <cstddef>
#include <stdio.h>
#include <unistd.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "plog/Log.h"

//#include <string.h>
//#include <wiringPiI2C.h>
#include "MCP3424.h"

MCP3424::MCP3424() {
    _conf = NULL;
    _config = 0x90;
    _fd = -1;   
}
MCP3424::MCP3424(struct MCP3424Config *_cnf) {
    _conf = _cnf;
    generateConfig();
    _fd = -1;
    _sign = 0;
}
MCP3424::~MCP3424() {
    /* Nothing */
}
void MCP3424::openI2C() {
    _fd = open("/dev/i2c-1",O_RDWR);
    if (_fd < 0) {
        PLOG_FATAL << "Failed to open i2c port";
    }
    if (ioctl(_fd,I2C_SLAVE,_conf->address) < 0){
        PLOG_FATAL << "Failed set i2c port for read";
    }
}
void MCP3424::closeI2C() {
    close(_fd);
    _fd = -1;
}
void MCP3424::writeConfig() {
    if (write(_fd,&_config,1) != 1){
        PLOG_FATAL << "Failed to write i2c";
    }
}
uint8_t MCP3424::readConfig() {
    if (read(_fd ,_rBuff ,4) < 0) {
        PLOG_FATAL << "Failed to read i2c \n";
    }

    if (_conf->bitrate == 18)
        return _rBuff[3];
    else 
        return _rBuff[2];
}
/*
void MCP3424::openI2C() {
    if (_fd > 0){
        _fd = wiringPiI2CSetup(_conf->address);
        if (_fd < 0 ) {
            PLOG_FATAL << ("Error in open fd\n");
        }
    }
}
void MCP3424::closeI2C() {
    close(_fd);
    _fd = -1;
}
uint8_t MCP3424::readConfig() {
    memset(_rBuff,0,sizeof(_rBuff));
    
    wiringPiI2CRead(_fd);
    if (read(_fd ,_rBuff ,4) < 0) {
        PLOG_FATAL << ("Error in read\n");
    }

    if (_conf->bitrate == 18)
        return _rBuff[3];
    else 
        return _rBuff[2];
}
void MCP3424::writeConfig() {
    wiringPiI2CWrite(_fd,_config);

    for (uint8_t i = 0; (i < 10) && (_config != readConfig()) ; ++i);
}*/
MCP3424Config *MCP3424::getConfig() {
    return _conf;
}
void MCP3424::configSetBit(uint8_t bit,uint8_t value) {
    if (value == 0)
		_config &= ~(1 << bit);
	else 
		_config |= 1 << bit;
}
uint32_t MCP3424::readRaw() {
    uint32_t raw = 0;
    _sign = 0;

    openI2C();

    _config |= 128;
    //PLOG_FATAL << ("actual config : 0x%X\n",_config);

    writeConfig();
    
    for (uint16_t i = 0; (i < 1000) && (readConfig() & 128); i++);
    
    closeI2C();

    switch (_conf->bitrate) {
    case 18:
        raw = ((_rBuff[0] & 3) << 16) | (_rBuff[1] << 8) | _rBuff[2];
        if ((raw >> 18) & 1) {
            _sign = 1;
            raw &= ~(1 << 17);
        } 
        break;
	case 16:
		raw = (_rBuff[0] << 8) | _rBuff[1];
		if ((raw >> 15) & 1) {
			_sign = 1;
			raw &= ~(1 << 15);
		}
		break;
	case 14:
		raw = ((_rBuff[0] & 63) << 8) | _rBuff[1];
		if ((raw >> 13) & 1) {
			_sign = 1;
			raw &= ~(1 << 13);
		}
		break;
	case 12:
		raw = ((_rBuff[0] & 15) << 8) | _rBuff[1];
		if ((raw >> 11) & 1) {
			_sign = 1;
			raw &= ~(1 << 11);
		}
		break;
    default:
        break;
    }

    return raw;
}
void MCP3424::validateConfig() {
	_conf->address &= 0b01101111;
    switch(_conf->bitrate) {
        case 12: case 14: case 16: case 18:
            break;
        default:
            _conf->bitrate = 12;
    }
    switch(_conf->gain) {
        case 1: case 2: case 4: case 8:
            break;
        default:
            _conf->gain = 1;
    }
    _conf->conv_mode &= 1;
    if (!((_conf->channel >= 1) && (_conf->channel <= 4)))
        _conf->channel = 1;
}
void MCP3424::generateConfig() {
    validateConfig();
    /* gen config for gain */
    for (uint8_t i = 1 ,n = 0 ; i <= 8 ; i*=2 , ++n)
        if (_conf->gain == i) { 
            for (uint8_t j = 0; j < 2; j++)
                configSetBit(j, n & (1 << j));
            break;
        }
    
    /* gen sample rate */
    for (uint8_t i = 12, n = 0 ; i <= 18 ; i += 2 , ++n)
        if (_conf->bitrate == i) {
            for (uint8_t j = 0 ; j < 2; j++)
                configSetBit(j + 2, n & (1 << j));
            break;
        }
    
    /* gen conversion mode */
    configSetBit(4, _conf->conv_mode? 1 : 0);

    /* set channel*/
    setChannel(_conf->channel);
}
void MCP3424::setChannel(uint8_t channel) {
    /* gen channel */
    for (uint8_t i = 1 ; i <= 4 ; i++)
        if (channel == i)
            for (uint8_t j = 0 ; j < 2; j++ )
                configSetBit(j + 5, (i-1) & (1 << j));
}
void MCP3424::setConfigValues(struct MCP3424Config *_cnf) {
    _conf = _cnf;
    generateConfig();
}
double MCP3424::readVoltage() {
    double voltage = 0.0;
    uint32_t raw;
    raw = readRaw();

    double gain = (double) _conf->gain / 2.0;
    double offset = 2.048 / (double)_conf->gain;

    double lsb = 0;

	switch (_conf->bitrate) {
	case 12:
		lsb = 0.0005;   break;
	case 14:
		lsb = 0.000125; break;
	case 16:
		lsb = 0.00003125;   break;
	case 18:
		lsb = 0.0000078125; break;
	default:
		return (9999);  break;
	}

    if (_sign == 1)
        voltage = (double) raw * (lsb / gain) - offset;
    else
        voltage = (double) raw * (lsb / gain);

    return voltage;
}
double MCP3424::readVoltage(uint8_t channel) {
    setChannel(channel);
    return this->readVoltage();
}
double MCP3424::getVoltage() {
    return this->readVoltage();
}


