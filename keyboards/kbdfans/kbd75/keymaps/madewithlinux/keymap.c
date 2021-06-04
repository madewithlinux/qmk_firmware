#include QMK_KEYBOARD_H
// make kbdfans/kbd75:madewithlinux:dfu

// We delay showing the popup so that fast Alt+Tab users aren't
// disturbed by the popup briefly flashing.
#define POPUP_DELAY_TIMEOUT 250

enum custom_keycodes {
    TABALT = SAFE_RANGE,
    TOGTALT,
};

bool is_tabalt_active = false;
uint16_t alt_tab_timer = 0;
bool alt_tab_waiting = false;
bool in_alt_tab_menu = false;
/*
Alt-Tab logic:

do_tabalt() {
    left alt down
    tap right alt
    tap tab
    left alt up

    alt_tab_waiting = false;
    in_alt_tab_menu = false;
}

alt_tab_waiting && alt released => do_tabalt()


alt_tab_waiting && alt_tab_timer expires => {
    // at this point it becomes a regular alt-tab
    tap tab
    alt_tab_waiting = false;
    in_alt_tab_menu = true;
}


tab:
in_alt_tab_menu? {
    yes => it's just a regular tab
}
pressed or released? {
    pressed =>
        alt_tab_waiting? {
            yes =>
                tap tab
                register tab down
                alt_tab_waiting = false;
                in_alt_tab_menu = true;
            no  =>
                alt_tab_waiting = true;
                alt_tab_timer = timer_read();
        }
    released =>
        alt_tab_waiting? {
            yes =>
                do nothing, I guess?
            no  =>
                TODO: I think this is invalid state?
        }
}

normal alt tab fallback logic:
    alt_tab_waiting   => custom
    in_alt_tab_menu   => normal
    !is_tabalt_active => normal
    alt isn't down    => normal

*/

const rgblight_segment_t PROGMEM _yes_layer[] = RGBLIGHT_LAYER_SEGMENTS( {9, 6, HSV_GREEN} );
const rgblight_segment_t PROGMEM _no_layer[] = RGBLIGHT_LAYER_SEGMENTS( {9, 6, HSV_RED} );
const rgblight_segment_t PROGMEM _tabalt_layer[] = RGBLIGHT_LAYER_SEGMENTS( {0, 2, HSV_WHITE} );

const rgblight_segment_t* const PROGMEM _rgb_layers[] =
    RGBLIGHT_LAYERS_LIST( _yes_layer, _no_layer, _tabalt_layer );

void keyboard_post_init_user(void) {
    rgblight_layers = _rgb_layers;
}

bool led_update_user(led_t led_state) {
    rgblight_set_layer_state(2, is_tabalt_active);
    return true;
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TOGTALT:
            if (!record->event.pressed) {
                rgblight_blink_layer(is_tabalt_active ? 0 : 1, 500);
                // make sure the other layer is off
                rgblight_set_layer_state(is_tabalt_active ? 1 : 0, 0);
            }
            break;
    }
}

inline void do_tabalt(void) {
    register_code(KC_LALT);
    tap_code(KC_RALT);
    tap_code(KC_TAB);
    unregister_code(KC_LALT);

    alt_tab_waiting = false;
    in_alt_tab_menu = false;
}

void matrix_scan_user(void) {
  if (alt_tab_waiting) {
    if (timer_elapsed(alt_tab_timer) > POPUP_DELAY_TIMEOUT) {
        // at this point it becomes a regular alt-tab
        tap_code(KC_TAB);
        alt_tab_waiting = false;
        in_alt_tab_menu = true;
    }
  }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
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
            is_tabalt_active = !is_tabalt_active;
        }
        break;

    case TABALT:
        if (!alt_tab_waiting && (!is_tabalt_active || in_alt_tab_menu || (get_mods() != MOD_BIT(KC_LALT)))) {
            // normal tab functionalty
            if (record->event.pressed) {
                register_code(KC_TAB);
            } else {
                unregister_code(KC_TAB);
            }
        } else {
            // special tabalt functionality
            if (record->event.pressed) {
                if (alt_tab_waiting) {
                    tap_code(KC_TAB);
                    register_code(KC_TAB);
                    alt_tab_waiting = false;
                    in_alt_tab_menu = true;
                } else {
                    alt_tab_waiting = true;
                    alt_tab_timer = timer_read();
                }
            } else {
                // TODO: unknown what to do here (if anything)
            }
        }
        break;
    case KC_LALT:
        if (!record->event.pressed) { // on release
            if (alt_tab_waiting) {
                do_tabalt();
            }
            // just reset all the state
            alt_tab_timer = 0;
            alt_tab_waiting = false;
            in_alt_tab_menu = false;
        }
        break;

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
};
