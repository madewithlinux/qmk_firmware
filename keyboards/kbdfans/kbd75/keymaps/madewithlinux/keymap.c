#include QMK_KEYBOARD_H
// make kbdfans/kbd75:madewithlinux:dfu

// We delay showing the popup so that fast Alt+Tab users aren't
// disturbed by the popup briefly flashing.
#define POPUP_DELAY_TIMEOUT 100
#define TABALT_STEP_DELAY   10


#ifdef CONSOLE_ENABLE
#include "print.h"
#define DEBUG
#endif

#ifdef DEBUG
#define IF_DEBUG(x) x
#else
#define IF_DEBUG(x)
#endif

enum custom_keycodes {
    TABALT = SAFE_RANGE,
    TOGTALT,
};

uint16_t alt_tab_timer = 0;

typedef enum {
    TABALT_OFF = 0,
    TABALT_NORMAL = 1,
    TABALT_RALT = 2,
} tabalt_mode_t;

tabalt_mode_t tabalt_mode = TABALT_OFF;
#define is_tabalt_active (tabalt_mode != TABALT_OFF)

const rgblight_segment_t PROGMEM _tabalt_off_layer[] = RGBLIGHT_LAYER_SEGMENTS({9, 6, HSV_RED});
const rgblight_segment_t PROGMEM _tabalt_normal_layer[] = RGBLIGHT_LAYER_SEGMENTS({9, 6, HSV_BLUE});
const rgblight_segment_t PROGMEM _tabalt_ralt_layer[]  = RGBLIGHT_LAYER_SEGMENTS({9, 6, HSV_GREEN});
const rgblight_segment_t PROGMEM _tabalt_layer[] = RGBLIGHT_LAYER_SEGMENTS( {0, 2, HSV_WHITE} );

const rgblight_segment_t* const PROGMEM _rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    _tabalt_off_layer,
    _tabalt_normal_layer,
    _tabalt_ralt_layer,
    _tabalt_layer
);

void keyboard_post_init_user(void) {
    rgblight_layers = _rgb_layers;
}

bool led_update_user(led_t led_state) {
    rgblight_set_layer_state(3, is_tabalt_active);
    return true;
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TOGTALT:
            if (!record->event.pressed) {
                // just set the state by the enum
                rgblight_set_layer_state(TABALT_OFF, 0);
                rgblight_set_layer_state(TABALT_NORMAL, 0);
                rgblight_set_layer_state(TABALT_RALT, 0);
                rgblight_blink_layer(tabalt_mode, 500);
            }
            break;
    }
}

inline void do_normal_tabalt(void) {
    register_code(KC_LALT);
    tap_code(KC_TAB);
    unregister_code(KC_LALT);
}
inline void do_tabalt_step_1(void) {
    register_code(KC_LALT);
    tap_code(KC_RALT);
}
inline void do_tabalt_step_2(void) {
    tap_code(KC_TAB);
    unregister_code(KC_LALT);
}


typedef enum {
    START,
    ALT_DOWN,
    ALTTAB_DOWN,
    POST_ALT,
    WAITING,
    WAIT_RESET,
    TABALT_STEP_1,
    TABALT_STEP_2,
    TABALT_STEP_3,
} fsm_state_t;
// see tabalt-fsm.dot for structure of fsm

fsm_state_t fsm_state = START;

#ifdef DEBUG
const char* fsm_state_str(fsm_state_t fsm_state) {
    switch (fsm_state) {
        case START:
            return "START";
        case ALT_DOWN:
            return "ALT_DOWN";
        case ALTTAB_DOWN:
            return "ALTTAB_DOWN";
        case POST_ALT:
            return "POST_ALT";
        case WAITING:
            return "WAITING";
        case WAIT_RESET:
            return "WAIT_RESET";
        case TABALT_STEP_1:
            return "TABALT_STEP_1";
        case TABALT_STEP_2:
            return "TABALT_STEP_2";
        case TABALT_STEP_3:
            return "TABALT_STEP_3";
        default:
            return "UNKNOWN";
    }
}
#endif


void matrix_scan_user(void) {
    switch (fsm_state) {
        case WAIT_RESET: IF_DEBUG(print("case WAIT_RESET:\n");)
            if (get_mods() == 0) {
                fsm_state = START;
                IF_DEBUG(print("wait complete, fsm_state = START\n");)
            }
            break;

        case POST_ALT: IF_DEBUG(print("case POST_ALT:\n");)
            if (timer_elapsed(alt_tab_timer) > POPUP_DELAY_TIMEOUT) {
                unregister_code(KC_LALT);
                fsm_state = WAIT_RESET;
                alt_tab_timer = 0;
            }
            break;

        case WAITING: IF_DEBUG(print("case WAITING:\n");)
            if (timer_elapsed(alt_tab_timer) > POPUP_DELAY_TIMEOUT) {
                // at this point it becomes a regular alt-tab
                tap_code(KC_TAB);
                fsm_state = WAIT_RESET;
                alt_tab_timer = 0;
            }
            break;

        case TABALT_STEP_1: IF_DEBUG(print("case TABALT_STEP_1:\n");)
            if (tabalt_mode == TABALT_RALT) {
                do_tabalt_step_1();
                alt_tab_timer = timer_read();
                fsm_state = TABALT_STEP_2;
            } else {
                do_normal_tabalt();
                fsm_state = START;
                alt_tab_timer = 0;
            }
            break;

        case TABALT_STEP_2: IF_DEBUG(print("case TABALT_STEP_2:\n");)
            if (timer_elapsed(alt_tab_timer) > TABALT_STEP_DELAY) {
                do_tabalt_step_2();
                fsm_state = START;
                alt_tab_timer = 0;
            }
            break;

        default:
            break;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

IF_DEBUG(
    uprintf("KL: kc: 0x%04X, col: %u, row: %u, pressed: %b, time: %u, fsm_state: %s \n",
        keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time,
            fsm_state_str(fsm_state)
    );
)

    switch (keycode) {
        case TOGTALT:
            if (record->event.pressed) {
                if (is_tabalt_active) {
                    backlight_disable();
                }
            } else {
                if (is_tabalt_active) {
                    backlight_enable();
                }
                // advance to next mode
                switch (tabalt_mode) {
                    case TABALT_OFF:    tabalt_mode = TABALT_NORMAL; break;
                    case TABALT_NORMAL: tabalt_mode = TABALT_RALT;   break;
                    case TABALT_RALT:   tabalt_mode = TABALT_OFF;    break;
                }
                fsm_state = START;
                alt_tab_timer = 0;
            }
            return true;
IF_DEBUG(
        case KC_ENT:
            print("\n\n\n\n\n\n");
            return true;
)
    }

    if (is_tabalt_active) {
        switch (fsm_state) {
            case START:
                if (keycode == TABALT && record->event.pressed && (get_mods() == MOD_BIT(KC_LALT))) {
                    fsm_state = ALTTAB_DOWN;
                } else if (keycode == TABALT && record->event.pressed) {
                    register_code(KC_TAB);
                    fsm_state = WAIT_RESET;
                } else if (keycode == TABALT && !record->event.pressed) {
                    unregister_code(KC_TAB);
                    fsm_state = WAIT_RESET;
                } else if (keycode == KC_LALT && record->event.pressed) {
                    fsm_state = ALT_DOWN;
                } else {
                    fsm_state = WAIT_RESET;
                }
                break;

            case ALT_DOWN:
                if (keycode == KC_LALT && !record->event.pressed) {
                    alt_tab_timer = timer_read();
                    fsm_state = POST_ALT;
                    // don't send an alt up keycode yet
                    return false;
                } else if (keycode == TABALT && record->event.pressed) {
                    fsm_state = ALTTAB_DOWN;
                } else {
                    fsm_state = WAIT_RESET;
                }
                break;

            case POST_ALT:
                if (keycode == TABALT && record->event.pressed) {
                    fsm_state = POST_ALT;
                } else if (keycode == TABALT && !record->event.pressed) {
                    fsm_state = TABALT_STEP_1;
                    // don't send an alt up keycode yet
                    return false;
                } else {
                    // fsm_state = WAIT_RESET;
                }
                break;

            case ALTTAB_DOWN:
                if (keycode == KC_LALT && !record->event.pressed) {
                    alt_tab_timer = timer_read();
                    fsm_state = POST_ALT;
                    // don't send an alt up keycode yet
                    return false;
                } else if (keycode == TABALT && !record->event.pressed) {
                    alt_tab_timer = timer_read();
                    fsm_state = WAITING;
                } else {
                    fsm_state = WAIT_RESET;
                }
                break;

            case WAITING:
                if (keycode == KC_LALT && !record->event.pressed) {
                    fsm_state = TABALT_STEP_1;
                    // don't send an alt up keycode yet
                    return false;
                } else if (keycode == TABALT && record->event.pressed) {
                    tap_code(KC_TAB);
                    register_code(KC_TAB);
                    fsm_state = WAIT_RESET;
                    alt_tab_timer = 0;
                } else {
                    fsm_state = WAIT_RESET;
                    alt_tab_timer = 0;
                }
                break;

            case WAIT_RESET:
                // while waiting for reset, it's a regular tab key
                if (keycode == TABALT) {
                    if (record->event.pressed) {
                        register_code(KC_TAB);
                    } else {
                        unregister_code(KC_TAB);
                    }
                }
                break;

            default:
                break;
        }
        IF_DEBUG(uprintf("%s\n", fsm_state_str(fsm_state));)
    } else {
        // it's a normal tab button
        if (keycode == TABALT) {
            if (record->event.pressed) {
                register_code(KC_TAB);
            } else {
                unregister_code(KC_TAB);
            }
        }
    }
    return true;
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [0] = LAYOUT(
    KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_APP,   KC_INS,   KC_DEL,
    KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_DEL,   KC_BSPC,  KC_HOME,
    TABALT,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,            KC_PGUP,
    KC_LCTL,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,                      KC_ENT,   KC_PGDN,
    KC_LSFT,  MO(1),    KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,            KC_UP,    KC_END,
    KC_LCTL,  KC_LGUI,  KC_LALT,                      KC_LCTRL, KC_SPC,   KC_SPC,                       KC_RALT,  MO(1),    KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT
  ),

  [1] = LAYOUT(
    _______,  _______,  _______,  _______,  _______,  KC_MPRV,  KC_MNXT,  KC_MPLY,  KC_MSTP,  KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,  KC_PSCR,  _______,  _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  RESET  ,  _______,
    _______,  RGB_TOG,  RGB_MOD,  RGB_HUI,  RGB_HUD,  RGB_SAI,  RGB_SAD,  RGB_VAI,  RGB_VAD,  RGB_SPI,  RGB_SPD,  _______,  _______,  _______,            _______,
    _______,  VLK_TOG, RGB_RMOD,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,  _______,
    _______,  _______,  _______,  _______,  BL_DEC,   BL_TOGG,  BL_INC ,   BL_STEP, _______,  _______,  _______,  _______,  KC_APP ,            _______,  _______,
    _______,  _______,  _______,                      TOGTALT,  _______,  _______,                      MO(2)  ,  _______,  MO(2)  ,  _______,  _______,  _______
  ),

#ifndef DEBUG
  [2] = LAYOUT(
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,

    _______,
    RGB_MODE_PLAIN    , // 1
    RGB_MODE_BREATHE  , // 2
    RGB_MODE_RAINBOW  , // 3
    RGB_MODE_SWIRL    , // 4
    RGB_MODE_SNAKE    , // 5
    RGB_MODE_KNIGHT   , // 6
    RGB_MODE_XMAS     , // 7
    RGB_MODE_GRADIENT , // 8
    RGB_MODE_RGBTEST  , // 9
    _______,  _______,  _______,  _______,  _______,  _______,

    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,  _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,
    _______,  _______,  _______,                      _______,  _______,  _______,                      _______,  _______,  _______,  _______,  _______,  _______
  ),
#endif // DEBUG
};
