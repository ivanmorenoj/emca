#ifndef CFGSETTING_H_
#define CFGSETTING_H_

#include "commonConfig.h"

/**
 *  @return 1 success 0 of incomplete 
 *  @param _con - pointer to projectCfg struct
 *  @param _path - path to cfg file
 */
extern int getSettings(struct projectCfg *_con,const char *_path);
/**
 *  @param _con - pointer to projectCfg struct
 */
extern void printSettings(struct projectCfg *_cfg);

/**
 *  Write value of frame counter in cfg file
 *  @param _con - pointer to projectCfg struct
 *  @param _path - path to cfg file
 */
extern void writeFrameCounter(struct projectCfg *_con,const char *_path);

#endif //CFGSETTING_H_