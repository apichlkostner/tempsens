#pragma once

#define DHT20_I2C_ADDRESS 0x38

#define DHT20_MEASURE_CMD                   0xAC
#define DHT20_WRITE_REGISTER_CMD            0x70
#define DHT20_STATUS_CMD                    0x71

#define DHT20_MEASURE_NUM_TRIES 40

typedef struct  {
    const char* i2c_dev_name;
    int fd;
} dht20_data_t;

extern int dht20_init(dht20_data_t *this);
extern int dht20_measure(dht20_data_t *this, int *temperature, int *humidity);