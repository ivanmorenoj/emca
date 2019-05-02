#include <iostream>
#include "plog/Log.h"

#include "commonConfig.h"
#include "cfgSettings.h"

#define CFG_PATH    "cfg/testLibConfig.cfg"
#define LOG_PATH    "cfg/emcaLOG.log"

int main(int argc, char const *argv[])
{
    plog::init(plog::debug, LOG_PATH, 1000000, 3); // Initialize the logger. 1MB
    struct projectCfg mainCfg;
    if (getSettings(&mainCfg,CFG_PATH)) {
        std::cout << "succes reading file \n" ;
        printSettings(&mainCfg);
    }

    std::cout << "end of the program" << std::endl;
    return 0;
}

