#include <stddef.h>
#include <string.h>

#include "webserver.h"
#include "index_html.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_server.h"

#include "settings.h"

/* extern public functions */

/* Private data structures */

typedef enum
{
    IP_MODE = 0,
    IP,
    GATEWAY,
    NETMASK,
	DNS,
	SSID,
	PASSWORD
}wifi_id_t;

/* Private function declarations */
static esp_err_t    _page_get_handler   (httpd_req_t * p_req);
static esp_err_t    _page_post_handler  (httpd_req_t * p_req);
static const char * _get_wifi_id_str (wifi_id_t e_id);

/* Private variables */
static const char * g_p_cfg = "/?";

/******************************************* Public function definitions *******************************************/
void wifi_page_init (httpd_handle_t p_server_handle)
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
		httpd_resp_sendstr_chunk(p_req, g_style_end);

		const char * p_null = "";
		len = snprintf(p_temp, size, g_header, sensor_webpage_get_device_version(), sensor_webpage_get_active_str(), p_null, p_null);
		httpd_resp_send_chunk(p_req, p_temp, len);
		httpd_resp_sendstr_chunk(p_req, g_container);
		httpd_resp_sendstr_chunk(p_req, g_form_start);

		wifi_info_t * ps_wifi_info = settings_get_wifi_info();
		const char * p_temp1 = p_null;
	    const char * p_temp2 = p_null;
		
		const char * p_hidden   = "hidden-div";
	    const char * p_selected = "selected";

	    if (DYNAMIC == ps_wifi_info->e_ip_mode)
	    {
	    	p_temp1 = p_selected;
	    }
	    else
	    {
	    	p_temp2 = p_selected;
	    }

		const char * p_required = "required";

	    len = snprintf(p_temp, size, g_row_str, "WIFI SSID", ps_wifi_info->s_credential.ssid, _get_wifi_id_str(SSID), sizeof(ps_wifi_info->s_credential.ssid) - 1, p_null, p_required);
	    httpd_resp_send_chunk(p_req, p_temp, len);

	    len = snprintf(p_temp, size, g_row_str, "Password", ps_wifi_info->s_credential.password, _get_wifi_id_str(PASSWORD), sizeof(ps_wifi_info->s_credential.password) - 1, p_null, p_required);
	    httpd_resp_send_chunk(p_req, p_temp, len);
		
		len = snprintf(p_temp, size, g_row_select_list_hidden, "IP Mode", _get_wifi_id_str(IP_MODE), p_null, p_temp1, "Dynamic", p_temp2, "Static");
		httpd_resp_send_chunk(p_req, p_temp, len);
		
		len = snprintf(p_temp, size, g_hide_div_start, p_hidden, (STATIC == ps_wifi_info->e_ip_mode) ? "block" : "none");
		httpd_resp_send_chunk(p_req, p_temp, len);
		
		len = snprintf(p_temp, size, g_row_str, "IP Address", ps_wifi_info->s_ip, _get_wifi_id_str(IP), sizeof(ps_wifi_info->s_ip), p_null, p_required);
		httpd_resp_send_chunk(p_req, p_temp, len);

		len = snprintf(p_temp, size, g_row_str, "Subnet Mask", ps_wifi_info->s_netmask, _get_wifi_id_str(NETMASK), sizeof(ps_wifi_info->s_netmask), p_null, p_required);
		httpd_resp_send_chunk(p_req, p_temp, len);

		len = snprintf(p_temp, size, g_row_str, "Gateway", ps_wifi_info->s_gw, _get_wifi_id_str(GATEWAY), sizeof(ps_wifi_info->s_gw), p_null, p_required);
		httpd_resp_send_chunk(p_req, p_temp, len);

		len = snprintf(p_temp, size, g_row_str, "DNS", ps_wifi_info->s_dns, _get_wifi_id_str(DNS), sizeof(ps_wifi_info->s_dns), p_null, p_required);
		httpd_resp_send_chunk(p_req, p_temp, len);

		httpd_resp_sendstr_chunk(p_req, g_hide_div_end);
		httpd_resp_sendstr_chunk(p_req, g_form_end);
		httpd_resp_sendstr_chunk(p_req, g_script_start);
		httpd_resp_sendstr_chunk(p_req, g_script_no_space);

		len = snprintf(p_temp, size, g_script_show_div, p_hidden, p_hidden);
		httpd_resp_send_chunk(p_req, p_temp, len);

		httpd_resp_sendstr_chunk(p_req, g_script_end);
		httpd_resp_sendstr_chunk(p_req, g_footer);
		httpd_resp_sendstr_chunk(p_req, NULL);
		free(p_temp);
	}

	return e_err;
}

static esp_err_t _page_post_handler (httpd_req_t * p_req)
{
	settings_reset_inactivity_timer();
	printf("post handler");
	esp_err_t e_err = ESP_ERR_NO_MEM;
	uint32_t content_len = p_req->content_len + 1; /* extra one byte for NULL termination */

	if (content_len)
	{
		char * p_content = calloc(1, content_len);

		if (p_content)
		{
			e_err = ESP_OK;
			httpd_req_recv(p_req, p_content, content_len - 1); /* excluding one byte for NULL */

			credential_t * ps_wifi_credentials = settings_get_wifi_credential();
			wifi_info_t * ps_wifi_info = settings_get_wifi_info();
			sensor_webpage_parse_str(p_content, _get_wifi_id_str(SSID),  ps_wifi_credentials->ssid, sizeof(ps_wifi_credentials->ssid));
			sensor_webpage_parse_str(p_content, _get_wifi_id_str(PASSWORD), ps_wifi_credentials->password, sizeof( ps_wifi_credentials->password));
			ps_wifi_info->e_ip_mode = sensor_webpage_parse_num(p_content, _get_wifi_id_str(IP_MODE));
			if (STATIC == ps_wifi_info->e_ip_mode)
			{
				sensor_webpage_parse_str(p_content, _get_wifi_id_str(IP),  ps_wifi_info->s_ip, sizeof(ps_wifi_info->s_ip));
				sensor_webpage_parse_str(p_content, _get_wifi_id_str(GATEWAY), ps_wifi_info->s_gw, sizeof(ps_wifi_info->s_gw));
				sensor_webpage_parse_str(p_content, _get_wifi_id_str(DNS),  ps_wifi_info->s_dns, sizeof(ps_wifi_info->s_dns));
				sensor_webpage_parse_str(p_content, _get_wifi_id_str(NETMASK),  ps_wifi_info->s_netmask, sizeof(ps_wifi_info->s_netmask));
			}
			settings_save(0);
			free(p_content);
		}
	}
	else
	{
		e_err = ESP_OK;
	}

    httpd_resp_set_status(p_req, "303 See Other");
    httpd_resp_set_hdr(p_req, "Location", g_p_cfg);
    httpd_resp_sendstr(p_req, "form submitted!");

	return e_err;
}

static const char * _get_wifi_id_str (wifi_id_t e_id)
{
	switch (e_id)
	{
		case IP_MODE: return "mode";
		case IP: return "ip";
		case NETMASK: return "netmask";
		case GATEWAY: return "ip_gate";
		case DNS: return "dns";
		case SSID: return "ssid";
		case PASSWORD : return "pass";
		default: return "";
	}
}