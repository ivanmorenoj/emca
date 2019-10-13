#ifndef COMMONCONFIG_H_
#define COMMONCONFIG_H_

#include <time.h>
#include <stdint.h>
#include <iostream>

#define _LORA_PAYLOAD_LENGTH 12

/**
 * Alphasense sensor config data
 */
// Type of sensor
enum SensorType {
  CO_SENSOR  = 0,
  H2S_SENSOR = 1,
  NO_SENSOR  = 2,
  NO2_SENSOR = 3,
  O3_SENSOR  = 4,
  S02_SENSOR = 5
};
// type of sensor for compensation
enum CompensateType {
  CO_A4     =   0,
  CO_B4     =   1,
  H2S_A4    =   2,
  H2S_B4    =   3,
  NO_A4     =   4,
  NO_B4     =   5,
  NO2_A43F  =   6,
  NO2_B43F  =   7,
  OX_A431   =   8,
  OX_B431   =   9,
  SO2_A4    =  10,
  SO2_B4    =  11
};
/**
 * Alphasense sensor config struct
 */
struct alphasenseConfig {
  SensorType sensor;
  CompensateType compensate;
  char algorithm;
  float we_electronic;
  float we_total;
  float ae_electronic;
  float ae_total;
  float sensitivity;
};
/**
 * flagManager
 */
struct operatingConditions {
    time_t lifeTime;        // {life time } [time_t {sec}]
    time_t startupTime;     // {first time} [time_t {sec}] 
    float temperature[2];   // { min , max} [°C]
    float pressure[2];      // { min , max} [hPa]
    float humidity[2];      // { min , max} [%]
    float ranges[2];        // { min , max} [ppb] by nom    
};
/**
 * MCP3424 Config
 */
struct MCP3424Config {
    uint8_t address;
    uint8_t bitrate;
    uint8_t gain;
    uint8_t conv_mode;
    uint8_t channel;
};
/**
 * sqlConnector
 */
struct ambVariables{
    float temp;
    float hum;
    float pre;
};
struct gasVariables{
    float ppb;
    char* flag;
};

struct db_info {
    char *gasType;
    struct ambVariables *amb;
    double we;
    double ae;
    float ppb;
    float ppb_c;
};

struct db_values{
    struct ambVariables *amb;
    /**
     *  gas[0] = CO
     *  gas[1] = NO2
     *  gas[2] = SO2
     *  gas[3] = O3
     */
    struct gasVariables gas[4];
};

struct alphasenseCfg{
    struct alphasenseConfig     gas;
    struct MCP3424Config        adc[2];
    struct operatingConditions  cond;
};

struct aeroqualCfg{
    struct MCP3424Config        adc;
    struct operatingConditions  cond;
};

struct dbCfg {
  char user[30]; //inser_user
  char pass[20];
  char host[80];
  char schema[20];
};

struct timeCfg {
  int sampling;
  int restore;
};

/* LoRa cfg Settings */
struct lora{
  std::string      port;
  std::string      txPower;
  std::string      activationMethod;
  std::string      dataRate;
  std::string      channel;
  std::string      NwkSKey;
  std::string      AppSKey;
  std::string      DevAddr;
  uint32_t    frameCounter;
};

/* LoRa Payload structs */
struct byte_payload{
    // ambiental variables
    int8_t      _tem;   // -127 - 127 in °C
    uint8_t     _hum;   // 0 - 100 in %
    uint16_t    _pre;   // 0 - 65535 in hPa

    // gas variables
    uint16_t    _co;    // 0 - 65535 in ppb 
    uint16_t    _o3;    // 0 - 65535 in ppb 
    uint16_t    _so2;   // 0 - 65535 in ppb 
    uint16_t    _no2;   // 0 - 65535 in ppb 
};

union lora_payload{
    byte_payload _bytes;
    uint8_t _raw[_LORA_PAYLOAD_LENGTH];
};

struct projectCfg {
  int bme280Address;
  struct timeCfg        _tm;
  struct dbCfg          _sql;
  struct alphasenseCfg  _gas[3];
  struct aeroqualCfg    _o3gas;
  struct lora           _lora;
};

#endif //COMMONCONFIG_H_