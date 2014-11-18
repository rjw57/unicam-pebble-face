#include <pebble.h>

#include "unicam-time.h"
#include "led-bar-layer.h"

static Window *window;
static TextLayer *term_text_layer, *week_text_layer;
static TextLayer *date_text_layer, *day_text_layer;

static char placeholder_text[] = "PLACEHOLDER";

#define TEXT_LEN 32

static char week_text[TEXT_LEN], date_text[TEXT_LEN], day_text[TEXT_LEN];

static LedBarLayer *hours_led_bar_layer, *minutes_tens_led_bar_layer, *minutes_units_led_bar_layer;

// RESOURCES
GBitmap *filled_dot_bitmap, *empty_dot_bitmap;

// Called once per minute
static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) {
    static char tmp_str[TEXT_LEN];

    // Need this because it would appear tick_time is the raw pointer returned
    // from localtime() which is overwritten by uni_term_make().
    struct tm time_copy = *tick_time;

    // Get current date in Uni format
    struct uni_term_date term_date;
    if(NULL == uni_term_make(&time_copy, &term_date))
        return;

    // Date: e.g. 3 Nov
    strftime(tmp_str, sizeof(tmp_str), "%h", &time_copy);
    snprintf(date_text, sizeof(date_text), "%i %s", time_copy.tm_mday, tmp_str);
    text_layer_set_text(date_text_layer, date_text);

    // Week: e.g. Wk 2
    snprintf(week_text, sizeof(week_text), "Wk %i", term_date.week+1);
    text_layer_set_text(week_text_layer, week_text);

    // Day: e.g. Fri (6)
    strftime(tmp_str, sizeof(tmp_str), "%a", &time_copy);
    snprintf(day_text, sizeof(day_text), "%s (%i)", tmp_str, term_date.day+1);
    text_layer_set_text(day_text_layer, day_text);

    // Term name: e.g. Michaelmas
    text_layer_set_text(term_text_layer, uni_term_name_to_string(term_date.name));

    // Time itself
    int hour = time_copy.tm_hour % 12;
    if(hour == 0) { hour = 12; }
    led_bar_layer_set_value(hours_led_bar_layer, hour);
    led_bar_layer_set_value(minutes_tens_led_bar_layer, time_copy.tm_min / 10);
    led_bar_layer_set_value(minutes_units_led_bar_layer, time_copy.tm_min % 10);
}

// Convenience function to initialise a label and set the right defaults.
TextLayer* create_label(GRect frame)
{
    TextLayer* rv = text_layer_create(frame);
    text_layer_set_text_color(rv, GColorWhite);
    text_layer_set_text_alignment(rv, GTextAlignmentCenter);
    text_layer_set_background_color(rv, GColorClear);
    text_layer_set_font(rv, fonts_get_system_font(FONT_KEY_GOTHIC_24));
    return rv;
}

// Convenience function to initialise a LED bar and set the right defaults.
LedBarLayer* create_led_bar(GRect frame)
{
    LedBarLayer* rv = led_bar_layer_create(frame);
    led_bar_layer_set_on_bitmap(rv, filled_dot_bitmap);
    led_bar_layer_set_off_bitmap(rv, empty_dot_bitmap);
    led_bar_layer_set_gutter_size(rv, (GSize) { 2, 2 });
    led_bar_layer_set_led_size(rv, (GSize) { 21, 21 });
    return rv;
}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    const int text_height = 24;

    term_text_layer = create_label((GRect) { .origin = { 0, bounds.size.h-2*text_height-6 }, .size = { bounds.size.w, text_height } });
    text_layer_set_text(term_text_layer, placeholder_text);

    date_text_layer = create_label((GRect) { .origin = { bounds.size.w>>1, bounds.size.h-text_height-6 }, .size = { bounds.size.w>>1, text_height } });
    text_layer_set_text(date_text_layer, placeholder_text);

    day_text_layer = create_label((GRect) { .origin = { bounds.size.w>>1,  2 + 21*2+2 + 2 + 21 + 2 + 21 - 3}, .size = { bounds.size.w>>1, text_height } });
    text_layer_set_text(day_text_layer, placeholder_text);

    week_text_layer = create_label((GRect) { .origin = { 0, bounds.size.h-text_height-6 }, .size = { bounds.size.w>>1, text_height } });
    text_layer_set_text(week_text_layer, placeholder_text);

    hours_led_bar_layer = create_led_bar((GRect) { .origin = { 2, 2 }, .size = { bounds.size.w-4, 21*2 + 2 } });
    led_bar_layer_set_geometry(hours_led_bar_layer, 2, 6);
    led_bar_layer_set_max_value(hours_led_bar_layer, 12);

    minutes_tens_led_bar_layer = create_led_bar((GRect) { .origin = { 2, 2 + 21*2+2 + 2 }, .size = { bounds.size.w-4, 21 } });
    led_bar_layer_set_geometry(minutes_tens_led_bar_layer, 2, 6);
    led_bar_layer_set_max_value(minutes_tens_led_bar_layer, 5);

    minutes_units_led_bar_layer = create_led_bar((GRect) { .origin = { 2, 2 + 21*2+2 + 2 + 21 + 2 }, .size = { bounds.size.w-4, 21*2 + 2 } });
    led_bar_layer_set_geometry(minutes_units_led_bar_layer, 2, 6);
    led_bar_layer_set_max_value(minutes_units_led_bar_layer, 9);

    // Ensures time is displayed immediately (will break if NULL tick event accessed).
    // (This is why it's a good idea to have a separate routine to do the update itself.)
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    handle_minute_tick(current_time, MINUTE_UNIT);
    tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);

    layer_add_child(window_layer, text_layer_get_layer(term_text_layer));
    layer_add_child(window_layer, text_layer_get_layer(date_text_layer));
    layer_add_child(window_layer, text_layer_get_layer(day_text_layer));
    layer_add_child(window_layer, text_layer_get_layer(week_text_layer));

    layer_add_child(window_layer, led_bar_layer_get_layer(hours_led_bar_layer));
    layer_add_child(window_layer, led_bar_layer_get_layer(minutes_tens_led_bar_layer));
    layer_add_child(window_layer, led_bar_layer_get_layer(minutes_units_led_bar_layer));
}

static void window_unload(Window *window) {
    text_layer_destroy(term_text_layer);
    text_layer_destroy(date_text_layer);
    text_layer_destroy(day_text_layer);
    text_layer_destroy(week_text_layer);

    led_bar_layer_destroy(hours_led_bar_layer);
    led_bar_layer_destroy(minutes_tens_led_bar_layer);
    led_bar_layer_destroy(minutes_units_led_bar_layer);
}

static void init(void) {
    // Load resources
    filled_dot_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FILLED_DOT);
    empty_dot_bitmap = gbitmap_create_with_resource(RESOURCE_ID_EMPTY_DOT);

    window = window_create();
    window_set_background_color(window, GColorBlack);
    window_set_window_handlers(window, (WindowHandlers) {
            .load = window_load,
            .unload = window_unload,
            });
    window_stack_push(window, /* animated = */ true);
}

static void deinit(void) {
    window_destroy(window);

    // Free resources
    gbitmap_destroy(filled_dot_bitmap);
    gbitmap_destroy(empty_dot_bitmap);
}

int main(void) {
    init();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

    app_event_loop();
    deinit();
}
