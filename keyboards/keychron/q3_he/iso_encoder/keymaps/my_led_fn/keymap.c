/* Copyright 2024 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "keychron_common.h"

// Parentheses (same on both OS)
#define KC_PAR_L S(KC_8) // (
#define KC_PAR_R S(KC_9) // )

// macOS (German - Apple layout)
#define KC_M_SL LALT(KC_5) // [
#define KC_M_SR LALT(KC_6) // ]
#define KC_M_CL LALT(KC_8) // {
#define KC_M_CR LALT(KC_9) // }
#define KC_M_PI LALT(KC_7) // |

// Windows (German layout):
#define KC_W_SL RALT(KC_8)    // [
#define KC_W_SR RALT(KC_9)    // ]
#define KC_W_CL RALT(KC_7)    // {
#define KC_W_CR RALT(KC_0)    // }
#define KC_W_PI RALT(KC_NUBS) // |

// English comments: make Home/End behave as macOS/iPadOS shortcuts
#define KC_LHOME LGUI(KC_LEFT) // ⌘ + ←
#define KC_LEND LGUI(KC_RIGHT) // ⌘ + →

enum custom_keycodes { KC_MMUTE = SAFE_RANGE };

enum layers {
    MAC_BASE,
    MAC_FN,
    WIN_BASE,
    WIN_FN,
};

#ifdef RGB_MATRIX_ENABLE
#    include "rgb_matrix.h"

// LED index cache for target keys (DE-ISO positions)
static uint8_t led_idx_MINS = NO_LED; // ß
static uint8_t led_idx_EQL  = NO_LED; // ´
static uint8_t led_idx_LBRC = NO_LED; // Ü
static uint8_t led_idx_RBRC = NO_LED; // +
static uint8_t led_idx_QUOT = NO_LED; // Ä
static uint8_t led_idx_NUHS = NO_LED; // #
static uint8_t led_idx_SLSH = NO_LED; // -
static uint8_t led_idx_CAPS = NO_LED; // Caps Lock

// Map helper: record first-found LED index for a given keycode
static inline void map_if_match(uint16_t kc, uint8_t led) {
    switch (kc) {
        case KC_MINS:
            if (led_idx_MINS == NO_LED) led_idx_MINS = led;
            break;
        case KC_EQL:
            if (led_idx_EQL == NO_LED) led_idx_EQL = led;
            break;
        case KC_LBRC:
            if (led_idx_LBRC == NO_LED) led_idx_LBRC = led;
            break;
        case KC_RBRC:
            if (led_idx_RBRC == NO_LED) led_idx_RBRC = led;
            break;
        case KC_QUOT:
            if (led_idx_QUOT == NO_LED) led_idx_QUOT = led;
            break;
        case KC_NUHS:
            if (led_idx_NUHS == NO_LED) led_idx_NUHS = led;
            break;
        case KC_SLSH:
            if (led_idx_SLSH == NO_LED) led_idx_SLSH = led;
            break;
        case KC_CAPS:
            if (led_idx_CAPS == NO_LED) led_idx_CAPS = led;
            break;
    }
}

void keyboard_post_init_user(void) {
    // Scan both base layers to be layout-agnostic
    static const uint8_t layers_to_scan[] = { WIN_BASE, MAC_BASE };

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            const uint8_t led = g_led_config.matrix_co[row][col];
            if (led == NO_LED) continue;

            const keypos_t kp = (keypos_t){ .row = row, .col = col };
            for (size_t i = 0; i < ARRAY_SIZE(layers_to_scan); i++) {
                const uint8_t layer = layers_to_scan[i];
                const uint16_t kc   = keymap_key_to_keycode(layer, kp);
                map_if_match(kc, led);
            }
        }
    }
}

// Helper: restore a key's base color for known static modes.
static inline void set_base_color(uint8_t led_idx, uint8_t mode) {
    if (led_idx == NO_LED) return;
    switch (mode) {
        case RGB_MATRIX_CUSTOM_ALL_BLACK:
            rgb_matrix_set_color(led_idx, 0, 0, 0);
            break;
        case RGB_MATRIX_CUSTOM_ALL_WHITE:
            rgb_matrix_set_color(led_idx, 255, 255, 255);
            break;
        default:
            // Dynamic effects: don't restore here; they'll repaint next frame.
            break;
    }
}

// Helper: overlay color depending on current mode.
static inline void get_overlay_color(uint8_t mode, uint8_t *r, uint8_t *g, uint8_t *b) {
    if (mode == RGB_MATRIX_CUSTOM_ALL_WHITE) {
        *r = 255;
        *g = 0;
        *b = 0; // red in ALL_WHITE
    } else {
        *r = 255;
        *g = 255;
        *b = 255; // white in all other modes
    }
}

// Apply Caps Lock and FN overlay indicators with mode-dependent colors
bool rgb_matrix_indicators_user(void) {
    uint8_t mode      = rgb_matrix_get_mode();
    led_t   host_leds = host_keyboard_led_state();

    // FN is considered active when either of these layers is on.
    bool fn_active = layer_state_is(MAC_FN) || layer_state_is(WIN_FN);

    // Compute overlay color once (red in ALL_WHITE, white otherwise).
    uint8_t or_, og, ob;
    get_overlay_color(mode, &or_, &og, &ob);

    // FN overlay
    const uint8_t fn_keys[] = {led_idx_MINS, led_idx_EQL, led_idx_LBRC, led_idx_RBRC, led_idx_QUOT, led_idx_NUHS, led_idx_SLSH};

    for (size_t i = 0; i < ARRAY_SIZE(fn_keys); i++) {
        const uint8_t idx = fn_keys[i];
        if (idx == NO_LED) continue;

        if (fn_active) {
            rgb_matrix_set_color(idx, or_, og, ob);
        } else {
            set_base_color(idx, mode); // Only restores for known static modes.
        }
    }

    // Caps Lock indicator
    if (led_idx_CAPS != NO_LED) {
        if (host_leds.caps_lock) {
            // Caps ON → overlay color (red in ALL_WHITE, white otherwise)
            rgb_matrix_set_color(led_idx_CAPS, or_, og, ob);
        } else {
            // Caps OFF → restore base color for static modes
            set_base_color(led_idx_CAPS, mode);
        }
    }

    return true;
}

#endif // RGB_MATRIX_ENABLE

#define FN_MAC MO(MAC_FN)
#define FN_WIN MO(WIN_FN)

// Keymap (unchanged behavior, DE-ISO positions)
// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_iso_88(
        KC_ESC,   KC_BRID,  KC_BRIU,  KC_MCTRL, KC_LNPAD, RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_MUTE,   KC_SNAP,  KC_SIRI,  RGB_MOD,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,   KC_INS,   KC_LHOME,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,             KC_DEL,   KC_LEND,   KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,
        KC_LSFT,  KC_NUBS,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,             KC_UP,
        KC_LCTL,  KC_LOPTN, KC_LCMMD,                               KC_SPC,                                 KC_RCMMD, KC_ROPTN, FN_MAC,   KC_RCTL,   KC_LEFT,  KC_DOWN,  KC_RGHT),

    [MAC_FN] = LAYOUT_iso_88(
        _______,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   RGB_TOG,   _______,  _______,  RGB_TOG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  KC_PAR_L, KC_PAR_R, _______,   _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  KC_M_SL,  KC_M_SR,             _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  KC_M_CL,  KC_M_CR,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  KC_M_PI,            _______,             _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,   _______,  _______,  _______),

    [WIN_BASE] = LAYOUT_iso_88( 
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_MUTE,   KC_PSCR,  KC_MMUTE, RGB_MOD,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,   KC_INS,   KC_HOME,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,             KC_DEL,   KC_END,   KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,
        KC_LSFT,  KC_NUBS,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,             KC_UP,
        KC_LCTL,  KC_LWIN,  KC_LALT,                                KC_SPC,                                 KC_RALT,  KC_RWIN,  FN_WIN,   KC_RCTL,   KC_LEFT,  KC_DOWN,  KC_RGHT),

    [WIN_FN] = LAYOUT_iso_88(
        _______,  KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  RGB_TOG,   _______,  _______,  RGB_TOG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  KC_PAR_L, KC_PAR_R, _______,   _______,  KC_LHOME, _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  KC_W_SL,  KC_W_SR,             _______,  KC_LEND,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  KC_W_CL,  KC_W_CR,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  KC_W_PI,            _______,             _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,   _______,  _______,  _______),
};
// clang-format on

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [MAC_BASE] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [MAC_FN]   = {ENCODER_CCW_CW(RGB_VAD, RGB_VAI)},
    [WIN_BASE] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [WIN_FN]   = {ENCODER_CCW_CW(RGB_VAD, RGB_VAI)},
};
#endif // ENCODER_MAP_ENABLE

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    /* Keychron common handling */
    if (!process_record_keychron_common(keycode, record)) {
        return false;
    }

    if (!record->event.pressed) {
        return true;
    }

    switch (keycode) {
        case KC_MMUTE:
            // Toggle mic mute (Win+Alt+K)
            tap_code16(LGUI(LALT(KC_K)));
            return false;
    }

    return true;
}