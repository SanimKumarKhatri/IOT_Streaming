#include "driver/gpio.h"
#include "driver/i2c.h"

#define MPU6050_I2C_SCL_IO          GPIO_NUM_22       /*!< GPIO number used for I2C master clock */
#define MPU6050_I2C_SDA_IO          GPIO_NUM_21       /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                 /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define MPU6050_I2C_FREQ_HZ         400000           /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

/* MPU6050 ADDRESS*/
#define MPU6050_SENSOR_ADDRESS              0x68    /* AD0 = 0 --> address = 0b01101000*/
                                                    /* AD0 = 1 --> address = 0b01101001*/

#define MPU6050_DEFAULT_ADDRESS             0x68

/* Register Address Maps*/
#define MPU6050_CONFIG_REG                  0x1A
#define MPU6050_GYRO_CONFIG_REG             0x1B
#define MPU6050_ACCEL_CONFIG_REG            0x1C
#define MPU6050_SMPRT_DIV                   0x19
#define MPU6050_FIFO_EN                     0x23
#define MPU6050_I2C_MST_CTRL                0x24
#define MPU6050_ACCEL_XOUT_H                0x3B
#define MPU6050_ACCEL_XOUT_L                0x3C
#define MPU6050_ACCEL_YOUT_H                0x3D
#define MPU6050_ACCEL_YOUT_L                0x3E
#define MPU6050_ACCEL_ZOUT_H                0x3F
#define MPU6050_ACCEL_ZOUT_L                0x40
#define MPU6050_TEMP_OUT_H                  0x41
#define MPU6050_TEMP_OUT_L                  0x42
#define MPU6050_GYRO_XOUT_H                 0x43
#define MPU6050_GYRO_XOUT_L                 0x44
#define MPU6050_GYRO_YOUT_H                 0x45
#define MPU6050_GYRO_YOUT_L                 0x46
#define MPU6050_GYRO_ZOUT_H                 0x47
#define MPU6050_GYRO_ZOUT_L                 0x48
#define MPU6050_PWR_MGMT_1                  0x6B
#define MPU6050_PWR_MGMT_2                  0x6C
#define MPU6050_FIFO_COUNTH                 0x72
#define MPU6050_FIFO_COUNTL                 0x73
#define MPU6050_FIFO_R_W                    0x74
#define MPU6050_WHO_AM_I_REG_ADDR           0x75       

/* accelerometer specs */
#define MPU6050_ACCEL_RANGE_2G              0x00
#define MPU6050_ACCEL_RANGE_4G              0x08
#define MPU6050_ACCEL_RANGE_8G              0x10
#define MPU6050_ACCEL_RANGE_16G             0x18

/* gyroscope specs */
#define MPU6050_GYRO_RANGE_250              0x00
#define MPU6050_GYRO_RANGE_500              0x08
#define MPU6050_GYRO_RANGE_1000             0x10
#define MPU6050_GYRO_RANGE_2000             0x18

/* MPU6050 band values */
#define MPU6050_BAND_260_HZ                 0x00
#define MPU6050_BAND_184_HZ                 0x01
#define MPU6050_BAND_94_HZ                  0x02
#define MPU6050_BAND_44_HZ                  0x03
#define MPU6050_BAND_21_HZ                  0x04
#define MPU6050_BAND_10_HZ                  0x05
#define MPU6050_BAND_5_HZ                   0x06

#define TEMP_FIFO_EN                        0x80    // This enables TEMP_OUT_H and TEMP_OUT_L to be written into the FIFO buffer.
#define XG_FIFO_EN                          0x40    // This enables GYRO_XOUT_H and GYRO_XOUT_L to be written into the FIFO buffer.
#define YG_FIFO_EN                          0x20    // This enables GYRO_YOUT_H and GYRO_YOUT_L to be written into the FIFO buffer.
#define ZG_FIFO_EN                          0x10    // This enables GYRO_ZOUT_H and GYRO_ZOUT_L to be written into the FIFO buffer.
#define ACCEL_FIFO_EN                       0x08  
#define DISABLE_FIFO                        0x00 
#define WRITE_BIT                           I2C_MASTER_WRITE // I2C master write
#define READ_BIT                            I2C_MASTER_READ   // I2C master read
#define ACK_CHECK_EN                        0x1           // I2C master will check ack from slave
#define ACK_CHECK_DIS                       0x0          // I2C master will not check ack from slave
#define ACK_VAL                             0x0                // I2C ack value
#define NACK_VAL                            0x1               // I2C nack value

/**
 * @brief Initialize the MPU6050 I2C connection
 * @param accel_range accelerometer range scale
 * @param gyro_range gyroscope range scale
 * @param install_driver if need install the I2C driver
 * @return esp_err_t ESP_OK if success, otherwise ESP_FAIL
 */
esp_err_t mpu6050_init (uint8_t accel_range, uint8_t gyro_range, bool install_driver);

/**
 * @brief Read MPU6050 sensors registers 0x3B ~ 0x47
 * @return esp_err_t ESP_OK if success, otherwise ESP_FAIL
 */
esp_err_t mpu6050_read_sensors (void);

/**
 * @brief Read a single register from MPU6050 device
 * @param reg the register to be read
 * @return uint8_t value that was read
 */
uint8_t mpu6050_read_byte (uint8_t reg);

/**
 * @brief Write a single register in the MPU6050 device
 * @param reg the register to be write
 * @param value to be write
 * @return esp_err_t ESP_OK if success, otherwise ESP_FAIL
 */
esp_err_t mpu6050_write_byte (uint8_t reg, uint8_t value);

// Functions used to configure the device
/**
 * @brief Configure the MPU6050 sleep mode
 * @param mode true to enable or false to disable
 * @return esp_err_t ESP_OK if success, otherwise ESP_FAIL
 */
esp_err_t mpu6050_set_sleep_mode (bool mode);

/**
 * @brief Configure the accelerometer and gyroscope filter
 * @param bandwidth Filter value to be set
 * @return esp_err_t ESP_OK if success, otherwise ESP_FAIL
 */
esp_err_t mpu6050_set_filter_bandwidth (uint8_t bandwidth);

/**
 * @brief Configure the FIFO buffer
 * @param fifo mode that FIFO will operate
 * @return esp_err_t ESP_OK if success, otherwise ESP_FAIL
 */
esp_err_t mpu6050_set_fifo_buffer (uint8_t fifo);

/**
 * @brief Configure the MPU6050 accelerometer range
 * @param accel_range accelerometer range to be set
 * @return esp_err_t ESP_OK if success, otherwise ESP_FAIL
 */
esp_err_t mpu6050_set_accel_range (uint8_t accel_range);

/**
 * @brief Configure the MPU6050 gyroscope range
 * @param gyro_range gyroscope range to be set
 * @return esp_err_t ESP_OK if success, otherwise ESP_FAIL
 */
esp_err_t mpu6050_set_gyro_range (uint8_t gyro_range);

// Functions used to get the sensors values
/**
 * @brief Get the temperature
 * @return float the temperature value in Celsius
 */
float mpu6050_get_temperature (void);

/**
 * @brief Get the X acceleration
 * @return float X acceleration value in the range configured
 */
float mpu6050_get_accel_x (void);

/**
 * @brief Get the Y acceleration
 * @return float Y acceleration value in the range configured
 */
float mpu6050_get_accel_y (void);

/**
 * @brief Get the Z acceleration
 * @return float Z acceleration value in the range configured
 */
float mpu6050_get_accel_z (void);

/**
 * @brief Get the X gyroscope
 * @return float X gyroscope value in the range configured
 */
float mpu6050_get_gyro_x (void);

/**
 * @brief Get the Y gyroscope
 * @return float Y gyroscope value in the range configured
 */
float mpu6050_get_gyro_y (void);

/**
 * @brief Get the Z gyroscope
 * @return float Z gyroscope value in the range configured
 */
float mpu6050_get_gyro_z (void);
