#include <pebble.h>
#include "watchface.h"

static void main_window_load(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_load!");
  
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Apply color config without refreshing the ui (which has not been created yet)
  apply_config_colors(false);
  
  // Set background color
  window_set_background_color(s_main_window, s_background_color);
  
  // Create custom GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_UNIVERSALIS_ADF_BOLD_52));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_UNIVERSALIS_ADF_REGULAR_18));
  s_text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_UNIVERSALIS_ADF_BOLD_18));
  
  // TIME
  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(GRect(0, 0, bounds.size.w, 100));
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, s_time_font_color);
  text_layer_set_text(s_time_layer, "UTOPIA");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  // DATE
  // Create date TextLayer
  s_date_layer = text_layer_create(GRect(0, 55, bounds.size.w, 100));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, s_date_font_color);
  text_layer_set_text(s_date_layer, "Jessica Hyde");
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  // Apply to TextLayer
  text_layer_set_font(s_date_layer, s_date_font);
  // Add to Window
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  
  // BATTERY
  // Create battery BitmapLayer
  s_battery_layer = bitmap_layer_create(GRect(0, (30 + battery_layer_Yoffset), bounds.size.w, 150));
  bitmap_layer_set_background_color(s_battery_layer, GColorClear);
  
  s_battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_HIGH);
  
  bitmap_layer_set_bitmap(s_battery_layer, s_battery_bitmap);
  bitmap_layer_set_compositing_mode(s_battery_layer, GCompOpSet);
  
  // TEXT
  // Where is Jessica Hyde?
  // Don't put the gas away yet
  s_text_layer = text_layer_create(GRect(0, 130, bounds.size.w, 100));
  text_layer_set_background_color(s_text_layer, GColorClear);
  text_layer_set_text_color(s_text_layer, s_text_color);
  APP_LOG(APP_LOG_LEVEL_INFO, "MAIN WIN LOAD config_custom_text: %s", config_custom_text);
  text_layer_set_text(s_text_layer, config_custom_text);
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_text_layer, s_text_font);
  
  // Apply custom text config and refresh the text layer
  apply_config_text(true);
  
  // Add to Window
  layer_add_child(window_layer, bitmap_layer_get_layer(s_battery_layer));
}

static void main_window_unload(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_UNload!");
  
  // Destroy TextLayers
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_text_layer);
  
  // Unload Custom Font (ONLY FOR CUSTOM FONT!)
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
  fonts_unload_custom_font(s_text_font);
  
  // Destroy GBitmap
  gbitmap_destroy(s_battery_bitmap);
  // Destroy BitmapLayer
  bitmap_layer_destroy(s_battery_layer);
}

static void init() {
  // Load config from persistent storage
  load_config();
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  // Make sure the time is displayed from the start
  update_time();
  
  // Register for battery level updates
  battery_state_service_subscribe(battery_handler);
  // Ensure battery level is displayed from the start
  battery_handler(battery_state_service_peek());
  
  // Config handler
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(APP_MESSAGE_INBOX_SIZE_MINIMUM, APP_MESSAGE_OUTBOX_SIZE_MINIMUM);
  //app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}