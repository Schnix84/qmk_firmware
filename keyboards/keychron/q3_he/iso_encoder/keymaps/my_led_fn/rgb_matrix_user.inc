RGB_MATRIX_EFFECT(ALL_WHITE)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS
bool ALL_WHITE(effect_params_t *params) {
    RGB rgb = (RGB){255, 255, 255};
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return false;
}
#endif /* RGB_MATRIX_CUSTOM_EFFECT_IMPLS */