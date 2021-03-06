#include <stdio.h>
#include <time.h>
#include <string.h>

#include "plog/include/plog/Log.h"
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
#include "clientModel.h"

#define EVER    ;;    

#define INSTALL
#ifdef INSTALL
    #define CFG_PATH    "/etc/emca/config.cfg"
    #define LOG_PATH    "/var/log/emca/emca.log"
#else
    #define CFG_PATH    "mainConfig.cfg"
    #define LOG_PATH    "emcaLOG.log"
#endif 

/**
 *  AdjustFunction for alphasense sensor
 */
double adjust(double vi,unsigned char opt) {
    return  (-3.02157*vi) + 2.66755;
}

int main(int argc, char const *argv[])
{   
    plog::init(plog::debug, LOG_PATH, 1000000, 3); // Initialize logger. 1MB file size
    
    fprintf(stdout,"[+] Init program PID = %d\n  Config file in \'%s\'\n  Log file in \'%s\'\n",
                (int)getpid(),CFG_PATH,LOG_PATH);

    PLOG_INFO << "\n\t\t>>>>>>>>>>>>>>>>> Init program [" << (int)getpid() << "] <<<<<<<<<<<<<<<<<<<<";
    
    /* General Variables */
    uint8_t _errorCounter = 0;

    /* DataBase structs*/
    ambVariables _ab;
    db_info      _info;
    db_values    _values;
    lora_payload _lp;

    _info.amb   = &_ab;
    _values.amb = &_ab;

    /* General Config */
    projectCfg mainCfg;

    if (getSettings(&mainCfg,CFG_PATH)) {
        PLOG_INFO << "Load config from: " << CFG_PATH;
        //printSettings(&mainCfg);
    } else {
        PLOG_FATAL << "Culdn't get config";
        return EXIT_FAILURE;
    } 

    /* Client LoRa */
    clientModel _loraClient;
    _loraClient.setConfig(&mainCfg._lora);

    /* Initializing ambiental sensor */
    bme280 _amb(mainCfg.bme280Address);
    
    /* Initializing adc sensors */
    MCP3424 _adc[7];
    for (uint8_t i = 0; i < 3; ++i)
        for (uint8_t j = 0; j < 2; ++j)
            _adc[i*2 + j].setConfigValues(&mainCfg._gas[i].adc[j]);
    _adc[6].setConfigValues(&mainCfg._o3gas.adc);
    
    /* Initializing alphasense sensors*/
    AlphasenseSensor _gasS[3];
    for (uint8_t i = 0; i < 3; ++i) {
        _gasS[i].setConfigValues(mainCfg._gas[i].gas,
                                 &_amb, &_adc[i*2],
                                 &_adc[i*2 + 1]);
        _gasS[i].setAdjustFunct(adjust);
    }

    /* Initializing aeroqual sensor*/
    aeroqualSM50 _o3gas;
    _o3gas.setConfigValues(&_amb, &_adc[6]);
    
    /* Add sensors to main process */
    CircularList<flagManager> _mp;
    for (uint8_t i = 0; i < 3; ++i)
        _mp.addNode(new flagManager(&mainCfg._gas[i].cond, &_amb, &_gasS[i]));
    _mp.addNode(new flagManager(&mainCfg._o3gas.cond, &_amb, &_o3gas));

    /* Open serial port loaded from cfg file */
    if (!_loraClient.openSerial()) {
        PLOG_FATAL << "Cannot connect to serial port";
        return EXIT_FAILURE;
    }

    /* Send config Values to usb stick */
    _errorCounter = 0;
    while ( _loraClient.sendTP() || _loraClient.sendAM() || 
            _loraClient.sendDR() || _loraClient.sendCH() || 
            _loraClient.sendNK() || _loraClient.sendAK() || 
            _loraClient.sendDA() || _loraClient.sendFC() ) {
        PLOG_ERROR << "Problem when sending config to usb stick";
        sleep(15);
        if (++_errorCounter > 5) {
            PLOG_FATAL << "Cannot send config Values, exit";
            return EXIT_FAILURE;
        }
    }

    /* Initializing database */
    sqlConnector _db;
    _db.setUser(mainCfg._sql.user);
    _db.setPassword(mainCfg._sql.pass);
    _db.setHost(mainCfg._sql.host);
    _db.setSchema(mainCfg._sql.schema);
        
    /* Restore time after init sensor */
    PLOG_INFO << "Samplig Time is [" <<  mainCfg._tm.sampling << "] seconds"; 
    
    if (time(NULL) - mainCfg._tm.latestMeasure > (uint32_t)mainCfg._tm.restore) {
        PLOG_INFO << "Wait for [" << mainCfg._tm.restore << "] seconds";
        for (uint8_t t = 0; t < mainCfg._tm.restore; ++t)
            sleep(1);
    } else {
        PLOG_INFO << "Latest time is less than restore time, only wait for [" << 180 << "] seconds";
        sleep(180);
    }

    /* Connect to database */
    _db.connect();
    PLOG_INFO  << "Init inisertions to database";
    PLOG_DEBUG << "Init with frameCounter = " << mainCfg._lora.frameCounter;
    
    /* Reset error counter */
    _errorCounter = 0;
    for (EVER) {
        _ab.temp  = _amb.getTemperature();
        _ab.pre   = _amb.getPressure();
        _ab.hum   = _amb.getHumidity();

        for (uint8_t i = 0; i < 4; ++i) {        
            _info.gasType = _mp.getCurrentData()->getGasSensor()->getSensorName();

            if (strcmp(_info.gasType, "O3") == 0) {
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

            _values.gas[i].ppb  = _info.ppb_c;
            _values.gas[i].flag = (char *)_mp.getCurrentData()->getFlag();
            
            _db.insert_in_GAS_INFO(&_info);
            _mp.next();
        }
        _db.insert_in_GAS_VALUES(&_values);

        if (!_loraClient.sendSP(&_lp, &_values)) {
            mainCfg._lora.frameCounter++;
            writeFrameCounter(&mainCfg, CFG_PATH);
            _errorCounter = 0;
        } else {
            _errorCounter++;
            PLOG_ERROR << "No response Recived when send payload for " << _errorCounter << " times";
            if (_errorCounter > 10) {
                PLOG_FATAL << "No response from LoRa Stick for +10 times, exit";
                return EXIT_FAILURE;
            }
        }

	for (int i = 0; i < mainCfg._tm.sampling; ++i)
	  sleep(1);
    }

    return EXIT_SUCCESS;
}
