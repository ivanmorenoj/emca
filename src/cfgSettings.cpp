#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>
#include <cstring>
#include <time.h>
#include "plog/Log.h"
#include "cfgSettings.h"

static time_t strToTime(const char *_inputStr) {
    //static const char *_format = "%d/%m/%Y";
    struct tm _st = {0};
    time_t _rawtime;
    if (strptime(_inputStr,"%d/%m/%Y",&_st) == NULL){
        PLOG_ERROR << "Error in parsing time";
    }
    _rawtime = mktime(&_st);
    return _rawtime;
}
static std::string timeToString(time_t _currenttime) {
    struct tm *_st;
    char _buff[80];
    _st = localtime(&_currenttime);
    strftime(_buff,sizeof(_buff),"%d/%m/%Y",_st);
    return std::string(_buff);
}
int getSettings(struct projectCfg *_con,const char *_path) {
    int success = 0;
    std::string _tmpStr;    
    libconfig::Config cfg;
    
    try {
        cfg.readFile(_path);
    } catch (const libconfig::FileIOException &fioex) {
        PLOG_ERROR << "I/O error while reading file." << std::endl;
        exit(EXIT_FAILURE);
    } catch (const libconfig::ParseException &pex) {
        PLOG_ERROR << "Parse error at " << pex.getFile() << ":" << pex.getLine()
             << " - " << pex.getError() << std::endl;
        exit(EXIT_FAILURE);
    }

    const libconfig::Setting &root = cfg.getRoot();

    try {
        /* lookup bme280 address */
        _con->bme280Address = cfg.lookup("bme280Adrres");
        
        /* Lookup time*/
        const libconfig::Setting &_tm = root["timeCfg"];
        _tm.lookupValue("sampling", _con->_tm.sampling); 
        _tm.lookupValue("restore", _con->_tm.restore);
        _tm.lookupValue("latestMeasure", _con->_tm.latestMeasure);

        /* lookup database config*/
        std::string tmp[4];
        const libconfig::Setting &_db = root["sqlDataBase"];
        _db.lookupValue("user",tmp[0]);
        _db.lookupValue("password",tmp[1]);
        _db.lookupValue("host",tmp[2]);
        _db.lookupValue("schema",tmp[3]);
        /* copy values */
        strcpy(_con->_sql.user,tmp[0].c_str());
        strcpy(_con->_sql.pass,tmp[1].c_str());
        strcpy(_con->_sql.host,tmp[2].c_str());
        strcpy(_con->_sql.schema,tmp[3].c_str());

        /* lookup alphasense sensor config*/
        const libconfig::Setting &_gas = root["gasSensors"]["alphasenseSensor"];
        for (int i = 0 ; i < _gas.getLength() ; ++i) {
            /* lookup sensor config */
            const libconfig::Setting &_alph = _gas[i];

            const libconfig::Setting &_gas = _alph["Sensor"];

            _gas.lookupValue("SensorType"    ,(int &)_con->_gas[i].gas.sensor);
            _gas.lookupValue("Compensate"    ,(int &)_con->_gas[i].gas.compensate);
            _gas.lookupValue("Algorithm"     ,(int &)_con->_gas[i].gas.algorithm);
            _gas.lookupValue("we_electronic" ,_con->_gas[i].gas.we_electronic);        
            _gas.lookupValue("we_total"      ,_con->_gas[i].gas.we_total);
            _gas.lookupValue("ae_electronic" ,_con->_gas[i].gas.ae_electronic);        
            _gas.lookupValue("ae_total"      ,_con->_gas[i].gas.ae_total);
            _gas.lookupValue("sensitivity"   ,_con->_gas[i].gas.sensitivity);

            /* lookup adc settings */
            const libconfig::Setting &_adc = _alph["Adc"];
            for (int j = 0; j < _adc.getLength() ; ++j) {
                const libconfig::Setting &_ad = _adc[j];
                _con->_gas[i].adc[j].address    = (uint8_t)(int)_ad.lookup("address");
                _con->_gas[i].adc[j].bitrate    = (uint8_t)(int)_ad.lookup("bitrate");
                _con->_gas[i].adc[j].gain       = (uint8_t)(int)_ad.lookup("pga");
                _con->_gas[i].adc[j].conv_mode  = (uint8_t)(int)_ad.lookup("convmode");
                _con->_gas[i].adc[j].channel    = (uint8_t)(int)_ad.lookup("channel");
            }

            /* lookup operation settings */
            const libconfig::Setting &_op = _alph["Operation"];

            _con->_gas[i].cond.lifeTime = (int)_op["lifetime"];
            _con->_gas[i].cond.lifeTime *= 86400;
            //_con->_gas[i].cond.startupTime = (int)_op["startuptime"];
            _op.lookupValue("startuptime",_tmpStr);
            _con->_gas[i].cond.startupTime = (int) strToTime(_tmpStr.c_str());

            for(int k = 0; k < 2 ; ++k){
                _con->_gas[i].cond.temperature[k] = _op["temperature"][k];
                _con->_gas[i].cond.pressure[k]    = _op["pressure"][k];
                _con->_gas[i].cond.humidity[k]    = _op["humidity"][k];
                _con->_gas[i].cond.ranges[k]      = _op["ranges"][k];
            }
        }
        
        /* lookup aeroqual settings */
        const libconfig::Setting &_aq = root["gasSensors"]["aeroqual"];
        
        /* lookup adc settings*/
        const libconfig::Setting &_aq_ad = _aq["Adc"];
        _con->_o3gas.adc.address   = (uint8_t)(int)_aq_ad.lookup("address");
        _con->_o3gas.adc.bitrate   = (uint8_t)(int)_aq_ad.lookup("bitrate");
        _con->_o3gas.adc.gain      = (uint8_t)(int)_aq_ad.lookup("pga");
        _con->_o3gas.adc.conv_mode = (uint8_t)(int)_aq_ad.lookup("convmode");
        _con->_o3gas.adc.channel   = (uint8_t)(int)_aq_ad.lookup("channel");

        /* lookup operation settings */
        const libconfig::Setting &_op_a = _aq["Operation"];

        _con->_o3gas.cond.lifeTime = (int)_op_a["lifetime"];
        _con->_o3gas.cond.lifeTime *= 86400; 
        //_con->_o3gas.cond.startupTime = (int)_op_a["startuptime"];        
        _op_a.lookupValue("startuptime",_tmpStr);
        _con->_o3gas.cond.startupTime = (int) strToTime(_tmpStr.c_str());
        for(int k = 0; k < 2 ; ++k){
            _con->_o3gas.cond.temperature[k] = _op_a["temperature"][k];
            _con->_o3gas.cond.pressure[k]    = _op_a["pressure"][k];
            _con->_o3gas.cond.humidity[k]    = _op_a["humidity"][k];
            _con->_o3gas.cond.ranges[k]      = _op_a["ranges"][k];
        }

         /* lookup LoRa settings */
        const libconfig::Setting &_LoRa = root["LoRa"];
        _LoRa.lookupValue("port"        ,_con->_lora.port);
        _LoRa.lookupValue("txPower"     ,_con->_lora.txPower);
        _LoRa.lookupValue("actMethod"   ,_con->_lora.activationMethod);
        _LoRa.lookupValue("dataRate"    ,_con->_lora.dataRate);
        _LoRa.lookupValue("channel"     ,_con->_lora.channel);
        _LoRa.lookupValue("NwkSKey"     ,_con->_lora.NwkSKey);
        _LoRa.lookupValue("AppSKey"     ,_con->_lora.AppSKey);
        _LoRa.lookupValue("DevAddr"     ,_con->_lora.DevAddr);
        _LoRa.lookupValue("frameCounter",_con->_lora.frameCounter);

        success = 1;
    }catch (const libconfig::SettingNotFoundException &nfex) {
        PLOG_FATAL << "Settings not found!";
        exit(EXIT_FAILURE);
    }catch (const libconfig::SettingTypeException &ste) {
        PLOG_FATAL << "Settings Type exception on " << ste.what();
        exit(EXIT_FAILURE);        
    }

    return success;
}

void printSettings(struct projectCfg *_cfg){
    std::cout << "BME280 address:\t" << _cfg->bme280Address << std::endl;

    std::cout << "\nTime data"
              << "\n\tSampling:\t" << _cfg->_tm.sampling 
              << "\n\tRestore:\t" << _cfg->_tm.restore 
              << "\n\tLatest Measurement:\t" << _cfg->_tm.latestMeasure << std::endl;

    std::cout << "\nData Base"
              << "\n\tDB user:\t" << std::string(_cfg->_sql.user)
              << "\n\tDB pass:\t" << std::string(_cfg->_sql.pass)
              << "\n\tDB host:\t" << std::string(_cfg->_sql.host)
              << "\n\tDB schema:\t" << std::string(_cfg->_sql.schema) << std::endl;
    
    for(int i = 0; i < 3; ++i){
        std::cout << "\nAlphasense sensor [" << i+1 << "]" 
                  << "\n\tSensor type:\t\t" << (int)_cfg->_gas[i].gas.sensor
                  << "\n\tSensor compensate:\t" << (int)_cfg->_gas[i].gas.compensate
                  << "\n\tSensor algorithm:\t" << (int)_cfg->_gas[i].gas.algorithm
                  << "\n\tSensor we electronic:\t" << _cfg->_gas[i].gas.we_electronic
                  << "\n\tSensor ae electronic:\t" << _cfg->_gas[i].gas.ae_electronic
                  << "\n\tSensor we total:\t" << _cfg->_gas[i].gas.we_total
                  << "\n\tSensor ae total:\t" << _cfg->_gas[i].gas.ae_total
                  << "\n\tSensor sensotivity:\t" << _cfg->_gas[i].gas.sensitivity << std::endl;
        for(int j = 0; j < 2; ++j) { 
            std::cout << "\nADC [" << i+1 << "]["<< j + 1 << "]" 
                      << "\n\taddress:\t" << (int)_cfg->_gas[i].adc[j].address
                      << "\n\tbitrate:\t" << (int)_cfg->_gas[i].adc[j].bitrate
                      << "\n\tpga:\t\t" << (int)_cfg->_gas[i].adc[j].gain
                      << "\n\tconversionmode:\t" << (int)_cfg->_gas[i].adc[j].conv_mode
                      << "\n\tchannel:\t" << (int)_cfg->_gas[i].adc[j].channel << std::endl;
        }
        std::cout << "\nRanges " << "[" << i + 1<< "]"
                  << "\n\tlifetime:\t" << timeToString((time_t)(_cfg->_gas[i].cond.startupTime + _cfg->_gas[i].cond.lifeTime))
                  << "\n\tstartuptime:\t" << timeToString((time_t)_cfg->_gas[i].cond.startupTime)
                  << "\n\ttemperature:\t" << _cfg->_gas[i].cond.temperature[0]
                  << "\t" << _cfg->_gas[i].cond.temperature[1] 
                  << "\n\tpressure:\t" << _cfg->_gas[i].cond.pressure[0]
                  << "\t" << _cfg->_gas[i].cond.pressure[1] 
                  << "\n\thumidity:\t" << _cfg->_gas[i].cond.humidity[0]
                  << "\t" << _cfg->_gas[i].cond.humidity[1] 
                  << "\n\trangues:\t" << _cfg->_gas[i].cond.ranges[0]
                  << "\t" << _cfg->_gas[i].cond.ranges[1] << std::endl;
    }
    
    std::cout << "\nADC aeroqual" 
              << "\n\taddress:\t" << (int)_cfg->_o3gas.adc.address
              << "\n\tbitrate:\t" << (int)_cfg->_o3gas.adc.bitrate
              << "\n\tpga:\t\t" << (int)_cfg->_o3gas.adc.gain
              << "\n\tconversionmode:\t" << (int)_cfg->_o3gas.adc.conv_mode
              << "\n\tchannel:\t" << (int)_cfg->_o3gas.adc.channel << std::endl;
    
    std::cout << "\nRanges aeroqual "
              << "\n\tlifetime:\t" << timeToString((time_t)(_cfg->_o3gas.cond.startupTime + _cfg->_o3gas.cond.lifeTime))
              << "\n\tstartuptime:\t" << timeToString((time_t)_cfg->_o3gas.cond.startupTime)
              << "\n\ttemperature:\t" << _cfg->_o3gas.cond.temperature[0]
              << "\t" << _cfg->_o3gas.cond.temperature[1] 
              << "\n\tpressure:\t" << _cfg->_o3gas.cond.pressure[0]
              << "\t" << _cfg->_o3gas.cond.pressure[1] 
              << "\n\thumidity:\t" << _cfg->_o3gas.cond.humidity[0]
              << "\t" << _cfg->_o3gas.cond.humidity[1] 
              << "\n\trangues:\t" << _cfg->_o3gas.cond.ranges[0]
              << "\t" << _cfg->_o3gas.cond.ranges[1] << std::endl;

    std::cout << "\nLoRa"
          << "\n\tport:\t\t"        << _cfg->_lora.port
          << "\n\ttxPower:\t"       << _cfg->_lora.txPower
          << "\n\tactMethod:\t"     << _cfg->_lora.activationMethod
          << "\n\tdataRate:\t"      << _cfg->_lora.dataRate
          << "\n\tchannel:\t"       << _cfg->_lora.channel
          << "\n\tNwkSKey:\t"       << _cfg->_lora.NwkSKey
          << "\n\tAppSKey:\t"       << _cfg->_lora.AppSKey
          << "\n\tDevAddr:\t"       << _cfg->_lora.DevAddr
          << "\n\tFramecounter:\t"  << _cfg->_lora.frameCounter << std::endl;
}

void writeFrameCounter(struct projectCfg *_con,const char *_path) {
    libconfig::Config cfg;

    try {
        cfg.readFile(_path);

        libconfig::Setting &frameCounter = cfg.lookup("LoRa.frameCounter");
        libconfig::Setting &latestMeasure = cfg.lookup("timeCfg.latestMeasure");

        frameCounter = (int)_con->_lora.frameCounter;
        latestMeasure = time(NULL); 

        cfg.writeFile(_path);

    } catch (const libconfig::FileIOException &fioex) {
        PLOG_ERROR << "I/O error while reading file." << std::endl;
    } catch (const libconfig::ParseException &pex) {
        PLOG_ERROR << "Parse error at " << pex.getFile() << ":" << pex.getLine()
             << " - " << pex.getError() << std::endl;
    }
}