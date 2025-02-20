#include "driver/gpio.h"
#include "driver/i2c.h"

#define I2C_MPU6050_SCL_IO          GPIO_NUM_22       /*!< GPIO number used for I2C master clock */
#define I2C_MPU6050_SDA_IO          GPIO_NUM_21       /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

/* MPU6050 ADDRESS*/
#define MPU6050_SENSOR_ADDRESS              0x68    /* AD0 = 0 --> address = 0b01101000*/
                                                    /* AD0 = 1 --> address = 0b01101001*/

/* Register Address Maps*/
#define MPU6050_CONFIG_REG                  0x1A
#define MPU6050_GYRO_CONFIG_REG             0x1B
#define MPU6050_ACCEL_CONFIG_REG            0x1C
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
#define MPU6050_ACCEL_RANGE_4G              0x01
#define MPU6050_ACCEL_RANGE_8G              0x02
#define MPU6050_ACCEL_RANGE_16G             0x03

/* gyroscope specs */
#define MPU6050_GYRO_RANGE_250              0x00
#define MPU6050_GYRO_RANGE_500              0x01
#define MPU6050_GYRO_RANGE_1000             0x02
#define MPU6050_GYRO_RANGE_2000             0x03

#define WRITE_BIT                           I2C_MASTER_WRITE // I2C master write
#define READ_BIT                            I2C_MASTER_READ   // I2C master read
#define ACK_CHECK_EN                        0x1           // I2C master will check ack from slave
#define ACK_CHECK_DIS                       0x0          // I2C master will not check ack from slave
#define ACK_VAL                             0x0                // I2C ack value
#define NACK_VAL                            0x1               // I2C nack value
/**
 * @brief Write a single register in the MPU6050 device
 * @param reg the register to be write
 * @param value to be write
 * @return esp_err_t ESP_OK if success, otherwise ESP_FAIL
 */
esp_err_t mpu6050_write_byte (uint8_t reg, uint8_t value);

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