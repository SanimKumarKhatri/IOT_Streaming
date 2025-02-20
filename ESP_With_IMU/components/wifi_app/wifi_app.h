/*
 * wifi_app.h
 *
 *  Created on: Feb 20, 2025
 *      Author: Sanim
 */

 #ifndef __WIFI_APP_H__
 #define __WIFI_APP_H__
 
 void wifi_app_init (void);
 
 uint32_t wifi_app_wait_for_station_connection (uint32_t wait_ms);
 
 #endif /* __WIFI_APP_H__ */
 