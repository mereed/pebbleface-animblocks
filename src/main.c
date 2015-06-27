#include <pebble.h>

#define ANIM_DURATION 1000

static AppSync sync;
static uint8_t sync_buffer[128];

static const uint32_t WEATHER_ICONS[] = {
  RESOURCE_ID_CLEAR_DAY,
  RESOURCE_ID_CLEAR_NIGHT,
  RESOURCE_ID_WINDY,
  RESOURCE_ID_COLD,
  RESOURCE_ID_PARTLY_CLOUDY_DAY,
  RESOURCE_ID_PARTLY_CLOUDY_NIGHT,
  RESOURCE_ID_HAZE,
  RESOURCE_ID_CLOUD,
  RESOURCE_ID_RAIN,
  RESOURCE_ID_SNOW,
  RESOURCE_ID_HAIL,
  RESOURCE_ID_CLOUDY,
  RESOURCE_ID_STORM,
  RESOURCE_ID_FOG,
  RESOURCE_ID_NA,
};

static int bluetoothvibe;
static int hourlyvibe;
static int hidesec;

static bool appStarted = false;

enum WeatherKey {
  WEATHER_ICON_KEY = 0x0,
  INVERT_COLOR_KEY = 0x1,   
  BLUETOOTHVIBE_KEY = 0x2,
  HOURLYVIBE_KEY = 0x3,
  HIDESEC_KEY = 0x4,
  HIDE_BATT_KEY = 0x5,
  HIDE_WEATHER_KEY = 0x6,
  WEATHER_TEMPERATURE_KEY = 0x7,
};

TextLayer *time_layer;
TextLayer *ampm_layer;
TextLayer *layer_date_text;
TextLayer *layer_daynum_text;
TextLayer *layer_secs_text;
TextLayer *layer_bt_text;

Layer* weather_holder;
BitmapLayer *icon_layer;
GBitmap *icon_bitmap = NULL;
TextLayer *temp_layer;

int charge_percent = 0;
TextLayer *battery_text_layer;
static GFont *custom_font;
static GFont *custom_font2;
static GFont *custom_font3;
static GFont *custom_font4;
static GFont *custom_font5;

static Window *window;
static Layer *window_layer;

static PropertyAnimation *animation1, *animation2, *animation2a, *animation3, *animation4, *animation5, *animation6, *animation7, *animation8;

BitmapLayer *block1_layer;
GBitmap *block1_bitmap;

BitmapLayer *block2_layer;
GBitmap *block2_bitmap;

BitmapLayer *block2a_layer;
GBitmap *block2a_bitmap;

BitmapLayer *block3_layer;
GBitmap *block3_bitmap;

BitmapLayer *block4_layer;
GBitmap *block4_bitmap;

BitmapLayer *block5_layer;
GBitmap *block5_bitmap;

BitmapLayer *block6_layer;
GBitmap *block6_bitmap;

BitmapLayer *block7_layer;
GBitmap *block7_bitmap;

BitmapLayer *block8_layer;
GBitmap *block8_bitmap;

GBitmap *img_battery_100;
GBitmap *img_battery_90;
GBitmap *img_battery_80;
GBitmap *img_battery_70;
GBitmap *img_battery_60;
GBitmap *img_battery_50;
GBitmap *img_battery_40;
GBitmap *img_battery_30;
GBitmap *img_battery_20;
GBitmap *img_battery_10;
GBitmap *img_battery_charge;
BitmapLayer *layer_batt_img;


void tick_handler(struct tm *t, TimeUnits units){
  static char time_buffer[] = "00:00";
    static char date_text[] = "wednesday";
    static char daynum_text[] = "xxx 00";
    static char secs_text[] = "00";
    static char ampm_text[] = "xx";

   strftime(date_text, sizeof(date_text), "%A", t);
   text_layer_set_text(layer_date_text, date_text);

   strftime(daynum_text, sizeof(daynum_text), "%b %e", t);
   text_layer_set_text(layer_daynum_text, daynum_text);
	
   strftime(secs_text, sizeof(secs_text), "%S", t);
   text_layer_set_text(layer_secs_text, secs_text);
	
  if(clock_is_24h_style()){
    strftime(time_buffer, sizeof(time_buffer), "%R", t);
  }
  else{
    strftime(time_buffer, sizeof(time_buffer), "%l:%M", t);
    strftime(ampm_text, sizeof(ampm_text), "%P", t);
	text_layer_set_text(ampm_layer, ampm_text);
  }
  text_layer_set_text(time_layer, time_buffer);
}

static void sync_tuple_changed_callback(const uint32_t key,
                                        const Tuple* new_tuple,
                                        const Tuple* old_tuple,
                                        void* context) {

  // App Sync keeps new_tuple in sync_buffer, so we may use it directly
  switch (key) {
    case WEATHER_ICON_KEY:
      if (icon_bitmap) {
        gbitmap_destroy(icon_bitmap);
      }
      icon_bitmap = gbitmap_create_with_resource(
          WEATHER_ICONS[new_tuple->value->uint8]);
      bitmap_layer_set_bitmap(icon_layer, icon_bitmap);
      break;

   case WEATHER_TEMPERATURE_KEY:
      text_layer_set_text(temp_layer, new_tuple->value->cstring);  
   break;
	
    case BLUETOOTHVIBE_KEY:
      bluetoothvibe = new_tuple->value->uint8 != 0;
	  persist_write_bool(BLUETOOTHVIBE_KEY, bluetoothvibe);
      break;      
	  
    case HOURLYVIBE_KEY:
      hourlyvibe = new_tuple->value->uint8 != 0;
	  persist_write_bool(HOURLYVIBE_KEY, hourlyvibe);	  
      break;	  
	
	case HIDESEC_KEY:
      hidesec = new_tuple->value->uint8 != 0;
	  persist_write_bool(HIDESEC_KEY, hidesec);	  

   if(hidesec) {
        tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
		layer_set_hidden(text_layer_get_layer(layer_secs_text), true);
		layer_set_hidden(bitmap_layer_get_layer(block2a_layer), true);
	   
		layer_set_hidden(bitmap_layer_get_layer(icon_layer), false);
		layer_set_hidden(text_layer_get_layer(temp_layer), false);
      }
      else {
        tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
		layer_set_hidden(bitmap_layer_get_layer(block2a_layer), false);
		layer_set_hidden(text_layer_get_layer(layer_secs_text), false);

		layer_set_hidden(bitmap_layer_get_layer(icon_layer), true);
		layer_set_hidden(text_layer_get_layer(temp_layer), true);
      }
	  break;   
  }
}

static void animation() {

  GRect from_frame1 = GRect(-102, 6, 102, 48);
  GRect to_frame1 = GRect(6, 6, 102, 48);
  // Schedule the next animation
  animation1 = property_animation_create_layer_frame(bitmap_layer_get_layer(block1_layer), &from_frame1, &to_frame1);
  animation_set_duration((Animation*)animation1, ANIM_DURATION);
  animation_set_delay((Animation*)animation1, 0);
  animation_set_curve((Animation*)animation1, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation1);

  GRect from_frame2 = GRect(-48, 61, 49, 48);
  GRect to_frame2 = GRect(6, 61, 49, 48);
  // Schedule the next animation
  animation2 = property_animation_create_layer_frame(bitmap_layer_get_layer(layer_batt_img), &from_frame2, &to_frame2);
  animation_set_duration((Animation*)animation2, ANIM_DURATION);
  animation_set_delay((Animation*)animation2, 1000);
  animation_set_curve((Animation*)animation2, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation2);

  GRect from_frame2a = GRect(-50, 61, 50, 21);
  GRect to_frame2a = GRect(58, 61, 50, 21);
  // Schedule the next animation
//	if (hidesec) {
		  animation2a = property_animation_create_layer_frame(bitmap_layer_get_layer(icon_layer), &from_frame2a, &to_frame2a);
//	} else {
//		  animation2a = property_animation_create_layer_frame(bitmap_layer_get_layer(block2a_layer), &from_frame2a, &to_frame2a);
//	}
  animation_set_duration((Animation*)animation2a, ANIM_DURATION);
  animation_set_delay((Animation*)animation2a, 1500);
  animation_set_curve((Animation*)animation2a, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation2a);

  GRect from_frame3 = GRect(-50, 85, 50, 24);
  GRect to_frame3 = GRect(58, 85, 50, 24);
  // Schedule the next animation
  animation3 = property_animation_create_layer_frame(bitmap_layer_get_layer(block3_layer), &from_frame3, &to_frame3);
  animation_set_duration((Animation*)animation3, ANIM_DURATION);
  animation_set_delay((Animation*)animation3, 2000);
  animation_set_curve((Animation*)animation3, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation3);
	
  GRect from_frame4 = GRect(-102, 116, 102, 48);
  GRect to_frame4 = GRect(6, 116, 102, 48);
  // Schedule the next animation
  animation4 = property_animation_create_layer_frame(bitmap_layer_get_layer(block4_layer), &from_frame4, &to_frame4);
  animation_set_duration((Animation*)animation4, ANIM_DURATION);
  animation_set_delay((Animation*)animation4, 2500);
  animation_set_curve((Animation*)animation4, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation4);
	
  GRect from_frame5 = GRect(-20, 116, 50, 20);
  GRect to_frame5 = GRect(58, 116, 50, 20);
  // Schedule the next animation
  animation5 = property_animation_create_layer_frame(bitmap_layer_get_layer(block5_layer), &from_frame5, &to_frame5);
  animation_set_duration((Animation*)animation5, ANIM_DURATION);
  animation_set_delay((Animation*)animation5, 3000);
  animation_set_curve((Animation*)animation5, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation5);

  GRect from_frame6 = GRect(164, 6, 24, 48);
  GRect to_frame6 = GRect(116, 6, 24, 48);
  // Schedule the next animation
  animation6 = property_animation_create_layer_frame(bitmap_layer_get_layer(block6_layer), &from_frame6, &to_frame6);
  animation_set_duration((Animation*)animation6, ANIM_DURATION);
  animation_set_delay((Animation*)animation6, 3500);
  animation_set_curve((Animation*)animation6, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation6);

  GRect from_frame7 = GRect(164, 61, 24, 48);
  GRect to_frame7 = GRect(116, 61, 24, 48);
  // Schedule the next animation
  animation7 = property_animation_create_layer_frame(bitmap_layer_get_layer(block7_layer), &from_frame7, &to_frame7);
  animation_set_duration((Animation*)animation7, ANIM_DURATION);
  animation_set_delay((Animation*)animation7, 3700);
  animation_set_curve((Animation*)animation7, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation7);
	
  GRect from_frame8 = GRect(164, 116, 24, 48);
  GRect to_frame8 = GRect(116, 116, 24, 48);
  // Schedule the next animation
  animation8 = property_animation_create_layer_frame(bitmap_layer_get_layer(block8_layer), &from_frame8, &to_frame8);
  animation_set_duration((Animation*)animation8, ANIM_DURATION);
  animation_set_delay((Animation*)animation8, 3900);
  animation_set_curve((Animation*)animation8, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation8);
	
}

void update_battery_state(BatteryChargeState charge_state) {
    static char battery_text[] = "x100%";

    if (charge_state.is_charging) {
        bitmap_layer_set_bitmap(layer_batt_img, img_battery_charge);
        snprintf(battery_text, sizeof(battery_text), "+%d%%", charge_state.charge_percent);
    } else {
        snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);       
	
        if (charge_state.charge_percent <= 10) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_10);
        } else if (charge_state.charge_percent <= 20) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_20);
		} else if (charge_state.charge_percent <= 30) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_30);
		} else if (charge_state.charge_percent <= 40) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_40);
		} else if (charge_state.charge_percent <= 50) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_50);
        } else if (charge_state.charge_percent <= 60) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_60);
		} else	if (charge_state.charge_percent <= 70) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_70);
		} else if (charge_state.charge_percent <= 80) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_80);
		} else if (charge_state.charge_percent <= 90) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_90);
		} else if (charge_state.charge_percent <= 99) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_100);
		} else {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_100);
        } 
    } 
    charge_percent = charge_state.charge_percent;   
    text_layer_set_text(battery_text_layer, battery_text);
} 

static void toggle_bluetooth(bool connected) {

if (!connected) {
	  
		static char bt_text[] = "xxx";  
	    snprintf(bt_text, sizeof(bt_text), "Off");
	    text_layer_set_text(layer_bt_text, bt_text);
	
     } else {
	
		static char bt_text[] = "xxx";
        snprintf(bt_text, sizeof(bt_text), "On");
        text_layer_set_text(layer_bt_text, bt_text);
	}
	
   if (appStarted && bluetoothvibe) {
      
        vibes_short_pulse();
	}
}

void bluetooth_connection_callback(bool connected) {
  toggle_bluetooth(connected);
}

void force_update(void) {
  update_battery_state(battery_state_service_peek());
  toggle_bluetooth(bluetooth_connection_service_peek());

  struct tm *t;
  time_t temp;        
  temp = time(NULL);        
  t = localtime(&temp);

  tick_handler(t, MINUTE_UNIT);
}

static void main_window_load(Window *window) {
	
  Layer *window_layer = window_get_root_layer(window);

  img_battery_100   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_100);
  img_battery_90   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_90);
  img_battery_80   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_80);
  img_battery_70   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_70);
  img_battery_60   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_60);
  img_battery_50   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_50);
  img_battery_40   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_40);
  img_battery_30   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_30);
  img_battery_20    = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_20);
  img_battery_10    = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_10);
  img_battery_charge = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_CHARGING);

  layer_batt_img  = bitmap_layer_create(GRect(-61, 61, 48, 48));
  bitmap_layer_set_bitmap(layer_batt_img, img_battery_100);
  layer_add_child(window_layer, bitmap_layer_get_layer(layer_batt_img));
	
  block1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK1);
  block1_layer = bitmap_layer_create(GRect(-102, 10, 102, 48));
  bitmap_layer_set_bitmap(block1_layer, block1_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(block1_layer));	
	
  block2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK2);
  block2_layer = bitmap_layer_create(GRect(-48, 61, 49, 48));
  bitmap_layer_set_bitmap(block2_layer, block2_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(block2_layer));	

  block2a_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK2A);
  block2a_layer = bitmap_layer_create(GRect(58, 61, 50, 21));
  bitmap_layer_set_bitmap(block2a_layer, block2a_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(block2a_layer));	
//  layer_set_hidden(bitmap_layer_get_layer(block2a_layer), true);
	
  block3_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK3);
  block3_layer = bitmap_layer_create(GRect(-50, 85, 50, 24));
  bitmap_layer_set_bitmap(block3_layer, block3_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(block3_layer));	
	
  block4_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK4);
  block4_layer = bitmap_layer_create(GRect(-48, 116, 48, 48));
  bitmap_layer_set_bitmap(block4_layer, block4_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(block4_layer));	

  block5_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK5);
  block5_layer = bitmap_layer_create(GRect(-60, 116, 50, 20));
  bitmap_layer_set_bitmap(block5_layer, block5_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(block5_layer));	
	
  block6_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK6);
  block6_layer = bitmap_layer_create(GRect(164, 10, 24, 48));
  bitmap_layer_set_bitmap(block6_layer, block6_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(block6_layer));	

  block7_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK7);
  block7_layer = bitmap_layer_create(GRect(164, 60, 24, 48));
  bitmap_layer_set_bitmap(block7_layer, block7_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(block7_layer));	

  block8_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK8);
  block8_layer = bitmap_layer_create(GRect(164, 116, 24, 48));
  bitmap_layer_set_bitmap(block8_layer, block8_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(block8_layer));	
	
  custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_14));
  custom_font2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_18));
 //custom_font3 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_22));
  custom_font5 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_19));
  custom_font4 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_36));

  Layer *weather_holder = layer_create(GRect(0, 0, 144, 168 ));
  layer_add_child(window_layer, weather_holder);

  icon_layer = bitmap_layer_create(GRect(-50, 61, 21, 21));
  layer_add_child(weather_holder, bitmap_layer_get_layer(icon_layer));

  temp_layer = text_layer_create(GRect(80, 62, 40, 30));
  text_layer_set_text_color(temp_layer, GColorBlack);
#ifdef PBL_COLOR
 text_layer_set_text_color(temp_layer, GColorWhite  );
#endif	
  text_layer_set_background_color(temp_layer, GColorClear);
  text_layer_set_font(temp_layer, custom_font);
	// text_layer_set_font(temp_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(temp_layer, GTextAlignmentLeft);
  layer_add_child(weather_holder, text_layer_get_layer(temp_layer));
	

  time_layer = text_layer_create(GRect(0, 13, 107, 40));
  text_layer_set_font(time_layer, custom_font4);
  text_layer_set_text_color(time_layer, GColorBlack);
#ifdef PBL_COLOR
 text_layer_set_text_color(time_layer, GColorWhite);
#endif
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_alignment(time_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  ampm_layer = text_layer_create(GRect(68, 6, 35, 40));
  text_layer_set_font(ampm_layer, custom_font);
  text_layer_set_text_color(ampm_layer, GColorBlack);
#ifdef PBL_COLOR
 text_layer_set_text_color(ampm_layer, GColorWhite);
#endif
  text_layer_set_background_color(ampm_layer, GColorClear);
  text_layer_set_text_alignment(ampm_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(ampm_layer));
	
  layer_date_text = text_layer_create(GRect(11, 137, 145, 30));
  text_layer_set_text_color(layer_date_text, GColorBlack);
#ifdef PBL_COLOR
 text_layer_set_text_color(layer_date_text, GColorWhite);
#endif
  text_layer_set_background_color(layer_date_text, GColorClear);
  text_layer_set_font(layer_date_text, custom_font5);
  text_layer_set_text_alignment(layer_date_text, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(layer_date_text));	

  layer_daynum_text = text_layer_create(GRect(68, 117, 37, 30));
  text_layer_set_text_color(layer_daynum_text, GColorBlack );
#ifdef PBL_COLOR
 text_layer_set_text_color(layer_daynum_text, GColorWhite);
#endif	
  text_layer_set_background_color(layer_daynum_text, GColorClear);
  text_layer_set_font(layer_daynum_text, custom_font);
  text_layer_set_text_alignment(layer_daynum_text, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(layer_daynum_text));	

  layer_secs_text = text_layer_create(GRect(60, 62, 50, 30));
  text_layer_set_text_color(layer_secs_text, GColorBlack );
#ifdef PBL_COLOR
 text_layer_set_text_color(layer_secs_text, GColorWhite);
#endif	
  text_layer_set_background_color(layer_secs_text, GColorClear);
  text_layer_set_font(layer_secs_text, custom_font);
  text_layer_set_text_alignment(layer_secs_text, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(layer_secs_text));		
	
	
  battery_text_layer = text_layer_create(GRect(11, 85, 50, 40));
  text_layer_set_text_color(battery_text_layer, GColorBlack);
#ifdef PBL_COLOR
 text_layer_set_text_color(battery_text_layer, GColorWhite);
#endif
  text_layer_set_background_color(battery_text_layer, GColorClear);
  text_layer_set_font(battery_text_layer, custom_font2);
  text_layer_set_text_alignment(battery_text_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(battery_text_layer));	
	
  layer_bt_text = text_layer_create(GRect(76, 85, 35, 40));
  text_layer_set_text_color(layer_bt_text, GColorBlack);
#ifdef PBL_COLOR
 text_layer_set_text_color(layer_bt_text, GColorWhite);
#endif
  text_layer_set_background_color(layer_bt_text, GColorClear);
  text_layer_set_font(layer_bt_text, custom_font2);
  text_layer_set_text_alignment(layer_bt_text, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(layer_bt_text));		

Tuplet initial_values[] = {
    TupletInteger(WEATHER_ICON_KEY, (uint8_t) 14),
    TupletCString(WEATHER_TEMPERATURE_KEY, ""),
    TupletInteger(INVERT_COLOR_KEY, persist_read_bool(INVERT_COLOR_KEY)),
    TupletInteger(BLUETOOTHVIBE_KEY, persist_read_bool(BLUETOOTHVIBE_KEY)),
    TupletInteger(HIDESEC_KEY, persist_read_bool(HIDESEC_KEY)),
    TupletInteger(HOURLYVIBE_KEY, persist_read_bool(HOURLYVIBE_KEY)),
    TupletInteger(HIDE_BATT_KEY, persist_read_bool(HIDE_BATT_KEY)),
	TupletInteger(HIDE_WEATHER_KEY, persist_read_bool(HIDE_WEATHER_KEY)),
  };

  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values,
                ARRAY_LENGTH(initial_values), sync_tuple_changed_callback,
                NULL, NULL);

  appStarted = true;
  

    // draw first frame
    force_update();

}

static void main_window_unload(Window *window) {
 text_layer_destroy(time_layer);
 text_layer_destroy(ampm_layer);
 text_layer_destroy(layer_daynum_text);
 text_layer_destroy(layer_secs_text);
 text_layer_destroy(layer_date_text);
 text_layer_destroy(battery_text_layer);
 text_layer_destroy( layer_bt_text );

}

static void init(void) {
	
	 // Setup messaging
  const int inbound_size = 128;
  const int outbound_size = 128;
  app_message_open(inbound_size, outbound_size);	
  // Create main Window
  window = window_create();
  window_set_background_color(window, GColorBlack);
#ifdef PBL_COLOR
  window_set_background_color(window, GColorWhite);
#endif
  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(window, true);

  battery_state_service_subscribe(&update_battery_state);
  bluetooth_connection_service_subscribe(&toggle_bluetooth);
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
	
  // Start animation
  animation();
}

static void deinit(void) {

  // Stop any animation in progress
  animation_unschedule_all();
	
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();

  layer_remove_from_parent(bitmap_layer_get_layer(block1_layer));
  bitmap_layer_destroy(block1_layer);
  gbitmap_destroy(block1_bitmap);
  block1_bitmap = NULL;
	
  layer_remove_from_parent(bitmap_layer_get_layer(block2_layer));
  bitmap_layer_destroy(block2_layer);
  gbitmap_destroy(block2_bitmap);
  block2_bitmap = NULL;  

  layer_remove_from_parent(bitmap_layer_get_layer(block2a_layer));
  bitmap_layer_destroy(block2a_layer);
  gbitmap_destroy(block2a_bitmap);
  block2a_bitmap = NULL;  
  
  layer_remove_from_parent(bitmap_layer_get_layer(block3_layer));
  bitmap_layer_destroy(block3_layer);
  gbitmap_destroy(block3_bitmap);
  block3_bitmap = NULL;  
	
  layer_remove_from_parent(bitmap_layer_get_layer(block4_layer));
  bitmap_layer_destroy(block4_layer);
  gbitmap_destroy(block4_bitmap);
  block4_bitmap = NULL;  
	
  layer_remove_from_parent(bitmap_layer_get_layer(block5_layer));
  bitmap_layer_destroy(block5_layer);
  gbitmap_destroy(block5_bitmap);
  block5_bitmap = NULL;  
	
  layer_remove_from_parent(bitmap_layer_get_layer(block6_layer));
  bitmap_layer_destroy(block6_layer);
  gbitmap_destroy(block6_bitmap);
  block6_bitmap = NULL;  
	
  layer_remove_from_parent(bitmap_layer_get_layer(block7_layer));
  bitmap_layer_destroy(block7_layer);
  gbitmap_destroy(block7_bitmap);
  block7_bitmap = NULL;

  layer_remove_from_parent(bitmap_layer_get_layer(block8_layer));
  bitmap_layer_destroy(block8_layer);
  gbitmap_destroy(block8_bitmap);
  block8_bitmap = NULL;
	
  layer_remove_from_parent(bitmap_layer_get_layer(layer_batt_img));
  bitmap_layer_destroy(layer_batt_img);
	
  gbitmap_destroy(img_battery_100);
  gbitmap_destroy(img_battery_90);
  gbitmap_destroy(img_battery_80);
  gbitmap_destroy(img_battery_70);
  gbitmap_destroy(img_battery_60);
  gbitmap_destroy(img_battery_50);
  gbitmap_destroy(img_battery_40);
  gbitmap_destroy(img_battery_30);
  gbitmap_destroy(img_battery_20);
  gbitmap_destroy(img_battery_10);
  gbitmap_destroy(img_battery_charge);	

  fonts_unload_custom_font(custom_font);
  fonts_unload_custom_font(custom_font2);
  fonts_unload_custom_font(custom_font3);
  fonts_unload_custom_font(custom_font4);
  fonts_unload_custom_font(custom_font5);

  layer_remove_from_parent(window_layer);
  layer_destroy(window_layer);
	
  // Destroy main Window
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}