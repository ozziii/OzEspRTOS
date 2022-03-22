#ifndef OZ_MULTIROOM_H
#define OZ_MULTIROOM_H


#include "Arduino.h"
#include "driver/i2s.h"



#define MRTAG "MR"
#define MULTIROOM_BUFFER_LENGTH 512
#define MULTIROOM_MAX_ERROR 20
#define MULTIROOM_RECEVE_TASK_STACK_DEEP 4096
#define MULTIROOM_SEND_TASK_STACK_DEEP 4096
#define MULTIROOM_TASK_PRIORITY 5
#define MULTIROOM_SAMPLE_RATE 44100
#define MULTIROOM_CONFIG_BUFFER_COUNT 8
#define MULTIROOM_CONFIG_BUFFER_LENGHT 512



typedef enum
{
    MULTIROOM_SPEAKER = 0x0, /*!< I2S REVICE*/
    MULTIROOM_SERVER = 0x1,  /*!< I2S SEND*/
} multiroom_mode_t;

typedef enum
{
    MR_COMMAND_ADD_CLIENT,
    MR_COMMAND_DEL_CLIENT,
    MR_COMMAND_STOP
} multiroom_command_t;

struct multiroom_message_t
{
    multiroom_command_t cmd;
    uint32_t ip;
};

struct multiroom_parameter
{
    i2s_port_t i2s_port;
    uint16_t port;
    TimerHandle_t start_tiker;
    TimerHandle_t stop_tiker;
    QueueHandle_t message_handler;
    uint8_t _bck_pin;
    uint8_t _rlc_pin;
    uint8_t _data_pin;
};

class oz_multiroom
{
public:
    oz_multiroom();

    /**
     *
     */
    bool begin(multiroom_mode_t mode, uint16_t UdpPort, i2s_port_t I2sPort, int BCK, int RLC, int Data);

    /**
     *
     */
    bool start(TimerHandle_t start_task = nullptr, TimerHandle_t stop_task = nullptr);

    /**
     *
     */
    bool stop();

    /**
     *
     */
    bool add_client(uint32_t address);

    /**
     *
     */
    bool remove_client(uint32_t address);

protected:

    bool _start_server();

    bool _start_speaker(TimerHandle_t start_task, TimerHandle_t stop_task);

    bool _set_client(uint32_t address, bool add);

    multiroom_mode_t _mode;

    multiroom_parameter * parameter;

    TaskHandle_t _task_handle;
};

#endif