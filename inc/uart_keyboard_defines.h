#ifndef UART_KEYBOARD_DEFINES_H
#define UART_KEYBOARD_DEFINES_H

#define CMD_ACK                  0x7F
#define CMD_RESEND               0xFE
#define CMD_RETURN               0xA0
#define CMD_INITIALIZED          0xAA

#define CMD_KEYBOARD_ACTIVATE    0xF4
#define CMD_KEYBOARD_DEACTIVATE  0xF5
#define CMD_KEYBOARD_RESET       0xFF
#define CMD_KEYBOARD_BOOTLOADER  0xE2
#define CMD_KEYBOARD_TYPEMATIC   0xF3
#define CMD_KEYBOARD_SET_DEBUG   0xF6
#define CMD_KEYBOARD_ECHO        0xEE
#define CMD_KEYBOARD_DEBUG       0xEF
#define CMD_KEYBOARD_ID          0xF2

#define ECHO_FLAG                (1UL << 0)
#define KEYBOARD_ACTIVE          (1UL << 1)
#define KEYBOARD_INACTIVE        (1UL << 2)
#define KEYBOARD_RESETTED        (1UL << 3)
#define KEYBOARD_BOOTLOADER_STARTED (1UL << 4)
#define KEYBOARD_TYPEMATIC_SET   (1UL << 5)
#define KEYBOARD_DEBUG_SET       (1UL << 6)
#define KEYBOARD_ID              (1UL << 7)

#define MODIFIER_SHIFT           0
#define MODIFIER_ALT             1
#define MODIFIER_CTRL            2
#define KB_MODIFIER_NONE         0x00
#define KB_MODIFIER_SHIFT        (1UL << MODIFIER_SHIFT)
#define KB_MODIFIER_ALT          (1UL << MODIFIER_ALT)
#define KB_MODIFIER_CTRL         (1UL << MODIFIER_CTRL)
#define KB_MODIFIER_SHIFTALT     ((1UL << MODIFIER_ALT) | (1UL << MODIFIER_SHIFT))
#define KB_MODIFIER_ALTCTRL      ((1UL << MODIFIER_CTRL) | (1UL << MODIFIER_ALT))
#define KB_MODIFIER_CTRLSHIFT    ((1UL << MODIFIER_CTRL) | (1UL << MODIFIER_SHIFT))
#define KB_MODIFIER_CTRLALTSHIFT ((1UL << MODIFIER_CTRL) | (1UL << MODIFIER_ALT) | (1UL << MODIFIER_SHIFT))

#define KB_EVENT_ON              (1UL << 0)
#define KB_EVENT_OFF             (1UL << 1)
#define KB_EVENT_INSERT          (1UL << 2)
#define KB_EVENT_PAST            (1UL << 3)
#define KB_EVENT_ARROWUP         (1UL << 10)
#define KB_EVENT_ARROWDOWN       (1UL << 11)
#define KB_EVENT_ARROWLEFT       (1UL << 12)
#define KB_EVENT_ARROWRIGHT      (1UL << 13)
#define KB_EVENT_FUNC1           (1UL << 14)
#define KB_EVENT_FUNC2           (1UL << 15)
#define KB_EVENT_FUNC3           (1UL << 16)
#define KB_EVENT_FUNC4           (1UL << 17)
#define KB_EVENT_FUNC5           (1UL << 18)
#define KB_EVENT_FUNC6           (1UL << 19)
#define KB_EVENT_KEY             (1UL << 30)
#define KB_EVENT_ALL             ((KB_EVENT_ON | KB_EVENT_OFF | KB_EVENT_INSERT | KB_EVENT_PAST | KB_EVENT_ARROWUP | KB_EVENT_ARROWDOWN | KB_EVENT_ARROWLEFT | KB_EVENT_ARROWRIGHT | \
                                  KB_EVENT_FUNC1 | KB_EVENT_FUNC2 | KB_EVENT_FUNC3 | KB_EVENT_FUNC4 | KB_EVENT_FUNC5 | KB_EVENT_FUNC6 | KB_EVENT_KEY))

#endif
