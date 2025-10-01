RGB_MATRIX_EFFECT(ALL_WHITE)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS
bool ALL_WHITE(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    HSV hsv = rgb_matrix_config.hsv;
    hsv.h = 0;
    hsv.s = 0; // 0 => white
    RGB rgb = hsv_to_rgb(hsv);

    for (uint8_t i = led_min; i < led_max; i++) {
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    return false;
}
#endif