RGB_MATRIX_EFFECT(ALL_WHITE)
RGB_MATRIX_EFFECT(ALL_BLACK)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS
bool ALL_WHITE(effect_params_t *params) {
    if (params->init) {
        rgb_matrix_set_color_all(255, 255, 255);
    }
    return false;
}

bool ALL_BLACK(effect_params_t *params) {
    if (params->init) {
        rgb_matrix_set_color_all(0, 0, 0);
    }
    return false;
}
#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS