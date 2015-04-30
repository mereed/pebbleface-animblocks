#include <pebble.h>

#define ANIM_DURATION 1000

TextLayer *time_layer;
TextLayer *layer_date_text;
TextLayer *layer_daynum_text;
TextLayer *layer_bt_text;

int charge_percent = 0;
TextLayer *battery_text_layer;
static GFont *custom_font;
static GFont *custom_font2;
static GFont *custom_font3;
static GFont *custom_font4;
static GFont *custom_font5;

static Window *window;
static Layer *window_layer;

static PropertyAnimation *animation1, *animation2, *animation3, *animation4, *animation5, *animation6, *animation7, *animation8;

BitmapLayer *block1_layer;
GBitmap *block1_bitmap;

BitmapLayer *block2_layer;
GBitmap *block2_bitmap;

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




static void animation() {

  GRect from_frame1 = GRect(-102, 6, 102, 48);
  GRect to_frame1 = GRect(6, 6, 102, 48);
  // Schedule the next animation
  animation1 = property_animation_create_layer_frame(bitmap_layer_get_layer(block1_layer), &from_frame1, &to_frame1);
  animation_set_duration((Animation*)animation1, ANIM_DURATION);
  animation_set_delay((Animation*)animation1, 0);
  animation_set_curve((Animation*)animation1, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation1);

  GRect from_frame2 = GRect(-48, 61, 48, 48);
  GRect to_frame2 = GRect(6, 61, 48, 48);
  // Schedule the next animation
  animation2 = property_animation_create_layer_frame(bitmap_layer_get_layer(layer_batt_img), &from_frame2, &to_frame2);
  animation_set_duration((Animation*)animation2, ANIM_DURATION);
  animation_set_delay((Animation*)animation2, 1000);
  animation_set_curve((Animation*)animation2, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation2);

  GRect from_frame3 = GRect(-48, 61, 47, 48);
  GRect to_frame3 = GRect(61, 61, 47, 48);
  // Schedule the next animation
  animation3 = property_animation_create_layer_frame(bitmap_layer_get_layer(block3_layer), &from_frame3, &to_frame3);
  animation_set_duration((Animation*)animation3, ANIM_DURATION);
  animation_set_delay((Animation*)animation3, 1500);
  animation_set_curve((Animation*)animation3, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation3);
	
  GRect from_frame4 = GRect(-102, 117, 102, 48);
  GRect to_frame4 = GRect(6, 117, 102, 48);
  // Schedule the next animation
  animation4 = property_animation_create_layer_frame(bitmap_layer_get_layer(block4_layer), &from_frame4, &to_frame4);
  animation_set_duration((Animation*)animation4, ANIM_DURATION);
  animation_set_delay((Animation*)animation4, 2000);
  animation_set_curve((Animation*)animation4, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation4);
	
  GRect from_frame5 = GRect(-20, 117, 20, 20);
  GRect to_frame5 = GRect(88, 117, 20, 20);
  // Schedule the next animation
  animation5 = property_animation_create_layer_frame(bitmap_layer_get_layer(block5_layer), &from_frame5, &to_frame5);
  animation_set_duration((Animation*)animation5, ANIM_DURATION);
  animation_set_delay((Animation*)animation5, 2500);
  animation_set_curve((Animation*)animation5, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation5);

  GRect from_frame6 = GRect(164, 6, 24, 48);
  GRect to_frame6 = GRect(116, 6, 24, 48);
  // Schedule the next animation
  animation6 = property_animation_create_layer_frame(bitmap_layer_get_layer(block6_layer), &from_frame6, &to_frame6);
  animation_set_duration((Animation*)animation6, ANIM_DURATION);
  animation_set_delay((Animation*)animation6, 3000);
  animation_set_curve((Animation*)animation6, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation6);

  GRect from_frame7 = GRect(164, 61, 24, 48);
  GRect to_frame7 = GRect(116, 61, 24, 48);
  // Schedule the next animation
  animation7 = property_animation_create_layer_frame(bitmap_layer_get_layer(block7_layer), &from_frame7, &to_frame7);
  animation_set_duration((Animation*)animation7, ANIM_DURATION);
  animation_set_delay((Animation*)animation7, 3200);
  animation_set_curve((Animation*)animation7, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation7);
	
  GRect from_frame8 = GRect(164, 117, 24, 48);
  GRect to_frame8 = GRect(116, 117, 24, 48);
  // Schedule the next animation
  animation8 = property_animation_create_layer_frame(bitmap_layer_get_layer(block8_layer), &from_frame8, &to_frame8);
  animation_set_duration((Animation*)animation8, ANIM_DURATION);
  animation_set_delay((Animation*)animation8, 3400);
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
}

void bluetooth_connection_callback(bool connected) {
  toggle_bluetooth(connected);
}

void tick_handler(struct tm *t, TimeUnits units){
  static char time_buffer[] = "00:00";
    static char date_text[] = "wednesday";
    static char daynum_text[] = "00";

   strftime(date_text, sizeof(date_text), "%A", t);
   text_layer_set_text(layer_date_text, date_text);

   strftime(daynum_text, sizeof(daynum_text), "%e", t);
   text_layer_set_text(layer_daynum_text, daynum_text);
	
  if(clock_is_24h_style()){
    strftime(time_buffer, sizeof(time_buffer), "%R", t);
  }
  else{
    strftime(time_buffer, sizeof(time_buffer), "%l:%M", t);
  }
  text_layer_set_text(time_layer, time_buffer);
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
  block2_layer = bitmap_layer_create(GRect(-48, 60, 48, 48));
  bitmap_layer_set_bitmap(block2_layer, block2_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(block2_layer));	

  block3_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK3);
  block3_layer = bitmap_layer_create(GRect(-48, 64, 48, 48));
  bitmap_layer_set_bitmap(block3_layer, block3_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(block3_layer));	
	
  block4_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK4);
  block4_layer = bitmap_layer_create(GRect(-48, 114, 48, 48));
  bitmap_layer_set_bitmap(block4_layer, block4_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(block4_layer));	

  block5_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK5);
  block5_layer = bitmap_layer_create(GRect(-20, 114, 20, 20));
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
  block8_layer = bitmap_layer_create(GRect(164, 110, 24, 48));
  bitmap_layer_set_bitmap(block8_layer, block8_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(block8_layer));	
	
  custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_14));
  custom_font2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_18));
  custom_font3 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_22));
  custom_font5 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_20));
  custom_font4 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_36));
	
  time_layer = text_layer_create(GRect(0, 13, 106, 40));
  text_layer_set_font(time_layer, custom_font4);
  text_layer_set_text_color(time_layer, GColorBlack);
#ifdef PBL_COLOR
 text_layer_set_text_color(time_layer, GColorWhite);
#endif
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_alignment(time_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  layer_date_text = text_layer_create(GRect(11, 137, 145, 30));
  text_layer_set_text_color(layer_date_text, GColorBlack);
#ifdef PBL_COLOR
 text_layer_set_text_color(layer_date_text, GColorWhite);
#endif
  text_layer_set_background_color(layer_date_text, GColorClear);
  text_layer_set_font(layer_date_text, custom_font5);
  text_layer_set_text_alignment(layer_date_text, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(layer_date_text));	

  layer_daynum_text = text_layer_create(GRect(91, 117, 30, 30));
  text_layer_set_text_color(layer_daynum_text, GColorBlack );
#ifdef PBL_COLOR
 text_layer_set_text_color(layer_daynum_text, GColorWhite);
#endif	
  text_layer_set_background_color(layer_daynum_text, GColorClear);
  text_layer_set_font(layer_daynum_text, custom_font);
  text_layer_set_text_alignment(layer_daynum_text, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(layer_daynum_text));	

  battery_text_layer = text_layer_create(GRect(0, 85, 50, 40));
  text_layer_set_text_color(battery_text_layer, GColorBlack);
#ifdef PBL_COLOR
 text_layer_set_text_color(battery_text_layer, GColorWhite);
#endif
  text_layer_set_background_color(battery_text_layer, GColorClear);
  text_layer_set_font(battery_text_layer, custom_font2);
  text_layer_set_text_alignment(battery_text_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(battery_text_layer));	
	
  layer_bt_text = text_layer_create(GRect(68, 81, 35, 40));
  text_layer_set_text_color(layer_bt_text, GColorBlack);
#ifdef PBL_COLOR
 text_layer_set_text_color(layer_bt_text, GColorWhite);
#endif
  text_layer_set_background_color(layer_bt_text, GColorClear);
  text_layer_set_font(layer_bt_text, custom_font3);
  text_layer_set_text_alignment(layer_bt_text, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(layer_bt_text));		
	
    // draw first frame
    force_update();

}

static void main_window_unload(Window *window) {
 text_layer_destroy(time_layer);
 text_layer_destroy(layer_daynum_text);
 text_layer_destroy(layer_date_text);
 text_layer_destroy(battery_text_layer);
 text_layer_destroy( layer_bt_text );

}

static void init(void) {
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
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	
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