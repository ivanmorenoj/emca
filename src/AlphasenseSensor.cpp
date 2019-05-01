#include "AlphasenseSensor.h"
#include "CompensationFactors.h"
#include <stdio.h>

// first constructor, set default values
AlphasenseSensor::AlphasenseSensor(){
  Type_sensor = 0;
  Type_compensate = 0;
  Algorithm = 1;
  We_z = Ae_z = We_t = Ae_t = 0.0;
  Sensitivity = 0;
  ppm_c = 0;
  ppm_u = 0;
  ambiental = NULL;
  we_adc = ae_adc = NULL;
}
AlphasenseSensor::AlphasenseSensor(SensorType St,CompensateType Ct,char Alg,float we_e,float we_t,float ae_e,float ae_t,float sen){
  // set the sensor features specifies in bag
  set_features(St,Ct,Alg,we_e,we_t,ae_e,ae_t,sen);
  ambiental = NULL;
  we_adc = ae_adc = NULL;
}
//  Destroyer - nothing to do
AlphasenseSensor::~AlphasenseSensor(){

}
void AlphasenseSensor::set_features(SensorType St,CompensateType Ct,char Alg,float we_e,float we_t,float ae_e,float ae_t,float sen){
  Type_sensor = St;
  Type_compensate = Ct;
  // limiting the values of algorithm
  Algorithm = (Alg> 0 && Alg<= 4)? Alg : 1;
  We_z = we_t - we_e;
  Ae_z = ae_t - ae_e;
  We_t = we_t;
  Ae_t = ae_t;
  Sensitivity = sen;
  ppm_c = ppm_u = 0;
}
void AlphasenseSensor::set_algorithm(char alg){
  // limit the values in [1 - 4]
  Algorithm = (alg> 0 && alg<= 4)? alg : 1;
}
double AlphasenseSensor::calculate_ppm_u(double we_u,double ae_u){
  // calculate the gas concentration according application notes
  //ppm_u = ((we_u - We_z) - (ae_u - Ae_z)) / Sensitivity;
  ppm_u = ((we_u - We_t) - (ae_u - Ae_t)) / Sensitivity;
  return ppm_u;
}
double AlphasenseSensor::calculate_ppm_c(double we_u,double ae_u,float temp){
  ppm_c = get_WE_correct(we_u,ae_u,temp) / Sensitivity;
  return ppm_c;
}
// Private methods
float AlphasenseSensor::get_temp_comp_fac(float temp){
    unsigned char factor_index = 0;
    // choosing the factor according temperature, sensor and algorithm
    if((temp >= -35.0) || (temp <= 55.0) ) // if is out of range
      for(float t = -35.0 ;t <= 55.0; t += 10.0){ // 10 degrees jumps
        if((temp >= t) && (temp <= (t + 10.0)))
          break;
        factor_index++;
      }
    else
      factor_index = 3; // default from zero
    return CompensationFactors[Type_compensate][Algorithm-1][factor_index];
}
double AlphasenseSensor::get_WE_correct(double we_u,double ae_u,float temperature){
  double aux = 0.0;
  /*
  *   This part calculate the algorithm to correct WE output for the efects of temperature
  *   See table 1 of aplication notes "Correcting for background current..."
  */
  switch (Algorithm) {
    case 1: // first algorithm
      aux = we_u - get_temp_comp_fac(temperature)*ae_u;   break;
    case 2: // second algorithm
      aux = we_u - get_temp_comp_fac(temperature)*ae_u*(We_z/Ae_z);    break;
    case 3: // third algorithm
      aux =  we_u - (We_z - Ae_z) - get_temp_comp_fac(temperature)*ae_u;   break;
    case 4: // fourth algorithm
      aux = we_u - We_z - get_temp_comp_fac(temperature);  break;
  }
  return aux;
}
char * AlphasenseSensor::get_sensor_name(){
  char * ptr; 
  switch(Type_sensor){
    case CO_SENSOR:   ptr = (char *)"CO";   break;
    case H2S_SENSOR:  ptr = (char *)"H2S";  break;
    case NO_SENSOR:   ptr = (char *)"NO";   break;
    case NO2_SENSOR:  ptr = (char *)"NO2";  break;
    case O3_SENSOR:   ptr = (char *)"O3";   break;
    case S02_SENSOR:  ptr = (char *)"SO2";  break;
    default:
      ptr = (char *)"--";
  }
  return ptr;
} 
AlphasenseSensor::AlphasenseSensor(alphasenseConfig conf,ambientSensor *amb,adcSensor *ad_we,adcSensor *ad_ae){
  setConfigValues(conf,amb,ad_we,ad_ae);
}
float AlphasenseSensor::getConcentration(char pp,char typeC){
  float concentration = 0,temp,we_v,ae_v;
  if (isTime()) {
    temp = ambiental->getTemperature();
    if(adjustFunc) {
      we_v = adjustFunc(we_adc->getVoltage(),0);
      ae_v = adjustFunc(ae_adc->getVoltage(),1);
    } else {
      we_v = we_adc->getVoltage();
      ae_v = ae_adc->getVoltage();
    }

    ppm_u = calculate_ppm_u(we_v,ae_v);
    ppm_c = calculate_ppm_c(we_v,ae_v,temp);
  }

  if(typeC == 0 )  // corrected
    if(pp == 0)
      concentration = ppm_c;
    else
      concentration = ppm_c*1000;
  else
    if(pp == 0)
      concentration = ppm_u;
    else
      concentration = ppm_u*1000;

  return concentration;
}
void AlphasenseSensor::setConfigValues(alphasenseConfig cnf,ambientSensor *amb,adcSensor *ad_we,adcSensor *ad_ae){
  set_features(cnf.sensor,cnf.compensate,cnf.algorithm,cnf.we_electronic,
               cnf.we_total,cnf.ae_electronic,cnf.ae_total,cnf.sensitivity);
  ambiental = amb;
  we_adc = ad_we;
  ae_adc = ad_ae;
}
adcSensor *AlphasenseSensor::getAdc(int type) {
  if (type) // != 0
    return we_adc;
  else
    return ae_adc;
}
ambientSensor *AlphasenseSensor::getAmbient() {
  return ambiental;
}