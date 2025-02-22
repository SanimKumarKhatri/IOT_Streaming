/*
 * wifi_app.c
 *
 *  Created on: Jan 13, 2019
 *      Author: Dell
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ping.h"
#include "nvs_flash.h"
#include "ping/ping_sock.h"

#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "sdkconfig.h"

#include "settings.h"
#include "wifi_app.h"
#include "esp_netif.h"
#include "mqtt.h"

/* extern Public functions */

/* Private macros */
#define LOG_TAG 		"<wifi_app>"
#define WIFI_GOT_IP_BIT BIT0
#define WIFI_CONNECTED_BIT BIT0

/* Private variables */
static EventGroupHandle_t g_s_wifi_grp_event;
static char g_ip_string[16];
static char g_hotspot_ssid[12] = "fall_detect\0";
static char g_hotspot_pw[10] = "12345678\0";

/* Private function declarations */
static void _wifi_task (void * pv);
static void _configure_wifi_settings (const char * p_ssid, const char * p_password);
static void _static_ip (char * p_s_ip, char * p_s_gw, char * p_s_netmask, char * p_s_dns);
static void _static_ip_ap (char * p_s_ip, char * p_s_gw, char * p_s_netmask, char * p_s_dns);
static void _on_ping_success (esp_ping_handle_t hdl, void *args);
static void _on_ping_timeout (esp_ping_handle_t hdl, void *args);
static void _on_ping_end (esp_ping_handle_t hdl, void *args);
static void _check_internet (void);
static esp_netif_t * gp_s_sta_netif = NULL;
static esp_netif_t * gp_s_ap_netif  = NULL;
static esp_ping_handle_t ping_handle;

/************************************************ Public functions ****************************************/
void wifi_app_init (void)
{
	g_s_wifi_grp_event = xEventGroupCreate();
	esp_netif_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_LOGI("<wifi bro>", "Hello World");
    xTaskCreatePinnedToCore(_wifi_task, "sTask", 3 * 1024, NULL, 5, NULL, 1);
}

void wifi_connect_to_saved_ssid (void)
{
	ESP_LOGI(LOG_TAG, "connectingToSavedSsid");
	esp_wifi_stop();
	esp_wifi_deinit();
	esp_netif_ip_info_t ip_info;
    memset(&ip_info, 0, sizeof(esp_netif_ip_info_t));
    esp_netif_set_ip_info(gp_s_sta_netif, &ip_info);
	if ( STATIC == settings_get_wifi_mode())
	{
		_static_ip(settings_get_wifi_static_ip(), settings_get_wifi_static_gateway(), settings_get_wifi_static_subnet(), settings_get_wifi_static_dns());
	}
	_configure_wifi_settings(settings_get_wifi_credential()->ssid, settings_get_wifi_credential()->password);
	ESP_LOGI(LOG_TAG, "%s|%s", settings_get_wifi_credential()->ssid, settings_get_wifi_credential()->password);
	settings_init();

	wifi_app_wait_for_station_connection(portMAX_DELAY);
}

uint32_t wifi_app_wait_for_station_connection (uint32_t wait_ms)
{
	uint32_t ret = 0;
	EventBits_t bits = xEventGroupWaitBits(g_s_wifi_grp_event, WIFI_GOT_IP_BIT, pdFALSE, pdFALSE, wait_ms);

	if (bits == WIFI_GOT_IP_BIT)
	{
		ret = 1;
	}

	return ret;
}

// station_t wifi_app_get_station_type (void)
// {
// 	return e_station_type;
// }

const char * wifi_app_get_rssi (void)
{
	char buffer[5] = {0};
	static wifi_ap_record_t wifi;
	esp_wifi_sta_get_ap_info(&wifi);

	snprintf(buffer, sizeof(buffer), "%d", wifi.rssi);
	printf("\nBUFFER %s ",buffer);
	return (const char *)&buffer[0];
}

const char * wifi_app_get_ip_string (void)
{
	return (const char *)&g_ip_string[0];
}


/************************************************ Private functions ********************************************/
static void _on_ping_success (esp_ping_handle_t hdl, void *args)
{
    uint8_t ttl;
    uint16_t seqno;
    uint32_t elapsed_time, recv_len, transmitted, received;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
	esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    printf("%lu bytes icmp_seq=%d ttl=%d time=%lu ms transmit:%lu, recv: %ld\n",
			recv_len, seqno, ttl, elapsed_time, transmitted, received);
	if (received > 0) 
	{
        ESP_LOGI(LOG_TAG, "Internet is working!");
    } 
	else 
	{
        ESP_LOGW(LOG_TAG, "No internet access!");
    }
    ESP_LOGW("pingSuccess", "");
}

static void _on_ping_timeout (esp_ping_handle_t hdl, void *args)
{
    uint16_t seqno;
    ip_addr_t target_addr;
	uint32_t received;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
	esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
	if (received > 0) 
	{
        ESP_LOGI(LOG_TAG, "Internet is working!");
    } 
	else 
	{
        ESP_LOGW(LOG_TAG, "No internet access!");
    }
    printf(" icmp_seq=%d timeout received: %lu\n", seqno, received);
    ESP_LOGW("pingTimeout", "");
}

static void _on_ping_end (esp_ping_handle_t hdl, void *args)
{
    uint32_t transmitted;
    uint32_t received;
    uint32_t total_time_ms;

    esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));
	if (received > 0) 
	{
        ESP_LOGI(LOG_TAG, "Internet is working!");
    } 
	else 
	{
        ESP_LOGW(LOG_TAG, "No internet access!");
    }
    printf("%lu packets transmitted, %lu received, time %lums\n", transmitted, received, total_time_ms);
}

static void _check_internet (void) 
{
	esp_ping_config_t ping_cfg = ESP_PING_DEFAULT_CONFIG();
	ping_cfg.count = 0;
	ping_cfg.interval_ms = 20000;
	ping_cfg.timeout_ms  = 1000;
	ping_cfg.data_size = 32;
    ipaddr_aton("8.8.8.8",  &ping_cfg.target_addr);
    
    esp_ping_callbacks_t cbs = {
        .on_ping_success = _on_ping_success,
        .on_ping_timeout = _on_ping_timeout,
        .on_ping_end = _on_ping_end
    };

    esp_err_t err = esp_ping_new_session(&ping_cfg, &cbs, &ping_handle);
	if (err != ESP_OK) 
	{
		ESP_LOGE(LOG_TAG, "Failed to create ping session! Error: %s", esp_err_to_name(err));
		return;
	}

    esp_ping_start(ping_handle);

	ESP_LOGI(LOG_TAG, "Ping task started... (Every 1 minute)");
}

static void _configure_wifi_settings (const char * p_ssid, const char * p_password)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config;

	memset(&wifi_config, 0, sizeof(wifi_config_t));
	strncpy((char *)wifi_config.sta.ssid, p_ssid, 32);
	strncpy((char *)wifi_config.sta.password, p_password, 64);
	wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
	wifi_config.sta.scan_method = WIFI_FAST_SCAN;
	wifi_config.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;
	wifi_config.sta.threshold.rssi = -127;
	wifi_config.sta.bssid_set = 0;

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
	esp_wifi_set_ps(WIFI_PS_NONE);
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
	ESP_ERROR_CHECK(esp_wifi_start());
}

static void _start_ap_mode() 
{
	_static_ip_ap("192.168.4.1", "192.168.4.1", "255.255.255.0", "8.8.8.8");

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {0};
	strncpy((char * )wifi_config.ap.ssid, g_hotspot_ssid, sizeof(wifi_config.ap.ssid));
	strncpy((char * )wifi_config.ap.password, g_hotspot_pw, sizeof(wifi_config.ap.password));

	wifi_config.ap.ssid_len =  strlen(g_hotspot_ssid);
	wifi_config.ap.max_connection = 2;
	wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;

    if (strlen(settings_get_hotspot_info()->password) == 0) 
	{
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
	
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(LOG_TAG, "Switched to AP mode. SSID: %s, Password: %s", wifi_config.ap.ssid, wifi_config.ap.password);
}

static void _static_ip_ap(char * p_s_ip, char * p_s_gw, char * p_s_netmask, char * p_s_dns)
{
	esp_netif_ip_info_t s_netif_ip_info = { 0 };
	esp_netif_dns_info_t s_dns_info = { .ip = { .type = ESP_IPADDR_TYPE_V4 } };

	esp_netif_str_to_ip4(p_s_ip, &s_netif_ip_info.ip);
	esp_netif_str_to_ip4(p_s_gw, &s_netif_ip_info.gw);
	esp_netif_str_to_ip4(p_s_netmask, &s_netif_ip_info.netmask);
	esp_netif_str_to_ip4(p_s_dns, &s_dns_info.ip.u_addr.ip4);

	esp_netif_dhcpc_stop(gp_s_sta_netif);
	esp_netif_set_ip_info(gp_s_sta_netif, &s_netif_ip_info);

	esp_netif_set_dns_info(gp_s_sta_netif, ESP_NETIF_DNS_MAIN, &s_dns_info);
	esp_netif_dhcpc_start(gp_s_sta_netif);
}

static void _static_ip(char * p_s_ip, char * p_s_gw, char * p_s_netmask, char * p_s_dns)
{
	esp_netif_ip_info_t s_netif_ip_info = { 0 };
	esp_netif_dns_info_t s_dns_info = { .ip = { .type = ESP_IPADDR_TYPE_V4 } };

	esp_netif_dhcpc_stop(gp_s_sta_netif);

	esp_netif_str_to_ip4(p_s_ip, &s_netif_ip_info.ip);
	esp_netif_str_to_ip4(p_s_gw, &s_netif_ip_info.gw);
	esp_netif_str_to_ip4(p_s_netmask, &s_netif_ip_info.netmask);
	esp_netif_str_to_ip4(p_s_dns, &s_dns_info.ip.u_addr.ip4);

	esp_netif_set_ip_info(gp_s_sta_netif, &s_netif_ip_info);

	esp_netif_set_dns_info(gp_s_sta_netif, ESP_NETIF_DNS_MAIN, &s_dns_info);
}

static void _wifi_event_handler (void * p_ctx, esp_event_base_t e_evt_base, int32_t evt_id, void * p_evt_data)
{
	if (WIFI_EVENT == e_evt_base)
	{
		if (WIFI_EVENT_STA_START == evt_id)
		{
			esp_wifi_connect();
		}
		else if (WIFI_EVENT_STA_CONNECTED == evt_id)
		{
			_check_internet();
		}
		else if (WIFI_EVENT_STA_DISCONNECTED == evt_id)
		{
			wifi_event_sta_disconnected_t * p_event = (wifi_event_sta_disconnected_t *) p_evt_data;
			ESP_LOGW(LOG_TAG, "Disconnect reason: %d\n", p_event->reason);;
			esp_wifi_connect();
			memset(g_ip_string, 0, sizeof(g_ip_string));
			xEventGroupClearBits(g_s_wifi_grp_event, WIFI_GOT_IP_BIT);
		}
	}
	else if ( (IP_EVENT == e_evt_base) && (IP_EVENT_STA_GOT_IP == evt_id) )
	{
	    ip_event_got_ip_t * ps_ip_event = (ip_event_got_ip_t*) p_evt_data;
        ESP_LOGI(LOG_TAG, "Got IP:" IPSTR, IP2STR(&ps_ip_event->ip_info.ip));
		esp_ip4addr_ntoa(&ps_ip_event->ip_info.ip, g_ip_string, sizeof(g_ip_string));
		xEventGroupSetBits(g_s_wifi_grp_event, WIFI_GOT_IP_BIT);
		mqtt_app_init();
	}
}


static void _wifi_task (void * pv)
{
	gp_s_sta_netif = esp_netif_create_default_wifi_sta();
	gp_s_ap_netif = esp_netif_create_default_wifi_ap();
   	esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, _wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, _wifi_event_handler, NULL);	

	ESP_LOGI(LOG_TAG, "Hotspot");
	_start_ap_mode();
	settings_init_inactivity_timer();

	vTaskDelay(1000);
	vTaskDelete(NULL);
}
