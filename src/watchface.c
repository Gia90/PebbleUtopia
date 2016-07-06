#include <pebble.h>
#include "watchface.h"

Window *s_main_window;
GColor s_background_color, s_text_color;
TextLayer *s_time_layer, *s_date_layer, *s_text_layer;

BitmapLayer *s_battery_layer;
GBitmap *s_battery_bitmap;
int16_t battery_layer_Yoffset = 0;

GFont s_time_font, s_date_font, s_text_font;
GColor s_time_font_color, s_date_font_color;

// CONFIGURATION VARS
color_preset_t config_color_preset = UT1;
int config_bg_custom_color;
int config_text_custom_color;

bool config_show_text = false;
char config_custom_text[CUSTOM_TEXT_MAX_LENGTH] = CUSTOM_TEXT_DEFAULT;


/***************************** FEATURES FUNCTIONS **********************************/
/** TIME & DATE **/
void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  
  // Write the current date into a buffer
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%a %d %b", tick_time);
  // Show the date
  text_layer_set_text(s_date_layer, date_buffer);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
/** END TIME & DATE **/

/** BATTERY **/
void update_battery(int battery_level) {
  // Load the picture corresponding to the current battery level
  
  // Destroy GBitmap
  gbitmap_destroy(s_battery_bitmap);
  
  uint32_t battery_image_res_id = RESOURCE_ID_IMAGE_BATTERY_HIGHEST;
  
  switch(battery_level/10)
  {
    case 0:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "0-1");
            battery_image_res_id = RESOURCE_ID_IMAGE_BATTERY_LOWEST;
            break;
    case 1:
    case 2:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "2-3");
            battery_image_res_id = RESOURCE_ID_IMAGE_BATTERY_LOW;
            break;
    case 3:
    case 4:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "4-5");
            battery_image_res_id = RESOURCE_ID_IMAGE_BATTERY_MEDIUM;
            break;    
    case 5:
    case 6:
    case 7:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "6-7");
            battery_image_res_id = RESOURCE_ID_IMAGE_BATTERY_HIGH;
            break;
    case 8:
    case 9:
    case 10:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "8-10");
            battery_image_res_id = RESOURCE_ID_IMAGE_BATTERY_HIGHEST;
            break;
  }
  s_battery_bitmap = gbitmap_create_with_resource(battery_image_res_id);
  
  // Display battery image
  bitmap_layer_set_bitmap(s_battery_layer, s_battery_bitmap);
  bitmap_layer_set_compositing_mode(s_battery_layer, GCompOpSet);
}

void battery_handler(BatteryChargeState state) {
  // Record the new battery level
  update_battery(state.charge_percent);
}
/** END BATTERY **/


/**************************** CONFIG FUNCTIONS *********************************/
// Handle new config inbox message received
void inbox_received_handler(DictionaryIterator *iterator, void *context)
{
  Tuple *t=NULL;

  APP_LOG(APP_LOG_LEVEL_DEBUG, "inbox_received_handler() called");
  
  // Config received for background color presets
  t = dict_find(iterator, MESSAGE_KEY_COLOR_PRESET);

  if (t) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "got MESSAGE_KEY_COLOR_PRESET");
    config_color_preset = atoi(t->value->cstring);
    APP_LOG(APP_LOG_LEVEL_INFO, "Persisting config_color_preset: %d", (int) config_color_preset);
    persist_write_int(MESSAGE_KEY_COLOR_PRESET, config_color_preset);
    
    // If custom color
    if(config_color_preset == 2)
    {
      t = dict_find(iterator, MESSAGE_KEY_BG_CUSTOM_COLOR);
      if (t) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "got MESSAGE_KEY_BG_CUSTOM_COLOR");
        config_bg_custom_color = (int)t->value->int32;
        APP_LOG(APP_LOG_LEVEL_INFO, "Persisting config_bg_custom_color: %d", (int) config_bg_custom_color);
        persist_write_int(MESSAGE_KEY_BG_CUSTOM_COLOR, config_bg_custom_color);
      }
      
      t = dict_find(iterator, MESSAGE_KEY_TEXT_CUSTOM_COLOR);
      if (t) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "got MESSAGE_KEY_TEXT_CUSTOM_COLOR");
        config_text_custom_color = (int)t->value->int32;
        APP_LOG(APP_LOG_LEVEL_INFO, "Persisting config_text_custom_color: %d", (int) config_text_custom_color);
        persist_write_int(MESSAGE_KEY_TEXT_CUSTOM_COLOR, config_text_custom_color);
      }
    }
  
    // Reload Watchface!
    apply_config_colors(true);
  }
  
  // Config received for show text
  t = dict_find(iterator, MESSAGE_KEY_SHOW_TEXT);

  if (t) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "got MESSAGE_KEY_SHOW_TEXT");
    config_show_text = (bool)t->value->int16;
    APP_LOG(APP_LOG_LEVEL_INFO, "Persisting config_show_text: %d", (int) config_show_text);
    persist_write_bool(MESSAGE_KEY_SHOW_TEXT, config_show_text);
    
    if(config_show_text)
    {
      t = dict_find(iterator, MESSAGE_KEY_CUSTOM_TEXT);
    
      if (t) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "got MESSAGE_KEY_CUSTOM_TEXT");
        if( (t->value->cstring)[0] != '\0' )
        {
          strncpy(config_custom_text, t->value->cstring, CUSTOM_TEXT_MAX_LENGTH);
        }
        else
        {
          strncpy(config_custom_text, CUSTOM_TEXT_DEFAULT, CUSTOM_TEXT_MAX_LENGTH);
          APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting MESSAGE_KEY_CUSTOM_TEXT to default value since it is blank.");
        }
        APP_LOG(APP_LOG_LEVEL_INFO, "Persisting config_custom_text: %s", config_custom_text);
        persist_write_string(MESSAGE_KEY_CUSTOM_TEXT, config_custom_text);
      }
    }
    
    // Refresh
    apply_config_text(true);
  }
}

// Load existing config from persistent storage
void load_config() {
  if (persist_exists(MESSAGE_KEY_COLOR_PRESET)) {
    config_color_preset = persist_read_int(MESSAGE_KEY_COLOR_PRESET);
  }
  if (persist_exists(MESSAGE_KEY_BG_CUSTOM_COLOR)) {
    config_bg_custom_color = persist_read_int(MESSAGE_KEY_BG_CUSTOM_COLOR);
  }
  if (persist_exists(MESSAGE_KEY_TEXT_CUSTOM_COLOR)) {
    config_text_custom_color = persist_read_int(MESSAGE_KEY_TEXT_CUSTOM_COLOR);
  }
  if (persist_exists(MESSAGE_KEY_SHOW_TEXT)) {
    config_show_text = persist_read_bool(MESSAGE_KEY_SHOW_TEXT);
  }
  if (persist_exists(MESSAGE_KEY_CUSTOM_TEXT)) {
    persist_read_string(MESSAGE_KEY_CUSTOM_TEXT, config_custom_text, CUSTOM_TEXT_MAX_LENGTH);
  }
}

// Update background color and,  optionally, update the ui
void apply_config_colors(bool update_ui) {
  
  switch(config_color_preset)
  {
    case UT1:
              s_background_color = GColorYellow;
              s_text_color = GColorBrass;
              break;
    case UT2:
              s_background_color = GColorBrightGreen;
              s_text_color = GColorDarkGreen;
              break;
    case CSTM:
              s_background_color = GColorFromHEX(config_bg_custom_color);
              s_text_color = GColorFromHEX(config_text_custom_color);
              break;
  }
  if(update_ui)
  {
    window_set_background_color(s_main_window, s_background_color);
    text_layer_set_text_color(s_text_layer, s_text_color);
  }
}

void apply_config_text(bool update_ui) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_bounds(window_layer);
  
  if(config_show_text)
  {
    // Dynamic positioning offset for battery layer
    battery_layer_Yoffset = 0;
  }
  else
  {
    // Dynamic positioning offset for battery layer (move it down)
    battery_layer_Yoffset = 15;
  }
  
  if(update_ui)
  {
    // Add/remove text layer to the Window's root layer
    if(config_show_text)
    {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "layer_add_child CUSTOM_TEXT");
      text_layer_set_text(s_text_layer, config_custom_text);
      
      layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
    }
    else
    {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "layer_remove_from_parent CUSTOM_TEXT");
      layer_remove_from_parent(text_layer_get_layer(s_text_layer));
    }
    
    // Move battery layer 
    layer_set_frame(bitmap_layer_get_layer(s_battery_layer), GRect(0, (30 + battery_layer_Yoffset), bounds.size.w, 150));
  }
}