/**
 *  Author: Ivan Moreno <ivan1697@gmail.com>
 *      october 2019
 */
#include "plog/include/plog/Log.h"
#include "commonConfig.h"
#include "cfgSettings.h"
#include "clientModel.h"

#define CFG_PATH    "cfg/testLibConfig.cfg"
#define LOG_PATH    "cfg/loralogger.log"
#define EVER        ;;

int main(int argc, char const *argv[])
{
    /* structs */
    projectCfg _mainCfg;
    ambVariables _amb;
    db_values _gv;
    lora_payload _lp;

    /* Init */
    _gv.amb = &_amb;

    /* objets */
    clientModel _client;

    plog::init(plog::debug, LOG_PATH, 1000000, 3); // Initialize the logger. 1MB
    PLOG_INFO << ">>>>>>>>>>>>>>>Init Program<<<<<<<<<<<<<<<";

    /* Pring log path and cfg path */
    printf("Logs saved in: %s\nConfig file loaded from: %s\n",LOG_PATH,CFG_PATH);

    /* General Config */
    if (getSettings(&_mainCfg,CFG_PATH)) {
        PLOG_INFO << "Load config from: " << CFG_PATH;
        printSettings(&_mainCfg);
    } else {
        PLOG_FATAL << "Couldn't get config";
        return EXIT_FAILURE;
    }

    /* Connect with lora stick */
    _client.setConfig(&_mainCfg._lora);

    /*Open serial port loaded from cfg file */
    if (!_client.openSerial()) {
        PLOG_FATAL << "Cannot connect to serial port";
        return EXIT_FAILURE;
    }
    
    /*Send config Values to usb stick */
    std::cout << "Sending Config Values to LoRa Stick\n";

    uint8_t _errorCounter = 0;
    while ( _client.sendTP() || _client.sendAM() || 
            _client.sendDR() || _client.sendCH() || 
            _client.sendNK() || _client.sendAK() || 
            _client.sendDA() || _client.sendFC() ) {
        PLOG_ERROR << "Problem when sending config to usb stick";
        sleep(30);
        if (_errorCounter++ > 5) {
            PLOG_ERROR << "Cannot send config Values, exit";
            return EXIT_FAILURE;
        }
    }

    for (EVER) {
        _amb.hum    = 54;
        _amb.temp   = 30;
        _amb.pre    = 78000;

        for (uint8_t i = 0; i < 4; i++)
            _gv.gas[i].ppb = 1000 + i*100;

        std::cout << "Sending data... " << _mainCfg._lora.frameCounter << std::endl;

        if (!_client.sendSP(&_lp,&_gv)) {
            _mainCfg._lora.frameCounter++;
            writeFrameCounter(&_mainCfg,CFG_PATH);
        }
        
        sleep(30);
    }
    
    return EXIT_SUCCESS;
}
