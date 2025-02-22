#include "mqtt.h"
#include "esp_log.h"

/******* Private Global Variables *******/
#define LOG_TAG "<mqtt_app>"

/*********************Private Function Declaration ******************************/
static esp_err_t mqtt_event_handler_cb (esp_mqtt_event_handle_t s_event);
static void mqtt_event_handler (void *p_handler_args, esp_event_base_t base, int32_t event_id, void *p_event_data);

/**************************Public Function Definition **************************/
void mqtt_app_init (void)
{
    esp_mqtt_client_config_t s_mqtt_cfg = {
        .broker.address.uri = "mqtt://mqtt.eclipseprojects.io",
    };
    esp_mqtt_client_handle_t s_client = esp_mqtt_client_init(&s_mqtt_cfg);
    esp_mqtt_client_register_event(s_client, ESP_EVENT_ANY_ID, mqtt_event_handler, s_client);
    esp_mqtt_client_start(s_client);
}

/*********************Private Function Definition ******************************/
static esp_err_t mqtt_event_handler_cb (esp_mqtt_event_handle_t s_event)
{
    esp_mqtt_client_handle_t client = s_event->client;
    switch (s_event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(LOG_TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client, "my_topic", 0);
        esp_mqtt_client_publish(client, "my_topic", "Hi to all from ESP32 .........", 0, 1, 0);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(LOG_TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(LOG_TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", s_event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(LOG_TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", s_event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(LOG_TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", s_event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(LOG_TAG, "MQTT_EVENT_DATA");
        ESP_LOGI(LOG_TAG, "TOPIC=%.*s", s_event->topic_len, s_event->topic);
        ESP_LOGI(LOG_TAG, "DATA=%.*s", s_event->data_len, s_event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(LOG_TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(LOG_TAG, "Other event id:%d", s_event->event_id);
        break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *p_handler_args, esp_event_base_t base, int32_t event_id, void *p_event_data)
{
    ESP_LOGI(LOG_TAG, "Event dispatched from event loop base=%s, event_id=%ld", base, event_id);
    mqtt_event_handler_cb(p_event_data);
}
