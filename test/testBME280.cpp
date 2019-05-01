#include "bme280.h"
#include <stdio.h>

int main(){
    bme280 amb(0x77);

    printf("Temperature: %.4f\n Pressure: %.4f\n Humidity: %.4f\n",
        amb.getTemperature(),amb.getPressure(),amb.getHumidity());

    printf("end of the program\n");
    return 0;
}