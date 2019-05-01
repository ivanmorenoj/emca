#include <stdio.h>
#include <time.h>
#include <string.h>

#include "commonConfig.h"
#include "MCP3424.h"
#include "bme280.h"
#include "AlphasenseSensor.h"
#include "aeroqualSM50.h"
#include "sqlConnector.h"
#include "flagManager.h"
#include "CircularList.h"
#include "CircularList.cpp"
#include "cfgSettings.h"

#define CFG_PATH    "/home/dev/emca_gases/mainConfig.cfg"

/**
 *  adjustFunction for alphasense sensor
*/
double adjust(double vi,unsigned char opt){
    return  2.488132904 * vi;
}

int main(int argc, char const *argv[])
{   
    time_t rawtime;
    struct tm * timeinfo;
    char _tBuff[80];

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (_tBuff,80,"%x-%X",timeinfo);

    printf("[%s] Init program PID = %d\n",_tBuff,(int)getpid());
    
    /* DataBase structs*/
    struct ambVariables _ab;
    struct db_info _info;
    struct db_values _values;
    _info.amb = &_ab;
    _values.amb = &_ab;

    /* General Config */
    struct projectCfg mainCfg = {0};
    if (getSettings(&mainCfg,CFG_PATH)){
        printf("[+] Get config\n");
        printSettings(&mainCfg);
    }
    else {
        printf("[-] Culdn't get config\n");
        return 0;
    } 


    /* initializing ambiental sensor */
    bme280 _amb(mainCfg.bme280Address);
    
    /* initializing adc sensors */
    MCP3424 _adc[7];
    for(int i = 0; i < 3 ; ++i)
        for(int j = 0; j < 2 ; ++j)
            _adc[i*2 + j].setConfigValues(&mainCfg._gas[i].adc[j]);
    _adc[6].setConfigValues(&mainCfg._o3gas.adc);
    
    /* initializing alphasense sensors*/
    AlphasenseSensor _gasS[3];
    for(int i = 0; i < 3 ; ++i) {
        _gasS[i].setConfigValues(mainCfg._gas[i].gas,
                                 &_amb,&_adc[i*2],
                                 &_adc[i*2 + 1]);
        _gasS[i].setAdjustFunct(adjust);
    }

    /* initializing aeroqual sensor*/
    aeroqualSM50 _o3gas;
    _o3gas.setConfigValues(&_amb,&_adc[6]);
    
    /* initializing flag manager*/
    flagManager _fm[4];
    for(int i = 0; i < 3 ; ++i)
        _fm[i].setConfig(&mainCfg._gas[i].cond,&_amb,&_gasS[i]);
    _fm[3].setConfig(&mainCfg._o3gas.cond,&_amb,&_o3gas);
    
    /* initializing database */
    sqlConnector _db;
    _db.setUser(mainCfg._sql.user);
    _db.setPassword(mainCfg._sql.pass);
    _db.setHost(mainCfg._sql.host);
    _db.setSchema(mainCfg._sql.schema);
    
    /* add sensors to main process */
    CircularList<flagManager> _mp;
    for(int i = 0; i < 4 ; ++i)
        _mp.addNode(&_fm[i]);
    
    /* connect to database */
    _db.connect();
    printf("[+] Init inisertions to database\n");

    /*printf("Testing adc's\n");
    for (int i = 0; i < 7; i++) {
        printf("Voltage adc[%d]: %.6f\n",i,_adc[i].getVoltage());
    }*/
    
    for(;;) {
        _ab.temp = _amb.getTemperature();
        _ab.pre = _amb.getPressure();
        _ab.hum = _amb.getHumidity();

        //printf("TEMP\t: %f\nPRES\t: %f\nHUM\t: %f\n",_ab.temp,_ab.pre,_ab.hum);

        for(int i = 0; i < 4 ; ++i) {        
            _info.gasType = _mp.getCurrentData()->getGasSensor()->getSensorName();

            if (strcmp(_info.gasType, "O3") == 0){
                _info.ppb_c = _mp.getCurrentData()->getGasSensor()->getConcentration(1);
                _info.ppb   = 0.0;
                _info.we    = _mp.getCurrentData()->getGasSensor()->getAdc()->getVoltage();
                _info.ae    = 0.0;
            } else {
                _info.ppb_c = _mp.getCurrentData()->getGasSensor()->getConcentration(1,0);
                _info.ppb   = _mp.getCurrentData()->getGasSensor()->getConcentration(1,1);

                _info.we    = adjust(_mp.getCurrentData()->getGasSensor()->getAdc(1)->getVoltage(),0);
                _info.ae    = adjust(_mp.getCurrentData()->getGasSensor()->getAdc(0)->getVoltage(),0);
            }

            _values.gas[i].ppb = _info.ppb_c;
            _values.gas[i].flag = (char *)_mp.getCurrentData()->getFlag();
            
            _db.insert_in_GAS_INFO(&_info);
            _mp.next();
        }
        _db.insert_in_GAS_VALUES(&_values);

        sleep(mainCfg._tm.sampling);
    }

    return 0;
}
