#include <iostream>
#include <iomanip>
#include <cstdlib>

#include "commonConfig.h"
#include "plog/Log.h"
#include "cfgSettings.h"

#define CFG_PATH "cfg/testLibConfig.cfg"
#define LOG_PATH "cfg/cfglogger.log"

int main(int argc, char const *argv[])
{
    /*Init logger */
    plog::init(plog::debug, LOG_PATH, 1000000, 3); // Initialize the logger. 1MB

    struct projectCfg mainCfg;

    if (getSettings(&mainCfg,CFG_PATH)) {
        std::cout << "Load config" << std::endl;
        printSettings(&mainCfg);
    } else {
        std::cout << "Culdn't get config" << std::endl;
        return 0;
    }

    std::cout << "Write Frame Counter..." << std::endl;

    mainCfg._lora.frameCounter++;
    mainCfg._tm.latestMeasure = time(NULL);

    std::cout << "Time is: " << mainCfg._tm.latestMeasure << std::endl;

    writeFrameCounter(&mainCfg,CFG_PATH);

    std::cout  << "End of the program\n";

    return 0;
}


