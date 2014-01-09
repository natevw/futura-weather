#include <pebble.h>
#include "util.h"
#include "weather_layer.h"

static uint8_t WEATHER_ICONS[] = {
	RESOURCE_ID_ICON_CLEAR_DAY,
	RESOURCE_ID_ICON_CLEAR_NIGHT,
	RESOURCE_ID_ICON_RAIN,
	RESOURCE_ID_ICON_SNOW,
	RESOURCE_ID_ICON_SLEET,
	RESOURCE_ID_ICON_WIND,
	RESOURCE_ID_ICON_FOG,
	RESOURCE_ID_ICON_CLOUDY,
	RESOURCE_ID_ICON_PARTLY_CLOUDY_DAY,
	RESOURCE_ID_ICON_PARTLY_CLOUDY_NIGHT,
	RESOURCE_ID_ICON_THUNDER,
	RESOURCE_ID_ICON_RAIN_SNOW,
	RESOURCE_ID_ICON_RAIN_SLEET,
	RESOURCE_ID_ICON_SNOW_SLEET,
	RESOURCE_ID_ICON_COLD,
	RESOURCE_ID_ICON_HOT,
	RESOURCE_ID_ICON_DRIZZLE,
	RESOURCE_ID_ICON_NOT_AVAILABLE,
	RESOURCE_ID_ICON_PHONE_ERROR,
	RESOURCE_ID_ICON_CLOUD_ERROR,
	RESOURCE_ID_ICON_LOADING1,
	RESOURCE_ID_ICON_LOADING2,
	RESOURCE_ID_ICON_LOADING3,
};

void weather_layer_init(WeatherLayer* weather_layer, GPoint pos) {
    weather_layer->layer = layer_create(GRect(pos.x, pos.y, 144, 80));
	
	// Add background layer
    weather_layer->temp_layer_background = text_layer_create(GRect(0, 10, 144, 68));
	text_layer_set_background_color(weather_layer->temp_layer_background, GColorWhite);
	layer_add_child(weather_layer->layer, (Layer *)weather_layer->temp_layer_background);
	
    // Add temperature layer
    weather_layer->temp_layer = text_layer_create(GRect(70, 19, 72, 80));
	text_layer_set_background_color(weather_layer->temp_layer, GColorClear);
	text_layer_set_text_alignment(weather_layer->temp_layer, GTextAlignmentCenter);
	text_layer_set_font(weather_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_40)));
	layer_add_child(weather_layer->layer, (Layer *)weather_layer->temp_layer);
	
	// Note absence of icon layer
	weather_layer->has_weather_icon = false;
}

void weather_layer_set_icon(WeatherLayer* weather_layer, WeatherIcon icon) {
	
	// Remove any possible existing weather icon
	if(weather_layer->has_weather_icon) {
		layer_remove_from_parent((Layer *)weather_layer->icon_layer);
		bitmap_layer_destroy(weather_layer->icon_layer);
		weather_layer->has_weather_icon = false;
	}
	
	// Add weather icon
    weather_layer->icon_layer= bitmap_layer_create(GRect(9, 13, 60, 60));
    bitmap_layer_set_bitmap(weather_layer->icon_layer, gbitmap_create_with_resource(WEATHER_ICONS[icon]));
	layer_add_child(weather_layer->layer, (Layer *)weather_layer->icon_layer);
	weather_layer->has_weather_icon = true;
}

void weather_layer_set_temperature(WeatherLayer* weather_layer, int16_t t, bool is_stale) {
	memcpy(weather_layer->temp_str, itoa(t), 4);
	int degree_pos = strlen(weather_layer->temp_str);
	
	// Temperature between -9°-9° or 20°-99°
	if (strlen(weather_layer->temp_str) == 1 || 
		(strlen(weather_layer->temp_str) == 2 && weather_layer->temp_str[0] != '1')) {
	  text_layer_set_font(weather_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_40)));
	  text_layer_set_text_alignment(weather_layer->temp_layer, GTextAlignmentCenter);
	  // Add degree symbol. If temperature is stale, add blank space instead.
	  if (!is_stale) {
	    memcpy(&weather_layer->temp_str[degree_pos], "°", 3);       // TODO: shouldn't this copy only 2 bytes?
	  }
	  else {
		memcpy(&weather_layer->temp_str[degree_pos], " ", 3);
	  }
		
	  // Is the temperature below zero?
	  if (weather_layer->temp_str[0] == '-') {
		memmove(
          weather_layer->temp_str + 1 + 1,
          weather_layer->temp_str + 1,
          5 - (1 + 1)
        );
		memcpy(&weather_layer->temp_str[1], " ", 1);
	  }
	} 
	
	// Temperature between 10°-19°
	else if (strlen(weather_layer->temp_str) == 2 && weather_layer->temp_str[0] == '1') {
	  text_layer_set_font(weather_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_40)));
	  // Move temperature slightly to the left
	  text_layer_set_text_alignment(weather_layer->temp_layer, GTextAlignmentLeft);
	  // Add degree symbol. If temperature is stale, add blank space instead.
	  if (!is_stale) {
	  	memcpy(&weather_layer->temp_str[degree_pos], "°", 3);
	  }
	  else {
		text_layer_set_text_alignment(weather_layer->temp_layer, GTextAlignmentCenter);
		memcpy(&weather_layer->temp_str[degree_pos], " ", 3);
	  }
	} 
	
	// Temperature above 99° or below -9°
	else if (strlen(weather_layer->temp_str) > 2) { 
	  // Shrink font size
	  text_layer_set_font(weather_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_35)));
	  text_layer_set_text_alignment(weather_layer->temp_layer, GTextAlignmentCenter);
	  // Add degree symbol. If temperature is stale, add blank space instead.
	  if (!is_stale) {
	    memcpy(&weather_layer->temp_str[degree_pos], "°", 3);
	  }
	  else {
		memcpy(&weather_layer->temp_str[degree_pos], " ", 3);
	  }
		
	  // Is the temperature below zero?
	  if (weather_layer->temp_str[0] == '-') {
		memmove(
          weather_layer->temp_str + 1 + 1,
          weather_layer->temp_str + 1,
          5 - (1 + 1)
        );
		memcpy(&weather_layer->temp_str[1], " ", 1);
	  }
	}
	
	text_layer_set_text(weather_layer->temp_layer, weather_layer->temp_str);
}

void weather_layer_deinit(WeatherLayer* weather_layer) {
	if(weather_layer->has_weather_icon)
		bitmap_layer_destroy(weather_layer->icon_layer);
}
