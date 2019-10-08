#ifndef CLIENT_MODEL_H_
#define CLIENT_MODEL_H_

#include "commonConfig.h"
#include "serial.h"


/**
 *  All functions return 0 on success and non zero in otherwise 
 */

class clientModel
{
private:
    Serial _ser;
    lora *_loraCfg;
    uint8_t _status;
    char _buff[100];
    uint8_t sendAndWaitOK(uint8_t _len);

public:
    clientModel();
    clientModel(lora *_lora_cfg);
    ~clientModel();
    void setConfig(lora *_lora_cfg);
    Serial *getSerial();
    uint8_t openSerial();
    void closeSerial();
    uint8_t getStatus();

    /* client server methods */

    /* Chech comminication */
    uint8_t sendCC();
    /* Send Payload */
    uint8_t sendSP(lora_payload *_loraPayload,gas_values *_gasvalues);
    /* send Tx Power */
    uint8_t sendTP();
    /* send Activation method */
    uint8_t sendAM();
    /* send Data Rate */
    uint8_t sendDR();
    /* send Channel */
    uint8_t sendCH();
    /* send Network key */
    uint8_t sendNK();
    /* send App key */
    uint8_t sendAK();
    /* send Device Address */
    uint8_t sendDA();
    /* send Frame counter */
    uint8_t sendFC();
};


#endif //CLIENT_MODEL_H_