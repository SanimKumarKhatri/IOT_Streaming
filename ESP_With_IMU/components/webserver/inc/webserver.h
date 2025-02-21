/*
 * ims_webpage.h
 *
 *  Created on: Mar 26, 2020
 *      Author: User
 */

#ifndef __IMS_WEBPAGE_H__
#define __IMS_WEBPAGE_H__
#include "esp_http_server.h"
#include "lwip/ip_addr.h"

typedef enum
{
	DISABLE = 0,
    ENABLE
} option_t;

typedef esp_err_t (*http_req_handler_t)(httpd_req_t * p_req);

static inline httpd_uri_t webpage_config (const char * p_uri, httpd_method_t e_method, http_req_handler_t fp_handler, void * p_user_ctx)
{
	httpd_uri_t s_httpd_uri;
	s_httpd_uri.uri      = p_uri;
	s_httpd_uri.method   = e_method;
	s_httpd_uri.handler  = fp_handler;
	s_httpd_uri.user_ctx = p_user_ctx;

	return s_httpd_uri;
}

void sensor_webpage_init (void);

void sensor_webpage_deinit (void);

const char * sensor_webpage_get_device_title (void);

uint32_t sensor_webpage_parse_str (char * data, const char * check_str, char * dest, uint32_t max_size);

int32_t sensor_webpage_parse_num (char *data, const char * check_str);

bool sensor_webpage_parse_bool(const char *p_content, const char *p_key); 

float sensor_webpage_parse_float (char * p_data, const char * check_str);

const char * sensor_webpage_get_active_str (void);

const char * sensor_webpage_get_enable_disable_str (option_t c);

const char * sensor_webpage_get_device_version (void);

void sensor_webpage_parse_ip (char * p_data, const char * p_check_str, ip_addr_t * p_ip);

#endif /* __IMS_WEBPAGE_H__ */
