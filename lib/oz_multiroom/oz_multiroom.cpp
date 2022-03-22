#include "oz_multiroom.h"

#include "WiFi.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "queue"

#include <debug_oz.h>

static bool thread_is_running;

static bool mr_is2_begin(multiroom_mode_t mode, multiroom_parameter *parameter)
{
    i2s_config_t i2s_config_rx;

    if (mode == MULTIROOM_SERVER)
    {
        i2s_config_rx.mode = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_RX);
    }
    else
    {
        i2s_config_rx.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
    }

    i2s_config_rx.sample_rate = MULTIROOM_SAMPLE_RATE;
    i2s_config_rx.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    i2s_config_rx.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    i2s_config_rx.communication_format = I2S_COMM_FORMAT_I2S_MSB;
    i2s_config_rx.dma_buf_count = MULTIROOM_CONFIG_BUFFER_COUNT; // number of buffers, 128 max.
    i2s_config_rx.dma_buf_len = MULTIROOM_CONFIG_BUFFER_LENGHT;  // size of each buffer
    i2s_config_rx.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;       // Interrupt level 1
    i2s_config_rx.tx_desc_auto_clear = true;
    i2s_config_rx.use_apll = false;

    esp_err_t err = i2s_driver_install(parameter->i2s_port, &i2s_config_rx, 0, NULL);

    if (err != ESP_OK)
    {
        OZ_LOGV(MRTAG, "Error install driver err: %d", err);
        return false;
    }

    i2s_pin_config_t pin_config_tx;
    pin_config_tx.bck_io_num = parameter->_bck_pin;
    pin_config_tx.ws_io_num = parameter->_rlc_pin;

    if (mode == MULTIROOM_SERVER)
    {
        pin_config_tx.data_out_num = I2S_PIN_NO_CHANGE;
        pin_config_tx.data_in_num = parameter->_data_pin;
    }
    else
    {
        pin_config_tx.data_out_num = parameter->_data_pin;
        pin_config_tx.data_in_num = I2S_PIN_NO_CHANGE;
    }

    err = i2s_zero_dma_buffer(parameter->i2s_port);

    if (err != ESP_OK)
    {
        OZ_LOGV(MRTAG, "Zero dma err: %d", err);
        return false;
    }

    err = i2s_set_pin(parameter->i2s_port, &pin_config_tx);

    if (err != ESP_OK)
    {
        OZ_LOGV(MRTAG, "Set Pin err: %d", err);
        return false;
    }

    if (mode == MULTIROOM_SPEAKER)
    {
        REG_WRITE(PIN_CTRL, 0xFF0);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    }

    return true;
}

static bool mr_i2s_stop(multiroom_parameter *parameter)
{
    if (parameter == nullptr)
        return false;
    if (parameter->i2s_port >= I2S_NUM_MAX)
        return false;

    // esp_err_t err = i2s_stop(this->parameter->i2s_port);
    // if (err != ESP_OK)
    //{
    //     OZ_LOGV(MRTAG, "Can't stop i2s");
    //     return false;
    // }

    esp_err_t err = i2s_driver_uninstall(parameter->i2s_port);
    if (err != ESP_OK)
    {
        OZ_LOGV(MRTAG, "Can't unistall i2s driver");
        return false;
    }

    return true;
}

static int multiroom_create_socket(uint16_t Port)
{
    int _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (_socket < 0)
        return _socket;

    int yes = 0;
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
    {
        close(_socket);
        return -1;
    }

    // Bind the socket to any address
    sockaddr_in saddr = {0};
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(Port);
    if (bind(_socket, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in)) < 0)
    {
        close(_socket);
        return -1;
    }

    fcntl(_socket, F_SETFL, O_NONBLOCK);

    return _socket;
}

/**
 *
 *
 *
 *
 */
static void multiroom_recive_task(void *pvParameters)
{
    OZ_LOGD(MRTAG, "Recive task Start");

    auto parameter = reinterpret_cast<multiroom_parameter *>(pvParameters);

    if (parameter->message_handler == 0)
    {
        OZ_LOGE(MRTAG, "ERROR Message queue not initialized!! Thread Shudown!!");
        esp_restart();
        vTaskDelete(NULL);
    }

    if (!mr_is2_begin(MULTIROOM_SPEAKER, parameter))
    {
        OZ_LOGE(MRTAG, "ERROR initialize i2s");
        esp_restart();
        vTaskDelete(NULL);
    }

    thread_is_running = true;
    xQueueReset(parameter->message_handler);

    uint16_t *buffer = new uint16_t[MULTIROOM_BUFFER_LENGTH * 2];
    uint16_t *sample = new uint16_t[2];

    multiroom_message_t *message;
    bool state = false;
    size_t bytes_written = 0;
    uint8_t init_error_count = 0;
    int socket = -1;
    struct timeval tv = {
        .tv_sec = 3,
        .tv_usec = 0,
    };

    for (;;)
    {
        // GET message
        if (xQueueReceive(parameter->message_handler, &message, 1) == pdPASS)
        {
            if (message->cmd == MR_COMMAND_STOP)
            {
                shutdown(socket, 0);
                close(socket);
                OZ_LOGD(MRTAG, "Thread Shudown");
                delete[] buffer;
                delete[] sample;
                delete message;
                thread_is_running = false;
                if (!mr_i2s_stop(parameter))
                {
                    OZ_LOGE(MRTAG, "ERROR unistall i2s dirver");
                    esp_restart();
                }
                vTaskDelete(NULL);
            }
        }

        // On loop of socket erro restart ESP;
        init_error_count++;
        if (init_error_count > MULTIROOM_MAX_ERROR)
        {
            OZ_LOGE(MRTAG, "too many error!!! restart");
            esp_restart();
        }

        // wait for wifi connected... //TODO
        while (!WiFi.isConnected())
            delay(1000);

        // On socket create error wait 2 s and try again
        socket = multiroom_create_socket(parameter->port);
        if (socket < 0)
        {
            OZ_LOGE(MRTAG, "Failed to create IPv4 multicast socket");
            delay(2000);
            continue;
        }

        // Socket create enter in main loop
        int err = 1;
        while (err > 0)
        {
            // GET message
            if (xQueueReceive(parameter->message_handler, &message, 1) == pdPASS)
            {
                if (message->cmd == MR_COMMAND_STOP)
                {
                    shutdown(socket, 0);
                    close(socket);
                    OZ_LOGD(MRTAG, "Thread Shudown");
                    delete[] buffer;
                    delete[] sample;
                    delete message;
                    thread_is_running = false;
                    if (!mr_i2s_stop(parameter))
                    {
                        OZ_LOGE(MRTAG, "ERROR unistall i2s dirver");
                        esp_restart();
                    }
                    vTaskDelete(NULL);
                }
            }

            fd_set rfds;

            // Inizializza a zero l'insieme dei socket da ascoltare
            FD_ZERO(&rfds);

            // inserisce il socket tra i descrittori da controllare
            FD_SET(socket, &rfds);

            // aspetta che il socket sia leggibile per tempo tv
            int s = select(socket + 1, &rfds, NULL, NULL, &tv);

            // errore nel socket
            if (s < 0)
            {
                OZ_LOGE(MRTAG, "Select failed: errno %d", errno);
                err = -1;
                break;
            }
            // recieve data
            else if (s > 0)
            {

                // Controlla se il socket è leggibile
                if (!FD_ISSET(socket, &rfds))
                    continue;

                struct sockaddr_in6 raddr; // Large enough for both IPv4 or IPv6
                socklen_t socklen = sizeof(raddr);
                int len = recvfrom(socket, buffer, MULTIROOM_BUFFER_LENGTH * 2, 0, (struct sockaddr *)&raddr, &socklen);

                // errore nella lettura del socket esce dal ciclo
                if (len < 0)
                {
                    OZ_LOGE(MRTAG, "multicast recvfrom failed: errno %d ", errno);
                    err = -1;
                    shutdown(socket, 0);
                    close(socket);
                    break;
                };

                // Se arriva il flusso manda il comando di start
                if (!state)
                {
                    state = true;
                    if (parameter->start_tiker != NULL)
                        xTimerStart(parameter->start_tiker, 0);
                }

                //####################################  MONO TO STEREO  ################################

                size_t len_in_word = len / 2;

                for (size_t index = 0; index < len_in_word; index++)
                {
                    sample[0] = buffer[index];
                    sample[1] = buffer[index];
                    // esp_err_t i2s_error = i2s_write(parameter->i2s_port, sample, 4 , &bytes_written, pdMS_TO_TICKS(1000));
                    i2s_write(parameter->i2s_port, sample, 4, &bytes_written, pdMS_TO_TICKS(1000));
                }

                init_error_count = 0;
            }
            // NO data recieve
            else
            {
                OZ_LOGD(MRTAG, "Stop data recive");
                // Se smette il flusso manda il comando di stop
                if (state)
                {
                    state = false;
                }

                if (parameter->stop_tiker != NULL)
                    xTimerStart(parameter->stop_tiker, 0);

                delay(200); // if no data arrive slow down clock;
                init_error_count = 0;
            }
        }

        shutdown(socket, 0);
        close(socket);
    }

    shutdown(socket, 0);
    close(socket);
    delete[] buffer;
    delete[] sample;
    thread_is_running = false;
    vTaskDelete(NULL);
}

/**
 *
 *
 *
 *
 */
static void multiroom_send_task(void *pvParameters)
{
    auto parameter = reinterpret_cast<multiroom_parameter *>(pvParameters);

    if (parameter->message_handler == nullptr)
    {
        OZ_LOGE(MRTAG, "ERROR Message queue not initialized!! Thread Shudown!!");
        vTaskDelete(NULL);
    }

    if (!mr_is2_begin(MULTIROOM_SERVER, parameter))
    {
        OZ_LOGE(MRTAG, "ERROR initialize i2s");
        esp_restart();
        vTaskDelete(NULL);
    }

    thread_is_running = true;
    xQueueReset(parameter->message_handler);

    std::list<uint32_t> client;
    uint16_t *buffer = new uint16_t[MULTIROOM_BUFFER_LENGTH];
    uint8_t init_error_count = 0;
    size_t item_size = 0;
    multiroom_message_t *message;
    int socket = -1;

    struct sockaddr_in recipient;
    recipient.sin_family = AF_INET;
    recipient.sin_port = htons(parameter->port);

    for (;;)
    {
        // GET message
        if (xQueueReceive(parameter->message_handler, &message, 1) == pdPASS)
        {
            if (message->cmd == MR_COMMAND_STOP)
            {
                shutdown(socket, 0);
                close(socket);
                OZ_LOGD(MRTAG, "Thread Shudown");
                delete[] buffer;
                delete message;
                thread_is_running = false;
                if (!mr_i2s_stop(parameter))
                {
                    OZ_LOGE(MRTAG, "ERROR unistall i2s dirver");
                    esp_restart();
                }
                vTaskDelete(NULL);
            }
            else if (message->cmd == MR_COMMAND_ADD_CLIENT)
            {
                OZ_LOGD(MRTAG, "Add client %u", message->ip);
                client.remove(message->ip);
                client.push_front(message->ip);
            }
            else if (message->cmd == MR_COMMAND_DEL_CLIENT)
            {
                OZ_LOGD(MRTAG, "Remove client %u", message->ip);
                client.remove(message->ip);
            }

            delete message;
        }

        // On loop of socket erro restart ESP;
        init_error_count++;
        if (init_error_count > MULTIROOM_MAX_ERROR)
        {
            OZ_LOGE(MRTAG, "too many error restart");
            esp_restart();
        }

        // wait for wifi connected...
        while (!WiFi.isConnected())
            delay(1000);

        // On socket create error wait 2 s and try again
        int socket = multiroom_create_socket(parameter->port);

        if (socket < 0)
        {
            OZ_LOGE(MRTAG, "Failed to create IPv4 multicast socket");
            delay(2000);
            continue;
        }

        // recipient.sin_addr.s_addr = 2063706304;

        OZ_LOGD(MRTAG, "Start send thread");

        int err = 1;
        while (err > 0)
        {
            // Add o remove client from address
            if (xQueueReceive(parameter->message_handler, &message, 1) == pdPASS)
            {
                if (message->cmd == MR_COMMAND_STOP)
                {
                    shutdown(socket, 0);
                    close(socket);
                    OZ_LOGD(MRTAG, "Thread Shudown");
                    delete[] buffer;
                    delete message;
                    thread_is_running = false;
                    if (!mr_i2s_stop(parameter))
                    {
                        OZ_LOGE(MRTAG, "ERROR unistall i2s dirver");
                        esp_restart();
                    }
                    vTaskDelete(NULL);
                }
                else if (message->cmd == MR_COMMAND_ADD_CLIENT)
                {
                    OZ_LOGD(MRTAG, "Add client %u", message->ip);
                    client.remove(message->ip);
                    client.push_front(message->ip);
                }
                else if (message->cmd == MR_COMMAND_DEL_CLIENT)
                {
                    OZ_LOGD(MRTAG, "Remove client %u", message->ip);
                    client.remove(message->ip);
                }

                delete message;
            }

            // portMAX_DELAY
            esp_err_t err = i2s_read(parameter->i2s_port, buffer, MULTIROOM_BUFFER_LENGTH * 2, &item_size, portMAX_DELAY);

            // i2s erro
            if (err != ESP_OK)
            {
                OZ_LOGE(MRTAG, "I2S read error: %d ", err);
                // break;
            }

            // i2s no data recived
            if (item_size <= 0)
            {
                OZ_LOGV(MRTAG, "I2S empty data ");
                continue;
            }

            //####################################  STEREO TO MONO  ################################
            for (uint16_t i = 0; i < item_size / 4; i++)
            {
                buffer[i] = buffer[i * 2];
            }
            //####################################  STEREO TO MONO  ################################

            for (uint32_t address : client)
            {
                recipient.sin_addr.s_addr = address;

                // ENOMEM PROBLEM
                for (int tries = 0; tries < 100; tries++) // while wifi buffer is free
                {
                    err = sendto(socket, buffer, item_size / 2, 0, (struct sockaddr *)&recipient, sizeof(recipient));

                    if (err < 0 && errno == ENOMEM)
                        vTaskDelay(1); // if ENOMEM re send data;
                    else
                        break;
                }

                if (err < 0)
                {
                    OZ_LOGD(MRTAG, "IPV4 sendto failed. errno: %d ", errno);
                    break;
                }
            }
        }

        shutdown(socket, 0);
        close(socket);
    }

    shutdown(socket, 0);
    close(socket);
    delete[] buffer;
    thread_is_running = false;
}



/**
 * @class   oz_multiroom   ############################################################################
 */


oz_multiroom::oz_multiroom()
{
    this->parameter = new multiroom_parameter();
    this->parameter->i2s_port = I2S_NUM_MAX;
    this->parameter->port = 0;
    this->parameter->message_handler = 0;
    this->parameter->start_tiker = NULL;
    this->parameter->stop_tiker = NULL;
    this->_task_handle = NULL;
}

bool oz_multiroom::begin(multiroom_mode_t mode, uint16_t UdpPort, i2s_port_t I2sPort, int BCK, int RLC, int Data)
{
    this->parameter->i2s_port = I2sPort;
    this->parameter->port = UdpPort;
    this->_mode = mode;
    this->parameter->_bck_pin = BCK;
    this->parameter->_rlc_pin = RLC;
    this->parameter->_data_pin = Data;

    this->parameter->message_handler = xQueueCreate(5, sizeof(multiroom_message_t *));
    if (this->parameter->message_handler == 0)
    {
        OZ_LOGE(MRTAG, "Error to crearte queue");
        return false;
    }

    return true;
}

bool oz_multiroom::start(TimerHandle_t start_task, TimerHandle_t stop_task)
{
    if (thread_is_running)
    {
        xQueueReset(parameter->message_handler);
        OZ_LOGW(MRTAG, "Task alredy running!!!");
        return false;
    }

    // start speaker
    if (this->_mode == MULTIROOM_SPEAKER)
        return this->_start_speaker(start_task, stop_task);

    // start server
    if (this->_mode == MULTIROOM_SERVER)
        return this->_start_server();

    return false;
}

bool oz_multiroom::_start_server()
{
    xTaskCreate(
        multiroom_send_task,
        "multiroom_send_task",
        MULTIROOM_SEND_TASK_STACK_DEEP * 4,
        (void *)this->parameter,
        MULTIROOM_TASK_PRIORITY,
        &(this->_task_handle));

    return this->_task_handle != NULL;
}

bool oz_multiroom::_start_speaker(TimerHandle_t start_task, TimerHandle_t stop_task)
{
    this->parameter->start_tiker = start_task;
    this->parameter->stop_tiker = stop_task;

    xTaskCreate(
        multiroom_recive_task,
        "multiroom_recive_task",
        MULTIROOM_RECEVE_TASK_STACK_DEEP,
        (void *)this->parameter,
        MULTIROOM_TASK_PRIORITY,
        &(this->_task_handle));

    return this->_task_handle != NULL;
}

bool oz_multiroom::stop()
{
    if (!thread_is_running)
    {
        OZ_LOGW(MRTAG, "Stop Thread error... thread not running");
        return false;
    }

    if (this->parameter->message_handler == 0)
    {
        OZ_LOGE(MRTAG, "ERROR Message queue not initialized!! Message not send!!");
        return false;
    }

    multiroom_message_t *msg = new multiroom_message_t();
    msg->ip = 0;
    msg->cmd = MR_COMMAND_STOP;
    if (xQueueSend(this->parameter->message_handler, &msg, 10) == pdPASS)
    {
        return true;
    }

    return false;
}

bool oz_multiroom::add_client(uint32_t address)
{
    return this->_set_client(address, true);
}

bool oz_multiroom::remove_client(uint32_t address)
{
    return this->_set_client(address, false);
}

bool oz_multiroom::_set_client(uint32_t address, bool add)
{

    if (this->_mode != MULTIROOM_SERVER)
        return false;
    if (this->parameter->message_handler == 0)
        return false;

    multiroom_message_t *msg = new multiroom_message_t();
    msg->ip = address;
    msg->cmd = add ? MR_COMMAND_ADD_CLIENT : MR_COMMAND_DEL_CLIENT;
    return xQueueSend(this->parameter->message_handler, &msg, 0) == pdTRUE;
}
