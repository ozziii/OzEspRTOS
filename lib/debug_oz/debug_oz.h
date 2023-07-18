#ifndef DEBUG_O_H
#define DEBUG_O_H

#include <configuration.h>
#include "debug_list_oz.h"

#if OZDEBUG_LOG_LEVEL > OZDEBUG_LOG_LEVEL_NONE
#define OZ_LOG_BEGIN() debugList.begin()
#else
#define OZ_LOG_BEGIN() esp_log_level_set("*", ESP_LOG_NONE);
#endif


#if OZDEBUG_LOG_LEVEL >= OZDEBUG_LOG_LEVEL_VERBOSE
#define OZ_LOGV(tag, format, ...) debugList.write(pathToFileName(__FILE__),__FUNCTION__, __LINE__,OZDEBUG_LOG_LEVEL_VERBOSE,tag,format,##__VA_ARGS__)
// debugList.write(OZDEBUG_LOG_FORMAT(V,format), ##__VA_ARGS__)
#else
#define OZ_LOGV(tag, format, ...)
#endif

#if OZDEBUG_LOG_LEVEL >= OZDEBUG_LOG_LEVEL_DEBUG
#define OZ_LOGD(tag, format, ...) debugList.write(pathToFileName(__FILE__),__FUNCTION__, __LINE__,OZDEBUG_LOG_LEVEL_DEBUG,tag,format,##__VA_ARGS__)
// debugList.write(OZDEBUG_LOG_FORMAT(D,format), ##__VA_ARGS__)
#else
#define OZ_LOGD(tag, format, ...)
#endif

#if OZDEBUG_LOG_LEVEL >= OZDEBUG_LOG_LEVEL_INFO
#define OZ_LOGI(tag, format, ...) debugList.write(pathToFileName(__FILE__),__FUNCTION__, __LINE__,OZDEBUG_LOG_LEVEL_INFO,tag,format,##__VA_ARGS__)
//debugList.write(OZDEBUG_LOG_FORMAT(I,format), ##__VA_ARGS__)
#else
#define OZ_LOGI(tag, format, ...)
#endif

#if OZDEBUG_LOG_LEVEL >= OZDEBUG_LOG_LEVEL_WARN
#define OZ_LOGW(tag, format, ...) debugList.write(pathToFileName(__FILE__),__FUNCTION__, __LINE__,OZDEBUG_LOG_LEVEL_WARN,tag,format,##__VA_ARGS__)
#else
#define OZ_LOGW(tag, format, ...)
#endif

#if OZDEBUG_LOG_LEVEL >= OZDEBUG_LOG_LEVEL_ERROR
#define OZ_LOGE(tag, format, ...) debugList.write(pathToFileName(__FILE__),__FUNCTION__, __LINE__,OZDEBUG_LOG_LEVEL_ERROR,tag,format,##__VA_ARGS__)
#else
#define OZ_LOGE(tag, format, ...)
#endif

#endif