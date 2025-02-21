#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "settings.h"
#include "wifi_app.h"
#include "mpu6050.h"
#include "webserver.h"

static const char *TAG = "main";

void app_main(void)
{
    settings_init();
    wifi_app_init();
    sensor_webpage_init();
    mpu6050_init(MPU6050_ACCEL_RANGE_2G, MPU6050_GYRO_RANGE_250, true);
    mpu6050_set_filter_bandwidth(MPU6050_BAND_21_HZ);
    uint8_t data[2];

    ESP_LOGI(TAG, "I2C initialized successfully");
    
    ESP_LOGI(TAG, "helloo!");

    /* Read the MPU9250 WHO_AM_I register, on power up the register should have the value 0x71 */
    // ESP_ERROR_CHECK(mpu9250_register_read(MPU9250_WHO_AM_I_REG_ADDR, data, 1));
    ESP_LOGI(TAG, "ACCEL_X= %0.2f", mpu6050_get_accel_x());
    ESP_LOGI(TAG, "ACCEL_Y= %0.2f", mpu6050_get_accel_y());

}
