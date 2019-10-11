#ifndef MCP3424_H_
#define MCP3424_H_

#include <stdint.h>
#include "sen/adcSensor.h"
#include "commonConfig.h"

/**
 *  Config register
 *  bit 7: ~RDY 
 *      1 =   Output register has not been updated
 *      0 =  Output register has been updated with the latest conversion result
 *    When writing command:
 *      Continuous Conversion mode: No effect
 *      One-Shot Conversion mode:
 *      1 = Initiate a new conversion.
 *      0 = No effect.
 *  bit 6-5: [C1-C0] Channel Selection Bits
 *            0  0  =   Channel 1 (Default)
 *            0  1  =   Channel 2
 *            1  0  =   Channel 3
 *            1  1  =   Channel 4
 *  bit 4: ~O/C: Conversion Mode Bit
 *      1 = Continuous Conversion Mode (Default). The device performs data conversions continuously.
 *      0 = One-Shot Conversion Mode. The device performs a single conversion and enters a low power
 *          standby mode until it receives another write or read command.
 *  bit 3-2: [S1-S0] Sample Rate Selection Bit
 *            0  0 = 240 SPS    (12 bits) (Default)
 *            0  1 = 60 SPS     (14 bits)
 *            1  0 = 15 SPS     (16 bits)
 *            1  1 = 3.75 SPS   (18 bits)
 *  bit 1-0: [G1-G0] PGA Gain Selection Bits
 *            0  0 = x1 (Default)
 *            0  1 = x2
 *            1  0 = x4
 *            1  1 = x8
 * Default value after power on reset :     1 0 0 1 0 0 0 0 = 0x90 
 */

class MCP3424: public adcSensor{
private:
    struct MCP3424Config *_conf;
    int _fd;
    uint8_t _rBuff[4];
    uint8_t _config;
    uint8_t _sign;

    uint8_t writeConfig();
    void configSetBit(uint8_t bit,uint8_t value);
    void generateConfig();
    uint8_t openI2C();
    void closeI2C();
    uint8_t readConfig();
    uint32_t readRaw();
    void validateConfig();

public:
    MCP3424();
    MCP3424(struct MCP3424Config *_cnf);
    ~MCP3424();
    MCP3424Config *getConfig();
    void setConfigValues(struct MCP3424Config *_cnf);
    void setChannel(uint8_t channel);
    double readVoltage();
    double readVoltage(uint8_t channel);
    double getVoltage();

};


#endif //MCP3424_H_

