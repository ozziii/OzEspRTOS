#include "debug_list_oz.h"


#if OZDEBUG_LOG_LEVEL > OZDEBUG_LOG_LEVEL_NONE

void debug_list_oz::begin()
{
#if OZDEBUG_SERIAL_SUPPORT
    esp_log_level_set("*", (esp_log_level_t)OZDEBUG_ESPIDF_DEBUG_LEVEL);
    OZDEBUG_SERIAL_PORT.begin(OZDEBUG_SERIAL_PORT_BLAUD);
    OZDEBUG_SERIAL_PORT.print("\n\n");
#else
    esp_log_level_set("*", ESP_LOG_NONE);
#endif
}

void debug_list_oz::write(const char *file,const char *function,uint16_t line,uint8_t level,const char *tag,const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int len = vsnprintf(nullptr, 0, format, args) + 1;
    char *buffer = new char[len];
    vsnprintf(buffer, len, format, args);
    va_end(args);

#if OZDEBUG_SERIAL_SUPPORT
    const char *color_open;
    switch (level)
    {
    case OZDEBUG_LOG_LEVEL_ERROR:
        color_open = OZDEBUG_LOG_COLOR_E; 
        break;
    case OZDEBUG_LOG_LEVEL_WARN:
        color_open = OZDEBUG_LOG_COLOR_W;
        break;
    case OZDEBUG_LOG_LEVEL_INFO:
        color_open = OZDEBUG_LOG_COLOR_I;
        break;
    case OZDEBUG_LOG_LEVEL_DEBUG:
        color_open = OZDEBUG_LOG_COLOR_D;
        break;
    case OZDEBUG_LOG_LEVEL_VERBOSE:
        color_open = OZDEBUG_LOG_COLOR_V;
        break;
    }


    OZDEBUG_SERIAL_PORT.printf("%s[%s:%u][%s()] %s %s\n\r",color_open,file,line,function,buffer,OZDEBUG_LOG_RESET_COLOR);
#endif

#if OZDEBUG_WEB_SUPPORT
    String new_row;

    switch (level)
    {
    case OZDEBUG_LOG_LEVEL_ERROR:
        new_row = OZDEBUG_WEB_COLOR_E;
        break;
    case OZDEBUG_LOG_LEVEL_WARN:
        new_row = OZDEBUG_WEB_COLOR_W;
        break;
    case OZDEBUG_LOG_LEVEL_INFO:
        new_row += OZDEBUG_WEB_COLOR_I;
        break;
    case OZDEBUG_LOG_LEVEL_DEBUG:
        new_row = OZDEBUG_WEB_COLOR_D;
        break;
    case OZDEBUG_LOG_LEVEL_VERBOSE:
        new_row = OZDEBUG_WEB_COLOR_V;
        break;
    default:
        new_row = "<div>";
        break;
    }

    new_row += tag;
    new_row += "] ";
    new_row += buffer;
    new_row += OZDEBUG_WEB_RESET_COLOR;

    this->_debugList.push_back(new_row);

    if (this->_debugList.size() > OZDEBUG_WEB_MAX_LIST)
    {
        this->_debugList.pop_front();
    }

#endif

    delete[] buffer;
}

std::list<String> debug_list_oz::get()
{
    return this->_debugList;
}

debug_list_oz debugList;
#endif