#include <pebble.h>
#define KEY_CNT 0

static Window *main_window;
static TextLayer *plus_layer;
static TextLayer *minus_layer;
static TextLayer *count_layer;
static TextLayer *set_layer;
static StatusBarLayer *s_status_bar;

static int cnt;
static char buffer_cnt[12];
static bool setMode;

void deinit();
void init();
void main_window_load(Window *window);
void main_window_unload(Window *window);
void click_config_provider(void *context);
void up_click_handler(ClickRecognizerRef recognizer, void *context);
void down_click_handler(ClickRecognizerRef recognizer, void *context);
void down_long_click_handler(ClickRecognizerRef recognizer, void *context);
void updateCnt();
void changeSetMode();
void doNothing();

int main(void){
	init();
	app_event_loop();
	deinit();
}

void deinit(){
	window_destroy(main_window);
}

void init(){
	if(persist_exists(KEY_CNT)){ 
		cnt = persist_read_int(KEY_CNT);	
	} else {
		cnt = 0;
	}
	snprintf(buffer_cnt, sizeof(buffer_cnt), "%d", cnt);
	setMode = false;
	
	main_window = window_create();
	window_set_click_config_provider(main_window, click_config_provider);
	window_set_background_color(main_window, GColorWhite);
	window_set_window_handlers(main_window, (WindowHandlers) {
		.load = main_window_load, .unload = main_window_unload
	});
	window_stack_push(main_window, false);	
	light_enable_interaction(); 
}

void main_window_load(Window *window){

	Layer *window_layer = window_get_root_layer(window);
	s_status_bar = status_bar_layer_create();
	layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));
	
	set_layer = text_layer_create(GRect(0, 15, 80, 18));
	text_layer_set_background_color(set_layer, GColorClear);
	text_layer_set_text_color(set_layer, GColorWhite);
	text_layer_set_font(set_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(set_layer, GTextAlignmentLeft);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(set_layer));
	text_layer_set_text(set_layer, "Set Mode");
	
	plus_layer = text_layer_create(GRect(80, 10, 60, 28));
	text_layer_set_background_color(plus_layer, GColorClear);
	text_layer_set_text_color(plus_layer, GColorBlack);
	text_layer_set_font(plus_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(plus_layer, GTextAlignmentRight);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(plus_layer));
	text_layer_set_text(plus_layer, "+1");
	
	count_layer = text_layer_create(GRect(0, 72, 140, 35));
	text_layer_set_background_color(count_layer, GColorWhite);
	text_layer_set_text_color(count_layer, GColorBlack);
	text_layer_set_font(count_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(count_layer, GTextAlignmentRight);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(count_layer));
	updateCnt();
	
	minus_layer = text_layer_create(GRect(80, 135, 60, 28));
	text_layer_set_background_color(minus_layer, GColorWhite);
	text_layer_set_text_color(minus_layer, GColorBlack);
	text_layer_set_font(minus_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(minus_layer, GTextAlignmentRight);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(minus_layer));
	text_layer_set_text(minus_layer, "-1");
	
}

void main_window_unload(Window *window){
	text_layer_destroy(plus_layer);
	text_layer_destroy(count_layer);
	text_layer_destroy(minus_layer);
	text_layer_destroy(set_layer);
	status_bar_layer_destroy(s_status_bar);

}

void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
	window_long_click_subscribe(BUTTON_ID_DOWN, 1000, down_long_click_handler, doNothing);
	window_long_click_subscribe(BUTTON_ID_SELECT, 1000, changeSetMode, doNothing);
}

void up_click_handler(ClickRecognizerRef recognizer, void *context){
	
	if(setMode){
		cnt = cnt + 50;
	} else{
		cnt++;
	}
	
	if(cnt > 999999999){
		cnt = 999999999;
	}
	
	updateCnt();
}

void down_click_handler(ClickRecognizerRef recognizer, void *context){
	
	if(setMode){
		cnt = cnt - 25;
	} else{
		cnt--;
	}
	
	if(cnt < 1){
		cnt = 0;
	}
	
	updateCnt();
	
}

void down_long_click_handler(ClickRecognizerRef recognizer, void *context){
	
	if((cnt == 0) || (setMode)){
		return;
	}
	
	cnt = 0;
	updateCnt();
	
}

void updateCnt(){
	
	char tmpBuffer[12] = "";
	char tmpNumBuffer[5];
	int a = (int)(cnt / 1000000);
	int b = (cnt / 1000) % 1000;
	int c = cnt % 1000;


	if(a > 0){
		snprintf(tmpNumBuffer, sizeof(tmpNumBuffer), "%d,", a);
		strcat(tmpBuffer, tmpNumBuffer);
		
		snprintf(tmpNumBuffer, sizeof(tmpNumBuffer), "%d,", b);
		tmpNumBuffer[4] = '\0';
		tmpNumBuffer[3] = ',';
		if(b < 10){
			tmpNumBuffer[2] = tmpNumBuffer[0];
			tmpNumBuffer[1] = '0';
			tmpNumBuffer[0] = '0';
		}else if(b < 100){
			tmpNumBuffer[2] = tmpNumBuffer[1];
			tmpNumBuffer[1] = tmpNumBuffer[0];
			tmpNumBuffer[0] = '0';
		}
		strcat(tmpBuffer, tmpNumBuffer);
		
		snprintf(tmpNumBuffer, sizeof(tmpNumBuffer), "%d", c);
		tmpNumBuffer[4] = '\0';
		tmpNumBuffer[3] = '\0';
		if(c < 10){
			tmpNumBuffer[2] = tmpNumBuffer[0];
			tmpNumBuffer[1] = '0';
			tmpNumBuffer[0] = '0';
		}else if(c < 100){
			tmpNumBuffer[2] = tmpNumBuffer[1];
			tmpNumBuffer[1] = tmpNumBuffer[0];
			tmpNumBuffer[0] = '0';
		}
		strcat(tmpBuffer, tmpNumBuffer);
		
	} else if(b > 0){
		snprintf(tmpNumBuffer, sizeof(tmpNumBuffer), "%d,", b);
		strcat(tmpBuffer, tmpNumBuffer);
		
		snprintf(tmpNumBuffer, sizeof(tmpNumBuffer), "%d", c);
		tmpNumBuffer[4] = '\0';
		tmpNumBuffer[3] = '\0';
		if(c < 10){
			tmpNumBuffer[2] = tmpNumBuffer[0];
			tmpNumBuffer[1] = '0';
			tmpNumBuffer[0] = '0';
		}else if(c < 100){
			tmpNumBuffer[2] = tmpNumBuffer[1];
			tmpNumBuffer[1] = tmpNumBuffer[0];
			tmpNumBuffer[0] = '0';
		}
		strcat(tmpBuffer, tmpNumBuffer);
		
	}else{
		snprintf(tmpNumBuffer, sizeof(tmpNumBuffer), "%d", c);
		strcat(tmpBuffer, tmpNumBuffer);
	}
	
	strcpy(buffer_cnt, tmpBuffer);
	
	text_layer_set_text(count_layer, buffer_cnt);
	persist_write_int(KEY_CNT,cnt);
	
}

void changeSetMode(){
	if(setMode){
		text_layer_set_text(plus_layer, "+1");
		text_layer_set_text(minus_layer, "-1");
		text_layer_set_text_color(set_layer, GColorWhite);
		setMode = false;
	}else{
		text_layer_set_text(plus_layer, "+50");
		text_layer_set_text(minus_layer, "-25");
		text_layer_set_text_color(set_layer, GColorBlack);
		setMode = true;
	}
}

void doNothing(){
	
}
