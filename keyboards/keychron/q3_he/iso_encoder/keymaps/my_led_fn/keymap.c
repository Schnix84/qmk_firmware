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

enum custom_keycodes {
    MIC_MUTE_TGL = SAFE_RANGE
};

enum layers {
    MAC_BASE,
    MAC_FN,
    WIN_BASE,
    WIN_FN,
};

#ifdef RGB_MATRIX_ENABLE
#    include "rgb_matrix.h"

/* LED index cache for target keys (DE-ISO positions) */
static uint8_t led_idx_MINS = NO_LED; // ß
static uint8_t led_idx_EQL  = NO_LED; // ´
static uint8_t led_idx_LBRC = NO_LED; // Ü
static uint8_t led_idx_RBRC = NO_LED; // +
static uint8_t led_idx_QUOT = NO_LED; // Ä
static uint8_t led_idx_NUHS = NO_LED; // #
static uint8_t led_idx_SLSH = NO_LED; // -
static uint8_t led_idx_CAPS = NO_LED; // Caps Lock

/* Map helper: record first-found LED index for a given keycode */
static inline void map_if_match(uint16_t kc, uint8_t led) {
    switch (kc) {
        case KC_MINS: if (led_idx_MINS == NO_LED) led_idx_MINS = led; break;
        case KC_EQL:  if (led_idx_EQL  == NO_LED) led_idx_EQL  = led; break;
        case KC_LBRC: if (led_idx_LBRC == NO_LED) led_idx_LBRC = led; break;
        case KC_RBRC: if (led_idx_RBRC == NO_LED) led_idx_RBRC = led; break;
        case KC_QUOT: if (led_idx_QUOT == NO_LED) led_idx_QUOT = led; break;
        case KC_NUHS: if (led_idx_NUHS == NO_LED) led_idx_NUHS = led; break;
        case KC_SLSH: if (led_idx_SLSH == NO_LED) led_idx_SLSH = led; break;
        case KC_CAPS: if (led_idx_CAPS == NO_LED) led_idx_CAPS = led; break;
    }
}

void keyboard_post_init_user(void) {
    /* Scan both base layers to be layout-agnostic */
    const uint8_t layers_to_scan[] = { WIN_BASE, MAC_BASE };

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t led = g_led_config.matrix_co[row][col];
            if (led == NO_LED) continue;

            keypos_t kp = (keypos_t){ .row = row, .col = col };
            for (uint8_t i = 0; i < sizeof(layers_to_scan); i++) {
                uint16_t kc = keymap_key_to_keycode(layers_to_scan[i], kp);
                map_if_match(kc, led);
            }
        }
    }
}

/* Overlay indicators: add highlights without clearing the base effect */
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    /* Highlight bracket/pipe keys when any FN layer is held */
    if (layer_state_is(MAC_FN) || layer_state_is(WIN_FN)) {
        if (led_idx_MINS != NO_LED && led_idx_MINS >= led_min && led_idx_MINS < led_max) rgb_matrix_set_color(led_idx_MINS, 255, 0, 0);
        if (led_idx_EQL  != NO_LED && led_idx_EQL  >= led_min && led_idx_EQL  < led_max) rgb_matrix_set_color(led_idx_EQL,  255, 0, 0);
        if (led_idx_LBRC != NO_LED && led_idx_LBRC >= led_min && led_idx_LBRC < led_max) rgb_matrix_set_color(led_idx_LBRC, 255, 0, 0);
        if (led_idx_RBRC != NO_LED && led_idx_RBRC >= led_min && led_idx_RBRC < led_max) rgb_matrix_set_color(led_idx_RBRC, 255, 0, 0);
        if (led_idx_QUOT != NO_LED && led_idx_QUOT >= led_min && led_idx_QUOT < led_max) rgb_matrix_set_color(led_idx_QUOT, 255, 0, 0);
        if (led_idx_NUHS != NO_LED && led_idx_NUHS >= led_min && led_idx_NUHS < led_max) rgb_matrix_set_color(led_idx_NUHS, 255, 0, 0);
        if (led_idx_SLSH != NO_LED && led_idx_SLSH >= led_min && led_idx_SLSH < led_max) rgb_matrix_set_color(led_idx_SLSH, 255, 0, 0);
    }

    /* Caps Lock on = red, independent of layer/effect */
    if (host_keyboard_led_state().caps_lock && led_idx_CAPS != NO_LED) {
        rgb_matrix_set_color(led_idx_CAPS, 255, 0, 0);
    }
    return true;
}
#endif /* RGB_MATRIX_ENABLE */

#define FN_MAC MO(MAC_FN)
#define FN_WIN MO(WIN_FN)

/* Keymap (unchanged behavior, DE-ISO positions) */
// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
 [MAC_BASE] = LAYOUT_iso_88(
        KC_ESC,   KC_BRID,  KC_BRIU,  KC_MCTRL, KC_LNPAD, RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_MUTE,   KC_SNAP,  KC_SIRI,  RGB_MOD,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,   KC_INS,   KC_HOME,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,             KC_DEL,   KC_END,   KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,
        KC_LSFT,  KC_NUBS,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,             KC_UP,
        KC_LCTL,  KC_LOPTN, KC_LCMMD,                               KC_SPC,                                 KC_RCMMD, KC_ROPTN, FN_MAC,   KC_RCTL,   KC_LEFT,  KC_DOWN,  KC_RGHT),

    [MAC_FN] = LAYOUT_iso_88(
        _______,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   RGB_TOG,   _______,  _______,  RGB_TOG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,             _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,            _______,             _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,   _______,  _______,  _______),

    [WIN_BASE] = LAYOUT_iso_88(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_MUTE,   KC_PSCR,  MIC_MUTE_TGL, RGB_MOD,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,   KC_INS,   KC_HOME,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,             KC_DEL,   KC_END,   KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,
        KC_LSFT,  KC_NUBS,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,             KC_UP,
        KC_LCTL,  KC_LWIN,  KC_LALT,                                KC_SPC,                                 KC_RALT,  KC_RWIN,  FN_WIN,   KC_RCTL,   KC_LEFT,  KC_DOWN,  KC_RGHT),

    [WIN_FN] = LAYOUT_iso_88(
        _______,  KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  RGB_TOG,   _______,  _______,  RGB_TOG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,             _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,            _______,             _______,
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

    /* Mic mute/unmute: Win+Alt+K */
    if (keycode == MIC_MUTE_TGL) {
        register_mods(MOD_LGUI | MOD_LALT);
        tap_code(KC_K);
        unregister_mods(MOD_LGUI | MOD_LALT);
        return false;
    }

    /* Only transform when FN layer is active */
    if (!layer_state_is(MAC_FN) && !layer_state_is(WIN_FN)) {
        return true;
    }

    /* Fn + ß/´/Ü/+ /Ä/#/- → ( ) { } [ ] |   (DE-ISO positions) */
    switch (keycode) {
        case KC_MINS: tap_code16(S(KC_8));          return false; // ß → (
        case KC_EQL:  tap_code16(S(KC_9));          return false; // ´ → )
        case KC_LBRC: tap_code16(RALT(KC_7));       return false; // Ü → {
        case KC_RBRC: tap_code16(RALT(KC_0));       return false; // + → }
        case KC_QUOT: tap_code16(RALT(KC_8));       return false; // Ä → [
        case KC_NUHS: tap_code16(RALT(KC_9));       return false; // # → ]
        case KC_SLSH: tap_code16(RALT(KC_NUBS));    return false; // - → |
    }

    return true;
}
