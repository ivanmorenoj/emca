/**
 *   This library manage the Alphasense Sensors with ISB
 *   Author: Ivan Moreno
 *   September 2018
 */
#ifndef ALPHASENSESENSOR_H_
#define ALPHASENSESENSOR_H_

#include "commonConfig.h"
#include "sen/ambientSensor.h"
#include "sen/adcSensor.h"
#include "sen/gasSensor.h"


class AlphasenseSensor: public gasSensor{
public:
  /**
   *   Default constructor
   */
  AlphasenseSensor();
  /**
   *   Second constructor   - by values including in bag
   *   @param St - SensorType only available for 0 to 7 that define type of sensor
   *   @param Ct - CompensateType only available for 0 to 7 that specifies type of sensor for compesation
   *   @param Alg - choose the Algorithm for compensation
   *   @param we_e - WE electronic offset in volts
   *   @param we_t - WE total offset in volts
   *   @param ae_e - AE electronic offset in volts
   *   @param ae_t - AE total offset in volts
   *   @param sen - Sensitivity of sensor in volts
   */
  AlphasenseSensor(SensorType St,CompensateType Ct,char Alg,float we_e,float we_t,float ae_e,float ae_t,float sen);
  /**
   * 
   */
  AlphasenseSensor(alphasenseConfig conf,ambientSensor *amb,adcSensor *ad_we,adcSensor *ad_ae);
  /**
   *   Destroyer
   */
  ~AlphasenseSensor();
  /**
   *   Function that setting the values
   *   @param St - SensorType only available for 0 to 7 that define type of sensor
   *   @param Ct - CompensateType only available for 0 to 7 that specifies type of sensor for compesation
   *   @param Alg - choose the Algorithm for compensation
   *   @param we_e - WE electronic offset in volts
   *   @param we_t - WE total offset in volts
   *   @param ae_e - AE electronic offset in volts
   *   @param ae_t - AE total offset in volts
   *   @param sen - Sensitivity of sensor in volts
   */
  void set_features(SensorType St,CompensateType Ct,char Alg,float we_e,float we_t,float ae_e,float ae_t,float sen);
  /**
   *   Table 2 (From Application notes): Suggested Algorithms for both A type and B type sensors
   *   +---------+-----------+---------------+
   *   |         |  Sugested |  Alternative  |
   *   | Sensor  | Algorithm |   Algorithm   |
   *   |         |-----+-----+-------+-------|
   *   |         |  A  |  B  |   A   |   B   |
   *   +---------+-----+-----+-------+-------+
   *   |   CO    |  1  |  1  |   4   |   2   |
   *   |   H2S   |  2  |  1  |   1   |   2   |
   *   |   NO    |  3  |  2  |   4   |   3   |
   *   |   NO2   |  1  |  1  |   3   |   3   |
   *   |   OX    |  3  |  1  |   1   |   3   |
   *   |   SO2   |  4  |  4  |1 or 3 |1 or 2 |
   *   +---------+-----+-----+-------+-------+
   *
   *   Function that establishes the algorithm for compensation
   *   @param  alg - algorithm 1 to 4
   */
  void set_algorithm(char alg);
  /**
   *   Return concentration in particles per million of gas
   *   @param  we_u - working electrode in volts
   *   @param  ae_u - Auxiliar electrode in volts
   */
  double calculate_ppm_u(double we_u,double ae_u);
  /**
   *   Return concentration in particles per million of gas corrected by temperature factor
   *   @param  we_u - working electrode in volts
   *   @param  ae_u - Auxiliar electrode in volts
   *   @param  temp - temperature in celsius degrees
   */
  double calculate_ppm_c(double we_u,double ae_u,float temp);
  /**
   *   Return pointer char * to name of sensor 
   */
  char * get_sensor_name(); 
  /**
   *   Return concentration in particles per million of gas
   *   previously you have to call calculate_ppm_u function
   */
  inline double get_ppm_uncorrected() {return ppm_u;};
  /**
   *   Return concentration in particles per million of gas corrected by temperature factor
   *   previously you have to call calculate_ppm_c function
   */
  inline double get_ppm_corrected()   {return ppm_c;};
public:
  /**
   *  set config values
   *  @param cnf - struct to alphasense config values
   */
  void setConfigValues(alphasenseConfig cnf,ambientSensor *amb,adcSensor *ad_we,adcSensor *ad_ae);
  /**
   * Return a char * of sensor name
   */
  char *getSensorName(){  return get_sensor_name(); }
  /**
   * return gas concentration
   * @param pp - parts per million pp = 0, parts per billion = 1
   * @param typeC - type of concentration 0 = corrected by temperature 1 = without correction 
   */
  float getConcentration(char pp = 0,char typeC = 0);
  /**
   * return adc sensor 
   */
  adcSensor *getAdc(int type=0);
  /**
   * return ambien sensor 
   */
  ambientSensor *getAmbient();
private:
  //Private Functions
  /**
   *   Return correction factor acording to temperature, sensor type and algorithm
   *   @param temp - temperature in celsius degrees
   */
  float  get_temp_comp_fac(float temp);
  /**
   *   Return Working electrode correct by temperature, type sensor and algorithm
   *   @param we_u - Working electrode in volts
   *   @param ae_u - Auxiliar electrode in volts
   *   @param temperature - temperature in celsius degrees
   */
  double get_WE_correct(double we_u,double ae_u,float temperature);
  //private Variables
  ambientSensor *ambiental;
  adcSensor *we_adc, *ae_adc;

  int Type_sensor;
  int Type_compensate;
  int Algorithm;
  float We_z,Ae_z;
  float We_t,Ae_t;
  float Sensitivity;
  double ppm_c,ppm_u;
};
#endif
