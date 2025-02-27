#ifndef __SNTP_H__
#define __SNTP_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void sntp_app_init (void);
void sntp_get_current_time (char* datetime);

#endif /* __SNTP_H__ */
