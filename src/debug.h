#pragma once

#include <stdio.h>
#include <syslog.h>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define DHT20_DEBUG
//#define DHT20_SYSLOG

#ifndef DHT20_DEBUG
    #define DEBUG_LOG(msg,...)
#else
#ifndef DHT20_SYSLOG
    #define DEBUG_LOG(msg,...) printf("(%s:%d) " msg "\n" , __FILENAME__, __LINE__,##__VA_ARGS__)
#else
    #define DEBUG_LOG(msg,...) syslog(LOG_DEBUG, "(%s:%d) " msg "\n" , __FILENAME__, __LINE__,##__VA_ARGS__)
#endif
#endif

#ifndef DHT20_SYSLOG
#define ERROR_LOG(msg,...) printf("Error: (%s:%d) " msg "\n" , __FILENAME__, __LINE__,##__VA_ARGS__)
#else
#define ERROR_LOG(msg,...) syslog(LOG_ERR, "Error: (%s:%d) " msg "\n" , __FILENAME__, __LINE__,##__VA_ARGS__)
#endif