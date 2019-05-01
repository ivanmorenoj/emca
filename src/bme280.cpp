#include "bme280.h"
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <wiringPiI2C.h>
#include <stdint.h>
#include <unistd.h>

bme280::bme280(){
    _fd = -1;
}
bme280::bme280(int bme280_addr){
    _fd = wiringPiI2CSetup(bme280_addr);
    if(_fd < 0) {    //if the divece is found
        printf("Device not found");
    }
}
bme280::~bme280(){
    /*Close fd's*/
    if(_fd > 0)
        close(_fd);
}
void bme280::setAddress(int bme280_addr){
    _fd = wiringPiI2CSetup(bme280_addr);
    if(_fd < 0) {    //if the divece is found
        printf("Device not found");
    }
}
int32_t bme280::getTemperatureCalibration(int32_t adc_T){
    int32_t var1  = ((((adc_T>>3) - ((int32_t)_cal.dig_T1 <<1))) *
        ((int32_t)_cal.dig_T2)) >> 11;

    int32_t var2  = (((((adc_T>>4) - ((int32_t)_cal.dig_T1)) *
        ((adc_T>>4) - ((int32_t)_cal.dig_T1))) >> 12) *
        ((int32_t)_cal.dig_T3)) >> 14;

    return var1 + var2;
}
void bme280::readCalibrationData(){
  _cal.dig_T1 = (uint16_t)wiringPiI2CReadReg16(_fd, BME280_REGISTER_DIG_T1);
  _cal.dig_T2 = (int16_t)wiringPiI2CReadReg16(_fd, BME280_REGISTER_DIG_T2);
  _cal.dig_T3 = (int16_t)wiringPiI2CReadReg16(_fd, BME280_REGISTER_DIG_T3);

  _cal.dig_P1 = (uint16_t)wiringPiI2CReadReg16(_fd, BME280_REGISTER_DIG_P1);
  _cal.dig_P2 = (int16_t)wiringPiI2CReadReg16(_fd, BME280_REGISTER_DIG_P2);
  _cal.dig_P3 = (int16_t)wiringPiI2CReadReg16(_fd, BME280_REGISTER_DIG_P3);
  _cal.dig_P4 = (int16_t)wiringPiI2CReadReg16(_fd, BME280_REGISTER_DIG_P4);
  _cal.dig_P5 = (int16_t)wiringPiI2CReadReg16(_fd, BME280_REGISTER_DIG_P5);
  _cal.dig_P6 = (int16_t)wiringPiI2CReadReg16(_fd, BME280_REGISTER_DIG_P6);
  _cal.dig_P7 = (int16_t)wiringPiI2CReadReg16(_fd, BME280_REGISTER_DIG_P7);
  _cal.dig_P8 = (int16_t)wiringPiI2CReadReg16(_fd, BME280_REGISTER_DIG_P8);
  _cal.dig_P9 = (int16_t)wiringPiI2CReadReg16(_fd, BME280_REGISTER_DIG_P9);

  _cal.dig_H1 = (uint8_t)wiringPiI2CReadReg8(_fd, BME280_REGISTER_DIG_H1);
  _cal.dig_H2 = (int16_t)wiringPiI2CReadReg16(_fd, BME280_REGISTER_DIG_H2);
  _cal.dig_H3 = (uint8_t)wiringPiI2CReadReg8(_fd, BME280_REGISTER_DIG_H3);
  _cal.dig_H4 = (wiringPiI2CReadReg8(_fd, BME280_REGISTER_DIG_H4) << 4) | (wiringPiI2CReadReg8(_fd, BME280_REGISTER_DIG_H4+1) & 0xF);
  _cal.dig_H5 = (wiringPiI2CReadReg8(_fd, BME280_REGISTER_DIG_H5+1) << 4) | (wiringPiI2CReadReg8(_fd, BME280_REGISTER_DIG_H5) >> 4);
  _cal.dig_H6 = (int8_t)wiringPiI2CReadReg8(_fd, BME280_REGISTER_DIG_H6);
}
float bme280::compensateTemperature(int32_t t_fine){
    float T  = (t_fine * 5 + 128) >> 8;
    return T/100;
}
float bme280::compensatePressure(int32_t adc_P,int32_t t_fine){
    int64_t var1, var2, p;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_cal.dig_P6;
    var2 = var2 + ((var1*(int64_t)_cal.dig_P5)<<17);
    var2 = var2 + (((int64_t)_cal.dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)_cal.dig_P3)>>8) +
        ((var1 * (int64_t)_cal.dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)_cal.dig_P1)>>33;

    if (var1 == 0) {
        return 0;  // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p<<31) - var2)*3125) / var1;
    var1 = (((int64_t)_cal.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)_cal.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)_cal.dig_P7)<<4);
    return (float)p/256;
}
float bme280::compensateHumidity(int32_t adc_H, int32_t t_fine){
    int32_t v_x1_u32r;

    v_x1_u32r = (t_fine - ((int32_t)76800));

    v_x1_u32r = (((((adc_H << 14) - (((int32_t)_cal.dig_H4) << 20) -
        (((int32_t)_cal.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
            (((((((v_x1_u32r * ((int32_t)_cal.dig_H6)) >> 10) *
            (((v_x1_u32r * ((int32_t)_cal.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
        ((int32_t)2097152)) * ((int32_t)_cal.dig_H2) + 8192) >> 14));

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
            ((int32_t)_cal.dig_H1)) >> 4));

    v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
    v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
    float h = (v_x1_u32r>>12);
    return  h / 1024.0;
}
void bme280::getRawData(){
    wiringPiI2CWrite(_fd, 0xf7);

    _raw.pmsb = wiringPiI2CRead(_fd);
    _raw.plsb = wiringPiI2CRead(_fd);
    _raw.pxsb = wiringPiI2CRead(_fd);

    _raw.tmsb = wiringPiI2CRead(_fd);
    _raw.tlsb = wiringPiI2CRead(_fd);
    _raw.txsb = wiringPiI2CRead(_fd);

    _raw.hmsb = wiringPiI2CRead(_fd);
    _raw.hlsb = wiringPiI2CRead(_fd);

    _raw.temperature = 0;
    _raw.temperature = (_raw.temperature | _raw.tmsb) << 8;
    _raw.temperature = (_raw.temperature | _raw.tlsb) << 8;
    _raw.temperature = (_raw.temperature | _raw.txsb) >> 4;

    _raw.pressure = 0;
    _raw.pressure = (_raw.pressure | _raw.pmsb) << 8;
    _raw.pressure = (_raw.pressure | _raw.plsb) << 8;
    _raw.pressure = (_raw.pressure | _raw.pxsb) >> 4;

    _raw.humidity = 0;
    _raw.humidity = (_raw.humidity | _raw.hmsb) << 8;
    _raw.humidity = (_raw.humidity | _raw.hlsb);
}
float bme280::getAltitude(float pressure){
    /*
     *  Equation taken from BMP180 datasheet (page 16):
     *       http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf
     *  Note that using the equation from wikipedia can give bad results
     *  at high altitude.  See this thread for more information:
     *       http://forums.adafruit.com/viewtopic.php?f=22&t=58064
     */
  return 44330.0 * (1.0 - pow(pressure / MEAN_SEA_LEVEL_PRESSURE, 0.190294957));
}
void bme280::getValues(){
    if(isTime()){
        readCalibrationData();  // read calibration data

        wiringPiI2CWriteReg8(_fd, 0xf2, 0x01);   // humidity oversampling x 1
        wiringPiI2CWriteReg8(_fd, 0xf4, 0x25);   // pressure and temperature oversampling x 1, mode normal

        getRawData();     // get the raw data

        int32_t t_fine = getTemperatureCalibration(_raw.temperature);  //calibration

        _temperature = compensateTemperature(t_fine);                   // °C
        _pressure = compensatePressure(_raw.pressure, t_fine) / 100;    // hPa
        _humidity = compensateHumidity(_raw.humidity, t_fine);          // %
    }
}

float bme280::getTemperature(){
    getValues();
    return _temperature;
}
float bme280::getPressure(){
    getValues();
    return _pressure;
}
float bme280::getHumidity(){
    getValues();
    return _humidity;
}

