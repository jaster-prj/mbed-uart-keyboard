#include "../include/uart_keyboard.h"

UartKeyboard *uart_keyboard;

UartKeyboard::UartKeyboard(BufferedSerial *bufferedserial) {
    this->serial = bufferedserial;
    this->serial->set_baud(9600);
    init();
}
 
UartKeyboard::UartKeyboard(PinName tx, PinName rx) {
    this->serial = new BufferedSerial(tx, rx, 9600);
    init();
}

void UartKeyboard::init(void) {
    eventThread.start(callback(&keyboard_queue, &EventQueue::dispatch_forever));
    this->serial->set_format(8, BufferedSerial::Odd, 2);
    this->serial->sigio(callback(this, &UartKeyboard::irqSerial));
}

bool UartKeyboard::set_active(bool activate) {
    uint32_t flag;
    uint32_t flags_read = 0;
    if (activate) {
        tx_buffer[0] = CMD_KEYBOARD_ACTIVATE;
        flag = KEYBOARD_ACTIVE;
    } else {
        tx_buffer[0] = CMD_KEYBOARD_DEACTIVATE;
        flag = KEYBOARD_INACTIVE;
    }
    if (this->serial->writable()) {
        this->serial->write(tx_buffer, 1);
    }
    flags_read = event_flag.wait_any_for(flag, 100ms);
    if (flags_read == flag) {
        if (active_state == activate) {
            return true;
        }
    }
    return false;
}

bool UartKeyboard::get_active(void) {
    return active_state;
}

bool UartKeyboard::set_debug(bool activate) {
    uint32_t flags_read = 0;
    tx_buffer[0] = CMD_KEYBOARD_SET_DEBUG;
    tx_buffer[1] = (uint8_t)activate;
    if (this->serial->writable()) {
        this->serial->write(tx_buffer, 2);
    }
    flags_read = event_flag.wait_any_for(KEYBOARD_DEBUG_SET, 100ms);
    if (flags_read == KEYBOARD_DEBUG_SET) {
        if (debug_state == (uint8_t)activate) {
            return true;
        }
    }
    return false;
}

bool UartKeyboard::get_debug(void) {
    return debug_state;
}

bool UartKeyboard::echo(void) {
    tx_buffer[0] = CMD_KEYBOARD_ECHO;
    if (this->serial->writable()) {
        this->serial->write(tx_buffer, 1);
    }
    uint32_t flags_read = event_flag.wait_any_for(ECHO_FLAG, 100ms);
    if (flags_read == ECHO_FLAG) {
        return true;
    }
    return false;
}

bool UartKeyboard::get_ready(void) {
    return ready_state;
}

bool UartKeyboard::get_shift(void) {
    return shift;
}

bool UartKeyboard::get_caps(void) {
    return caps;
}

bool UartKeyboard::get_ctrl(void) {
    return ctrl;
}

bool UartKeyboard::get_alt(void) {
    return alt;
}

bool UartKeyboard::set_typematic(uint8_t typematic) {
    tx_buffer[0] = CMD_KEYBOARD_TYPEMATIC;
    tx_buffer[1] = typematic;
    if (this->serial->writable()) {
        this->serial->write(tx_buffer, 2);
    }
    uint32_t flags_read = event_flag.wait_any_for(KEYBOARD_TYPEMATIC_SET, 500ms);
    if (flags_read == KEYBOARD_TYPEMATIC_SET) {
        if (typematic_value == typematic) {
            return true;
        }
    }
    return false;
}

void UartKeyboard::debug(char *buffer, uint8_t size) {
    tx_buffer[0] = CMD_KEYBOARD_DEBUG;
    for (int i=0;i<size;i++){
        tx_buffer[i+1] = buffer[i];
    }
    if (this->serial->writable()) {
        this->serial->write(tx_buffer, size+1);
    }
}

bool UartKeyboard::get_id(char *id) {
    uint32_t flags_read = 0;
    tx_buffer[0] = CMD_KEYBOARD_ID;
    if (this->serial->writable()) {
        this->serial->write(tx_buffer, 1);
    }
    flags_read = event_flag.wait_any_for(KEYBOARD_ID, 100ms);
    if (flags_read == KEYBOARD_ID) {
        id[0] = this->id[0];
        id[1] = this->id[1];
        return true;
    }
    id = nullptr;
    return false;
}

void UartKeyboard::attach(Callback<void(char)> func) {
    cb_key = func;
}

void UartKeyboard::attach_on(Callback<void()> func) {
    cb_on = func;
}

void UartKeyboard::attach_off(Callback<void()> func) {
    cb_off = func;
}

void UartKeyboard::onNewSerial(void) {
    int8_t rx_in = 0;
    int8_t size = 0;
    char new_key = 0;
    while (this->serial->readable()) {
        this->serial->read(rx_buffer+rx_in, 1);
        rx_in += 1;
        ThisThread::sleep_for(10ms);
    }
    while(rx_in>0){
        switch(rx_buffer[0]) {
        case CMD_INITIALIZED:
            ready_state = true;
            rx_in -= 1;
            continue;
        case CMD_KEYBOARD_ECHO:
            event_flag.set(ECHO_FLAG);
            rx_in -= 1;
            continue;
        case CMD_RETURN:
            size = onCmdReturn(&rx_buffer[1]);
            if (size == -1) {
                return;
            }
            rx_in -= size;
            continue;
        default:
            size = onKey(rx_buffer, &new_key);
            if (size == -1) {
                return;
            }
            if (new_key != 0x00) {
                cb_key(new_key);
            }
            rx_in -= size;
            continue;
        }
    }
}

int8_t UartKeyboard::onCmdReturn(char *buffer) {
    switch(buffer[0]) {
    case CMD_KEYBOARD_ACTIVATE:
        if (buffer[1] == CMD_ACK) {
            active_state = true;
            event_flag.set(KEYBOARD_ACTIVE);
        }
        return 2;
    case CMD_KEYBOARD_DEACTIVATE:
        if (buffer[1] == CMD_ACK) {
            active_state = false;
            event_flag.set(KEYBOARD_INACTIVE);
        }
        return 2;
    case CMD_KEYBOARD_RESET:
        if (buffer[1] == CMD_ACK) {
            ready_state = false;
            event_flag.set(KEYBOARD_RESETTED);
        }
        return 2;
    case CMD_KEYBOARD_BOOTLOADER:
        if (buffer[1] == CMD_ACK) {
            ready_state = false;
            event_flag.set(KEYBOARD_BOOTLOADER_STARTED);
        }
        return 2;
    case CMD_KEYBOARD_TYPEMATIC:
        if (buffer[1] == CMD_ACK) {
            typematic_value = buffer[2];
            event_flag.set(KEYBOARD_TYPEMATIC_SET);
        }
        return 3;
    case CMD_KEYBOARD_SET_DEBUG:
        if (buffer[1] == CMD_ACK) {
            debug_state = buffer[2]>0;
            event_flag.set(KEYBOARD_DEBUG_SET);
        }
        return 3;
    case CMD_KEYBOARD_ID:
        id[0] = buffer[1];
        id[1] = buffer[2];
        event_flag.set(KEYBOARD_ID);
        return 3;
    default:
        return -1;
    }
}

int8_t UartKeyboard::onKey(char *buffer, char *key) {
    Scancode scancode = {
        .release = false,
        .special = 0,
        .code = 0
    };
    uint8_t i = 0;
    if (buffer[i] == 0xE0 || buffer[i] == 0xE1) {
        scancode.special = buffer[i];
        i++;
    }
    if (buffer[i] == 0xF0) {
        scancode.release = true;
        i++;
    }
    scancode.code = buffer[i];
    i++;
    if (is_scancode_modifier(&scancode)) {
        set_modifier_scancode(&scancode);
        return (int8_t)i;
    }
    if (is_scancode_key(&scancode)) {
        handle_key_scancode(&scancode);
        return (int8_t)i;
    }
    return (int8_t)-1;
}

bool UartKeyboard::is_scancode_modifier(Scancode *scancode) {
    if ((scancode->code == 0x1D || scancode->code == 0x2A || scancode->code == 0x3A || scancode->code == 0x38) && scancode->special==0) {
        return true;
    }
    return false;
}

void UartKeyboard::set_modifier_scancode(Scancode *scancode) {
    switch(scancode->code) {
    case 0x1D:
        ctrl = !scancode->release;
        return;
    case 0x2A:
        shift = !scancode->release;
        return;
    case 0x3A:
        if (!scancode->release) {
            caps = !caps;
        }
        return;
    case 0x38:
        alt = !scancode->release;
        return;
    }
}

uint8_t UartKeyboard::get_modifier_combi(void) {
    bool shiftcaps = shift ^ caps;
    return (ctrl << MODIFIER_CTRL) | (alt << MODIFIER_ALT) | (shiftcaps << MODIFIER_SHIFT);
}

bool UartKeyboard::is_scancode_key(Scancode *scancode) {
    //AsciiMap *listPointer = (AsciiMap *)asciiMapList;
    AsciiMap *asciimap;
    for (unsigned int i=0; i<(int)(sizeof(asciiMapList)/sizeof(asciiMapList[0]));i++) {
        asciimap = (AsciiMap *)&asciiMapList[i];
        if (scancode->special == asciimap->scancode.special && scancode->code == asciimap->scancode.code) {
            return true;
        }
    }
    return false;
}

int UartKeyboard::handle_key_scancode(Scancode *scancode) {
    //AsciiMap *listPointer = (AsciiMap *)asciiMapList;
    AsciiMap *asciimap;
    for (unsigned int i=0; i<(sizeof(asciiMapList)/sizeof(asciiMapList[0]));i++) {
        asciimap = (AsciiMap *)&asciiMapList[i];
        if (scancode->special == asciimap->scancode.special && scancode->code == asciimap->scancode.code) {
            break;
        }
        asciimap = nullptr;
    }
    if (asciimap == nullptr) {
        return -1;
    }
    if (scancode->release) {
        return 0;
    }
    uint8_t modifier_combi = get_modifier_combi();
    Code *code;
    switch (modifier_combi) {
    case KB_MODIFIER_NONE:
        code = asciimap->none;
        break;
    case KB_MODIFIER_SHIFT:
        code = asciimap->shift;
        break;
    case KB_MODIFIER_ALT:
        code = asciimap->alt;
        break;
    case KB_MODIFIER_CTRL:
        code = asciimap->strg;
        break;
    case KB_MODIFIER_SHIFTALT:
        code = asciimap->shiftalt;
        break;
    case KB_MODIFIER_ALTCTRL:
        code = asciimap->altstrg;
        break;
    case KB_MODIFIER_CTRLSHIFT:
        code = asciimap->strgshift;
        break;
    case KB_MODIFIER_CTRLALTSHIFT:
        code = asciimap->strgaltshift;
        break;
    default:
        return -1;
    }
    switch (code->type) {
    case CHAR:
        write_key((char)(code->code));
        keyboard_event.set((uint32_t)KB_EVENT_KEY);
        break;
    case EVENT:
        keyboard_event.set(code->code);
        break;
    default:
        return -1;
    }
    return 0;
}

uint8_t UartKeyboard::get_key_count(void) {
    if (kb_buffer_read>kb_buffer_write) {
        return (kb_buffer_write+255-kb_buffer_read);
    }
    return (kb_buffer_write-kb_buffer_read);
}

int UartKeyboard::get_key(char *key) {
    if (get_key_count()==0) {
        return -1;
    }
    *key = kb_buffer[kb_buffer_read];
    kb_buffer_read++;
    if (kb_buffer_read>=255) {
        kb_buffer_read = 0;
    }
    return 0;
}

void UartKeyboard::write_key(char key) {
    kb_buffer[kb_buffer_write] = key;
    kb_buffer_write++;
    if (kb_buffer_write>=255) {
        kb_buffer_write = 0;
    }
}

void UartKeyboard::irqSerial(void) {
    keyboard_queue.call(this, &UartKeyboard::onNewSerial);
}
