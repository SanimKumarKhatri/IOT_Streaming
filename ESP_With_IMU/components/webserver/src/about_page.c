#include <stddef.h>
#include <string.h>

#include "webserver.h"
#include "index_html.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_system.h"

#include "settings.h"

/* extern public functions */

/* Private data structures */

/* Private function declarations */
static esp_err_t    _page_get_handler   (httpd_req_t * p_req);
static esp_err_t    _page_post_handler  (httpd_req_t * p_req);

/* Private variables */
static const char * g_p_cfg = "/about?";

/******************************************* Public function definitions *******************************************/
void about_page_init (httpd_handle_t p_server_handle)
{
	httpd_uri_t s_get_config = webpage_config(g_p_cfg, HTTP_GET, _page_get_handler, NULL);
	httpd_uri_t s_post_config = webpage_config(g_p_cfg, HTTP_POST, _page_post_handler, NULL);

	httpd_register_uri_handler(p_server_handle, &s_get_config);
	httpd_register_uri_handler(p_server_handle, &s_post_config);
}

/******************************************* Private function definitions *******************************************/
static esp_err_t _page_get_handler (httpd_req_t * p_req)
{
	settings_reset_inactivity_timer();
	esp_err_t e_err = ESP_ERR_NO_MEM;
	uint32_t size = 1024;
	char * p_temp = calloc(1, size);

	if (p_temp)
	{
		e_err = ESP_OK;

		httpd_resp_set_type(p_req, HTTPD_TYPE_TEXT);
		uint32_t len = snprintf(p_temp, size, g_doc, "Fall Detect");
		httpd_resp_send_chunk(p_req, p_temp, len);

		httpd_resp_sendstr_chunk(p_req, g_style_start);
		httpd_resp_sendstr_chunk(p_req, g_style);
		httpd_resp_sendstr_chunk(p_req, g_style2);
		httpd_resp_sendstr_chunk(p_req, g_style3);
		httpd_resp_sendstr_chunk(p_req, g_style4);
		httpd_resp_sendstr_chunk(p_req, g_style6);
		httpd_resp_sendstr_chunk(p_req, g_style_end);

		const char * p_null = "";
		len = snprintf(p_temp, size, g_header, sensor_webpage_get_device_version(), p_null, p_null, sensor_webpage_get_active_str());
		httpd_resp_send_chunk(p_req, p_temp, len);
		httpd_resp_sendstr_chunk(p_req, g_container);

		httpd_resp_sendstr_chunk(p_req, g_form_start);
		httpd_resp_sendstr_chunk(p_req, g_restart_form_end);
		httpd_resp_sendstr_chunk(p_req, g_footer);
		httpd_resp_sendstr_chunk(p_req, NULL);
		free(p_temp);
	}

	return e_err;
}

static esp_err_t _page_post_handler (httpd_req_t * p_req)
{
	settings_reset_inactivity_timer();
	httpd_resp_send(p_req, "Restarting ESP32...", HTTPD_RESP_USE_STRLEN); 
    esp_restart();  
    return ESP_OK;
}

