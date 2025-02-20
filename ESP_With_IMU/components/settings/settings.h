/*
 * settings.h
 *
 *  Created on: Jan 12, 2019
 *      Author: Dell
 */

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "lwip/ip_addr.h"

typedef enum
{
	HOTSPOT_CONNETED = 0,
	WIFI_CONNECTED,
	WIFI_CONNCTED_NO_INTERNET,
	WIFI_DISCONNECTED,
}connection_status_t;

typedef enum
{
	STATIC = 0,
	DYNAMIC
} ip_mode_t;

typedef struct
{
	char ssid[32];
	char password[32];
} credential_t;

typedef struct
{
	credential_t s_credential;
	ip_mode_t e_ip_mode;
	char s_ip[15];
	char s_gw[15];
	char s_netmask[15];
	char s_dns[15];

} wifi_info_t;


typedef struct __attribute__((packed))
{
	char     broker_name[32];
	char     username[32];
	char     password[32];
	char     client_id[32];
	uint16_t port; /* 1883: mqtt or 8883: mqtts */
	uint16_t sub_qos          : 2;
	uint16_t pub_qos          : 2;
	uint16_t clean_session    : 1;
	uint16_t retain_flag      : 1;
} mqtt_setting_t;


typedef struct __attribute__((packed))
{
	uint32_t            settings_size;
	uint32_t 			ping_enable;
	mqtt_setting_t   	s_mqtt;
	wifi_info_t 		s_wifi_info;
	credential_t 		s_hotspot_info;
} settings_t;


esp_err_t settings_init (void);
esp_err_t settings_reset(bool restart_flag);
esp_err_t settings_save (bool restart_flag);
settings_t * settings_get_info (void);

/* Wifi */
wifi_info_t * settings_get_wifi_info (void);
const credential_t * settings_get_wifi_credential (void);
char * settings_get_wifi_static_ip (void);
char * settings_get_wifi_static_gateway (void);
char * settings_get_wifi_static_subnet (void);
char * settings_get_wifi_static_dns (void);
ip_mode_t settings_get_wifi_mode (void);

mqtt_setting_t * settings_get_mqtt_info (void);
char * settings_get_mqtt_client_id(void);

/* HOTSPOT */
credential_t * settings_get_hotspot_info (void);

void settings_app_ip_display (void);

void set_restart_flag (void);
void reset_restart_flag (void);

bool get_restart_flag (void);

#endif /* __SETTINGS_H__ */
