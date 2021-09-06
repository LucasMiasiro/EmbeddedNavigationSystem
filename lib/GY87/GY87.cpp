#include "GY87.h"
#include "driver/i2c.h"
#include "config.h"
#include <math.h> 

esp_err_t GY87::setup(){
    esp_err_t write_OK = ESP_OK;
    uint8_t opt = GY87_PWR_MGMT_1_OPT;

    setup_i2c();

    // IMU
    opt = GY87_PWR_MGMT_1_OPT;
    write(&opt, 1, GY87_IMU_ADD, GY87_PWR_MGMT_1_ADD);

    // ---------------------------------------------
    opt = GY87_USER_CTRL_OPT_1;
    write(&opt, 1, GY87_IMU_ADD, GY87_USER_CTRL_ADD);

    opt = GY87_CONFIG_1_OPT_1;
    write(&opt, 1, GY87_IMU_ADD, GY87_CONFIG_1_ADD);

    // MAG
    opt = GY87_CONFIG_2_OPT;
    write(&opt, 1, GY87_MAG_ADD, GY87_CONFIG_2_ADD);

    opt = GY87_CONFIG_1_OPT_2;
    write(&opt, 1, GY87_IMU_ADD, GY87_CONFIG_1_ADD);

    opt = GY87_USER_CTRL_OPT_2;
    write(&opt, 1, GY87_IMU_ADD, GY87_USER_CTRL_ADD);

    // IMU should read MAG
    opt = GY87_CONFIG_3_OPT;
    write(&opt, 1, GY87_IMU_ADD, GY87_CONFIG_3_ADD);

    opt = GY87_CONFIG_4_OPT;
    write(&opt, 1, GY87_IMU_ADD, GY87_CONFIG_4_ADD);

    opt = GY87_CONFIG_5_OPT;
    write(&opt, 1, GY87_IMU_ADD, GY87_CONFIG_5_ADD);

    opt = GY87_CONFIG_6_OPT;
    write(&opt, 1, GY87_IMU_ADD, GY87_CONFIG_6_ADD);

    #if LOG_GY87
    std::cout << "Setup:"<< "\t" << "write" << "\t" << "OK = "<< (write_OK == ESP_OK) << std::endl;
    #endif
    return write_OK;
};

esp_err_t GY87::read_test(){

    uint8_t buffer[20];
    esp_err_t read_OK;

    read_OK = read(buffer, 20, GY87_IMU_ADD, GY87_IMU_DATA_ADD);
    #if LOG_GY87
    // std::cout << "Setup:"<< "\t" << "read" << "\t" << "OK = "<< (read_OK == ESP_OK) << std::endl;

    // std::cout << (int16_t)(buffer[0] << 8 | buffer[1])*GRAVITY/GY87_ACCEL_SENS << ",\t";
    // std::cout << (int16_t)(buffer[2] << 8 | buffer[3])*GRAVITY/GY87_ACCEL_SENS << ",\t";
    // std::cout << (int16_t)(buffer[4] << 8 | buffer[5])*GRAVITY/GY87_ACCEL_SENS << ",\t";

    // std::cout << (int16_t)(buffer[6] << 8 | buffer[7])/GY87_TEMP_SENS << ",\t";

    // std::cout << (int16_t)(buffer[8] << 8 | buffer[9])/GY87_GYRO_SENS << ",\t";
    // std::cout << (int16_t)(buffer[10] << 8 | buffer[11])/GY87_GYRO_SENS << ",\t";
    // std::cout << (int16_t)(buffer[12] << 8 | buffer[13])/GY87_GYRO_SENS << ",\t";

    int16_t mag1_raw = (buffer[15] << 8 | buffer[14]);
    int16_t mag2_raw = (buffer[17] << 8 | buffer[16]);
    int16_t mag3_raw = (buffer[19] << 8 | buffer[18]);
    float mag1 = mag1_raw/GY87_MAG_SENS - (magCal[0]+magCal[1])/2;
    float mag2 = mag2_raw/GY87_MAG_SENS - (magCal[2]+magCal[3])/2;
    float mag3 = mag3_raw/GY87_MAG_SENS - (magCal[4]+magCal[5])/2;

    std::cout << mag1 << ",\t";
    std::cout << mag2 << ",\t";
    std::cout << mag3 << ",\t";
    std::cout << sqrt(mag1*mag1 + mag2*mag2 + mag3*mag3) << ",\t";

    std::cout << std::endl;
    #endif
    return read_OK;
};

bool GY87::calibrate_mag(float *mag_min_max){

    uint8_t buffer[20];
    bool newMax = 0;

    read(buffer, 20, GY87_IMU_ADD, GY87_IMU_DATA_ADD);
    int16_t mag1_raw = (buffer[15] << 8 | buffer[14]);
    int16_t mag2_raw = (buffer[17] << 8 | buffer[16]);
    int16_t mag3_raw = (buffer[19] << 8 | buffer[18]);
    float mag1 = mag1_raw/GY87_MAG_SENS;
    float mag2 = mag2_raw/GY87_MAG_SENS;
    float mag3 = mag3_raw/GY87_MAG_SENS;
    if (*mag_min_max == 0){
        *mag_min_max = mag1;
        *(mag_min_max+1) = mag1;
        *(mag_min_max+2) = mag2;
        *(mag_min_max+3) = mag2;
        *(mag_min_max+4) = mag3;
        *(mag_min_max+5) = mag3;
        return 0;
    }
    
    if (mag1 < *mag_min_max){
        *mag_min_max = mag1;
        newMax = 1;
        }
    if (mag1 > *(mag_min_max+1)){
        *(mag_min_max+1) = mag1;
        newMax = 1;
        }

    if (mag2 < *(mag_min_max+2)){
        *(mag_min_max+2) = mag2;
        newMax = 1;
        }
    if (mag3 > *(mag_min_max+3)){
        *(mag_min_max+3) = mag2;
        newMax = 1;
        }

    if (mag3 < *(mag_min_max+4)){
        *(mag_min_max+4) = mag3;
        newMax = 1;
        }
    if (mag3 > *(mag_min_max+5)){
        *(mag_min_max+5) = mag3;
        newMax = 1;
        }

    if (newMax){
        for (auto i = 0; i < 6; i++){
            std::cout << *(mag_min_max+i) << ", ";
        }
        std::cout << std::endl;
    }
    return newMax;
};



bool GY87::setup_i2c(){
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = GY87_SDA_IO;
    conf.scl_io_num = GY87_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = GY87_MASTER_FREQ_HZ;
    conf.clk_flags = 0;

    i2c_param_config(GY87_I2C_PORT, &conf);
    i2c_driver_install(GY87_I2C_PORT, conf.mode, 0, 0, 0);
    return 0;
};

esp_err_t GY87::write(uint8_t* data_ptr, const size_t size, const uint8_t address, const uint8_t register_address){

    i2c_cmd_handle_t command = i2c_cmd_link_create();
    esp_err_t i2c_OK = ESP_OK;

    i2c_master_start(command);
    i2c_master_write_byte(command, (address << 1) | GY87_I2C_WRITE_BIT, GY87_I2C_ACK_CHECK_EN);
    i2c_master_write_byte(command, register_address, GY87_I2C_ACK_CHECK_EN);
    i2c_master_write(command, data_ptr, size, GY87_I2C_ACK_CHECK_EN);
    i2c_master_stop(command);

    i2c_OK = i2c_master_cmd_begin(GY87_I2C_PORT, command, 1000/portTICK_RATE_MS);
    i2c_cmd_link_delete(command);
    return i2c_OK;
};

esp_err_t GY87::read(uint8_t* buffer_ptr, const size_t size, const uint8_t address, const uint8_t register_address){

    i2c_cmd_handle_t command = i2c_cmd_link_create();
    esp_err_t i2c_OK = ESP_OK;

    i2c_master_start(command);
    i2c_master_write_byte(command, (address << 1) | GY87_I2C_WRITE_BIT, GY87_I2C_ACK_CHECK_EN);
    i2c_master_write_byte(command, register_address, GY87_I2C_ACK_CHECK_EN);

    i2c_master_start(command);
    i2c_master_write_byte(command, (address << 1) | GY87_I2C_READ_BIT, GY87_I2C_ACK_CHECK_EN);
    for (auto i = 0; i < size - 1; i++)
    {
        i2c_master_read_byte(command, buffer_ptr + i, GY87_I2C_ACK_VAL);
    }
    i2c_master_read_byte(command, buffer_ptr + size - 1, GY87_I2C_NACK_VAL);
    i2c_master_stop(command);

    i2c_OK = i2c_master_cmd_begin(GY87_I2C_PORT, command, 1000/portTICK_RATE_MS);
    i2c_cmd_link_delete(command);
    return i2c_OK;
};