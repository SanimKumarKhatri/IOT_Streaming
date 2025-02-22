#include "mqtt.h"
#include "esp_log.h"

/******* Private Global Variables *******/
#define LOG_TAG "<mqtt_app>"

static esp_mqtt_client_handle_t gs_client;

/*********************Private Function Declaration ******************************/
static esp_err_t mqtt_event_handler_cb (esp_mqtt_event_handle_t s_event);
static void mqtt_event_handler (void *p_handler_args, esp_event_base_t base, int32_t event_id, void *p_event_data);

/**************************Public Function Definition **************************/
void mqtt_app_init (void)
{
    esp_mqtt_client_config_t s_mqtt_cfg = {
        .broker.address.uri = "mqtt://mqtt.eclipseprojects.io",
    };
    gs_client = esp_mqtt_client_init(&s_mqtt_cfg);
    esp_mqtt_client_register_event(gs_client, ESP_EVENT_ANY_ID, mqtt_event_handler, gs_client);
    esp_mqtt_client_start(gs_client);
}

int mqtt_publish_accel_data (float a_x, float a_y, float a_z)
{
    int err;
    char msg[20];
    snprintf(msg, sizeof(msg), "%0.2f,%0.2f,%0.2f", a_x, a_y, a_z);
    err = esp_mqtt_client_publish(gs_client, "mpu6050_test/accel_data", msg, 0, 1, 0);
    return err;
}

int mqtt_publish_gyro_data (float g_x, float g_y, float g_z)
{
    int err;
    char msg[20];
    snprintf(msg, sizeof(msg), "%0.2f,%0.2f,%0.2f", g_x, g_y, g_z);
    err = esp_mqtt_client_publish(gs_client, "mpu6050_test/gyro_data", msg, 0, 1, 0);
    return err;
}

int mqtt_publish_temp_data (float temp)
{
    int err;
    char msg[10];
    snprintf(msg, sizeof(msg), "%0.2f", temp);
    err = esp_mqtt_client_publish(gs_client, "mpu6050_test/temp_data", msg, 0, 1, 0);
    return err;
}

/*********************Private Function Definition ******************************/
static esp_err_t mqtt_event_handler_cb (esp_mqtt_event_handle_t s_event)
{
    gs_client = s_event->client;
    switch (s_event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(LOG_TAG, "MQTT_EVENT_CONNECTED");
        // esp_mqtt_client_subscribe(client, "my_tops
        esp_mqtt_client_publish(gs_client, "my_topic", "Hi to all from ESP32 .........", 0, 1, 0);
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
