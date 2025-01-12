#include "rgb_mixer.h"
#include "../../LVGL/lvgl.h"


typedef enum {
    RGB_MIXER_SLIDER_R = 0,
    RGB_MIXER_SLIDER_G,
    RGB_MIXER_SLIDER_B
}SliderType_e;


typedef struct {
    SliderType_e slider_type;
    lv_obj_t* pLabel;
}rgb_mixer_t;

// Slider call back function
void slider_callback(lv_event_t* e);

// Rectangle object as global, because we want to access it from event call back
lv_obj_t* pRect;

void rgb_mixer_create_ui(void) {

    static rgb_mixer_t Red, Green, Blue;

    // Retrieving the active screen
    lv_obj_t* act_scr = lv_scr_act();

    // Creating three slider objects of red, green, blue color
    lv_obj_t* pSlider_r = lv_slider_create(act_scr);
    lv_obj_t* pSlider_g = lv_slider_create(act_scr);
    lv_obj_t* pSlider_b = lv_slider_create(act_scr);

    // Set value range for slider
    lv_slider_set_range(pSlider_r, 0, 255);
    lv_slider_set_range(pSlider_g, 0, 255);
    lv_slider_set_range(pSlider_b, 0, 255);
  

    // Setting size of the sliders
    lv_obj_set_size(pSlider_r, 200, 10);
    lv_obj_set_size(pSlider_g, 200, 10);
    lv_obj_set_size(pSlider_b, 200, 10);

    // Aligning the sliders
    lv_obj_align(pSlider_r, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_align_to(pSlider_g, pSlider_r, LV_ALIGN_OUT_BOTTOM_MID, 0, 50);
    lv_obj_align_to(pSlider_b, pSlider_g, LV_ALIGN_OUT_BOTTOM_MID, 0, 50);

    // Creating pRectangle, setting size and aligning it with respect to pSlider_b
    pRect = lv_obj_create(act_scr);
    lv_obj_set_size(pRect, 200, 60);
    lv_obj_align_to(pRect, pSlider_b, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);

    // Setting border color of the pRectangle and border width
    lv_obj_set_style_border_color(pRect, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_width(pRect, 5,LV_PART_MAIN);

    // Setting pSlider_r's indicator and knob part to red color
    lv_obj_set_style_bg_color(pSlider_r, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(pSlider_r, lv_palette_main(LV_PALETTE_RED), LV_PART_KNOB);

    // Setting pSlider_g's indicator and knob part to green color
    lv_obj_set_style_bg_color(pSlider_g, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(pSlider_g, lv_palette_main(LV_PALETTE_GREEN), LV_PART_KNOB);

    // Setting pSlider_b's indicator and knob part to blue color
    lv_obj_set_style_bg_color(pSlider_b, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(pSlider_b, lv_palette_main(LV_PALETTE_BLUE), LV_PART_KNOB);

    // Creating heading of the RGB mixer ui
    lv_obj_t* pHeading = lv_label_create(act_scr);
    lv_label_set_text(pHeading, "RGB Mixer");
    lv_obj_align(pHeading, LV_ALIGN_TOP_MID, 0, 5);

    // Creating label for RED slider
    Red.pLabel = lv_label_create(act_scr);
    Red.slider_type = RGB_MIXER_SLIDER_R;
    lv_label_set_text(Red.pLabel, "0");
    lv_obj_align_to(Red.pLabel, pSlider_r, LV_ALIGN_OUT_TOP_MID, 0, 0);

    // Creating label for GREEN slider
    Green.pLabel = lv_label_create(act_scr);
    Green.slider_type = RGB_MIXER_SLIDER_G;
    lv_label_set_text(Green.pLabel, "0");
    lv_obj_align_to(Green.pLabel, pSlider_g, LV_ALIGN_OUT_TOP_MID, 0, 0);

    // Creating label for BLUE slider
    Blue.pLabel = lv_label_create(act_scr);
    Blue.slider_type = RGB_MIXER_SLIDER_B;
    lv_label_set_text(Blue.pLabel, "0");
    lv_obj_align_to(Blue.pLabel, pSlider_b, LV_ALIGN_OUT_TOP_MID, 0, 0);

    // Adding slider event call back
    lv_obj_add_event_cb(pSlider_r, slider_callback, LV_EVENT_VALUE_CHANGED, &Red);
    lv_obj_add_event_cb(pSlider_g, slider_callback, LV_EVENT_VALUE_CHANGED, &Green);
    lv_obj_add_event_cb(pSlider_b, slider_callback, LV_EVENT_VALUE_CHANGED, &Blue);

    // Initial value of the rectangle color bar
    lv_obj_set_style_bg_color(pRect, lv_color_make(0, 0, 0), LV_PART_MAIN);
}

// Slider call back function
void slider_callback(lv_event_t* e) {

    // Color component
    static uint8_t r, g, b;

    lv_obj_t* pSlider = lv_event_get_target(e);
    rgb_mixer_t* pUserData = lv_event_get_user_data(e);
    int32_t value = lv_slider_get_value(pSlider);
    lv_label_set_text_fmt(pUserData->pLabel, "%ld", value);

    if (pUserData->slider_type == RGB_MIXER_SLIDER_R) {
        r = value;
    }
    else if (pUserData->slider_type == RGB_MIXER_SLIDER_G) {
        g = value;
    }
    else if (pUserData->slider_type == RGB_MIXER_SLIDER_B) {
        b = value;
    }

    lv_obj_set_style_bg_color(pRect, lv_color_make(r, g, b), LV_PART_MAIN);
}
