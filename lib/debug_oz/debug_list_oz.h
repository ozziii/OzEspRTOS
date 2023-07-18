#ifndef DEBUG_LIST_O_H
#define DEBUG_LIST_O_H

#include <configuration.h>

#if OZDEBUG_LOG_LEVEL > OZDEBUG_LOG_LEVEL_NONE

#include <Arduino.h>
#include "esp_log.h"
#include <list>

//############################### COLOR SECTION #################################
#if OZDEBUG_SERIAL_COLOR
#define OZDEBUG_LOG_COLOR_BLACK   "30"
#define OZDEBUG_LOG_COLOR_RED     "31" //ERROR
#define OZDEBUG_LOG_COLOR_GREEN   "32" //INFO
#define OZDEBUG_LOG_COLOR_YELLOW  "33" //WARNING
#define OZDEBUG_LOG_COLOR_BLUE    "34"
#define OZDEBUG_LOG_COLOR_MAGENTA "35"
#define OZDEBUG_LOG_COLOR_CYAN    "36" //DEBUG
#define OZDEBUG_LOG_COLOR_GRAY    "37" //VERBOSE
#define OZDEBUG_LOG_COLOR_WHITE   "38"

#define OZDEBUG_LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define OZDEBUG_LOG_BOLD(COLOR)   "\033[1;" COLOR "m"
#define OZDEBUG_LOG_RESET_COLOR   "\033[0m"

#define OZDEBUG_LOG_COLOR_E       OZDEBUG_LOG_COLOR(OZDEBUG_LOG_COLOR_RED)
#define OZDEBUG_LOG_COLOR_W       OZDEBUG_LOG_COLOR(OZDEBUG_LOG_COLOR_YELLOW)
#define OZDEBUG_LOG_COLOR_I       OZDEBUG_LOG_COLOR(OZDEBUG_LOG_COLOR_GREEN)
#define OZDEBUG_LOG_COLOR_D       OZDEBUG_LOG_COLOR(OZDEBUG_LOG_COLOR_CYAN)
#define OZDEBUG_LOG_COLOR_V       OZDEBUG_LOG_COLOR(OZDEBUG_LOG_COLOR_GRAY)
#else
#define OZDEBUG_LOG_COLOR_E  ""
#define OZDEBUG_LOG_COLOR_W ""
#define OZDEBUG_LOG_COLOR_I ""
#define OZDEBUG_LOG_COLOR_D ""
#define OZDEBUG_LOG_COLOR_V ""
#define OZDEBUG_LOG_RESET_COLOR ""
#endif

#if OZDEBUG_WEB_COLOR
#define OZDEBUG_WEB_COLOR_E      "<div style='color:red;'>[E]["
#define OZDEBUG_WEB_COLOR_W      "<div style='color:yellow;'>[W]["
#define OZDEBUG_WEB_COLOR_I      "<div style='color:#22da26;'>[I]["
#define OZDEBUG_WEB_COLOR_D      "<div style='color:cyan;'>[D]["
#define OZDEBUG_WEB_COLOR_V      "<div style='color:gray;'>[V]["
#define OZDEBUG_WEB_RESET_COLOR  "</div>"
#else
#define OZDEBUG_WEB_COLOR_E ""
#define OZDEBUG_WEB_COLOR_W ""
#define OZDEBUG_WEB_COLOR_I ""
#define OZDEBUG_WEB_COLOR_D ""
#define OZDEBUG_WEB_COLOR_V ""
#define OZDEBUG_WEB_RESET_COLOR ""
#endif

//############################### END COLOR SECTION #################################
 
class debug_list_oz
{
public:
    void begin();
    void write(const char *file,const char *function,uint16_t line,uint8_t level,const char *tag,const char *format, ...);
    std::list<String> get();
private:
    std::list<String> _debugList;
};


extern debug_list_oz debugList;

#endif
#endif