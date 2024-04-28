#ifndef APP_CANFD_H
#define APP_CANFD_H

#include "r_cg_macrodriver.h"
#include "CAN.h"

void App_CanFDInit(void);
void App_CanFDSend(const uint8_t *buf, uint8_t len, unsigned long id, uint8_t ext, uint8_t rtr, bool wait_sent);
void Can_Frame_Prepare(void);

#endif //APP_CANFD_H