#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "dht20.h"
#include "debug.h"

int main()
{
    const char *dev = "/dev/i2c-1";

    DEBUG_LOG("Starting dht20 test");

    dht20_data_t dht20 = {dev, 0};

    int ret = dht20_init(&dht20);

    int temperature;
    int humidity;

    while(1) {
        dht20_measure(&dht20, &temperature, &humidity);

        DEBUG_LOG("Temperature = %d.%d  Humidity = %d.%d", temperature/10, temperature%10, humidity/10, humidity%10);
        fflush(stdout);

        sleep(1);
    }

    return ret;
}