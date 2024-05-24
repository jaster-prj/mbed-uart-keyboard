#ifndef  UART_KEYBOARD_H
#define  UART_KEYBOARD_H

#include "mbed.h"
#include "uart_keyboard_defines.h"
#include "uart_keyboard_asciimap.h"


class UartKeyboard {
public:
    UartKeyboard(BufferedSerial *bufferedserial);
    UartKeyboard(PinName tx, PinName rx);
    bool set_active(bool activate);
    bool get_active(void);
    bool set_debug(bool activate);
    bool get_debug(void);
    bool echo(void);
    bool get_ready(void);
    bool get_shift(void);
    bool get_caps(void);
    bool get_ctrl(void);
    bool get_alt(void);
    bool set_typematic(uint8_t typematic);
    void debug(char *buffer, uint8_t size);
    bool get_id(char *id);
    void attach(Callback<void(char)> func = NULL);
    void attach_on(Callback<void()> func = NULL);
    void attach_off(Callback<void()> func = NULL);
    void irqSerial(void);
    void onNewSerial(void);
    EventFlags keyboard_event;
    uint8_t get_key_count();
    int get_key(char *key);
private:
    void init(void);
    int8_t onCmdReturn(char *buffer);
    int8_t onKey(char *buffer, char *key);
    bool is_scancode_modifier(Scancode *scancode);
    void set_modifier_scancode(Scancode *scancode);
    uint8_t get_modifier_combi(void);
    bool is_scancode_key(Scancode *scancode);
    int handle_key_scancode(Scancode *scancode);
    void write_key(char key);
    EventFlags event_flag;
    Thread eventThread;
    EventQueue keyboard_queue;
    Callback<void(char)> cb_key;
    Callback<void()> cb_on;
    Callback<void()> cb_off;
    BufferedSerial *serial     = nullptr;
    char rx_buffer[20]         = {0};
    char tx_buffer[20]         = {0};
    char kb_buffer[255]        = {0};
    char id[2]                 = {0};
    bool shift                 = false;
    bool caps                  = false;
    bool ctrl                  = false;
    bool alt                   = false;
    bool ready_state           = false;
    bool active_state          = true;
    bool debug_state           = false;

    char typematic_value;
    uint8_t kb_buffer_write    = 0;
    uint8_t kb_buffer_read     = 0;
};

extern UartKeyboard *uart_keyboard;

#endif
