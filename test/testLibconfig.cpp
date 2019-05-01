#include <iostream>

#include "commonConfig.h"
#include "cfgSettings.h"

#define CFG_PATH    "cfg/testLibConfig.cfg"

int main(int argc, char const *argv[])
{
    struct projectCfg mainCfg;
    if (getSettings(&mainCfg,CFG_PATH)) {
        std::cout << "succes reading file \n" ;
        printSettings(&mainCfg);
    }

    std::cout << "end of the program" << std::endl;
    return 0;
}

