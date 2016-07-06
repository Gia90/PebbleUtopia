#pragma once
#include <pebble.h>

Window *s_main_window;
GColor s_background_color, s_text_color;
TextLayer *s_time_layer, *s_date_layer, *s_text_layer;

BitmapLayer *s_battery_layer;
GBitmap *s_battery_bitmap;
int16_t battery_layer_Yoffset;

GFont s_time_font, s_date_font, s_text_font;
GColor s_time_font_color, s_date_font_color;

// CONFIGURATION VARS (PRIVATE)
typedef enum {UT1, UT2, CSTM} color_preset_t;
color_preset_t config_color_preset;

int config_bg_custom_color;
int config_text_custom_color;

bool config_show_text;
#define CUSTOM_TEXT_MAX_LENGTH 31
#define CUSTOM_TEXT_DEFAULT "Where is\nJESSICA HYDE?"
char config_custom_text[CUSTOM_TEXT_MAX_LENGTH];


/******************* FEATURES FUNCTIONS **********************/
/** TIME & DATE **/
extern void update_time();
extern void tick_handler(struct tm *tick_time, TimeUnits units_changed);
/** END TIME & DATE **/

/** BATTERY **/
extern void update_battery(int battery_level);
extern void battery_handler(BatteryChargeState state);
/** END BATTERY **/

/**************** CONFIGURATION FUNCTIONS ********************/
// Handle new config inbox message received
extern void inbox_received_handler(DictionaryIterator *iterator, void *context);

// Load existing config from persistent storage
extern void load_config();
// Update background color
extern void apply_config_colors(bool update_ui);
// Update text
extern void apply_config_text(bool update_ui);