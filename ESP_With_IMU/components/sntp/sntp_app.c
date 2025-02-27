#include <string.h>
#include "inttypes.h"
#include "lwip/apps/sntp_opts.h"
#include "lwip/apps/sntp.h"

#include <time.h>
#include <sys/time.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "esp_netif_sntp.h"
#include "lwip/ip_addr.h"

#define LOG_TAG "<sntp>"
#define SERVER_POOL "pool.ntp.org"

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 48
#endif

/***************************** Private Variables ***************************/

/***************************** Private Function Declaration ***************************/
static void _obtain_time (void);
static void time_sync_notification_cb (struct timeval *tv);

/***************************** Public Funciton Definition *****************************/
void sntp_get_current_time (char* datetime)
{
    time_t now;
    char strftime_buf[64];
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);

    if (timeinfo.tm_year < (2016 - 1900)) {
        ESP_LOGI(LOG_TAG, "Time is not set yet. Connect to WiFi and getting time over NTP.");
        _obtain_time();
        time(&now);
    }

    setenv("TZ", "Asia/Kathmandu", 1);
    tzset();

    localtime_r(&now, &timeinfo);
    uint8_t len = strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    snprintf(datetime, 64, "%s", strftime_buf);
    ESP_LOGI(LOG_TAG, "The current date/time: %s", strftime_buf);
}

static void _obtain_time(void)
{

#if LWIP_DHCP_GET_NTP_SRV
    /**
     * NTP server address could be acquired via DHCP,
     * see following menuconfig options:
     * 'LWIP_DHCP_GET_NTP_SRV' - enable STNP over DHCP
     * 'LWIP_SNTP_DEBUG' - enable debugging messages
     *
     * NOTE: This call should be made BEFORE esp acquires IP address from DHCP,
     * otherwise NTP option would be rejected by default.
     */
    ESP_LOGI(LOG_TAG, "Initializing SNTP");
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    config.start = false;                       // start SNTP service explicitly (after connecting)
    config.server_from_dhcp = true;             // accept NTP offers from DHCP server, if any (need to enable *before* connecting)
    config.renew_servers_after_new_IP = true;   // let esp-netif update configured SNTP server(s) after receiving DHCP lease
    config.index_of_first_server = 1;           // updates from server num 1, leaving server 0 (from DHCP) intact
    // configure the event on which we renew servers
#ifdef CONFIG_EXAMPLE_CONNECT_WIFI
    config.ip_event_to_renew = IP_EVENT_STA_GOT_IP;
#else
    config.ip_event_to_renew = IP_EVENT_ETH_GOT_IP;
#endif
    config.sync_cb = time_sync_notification_cb; // only if we need the notification function
    esp_netif_sntp_init(&config);

#endif /* LWIP_DHCP_GET_NTP_SRV */


#if LWIP_DHCP_GET_NTP_SRV
    ESP_LOGI(LOG_TAG, "Starting SNTP");
    esp_netif_sntp_start();
#if LWIP_IPV6 && SNTP_MAX_SERVERS > 2
    /* This demonstrates using IPv6 address as an additional SNTP server
     * (statically assigned IPv6 address is also possible)
     */
    ip_addr_t ip6;
    if (ipaddr_aton("2a01:3f7::1", &ip6)) {    // ipv6 ntp source "ntp.netnod.se"
        esp_sntp_setserver(2, &ip6);
    }
#endif  /* LWIP_IPV6 */

#else
    ESP_LOGI(LOG_TAG, "Initializing and starting SNTP");
#if CONFIG_LWIP_SNTP_MAX_SERVERS > 1
    /* This demonstrates configuring more than one server
     */
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG_MULTIPLE(2,
                               ESP_SNTP_SERVER_LIST(CONFIG_SNTP_TIME_SERVER, "pool.ntp.org" ) );
#else
    /*
     * This is the basic default config with one server and starting the service
     */
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG(CONFIG_SNTP_TIME_SERVER);
#endif
    config.sync_cb = time_sync_notification_cb;     // Note: This is only needed if we want
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    config.smooth_sync = true;
#endif

    esp_netif_sntp_init(&config);
#endif

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 15;
    while (esp_netif_sntp_sync_wait(2000 / portTICK_PERIOD_MS) == ESP_ERR_TIMEOUT && ++retry < retry_count) {
        ESP_LOGI(LOG_TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
    }
    time(&now);
    localtime_r(&now, &timeinfo);

}

static void time_sync_notification_cb (struct timeval *tv)
{
    ESP_LOGI(LOG_TAG, "Notification of a time synchronization event");
}