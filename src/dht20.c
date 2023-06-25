
#include "dht20.h"

#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>

#include "debug.h"

static int dht20_read(dht20_data_t *this, uint8_t reg, uint8_t *buf, size_t len);
static int dht20_write(dht20_data_t *this, uint8_t *buf, size_t len);
static void reset_register(dht20_data_t *this, uint8_t dht20_reg);

const uint8_t dht20_address = DHT20_I2C_ADDRESS;

int dht20_init(dht20_data_t *this)
{
    // from Datasheet 7.4
    // 1. after power on wait at least 100ms
    usleep(200000);

    this->fd = open(this->i2c_dev_name, O_RDWR);

    if (this->fd == -1) {
        ERROR_LOG("Open i2c bus %s: %s", this->i2c_dev_name, strerror(errno));
        return -1;
    }

    if (ioctl(this->fd, I2C_SLAVE, dht20_address) < 0) {
        ERROR_LOG("Connecting to DHT20");
        return -1;
    }

    uint8_t buf[1];
    dht20_read(this, DHT20_STATUS_CMD, buf, 1);

    if ((buf[0] & 0x18) != 0x18) {
        DEBUG_LOG("Not the correct status, status = %d -- %d", buf[0], buf[0] & 0x18);

        reset_register(this, 0x1b);
        reset_register(this, 0x1c);
        reset_register(this, 0x1e);

        return -1;
    }

    return 0;
}

int dht20_measure(dht20_data_t *this, int *temperature, int *humidity)
{
    uint8_t data[20];

    while (dht20_init(this) == -1) {
        usleep(100000);
    }

    // datasheet 7.4.2: wait 10ms
    usleep(10000);
        
    // send read command
    uint8_t command1[] = {DHT20_WRITE_REGISTER_CMD, 0xac, 0x33, 0x00};
    dht20_write(this, command1, sizeof(command1));

    // 7.4.3: wait 80ms
    usleep(80000);

    // read status till busy indication is false
    int cnt;
    for (cnt = 0; cnt < DHT20_MEASURE_NUM_TRIES; cnt++) {
        dht20_read(this, DHT20_STATUS_CMD, data, 1);
        if ((data[0] & 0x80) != 0x80)
            break;
        // check every 2ms, from sample code
        usleep(2000);
    }

    // try again the measurement sequence
    if (cnt == DHT20_MEASURE_NUM_TRIES)
       return -1;

    dht20_read(this, DHT20_STATUS_CMD, data, 7);

    int val_hum = 0;
    val_hum = (val_hum | data[1]) << 8;
    val_hum = (val_hum | data[2]) << 4;
    val_hum = (val_hum | (data[3] >> 4));

    int val_temp = 0;
    val_temp = (val_temp | (data[3] & 0x0f)) << 8;
    val_temp = (val_temp | data[4]) << 8;
    val_temp = (val_temp | data[5]);

    // integer (value in % * 10)
    // rh = (value / 2^20) * 100%
    *humidity = val_hum * 100 * 10 / 0x100000;
    // integer (value in degree celsius * 10)
    // t = (value in degree celsius / 2^20) * 200 - 50
    *temperature = val_temp * 200 * 10 / 0x100000 - 50 * 10;

    return 0;
}

static int dht20_read(dht20_data_t *this, uint8_t reg, uint8_t *buf, size_t len)
{
    struct i2c_msg msg[2];
    msg[0].addr = dht20_address;
    msg[0].flags = 0;
    msg[0].len = 1;
    msg[0].buf = &reg;

    msg[1].addr = dht20_address;
    msg[1].flags = I2C_M_RD | I2C_M_NOSTART;
    msg[1].len = len;
    msg[1].buf = buf;

    struct i2c_rdwr_ioctl_data msgs = {msg, 2};

    int ret = ioctl(this->fd, I2C_RDWR, &msgs);

    if (ret == -1) {
        ERROR_LOG("Error reading: %s", strerror(errno));
        return -1;
    }

    return 0;
}

static int dht20_write(dht20_data_t *this, uint8_t *buf, size_t len)
{
    struct i2c_msg msg[1];
    msg[0].addr = dht20_address;
    msg[0].flags = 0;
    msg[0].len = len;
    msg[0].buf = buf;

    struct i2c_rdwr_ioctl_data msgs = {msg, 1};

    int ret = ioctl(this->fd, I2C_RDWR, &msgs);

    if (ret == -1) {
        ERROR_LOG("Error writing: %s", strerror(errno));
        return 0;
    }

    return 0;
}


static void reset_register(dht20_data_t *this, uint8_t dht20_reg)
{
    uint8_t data[5];

    uint8_t command1[] = {DHT20_WRITE_REGISTER_CMD, dht20_reg, 0x00, 0x00};
    dht20_write(this, command1, sizeof(command1));
    usleep(5000);
    dht20_read(this, DHT20_STATUS_CMD, data, 3);
    usleep(10000);
    uint8_t command2[] = {DHT20_WRITE_REGISTER_CMD, 0xb0 | dht20_reg, data[1], data[2]};
    dht20_write(this, command2, sizeof(command2));
}
