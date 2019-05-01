#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>

#include "commonConfig.h"

using namespace std;
using namespace libconfig;

int main(int argc, char const *argv[])
{
    Config cfg;

    struct alphasenseConfig alph;
    struct MCP3424Config adc[2];

    try {
        cfg.readFile("cfg/testLibConfig.cfg");
    } catch (const FileIOException &fioex) {
        cerr << "I/O error while reading file." << endl;
        return(EXIT_FAILURE);
    } catch (const ParseException &pex) {
        cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
             << " - " << pex.getError() << endl;
        return(EXIT_FAILURE);
    }

    const Setting &root = cfg.getRoot();

    int address = cfg.lookup("BME280Address");

    try {
        const Setting &gas = root["gasSensors"]["COSensor"]["Sensor"];

        gas.lookupValue("SensorType"    ,(int&)alph.sensor);
        gas.lookupValue("Compensate"    ,(int&)alph.compensate);
        gas.lookupValue("Algorithm"     ,(int&)alph.algorithm);
        gas.lookupValue("we_electronic" ,alph.we_electronic);        
        gas.lookupValue("we_total"      ,alph.we_total);
        gas.lookupValue("ae_electronic" ,alph.ae_electronic);        
        gas.lookupValue("ae_total"      ,alph.ae_total);
        gas.lookupValue("sensitivity"   ,alph.sensitivity);

        cout << "addr: "        << hex << address << endl
            << "sensor: "       << alph.sensor << endl
            << "compensate: "   << alph.compensate << endl
            << "algorithm: "    << (int)alph.algorithm << endl
            << "we_electronic: "<< alph.we_electronic << endl
            << "we_total: "     << alph.we_total << endl
            << "ae_electronic: "<< alph.ae_electronic << endl
            << "ae_total: "     << alph.ae_total << endl
            << "sensitivity: "  << alph.sensitivity << endl;
        
        Setting &ads = root["gasSensors"]["COSensor"]["Adc"];
        
        int count = ads.getLength();

        for(int i = 0;i < count ;++i) {
            int var[5];
            const Setting &ad = ads[i];

            if(!(ad.lookupValue("address",var[0])
                && ad.lookupValue("bitrate",var[1])
                && ad.lookupValue("pga",var[2])
                && ad.lookupValue("convmode",var[3])
                && ad.lookupValue("channel",var[4])))
            continue;
            
            adc[i].address = (char)var[0];
            adc[i].bitrate = (char)var[1];
            adc[i].gain = (char)var[2];
            adc[i].conv_mode = (char)var[3];
            adc[i].channel = (char)var[4];

            cout << "\n>>>>>>ADC: " << i +1 << endl; 

            cout << "address: " << hex << (int)adc[i].address << endl
                 << "bitrate: " << dec <<(int)adc[i].bitrate << endl
                 << "pga: " << (int)adc[i].gain << endl
                 << "conversionmode: " << (int)adc[i].conv_mode << endl
                 << "channel: " << (int)adc[i].channel << endl;
        }
    }catch(const SettingNotFoundException &nfex){
        /**/
    }

    return 0;
}
