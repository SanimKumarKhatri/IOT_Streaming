#ifndef __MQTT_H__
#define __MQTT_H__

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "mqtt_client.h"

#define MQTT_TOPIC "mpu6050_test/data"
 
void mqtt_app_init(void);

int mqtt_publish_data (float ax, float ay, float az, float gx, float gy, float gz, float temp);
int mqtt_publish_accel_data (float a_x, float a_y, float a_z);
int mqtt_publish_gyro_data (float g_x, float g_y, float g_z);
int mqtt_publish_temp_data (float temp);

#endif /* __MQTT_H__ */