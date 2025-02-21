#include <ctype.h>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_server.h"

#include "webserver.h"

#define LOG_TAG "<webserver>"
/* extern public functions */
extern void wifi_page_init (httpd_handle_t gp_server_handle);
extern void about_page_init (httpd_handle_t gp_server_handle);
extern void mqtt_page_init (httpd_handle_t gp_server_handle);

/* Private function declarations */
static void _url_decode (uint8_t *dst, const uint8_t *src);

/* Private variables */
static httpd_handle_t gp_server_handle = NULL;

/******************************************* Public function definitions *******************************************/
void sensor_webpage_init (void)
{
	httpd_config_t s_config = HTTPD_DEFAULT_CONFIG();
    
	s_config.max_uri_handlers = 20;
	s_config.stack_size = 5 * 1024;
	s_config.uri_match_fn = httpd_uri_match_wildcard;

	if ( ESP_OK == httpd_start(&gp_server_handle, &s_config) )
	{
		ESP_LOGI(LOG_TAG, "httpStart status success");
		wifi_page_init(gp_server_handle);
		about_page_init(gp_server_handle);
	}
}

void sensor_webpage_deinit ( void )
{
	if (gp_server_handle)
	{
		httpd_stop(gp_server_handle);
		gp_server_handle = NULL;
	}
}

const char * sensor_webpage_get_device_title (void)
{
	return "ESP32_FALL";
}

const char * sensor_webpage_get_device_version (void)
{
    static char device[40] = {0};
    snprintf((char *)device, sizeof(device), (const char *)"Fall Detect (v1)");
    return device;
}

const char * sensor_webpage_get_hidden_div (void)
{
	return "hidden-div";
}

const char * sensor_webpage_get_active_str (void)
{
	return "active";
}

const char * sensor_webpage_get_no_space (void)
{
	return "onkeyup=\"nospaces(this)\"";
}

const char * sensor_webpage_get_enable_disable_str (option_t c)
{
    switch(c)
    {
        case ENABLE  : return "";
        case DISABLE : return "disable";
        default      : return "";
    }
}

uint32_t sensor_webpage_parse_str (char * data, const char * check_str, char * dest, uint32_t max_size)
{
	uint32_t ret  = 0;
	char *   ptr  = NULL;
	char *   ptr1 = NULL;

	ptr = strstr(data, check_str);

    if (ptr)
    {
		ptr += strlen(check_str) + 1;
		ptr1 = strchr(ptr, '&');
        int32_t size = ptr1 - ptr;

        if (size < 0)
        {
        	ptr1 = strchr(ptr, '\0');
        	size = ptr1 - ptr;
        }
        if ((size > 0) && (size < max_size))
        {
        	ret = 1;
			strncpy(dest, ptr, (size_t)size);
			dest[size] = '\0';

			_url_decode((uint8_t *)dest, (uint8_t *)dest);
		}
		else
		{
			memset(dest, 0, max_size);
		}
	}

    return ret;
}

int32_t sensor_webpage_parse_num (char *data, const char * check_str)
{
    char * ptr;
	uint32_t ret = 0;
    ptr = strstr(data, check_str);

	if (ptr)
    {
		ptr += strlen(check_str) + 1;
		ret = strtoul(ptr, NULL, 10);
	}

	return ret;
}

bool sensor_webpage_parse_bool(const char *p_content, const char *p_key) 
{
    char search_str[50];
	bool retval = false;

    snprintf(search_str, sizeof(search_str), "%s=on", p_key);
    
    if (strstr(p_content, search_str)) 
	{
        retval = true;
    } 

	return retval;
}

float sensor_webpage_parse_float (char * p_data, const char * check_str)
{
    char * ptr;
	float ret = 0.0;
    ptr = strstr(p_data, check_str);

	if (ptr)
    {
		ptr += strlen(check_str) + 1;
		ret = strtof(ptr, NULL);
	}

	return ret;
}

void sensor_webpage_parse_ip (char * p_data, const char * p_check_str, ip_addr_t * p_ip)
{
	const char * p_ptr = strstr(p_data, p_check_str);

	if (p_ptr)
	{
		p_ptr += strlen(p_check_str) + 1;

		uint32_t ip[4] = {0};
		int32_t ret = sscanf(p_ptr, "%lu.%lu.%lu.%lu", &ip[0], &ip[1], &ip[2], &ip[3]);
		printf("p_ptr: %s, string: %s, ret: %ld\n", p_ptr, p_check_str, ret);
		if (ret != 4)
		{
			memset(&p_ip->u_addr.ip4, 0, sizeof(p_ip->u_addr.ip4.addr));
		}
		else
		{
			IP4_ADDR(&p_ip->u_addr.ip4, ip[0], ip[1], ip[2], ip[3]);
		}
	}
}

/******************************************* Private function definitions *******************************************/
static void _url_decode (uint8_t *dst , const uint8_t *src)
{
	while (*src)
	{
		uint8_t a, b;
		
		if ( ('%' == *src) && ((a = src[1]) && (b = src[2])) && (isxdigit(a) && isxdigit(b)) )
		{
			if (a >= 'a')
			{
				a -= 'a'-'A';
			}

			if (a >= 'A')
			{
				a -= ('A' - 10);
			}
			else
			{
				a -= '0';
			}

			if (b >= 'a')
			{
				b -= 'a'-'A';
			}

			if (b >= 'A')
			{
				b -= ('A' - 10);
			}
			else
			{
				b -= '0';
			}

			*dst++ = 16*a+b;
			src += 3;
		}
		else if ( '+' == *src )
		{
			*dst++ = ' ';
			src++;
		}
		else
		{
			*dst++ = *src++;
		}
	}

	*dst++ = '\0';
}


