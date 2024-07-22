// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

// #include "i2c_master.h"
#include QMK_KEYBOARD_H
#include "oled/oled_driver.h"
#include "nec_transmit.h"


#define IR_PIN GP0

static const PIO pio = pio1;
int tx_sm = 0;

uint8_t reverse(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void send_ir_code_address_data(uint8_t address, uint8_t data) {
    uint32_t tx_frame = nec_encode_frame(reverse(address), reverse(data));
    pio_sm_put(pio, tx_sm, tx_frame);
}

// for some reason, samsung TV sends the same address twice (no bitwise inverse)
void send_ir_code_address_data_samsung(uint8_t address, uint8_t data) {
    address = reverse(address);
    data = reverse(data);
    uint32_t tx_frame = address | address << 8 | data << 16 | (data ^ 0xff) << 24;
    pio_sm_put(pio, tx_sm, tx_frame);
}


void keyboard_post_init_user() {
    uint pio_idx = pio_get_index(pio);
    /* Get PIOx peripheral out of reset state. */
    hal_lld_peripheral_unreset(pio_idx == 0 ? RESETS_ALLREG_PIO0 : RESETS_ALLREG_PIO1);

    tx_sm = nec_tx_init(pio, IR_PIN);
}

enum layer_names {
    // _BASE,
    _P1,
    _P2,
    _P1_GAME,
    _P2_GAME,
    _FN,
    _TEST
};

// ref `itgmania/src/InputMapper.cpp:57`
// #define P1_MENU_LEFT  KC_LEFT
// #define P1_MENU_RIGHT KC_RIGHT
// #define P1_MENU_UP    KC_UP
// #define P1_MENU_DOWN  KC_DOWN
// #define P1_START      KC_ENTER
// #define P1_SELECT     KC_SLASH
// #define P1_BACK       KC_ESC
// #define P2_MENU_LEFT  KC_KP_4
// #define P2_MENU_RIGHT KC_KP_6
// #define P2_MENU_UP    KC_KP_8
// #define P2_MENU_DOWN  KC_KP_2
// #define P2_START      KC_KP_ENTER
// #define P2_SELECT     KC_KP_0
// #define P2_BACK       KC_BACKSLASH
// #define COIN          KC_F1
// #define OPERATOR      KC_SCROLL_LOCK


// ref `itgmania/src/GameManager.cpp:112`
// #define P1_MENU_LEFT  KC_DEL
// #define P1_MENU_RIGHT KC_PGDN
// #define P1_MENU_UP    KC_HOME
// #define P1_MENU_DOWN  KC_END
// #define P1_LEFT       KC_LEFT
// #define P1_RIGHT      KC_RIGHT
// #define P1_UP         KC_UP
// #define P1_DOWN       KC_DOWN
// #define P2_MENU_LEFT  KC_KP_SLASH
// #define P2_MENU_RIGHT KC_KP_ASTERISK
// #define P2_MENU_UP    KC_KP_MINUS
// #define P2_MENU_DOWN  KC_KP_PLUS
// #define P2_LEFT       KC_KP_4
// #define P2_RIGHT      KC_KP_6
// #define P2_UP         KC_KP_8
// #define P2_DOWN       KC_KP_2
// #define P2_UPLEFT     KC_KP_7
// #define P2_UPRIGHT    KC_KP_9

// my mappings. note: these are intentionally swapped for P1!
#define CUSTOM_P1_MAPPINGS
#ifdef CUSTOM_P1_MAPPINGS
#define P1_MENU_LEFT  KC_LEFT
#define P1_MENU_RIGHT KC_RIGHT
#define P1_MENU_UP    KC_UP
#define P1_MENU_DOWN  KC_DOWN
#define P1_LEFT       KC_DEL
#define P1_RIGHT      KC_PGDN
#define P1_UP         KC_HOME
#define P1_DOWN       KC_END
#else
#define P1_MENU_LEFT  KC_DEL
#define P1_MENU_RIGHT KC_PGDN
#define P1_MENU_UP    KC_HOME
#define P1_MENU_DOWN  KC_END
#define P1_LEFT       KC_LEFT
#define P1_RIGHT      KC_RIGHT
#define P1_UP         KC_UP
#define P1_DOWN       KC_DOWN
#endif
#define P1_START      KC_ENTER
#define P1_SELECT     KC_SLASH
#define P1_BACK       KC_ESC
//
#define P2_MENU_LEFT  KC_KP_SLASH
#define P2_MENU_RIGHT KC_KP_ASTERISK
#define P2_MENU_UP    KC_KP_MINUS
#define P2_MENU_DOWN  KC_KP_PLUS
#define P2_LEFT       KC_KP_4
#define P2_RIGHT      KC_KP_6
#define P2_UP         KC_KP_8
#define P2_DOWN       KC_KP_2
#define P2_START      KC_KP_ENTER
#define P2_SELECT     KC_KP_0
#define P2_BACK       KC_BACKSLASH
//
#define COIN          KC_F1
#define OPERATOR      KC_SCROLL_LOCK
// requires KeyboardMouseShortcuts.lua module
#define PROFILE       KC_P


enum custom_keycodes {
    P1_OPEN_MENU = SAFE_RANGE,
    P1_CLOSE,
    P2_OPEN_MENU,
    P2_CLOSE,
    IR_SOUNDBAR_POWER_ON_OFF,
    IR_SOUNDBAR_VOL_UP,
    IR_SOUNDBAR_VOL_DOWN,
    IR_SOUNDBAR_TOGGLE_MUTE,
    IR_TV_POWER_ON_OFF,
    IR_TV_SOURCE,
    IR_TV_LEFT,
    IR_TV_RIGHT,
    IR_TV_UP,
    IR_TV_DOWN,
    IR_TV_SELECT,
    IR_TV_EXIT,
    IR_TV_HDMI1,
    IR_TV_HDMI2,
    IR_TV_HDMI3,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * ┌─────┬─────┬─────┬
     * │  7  │  8  │  9  │
     * ├─────┼─────┼─────┼
     * │  Pl │ Back│  Fn │
     * ├─────┼─────┼─────┼
     * │ Sel │  /\ │  St │
     * ├─────┼─────┼─────┼
     * │  <  │  \/ │  >  │
     * └─────┴─────┴─────┴
     */
    // [_BASE] = LAYOUT_ortho_4x3(
    //     _______     , _______     , _______,
    //     _______     , P1_BACK     , _______,
    //     P1_P2       , P1_MENU_UP  , P1_START,
    //     P1_MENU_LEFT, P1_MENU_DOWN, P1_MENU_RIGHT
    // ),
    [_P1] = LAYOUT_ortho_4x3(
        P1_SELECT   , P1_MENU_UP  , P1_START,
        P1_MENU_LEFT, P1_MENU_DOWN, P1_MENU_RIGHT,
        P1_OPEN_MENU, P1_CLOSE    , DF(_P2),
        P1_BACK     , MO(_P1_GAME), MO(_FN)
    ),
    [_P2] = LAYOUT_ortho_4x3(
        P2_SELECT   , P2_MENU_UP  , P2_START,
        P2_MENU_LEFT, P2_MENU_DOWN, P2_MENU_RIGHT,
        P2_OPEN_MENU, P2_CLOSE    , DF(_P1),
        P2_BACK     , MO(_P2_GAME), MO(_FN)
    ),
    [_P1_GAME] = LAYOUT_ortho_4x3(
        P1_SELECT   , P1_UP       , P1_START,
        P1_LEFT     , P1_DOWN     , P1_RIGHT,
        P1_OPEN_MENU, P1_CLOSE    , XXXXXXX,
        P1_BACK     , _______     , XXXXXXX
    ),
    [_P2_GAME] = LAYOUT_ortho_4x3(
        P2_SELECT   , P2_UP       , P2_START,
        P2_LEFT     , P2_DOWN     , P2_RIGHT,
        P2_OPEN_MENU, P2_CLOSE    , XXXXXXX,
        P2_BACK     , _______     , XXXXXXX
    ),
    [_FN] = LAYOUT_ortho_4x3(
        IR_SOUNDBAR_TOGGLE_MUTE , IR_SOUNDBAR_VOL_UP   , IR_TV_POWER_ON_OFF,
        _______                 , IR_SOUNDBAR_VOL_DOWN , IR_SOUNDBAR_POWER_ON_OFF,
        QK_BOOT                 , OPERATOR             , IR_TV_HDMI3,
        A(KC_F4)                , PROFILE              , _______
    ),
    [_TEST] = LAYOUT_ortho_4x3(
        KC_1,   KC_2,   KC_3,
        KC_4,   KC_5,   KC_6,
        KC_7,   KC_8,   KC_9,
        KC_A,   KC_B,   KC_C
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_P1] = { ENCODER_CCW_CW(P1_MENU_LEFT, P1_MENU_RIGHT) },
    [_P2] = { ENCODER_CCW_CW(P2_MENU_LEFT, P2_MENU_RIGHT) },
    [_P1_GAME] = { ENCODER_CCW_CW(P1_LEFT, P1_RIGHT) },
    [_P2_GAME] = { ENCODER_CCW_CW(P2_LEFT, P2_RIGHT) },
    [_FN] = { ENCODER_CCW_CW(IR_SOUNDBAR_VOL_DOWN, IR_SOUNDBAR_VOL_UP) },
    [_TEST] = { ENCODER_CCW_CW(P1_MENU_LEFT, P1_MENU_RIGHT) },
};
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case IR_SOUNDBAR_POWER_ON_OFF:
        if (record->event.pressed) {
            send_ir_code_address_data(0U, 2U);
        }
        return false;
    case IR_SOUNDBAR_VOL_UP:
        if (record->event.pressed) {
            send_ir_code_address_data(0U, 130U);
        }
        return false;
    case IR_SOUNDBAR_VOL_DOWN:
        if (record->event.pressed) {
            send_ir_code_address_data(0U, 162U);
        }
        return false;
    case IR_SOUNDBAR_TOGGLE_MUTE:
        if (record->event.pressed) {
            send_ir_code_address_data(0U, 18U);
        }
        return false;
    case IR_TV_POWER_ON_OFF:
        if (record->event.pressed) {
            send_ir_code_address_data_samsung(224U, 64U);
        }
        return false;
    case IR_TV_SOURCE:
        if (record->event.pressed) {
            send_ir_code_address_data_samsung(224U, 128U);
        }
        return false;
    case IR_TV_LEFT:
        if (record->event.pressed) {
            send_ir_code_address_data_samsung(224U, 166U);
        }
        return false;
    case IR_TV_RIGHT:
        if (record->event.pressed) {
            send_ir_code_address_data_samsung(224U, 70U);
        }
        return false;
    case IR_TV_UP:
        if (record->event.pressed) {
            send_ir_code_address_data_samsung(224U, 6U);
        }
        return false;
    case IR_TV_DOWN:
        if (record->event.pressed) {
            send_ir_code_address_data_samsung(224U, 134U);
        }
        return false;
    case IR_TV_SELECT:
        if (record->event.pressed) {
            send_ir_code_address_data_samsung(224U, 22U);
        }
        return false;
    case IR_TV_EXIT:
        if (record->event.pressed) {
            send_ir_code_address_data_samsung(224U, 180U);
        }
        return false;
    case IR_TV_HDMI1:
        if (record->event.pressed) {
            send_ir_code_address_data_samsung(224U, 151U);
        }
        return false;
    case IR_TV_HDMI2:
        if (record->event.pressed) {
            send_ir_code_address_data_samsung(224U, 125U);
        }
        return false;
    case IR_TV_HDMI3:
        if (record->event.pressed) {
            send_ir_code_address_data_samsung(224U, 67U);
        }
        return false;

    case P1_OPEN_MENU:
      if (record->event.pressed) {
        register_code(P1_MENU_LEFT);
        register_code(P1_MENU_RIGHT);
      } else {
        unregister_code(P1_MENU_LEFT);
        unregister_code(P1_MENU_RIGHT);
      }
      return false;
    case P1_CLOSE:
      if (record->event.pressed) {
        register_code(P1_UP);
        register_code(P1_DOWN);
      } else {
        unregister_code(P1_UP);
        unregister_code(P1_DOWN);
      }
      return false;
    case P2_OPEN_MENU:
      if (record->event.pressed) {
        register_code(P2_MENU_LEFT);
        register_code(P2_MENU_RIGHT);
      } else {
        unregister_code(P2_MENU_LEFT);
        unregister_code(P2_MENU_RIGHT);
      }
      return false;
    case P2_CLOSE:
      if (record->event.pressed) {
        register_code(P2_UP);
        register_code(P2_DOWN);
      } else {
        unregister_code(P2_UP);
        unregister_code(P2_DOWN);
      }
      return false;
    default:
      return true; // Process all other keycodes normally
  }
}

// #ifdef OLED_ENABLE

#define MAIN_6_BUTTONS \
                /*-----<>-----<>-----*/ \
                "Select  Up      Start" \
                /*-----<>-----<>-----*/ \
                "Left    Down    Right"
#define ALL_MENU_BUTTONS \
                MAIN_6_BUTTONS \
                /*-----<>-----<>-----*/ \
                "Menu    Close  P1<>P2" \
                /*-----<>-----<>-----*/ \
                "Back    Game         "
#define ALL_GAME_BUTTONS \
                MAIN_6_BUTTONS \
                /*-----<>-----<>-----*/ \
                "Menu    Close  P1<>P2" \
                /*-----<>-----<>-----*/ \
                "Back                 "




void render_layer_state_to_oled(void) {
    static uint16_t prev_layer = _TEST+1;
    uint16_t current_layer = get_highest_layer(layer_state | default_layer_state);
    // short-circuit if layer hasn't changed
    if (current_layer == prev_layer) {
        return;
    }
    prev_layer = current_layer;

    // oled_clear();
    switch (current_layer) {
        case _P1:
            // Font Name: 4Max
            // each line here is exactly 21 characters wide
            oled_write(
                //
                "88\"\"Yb   .d"  "   menu   "
                "88__dP .d88"    "   buttons"
                "88\"\"\"    88" "          "
                "88       88"    "          "
                ALL_MENU_BUTTONS,
                false);
            break;

        case _P2:
            // Font Name: 4Max
            // each line here is exactly 21 characters wide
            oled_write(
                //
                "88\"\"Yb oP\"Yb." " menu   "
                "88__dP \"' dP'"   " buttons"
                "88\"\"\"    dP' " "        "
                "88     .d8888"    "        "
                ALL_MENU_BUTTONS,
                false);
            break;

        case _P1_GAME:
            // Font Name: 4Max
            oled_write(
                //
                "88\"\"Yb   .d"  "      game"
                "88__dP .d88"    "   buttons"
                "88\"\"\"    88" "          "
                "88       88"    "          "
                ALL_GAME_BUTTONS,
                false);
            break;

        case _P2_GAME:
            oled_write(
                //
                "88\"\"Yb oP\"Yb." "    game"
                "88__dP \"' dP'"   " buttons"
                "88\"\"\"    dP' " "        "
                "88     .d8888"    "        "
                ALL_GAME_BUTTONS,
                false);
            break;

        case _FN:
            oled_write("_FN\n", false);
            oled_write("\n", false);
            oled_write("\n", false);
            oled_write("\n", false);
            oled_write(
                /*-----<>-----<>-----*/
                "mute   vol+     TV   "
                /*-----<>-----<>-----*/
                "____   vol-     sound"
                /*-----<>-----<>-----*/
                "flash  op       HDMI3"
                /*-----<>-----<>-----*/
                "exit   profile  Fn   "
                , false);

            // char buf[10];
            // oled_write("tx_sm: ", false);
            // snprintf(buf, 10, "%d", tx_sm);
            // oled_write(buf, false);
            // oled_write("\n", false);

            // oled_write("oled_max_chars: ", false);
            // snprintf(buf, buf_size, "%d", oled_max_chars());
            // oled_write(buf, false);
            // oled_write("\n", false);

            // oled_write("oled_max_lines: ", false);
            // snprintf(buf, buf_size, "%d", oled_max_lines());
            // oled_write(buf, false);
            // oled_write("\n", false);

            break;

        case _TEST:
            oled_write(
                //
                /*-----<>-----<>-----*/
                "_TEST                "
                "                     "
                "                     "
                "                     "
                /*-----<>-----<>-----*/
                "KC_1   KC_2   KC_3   "
                /*-----<>-----<>-----*/
                "KC_4   KC_5   KC_6   "
                /*-----<>-----<>-----*/
                "KC_7   KC_8   KC_9   "
                /*-----<>-----<>-----*/
                "KC_A   KC_B   KC_C   ",
                false);
            break;

        default:
            oled_write_P("unknown layer\n", false);
            break;
    }
    // oled_render_dirty(true);
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    // oled_set_brightness(128);
    oled_scroll_off();

    return OLED_ROTATION_0;
}

bool oled_task_user(void) {
    render_layer_state_to_oled();
    return false;
}

void oled_render_boot(bool bootloader) {
    oled_clear();
    for (int i = 0; i < 16; i++) {
        oled_set_cursor(0, i);
        if (bootloader) {
            oled_write_P(PSTR("Awaiting New Firmware "), false);
        } else {
            oled_write_P(PSTR("Rebooting "), false);
        }
    }

    oled_render_dirty(true);
}

bool shutdown_user(bool jump_to_bootloader) {
    oled_render_boot(jump_to_bootloader);
    return false;
}

// #endif // OLED_ENABLE
