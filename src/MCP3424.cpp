#include <cstddef>
#include <stdio.h>
#include <unistd.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "plog/include/plog/Log.h"

#include "MCP3424.h"

MCP3424::MCP3424() {
    _conf = NULL;
    _config = 0x90;
    _fd = -1;
    _generalError = 0;
}
MCP3424::MCP3424(struct MCP3424Config *_cnf) {
    _conf = _cnf;
    generateConfig();
    _fd = -1;
    _sign = 0;
    _generalError = 0;
}
MCP3424::~MCP3424() {
    /* Nothing */
}
uint8_t MCP3424::openI2C() {
    _fd = open("/dev/i2c-1",O_RDWR);
    if (_fd < 0) {
        PLOG_FATAL << "Failed to open i2c port";
        return 1;
    }
    if (ioctl(_fd,I2C_SLAVE,_conf->address) < 0){
        PLOG_FATAL << "Failed set i2c port for read";
        return 2;
    }
    return 0;
}
void MCP3424::closeI2C() {
    if (_fd > 0) {
        close(_fd);
        _fd = -1;
    }
}
uint8_t MCP3424::writeConfig() {
    if (write(_fd,&_config,1) != 1){
        //PLOG_FATAL << "Failed to write i2c";
        return 1;
    }
    return 0;
}
uint8_t MCP3424::readConfig() {
    if (read(_fd ,_rBuff ,4) < 0) {
        //PLOG_FATAL << "Failed to read i2c \n";
    }

    if (_conf->bitrate == 18)
        return _rBuff[3];
    else 
        return _rBuff[2];
}
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
    uint16_t tries;

    if (openI2C()) {
        _generalError++;
        return 0;
    }

    _config |= 128;

    tries = 10;
    do {
        if (!writeConfig())
            break;
        usleep(10);
    } while (--tries);

    if (tries < 1) {
        _generalError++;
        PLOG_FATAL << "Failed to write ConfigRegister, errCnt: " << (int)_generalError;
        closeI2C();
        return 0;
    }

    tries = 1000;
    while ((readConfig() & 128) && --tries)
        usleep(10);

    if (tries < 1) {
        _generalError++;
        PLOG_FATAL << "Failed to read ConfigRegister, errCnt: " << (int)_generalError;
        closeI2C();
        return 0;
    }
    
    closeI2C();

    /* Reset general error counter */
    _generalError = 0;

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
    _generalError = 0;
    generateConfig();
}
double MCP3424::readVoltage() {
    _voltage = 0.0;
    uint32_t raw;
    raw = readRaw();

    if (_generalError > 2) {
        PLOG_FATAL << "Error count > 2, exit on failure";
        exit(EXIT_FAILURE);
    }

    double gain = (double) _conf->gain / 2.0;
    double offset = 2.048 / (double)_conf->gain;

    double lsb = 0;

	switch (_conf->bitrate) {
	case 12:
		lsb = 0.0005;       break;
	case 14:
		lsb = 0.000125;     break;
	case 16:
		lsb = 0.00003125;   break;
	case 18:
		lsb = 0.0000078125; break;
	default:
		return (9999);
	}

    if (_sign == 1)
        _voltage = (double) raw * (lsb / gain) - offset;
    else
        _voltage = (double) raw * (lsb / gain);

    return _voltage;
}
double MCP3424::readVoltage(uint8_t channel) {
    setChannel(channel);
    return this->readVoltage();
}
double MCP3424::getVoltage() {
    if (isTime()) {
        return this->readVoltage();
    } else {
        return _voltage;
    }
}


