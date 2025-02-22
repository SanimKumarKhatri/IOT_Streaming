
#include "mpu6050.h"
#include "esp_log.h"
#include "esp_err.h"

#define LOG_TAG "<mpu6050>"

uint8_t buffer[14];

float accel_sensitivity;
float gyro_sensitivity;

static esp_err_t _mpu6050_register_read (uint8_t reg_addr, uint8_t *data, size_t len);
static esp_err_t _i2c_read_bytes (i2c_port_t i2c_num, uint8_t periph_address, uint8_t reg_address, uint8_t *data, size_t data_len);

esp_err_t mpu6050_init (uint8_t accel_range, uint8_t gyro_range, bool install_driver)
{
    ESP_LOGI(LOG_TAG, "Beginning connection");

    if (install_driver)
    {
        ESP_LOGI(LOG_TAG, "Installing I2C driver");

        i2c_config_t i2c_config = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = GPIO_NUM_21,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_io_num = GPIO_NUM_22,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master.clk_speed = MPU6050_I2C_FREQ_HZ,
        };

        i2c_param_config(I2C_NUM_0, &i2c_config);

        esp_err_t ret = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

        if (ret != ESP_OK)
        {
            ESP_LOGE(LOG_TAG, "I2C driver install failed");
            return ret;
        }
        ESP_LOGI(LOG_TAG, "I2C driver install success");
    }
    else
    {
        ESP_LOGI(LOG_TAG, "I2C driver not installed");
    }

    if (MPU6050_SENSOR_ADDRESS == MPU6050_DEFAULT_ADDRESS)
    {
        uint8_t data;
        _mpu6050_register_read(MPU6050_WHO_AM_I_REG_ADDR, &data, 1);
        ESP_LOGI(LOG_TAG, "register: 0x%x, value: 0x%x", MPU6050_WHO_AM_I_REG_ADDR, data);

        if (data != MPU6050_SENSOR_ADDRESS)
        {
            ESP_LOGE(LOG_TAG, "Device with 0x%x address not found", MPU6050_SENSOR_ADDRESS);
            return ESP_ERR_INVALID_ARG;
        }
        else
        {
            ESP_LOGI(LOG_TAG, "Device with 0x%x address found", MPU6050_SENSOR_ADDRESS);
        }
    }
    else
    {
        ESP_LOGI(LOG_TAG, "Device with 0x%x address, not default", MPU6050_SENSOR_ADDRESS);
    }

    // Configure the gyroscope and accelerometer scale ranges
    mpu6050_set_accel_range(accel_range);
    mpu6050_set_gyro_range(gyro_range);

    // Disable the FIFO buffer
    mpu6050_set_fifo_buffer(DISABLE_FIFO);

    // Set the sample rate divider to 0
    mpu6050_write_byte(MPU6050_SMPRT_DIV, 0);
    // Disable multi-master mode and set master clock to 400KHz
    mpu6050_write_byte(MPU6050_I2C_MST_CTRL, 0x0D);

    // Disable sleep mode
    return mpu6050_set_sleep_mode(false);
}

uint8_t mpu6050_read_byte (uint8_t reg)
{
    ESP_LOGI(LOG_TAG, "Reading the 0x%x register", reg);

    uint8_t value;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SENSOR_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SENSOR_ADDRESS << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &value, ACK_VAL);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    ESP_LOGI(LOG_TAG, "VAlue of the 0x%x register: 0x%x", reg, value);

    return value;
}

esp_err_t mpu6050_write_byte (uint8_t reg, uint8_t value)
{
    ESP_LOGI(LOG_TAG, "Writing in the 0x%x register the value 0x%x", reg, value);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SENSOR_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, value, ACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t mpu6050_set_sleep_mode (bool mode)
{
    switch (mode)
    {
        case true:
            ESP_LOGI(LOG_TAG, "Enable device sleep mode");
        break;

        case false:
            ESP_LOGI(LOG_TAG, "Disable device sleep mode");
        break;
    }

    return mpu6050_write_byte(MPU6050_PWR_MGMT_1, mode);
}

esp_err_t mpu6050_set_filter_bandwidth (uint8_t bandwidth)
{
    switch (bandwidth)
    {
        case MPU6050_BAND_5_HZ:
            ESP_LOGI(LOG_TAG, "Setting Bandwidth 5Hz");
        break;
        
        case MPU6050_BAND_10_HZ:
            ESP_LOGI(LOG_TAG, "Setting Bandwidth 10Hz");
        break;
        
        case MPU6050_BAND_21_HZ:
            ESP_LOGI(LOG_TAG, "Setting Bandwidth 21Hz");
        break;
        
        case MPU6050_BAND_44_HZ:
            ESP_LOGI(LOG_TAG, "Setting Bandwidth 44Hz");
        break;
        
        case MPU6050_BAND_94_HZ:
            ESP_LOGI(LOG_TAG, "Setting Bandwidth 94Hz");
        break;
        
        case MPU6050_BAND_184_HZ:
            ESP_LOGI(LOG_TAG, "Setting Bandwidth 184Hz");
        break;
        
        case MPU6050_BAND_260_HZ:
            ESP_LOGI(LOG_TAG, "Setting Bandwidth 260Hz");
        break;
        
        default:
        break;
    }

    return mpu6050_write_byte(MPU6050_CONFIG_REG, bandwidth);
}

esp_err_t mpu6050_set_fifo_buffer (uint8_t fifo)
{
    switch (fifo)
    {
        case DISABLE_FIFO:
            ESP_LOGI(LOG_TAG, "FIFO buffer was disabled");
        break;
        
        case TEMP_FIFO_EN:
            ESP_LOGI(LOG_TAG, "Temperature FIFO buffer was enabled");
        break;
        
        case XG_FIFO_EN:
            ESP_LOGI(LOG_TAG, "Gyroscope X FIFO buffer was enabled");
        break;
        
        case YG_FIFO_EN:
            ESP_LOGI(LOG_TAG, "Gyroscope Y FIFO buffer was enabled");
        break;
        
        case ZG_FIFO_EN:
            ESP_LOGI(LOG_TAG, "Gyroscope Z FIFO buffer was enabled");
        break;
        
        case ACCEL_FIFO_EN:
            ESP_LOGI(LOG_TAG, "Accelerometer FIFO buffer was enabled");
        break;
    }

    return mpu6050_write_byte(MPU6050_FIFO_EN, fifo);
}

esp_err_t mpu6050_set_accel_range (uint8_t accel_range)
{
    esp_err_t err =  ESP_FAIL;

    switch (accel_range)
    {
        case MPU6050_ACCEL_RANGE_2G:
            ESP_LOGI(LOG_TAG, "Setting accelerometer range to 2G");
            accel_sensitivity = 16384.0;
        break;

        case MPU6050_ACCEL_RANGE_4G:
            ESP_LOGI(LOG_TAG, "Setting accelerometer range to 4G");
            accel_sensitivity = 8192.0;
        break;

        case MPU6050_ACCEL_RANGE_8G:
            ESP_LOGI(LOG_TAG, "Setting accelerometer range to 8G");
            accel_sensitivity = 4096.0;
        break;

        case MPU6050_ACCEL_RANGE_16G:
            ESP_LOGI(LOG_TAG, "Setting accelerometer range to 16G");
            accel_sensitivity = 2048.0;
        break;

        default:
            ESP_LOGE(LOG_TAG, "Setting Out of range!!");
            goto exit;
        break;
    }

    err = mpu6050_write_byte(MPU6050_ACCEL_CONFIG_REG, accel_range);

    exit:
    return err;
}


esp_err_t mpu6050_set_gyro_range (uint8_t gyro_range)
{
    esp_err_t err = ESP_FAIL;

    switch (gyro_range)
    {
        case MPU6050_GYRO_RANGE_250:
            ESP_LOGI(LOG_TAG, "Setting gyroscope range to 250DPS");
            gyro_sensitivity = 131.0;
        break;
        
        case MPU6050_GYRO_RANGE_500:
            ESP_LOGI(LOG_TAG, "Setting gyroscope range to 500DPS");
            gyro_sensitivity = 65.5;
        break;

        case MPU6050_GYRO_RANGE_1000:
            ESP_LOGI(LOG_TAG, "Setting gyroscope range to 1000DPS");
            gyro_sensitivity = 32.8;
        break;

        case MPU6050_GYRO_RANGE_2000:
            ESP_LOGI(LOG_TAG, "Setting gyroscope range to 2000DPS");
            gyro_sensitivity = 16.4;
        break;

        default:
            ESP_LOGE(LOG_TAG, "Setting out of range");
            goto exit;
        break;
    }

    err = mpu6050_write_byte(MPU6050_GYRO_CONFIG_REG, gyro_range);

    exit:
    return err;
}

float mpu6050_get_temperature (void)
{
    _i2c_read_bytes(I2C_MASTER_NUM, MPU6050_SENSOR_ADDRESS, MPU6050_TEMP_OUT_H, &buffer, 2);
    int16_t result = (((int16_t)buffer[0]) << 8) | buffer[1];
    return (result / 340.0) + 36.53;
}

float mpu6050_get_accel_x (void)
{
    int16_t buffer[2];
    _i2c_read_bytes(I2C_MASTER_NUM, MPU6050_SENSOR_ADDRESS, MPU6050_ACCEL_XOUT_H, &buffer, 2);
    int16_t result = (((int16_t)buffer[0]) << 8) | buffer[1];
    return result / accel_sensitivity;
}

float mpu6050_get_accel_y (void)
{
    _i2c_read_bytes(I2C_MASTER_NUM, MPU6050_SENSOR_ADDRESS, MPU6050_ACCEL_YOUT_H, &buffer, 2);
    int16_t result = (((int16_t)buffer[0]) << 8) | buffer[1];
    return result / accel_sensitivity;
}

float mpu6050_get_accel_z (void)
{
    _i2c_read_bytes(I2C_MASTER_NUM, MPU6050_SENSOR_ADDRESS, MPU6050_ACCEL_ZOUT_H, &buffer, 2);
    int16_t result = (((int16_t)buffer[0]) << 8) | buffer[1];
    return result / accel_sensitivity;
}

float mpu6050_get_gyro_x (void)
{
    _i2c_read_bytes(I2C_MASTER_NUM, MPU6050_SENSOR_ADDRESS, MPU6050_GYRO_XOUT_H, &buffer, 2);
    int16_t result = (((int16_t)buffer[0]) << 8) | buffer[1];
    return result / gyro_sensitivity;
}

float mpu6050_get_gyro_y (void)
{
    _i2c_read_bytes(I2C_MASTER_NUM, MPU6050_SENSOR_ADDRESS, MPU6050_GYRO_YOUT_H, &buffer, 2);
    int16_t result = (((int16_t)buffer[0]) << 8) | buffer[1];
    return result / gyro_sensitivity;
}

float mpu6050_get_gyro_z (void)
{
    _i2c_read_bytes(I2C_MASTER_NUM, MPU6050_SENSOR_ADDRESS, MPU6050_GYRO_ZOUT_H, &buffer, 2);
    int16_t result = (((int16_t)buffer[0]) << 8) | buffer[1];
    return result / gyro_sensitivity;
}

esp_err_t mpu6050_read_sensors (void)
{
    ESP_LOGI(LOG_TAG, "Reading sensors registers");

    esp_err_t ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SENSOR_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, MPU6050_ACCEL_XOUT_H, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK)
        return ret;

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SENSOR_ADDRESS << 1 | READ_BIT, ACK_CHECK_EN);

    // Read the registers
    uint8_t i;
    for (i = 0; i < (sizeof(buffer) / sizeof(buffer[0])) - 1; i++)
    {
        i2c_master_read_byte(cmd, &buffer[i], ACK_VAL);
    }
    i2c_master_read_byte(cmd, &buffer[i], NACK_VAL);

    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);
    i2c_cmd_link_delete(cmd);

    return ret;
}

/**
 * @brief Read a sequence of bytes from a MPU6050 sensor registers
 */
static esp_err_t _mpu6050_register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_SENSOR_ADDRESS, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

esp_err_t _i2c_read_bytes(i2c_port_t i2c_num, uint8_t periph_address, uint8_t reg_address, uint8_t *data, size_t data_len)
{
    return i2c_master_write_read_device(i2c_num, periph_address, &reg_address, 1, data, data_len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

// /**
//  * @brief Write a byte to a MPU9250 sensor register
//  */
// static esp_err_t mpu9250_register_write_byte(uint8_t reg_addr, uint8_t data)
// {
//     int ret;
//     uint8_t write_buf[2] = {reg_addr, data};

//     ret = i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_SENSOR_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

//     return ret;
// }

// /**
//  * @brief i2c master initialization
//  */
// static esp_err_t i2c_master_init(void)
// {
//     int i2c_master_port = I2C_MASTER_NUM;

//     i2c_config_t conf = {
//         .mode = I2C_MODE_MASTER,
//         .sda_io_num = I2C_MASTER_SDA_IO,
//         .scl_io_num = I2C_MASTER_SCL_IO,
//         .sda_pullup_en = GPIO_PULLUP_ENABLE,
//         .scl_pullup_en = GPIO_PULLUP_ENABLE,
//         .master.clk_speed = I2C_MASTER_FREQ_HZ,
//     };

//     i2c_param_config(i2c_master_port, &conf);

//     return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
// }
