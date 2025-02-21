/*
 * settings.c
 *
 *  Created on: Jan 12, 2019
 *      Author: Dell
 */
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_system.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "settings.h"

#define INACTIVITY_TIMEOUT_MS 30000

/*extern public functions*/
extern void wifi_connect_to_saved_ssid (void);

/* Private variables */
static settings_t g_s_settings;
static connection_status_t ge_network_status;
static const char * g_p_settings  = "settings";
static const char * g_p_nvs_label = "nvs";
static bool restart_flag=false;
static inline void _initialize_nvs_flash (void);

static TimerHandle_t g_inactivity_timer;
/*************************************** Public function definitions *******************************************/
esp_err_t settings_init (void)
{
	esp_err_t e_err = ESP_OK;

	_initialize_nvs_flash();

	nvs_handle_t handle;
	e_err = nvs_open(g_p_nvs_label, NVS_READWRITE, &handle);

	if (ESP_OK == e_err)
	{
		uint32_t size = sizeof(g_s_settings);
		e_err = nvs_get_blob(handle, g_p_settings, &g_s_settings, &size);

		if ( (ESP_ERR_NVS_NOT_FOUND == e_err) || (g_s_settings.settings_size != sizeof(settings_t)) )
		{
			memset(&g_s_settings, 0, sizeof(g_s_settings));

			g_s_settings.settings_size = sizeof(g_s_settings);

			/* default host name */
			ge_network_status = WIFI_DISCONNECTED;

			strncpy(g_s_settings.s_wifi_info.s_credential.ssid, "RTS_IOT", 32);
			strncpy(g_s_settings.s_wifi_info.s_credential.password, "researchanddevelopment99", 32);
			strncpy(g_s_settings.s_wifi_info.s_ip, "0.0.0.0", 15);
			strncpy(g_s_settings.s_wifi_info.s_gw, "0.0.0.0", 15);
			strncpy(g_s_settings.s_wifi_info.s_netmask, "0.0.0.0", 15);
			strncpy(g_s_settings.s_wifi_info.s_dns, "0.0.0.0", 15);
			g_s_settings.s_wifi_info.e_ip_mode = DYNAMIC;

			strncpy(g_s_settings.s_hotspot_info.ssid , "LED_MATRIX_DISPLAY", 32);
			strncpy(g_s_settings.s_hotspot_info.password, "rtsdevice123*#", 32);

			/* default mqtt settings */
			g_s_settings.s_mqtt.clean_session = 1;
			g_s_settings.s_mqtt.port = 1883;
			g_s_settings.s_mqtt.pub_qos = 2;
			g_s_settings.s_mqtt.sub_qos = 2;
			g_s_settings.s_mqtt.retain_flag = 0;
			strlcpy(g_s_settings.s_mqtt.broker_name, "hardware.wscada.net", sizeof(g_s_settings.s_mqtt.broker_name));
			strlcpy(g_s_settings.s_mqtt.username, "rtsfirmware", sizeof(g_s_settings.s_mqtt.username));
			strlcpy(g_s_settings.s_mqtt.password, "X9pm?F=tTvr=9$yS", sizeof(g_s_settings.s_mqtt.password));

			e_err = nvs_set_blob(handle, g_p_settings, &g_s_settings, sizeof(g_s_settings));

			if (ESP_OK == e_err)
			{
				e_err = nvs_commit(handle);
			}
		}

		nvs_close(handle);
	}

	return e_err;
}

esp_err_t settings_save (bool restart_flag)
{
	nvs_handle_t handle;
	esp_err_t e_err = nvs_open(g_p_nvs_label, NVS_READWRITE, &handle);

	if (ESP_OK == e_err)
	{
		e_err = nvs_set_blob(handle, g_p_settings, &g_s_settings, sizeof(g_s_settings));

		if (ESP_OK == e_err)
		{
			e_err = nvs_commit(handle);
		}

		nvs_close(handle);
	}

	if (restart_flag)
	{
		esp_restart();
	}

	return e_err;
}



esp_err_t settings_reset(bool restart_flag)
{
	esp_err_t e_err = ESP_ERR_INVALID_RESPONSE;
	if (1)
	{

		memset(&g_s_settings, 0, sizeof(g_s_settings));

		g_s_settings.settings_size = sizeof(g_s_settings);

		/* default host name */
		strncpy(g_s_settings.s_wifi_info.s_credential.ssid, "infrastructure", 32);
		strncpy(g_s_settings.s_wifi_info.s_credential.password, "rtsdevice123*#", 32);
		strncpy(g_s_settings.s_wifi_info.s_ip, "0.0.0.0", 15);
		strncpy(g_s_settings.s_wifi_info.s_gw, "0.0.0.0", 15);
		strncpy(g_s_settings.s_wifi_info.s_netmask, "0.0.0.0", 15);
		strncpy(g_s_settings.s_wifi_info.s_dns, "0.0.0.0", 15);
		g_s_settings.s_wifi_info.e_ip_mode = DYNAMIC;

		strncpy(g_s_settings.s_hotspot_info.ssid , "LED_MATRIX_DISPLAY", 32);
		strncpy(g_s_settings.s_hotspot_info.password, "rtsdevice123*#", 32);

		/* default mqtt settings */
		g_s_settings.s_mqtt.clean_session = 1;
		g_s_settings.s_mqtt.port = 1883;
		g_s_settings.s_mqtt.pub_qos = 2;
		g_s_settings.s_mqtt.sub_qos = 2;
		g_s_settings.s_mqtt.retain_flag = 0;
		strlcpy(g_s_settings.s_mqtt.broker_name, "hardware.wscada.net", sizeof(g_s_settings.s_mqtt.broker_name));
		strlcpy(g_s_settings.s_mqtt.username, "rtsfirmware", sizeof(g_s_settings.s_mqtt.username));
		strlcpy(g_s_settings.s_mqtt.password, "X9pm?F=tTvr=9$yS", sizeof(g_s_settings.s_mqtt.password));
	
		e_err = settings_save(0);
	}
	return e_err;

}

settings_t * settings_get_info (void)
{
	return &g_s_settings;
}

wifi_info_t * settings_get_wifi_info (void)
{
	return &g_s_settings.s_wifi_info;
}

char * settings_get_wifi_static_ip (void)
{
       return (char *)&g_s_settings.s_wifi_info.s_ip;
}

char * settings_get_wifi_static_gateway (void)
{
       return (char *)&g_s_settings.s_wifi_info.s_gw;
}

char * settings_get_wifi_static_subnet (void)
{
       return (char *)&g_s_settings.s_wifi_info.s_netmask;
}

char * settings_get_wifi_static_dns (void)
{
       return (char *)&g_s_settings.s_wifi_info.s_dns;
}

const credential_t * settings_get_wifi_credential (void)
{
	return (const credential_t *)&g_s_settings.s_wifi_info.s_credential;
}

ip_mode_t settings_get_wifi_mode (void)
{
	return g_s_settings.s_wifi_info.e_ip_mode;
}


credential_t * settings_get_hotspot_info (void)
{
	return &g_s_settings.s_hotspot_info;
}

static inline void _initialize_nvs_flash (void)
{
	esp_err_t e_err = nvs_flash_init();

	if ( (ESP_ERR_NVS_NO_FREE_PAGES == e_err) || (ESP_ERR_NVS_NEW_VERSION_FOUND == e_err) )
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		e_err = nvs_flash_init();
	}

	ESP_ERROR_CHECK(e_err);
}


mqtt_setting_t * settings_get_mqtt_info (void)
{
	return &g_s_settings.s_mqtt;
}

char * settings_get_mqtt_client_id(void)
{
	return &g_s_settings.s_mqtt.client_id;
}

void set_restart_flag (void)
{
	restart_flag=true;
}
void reset_restart_flag (void)
{
	restart_flag=false;
}
bool get_restart_flag (void)
{
	return restart_flag;
}

void settings_on_inactivity_timeout (void)
{
	ESP_LOGI("TIMER", "Inactivity detected: Connecting to saved SSID.");
	wifi_connect_to_saved_ssid();
}

void settings_init_inactivity_timer (void)
{
	g_inactivity_timer = xTimerCreate("InactivityTimer", pdMS_TO_TICKS(INACTIVITY_TIMEOUT_MS), pdFALSE, NULL, settings_on_inactivity_timeout);
	if (g_inactivity_timer == NULL) 
	{
        ESP_LOGE("TIMER", "Failed to create inactivity timer");
    } 
	else 
	{
        xTimerStart(g_inactivity_timer, 0); 
    }
}

void settings_reset_inactivity_timer (void)
{
	 if (xTimerIsTimerActive(g_inactivity_timer))
	{
		if (xTimerReset(g_inactivity_timer, 0) != pdPASS) 
		{
			ESP_LOGE("TIMER", "Failed to reset inactivity timer");
		}
	}
}	
