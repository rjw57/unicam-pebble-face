#include <pebble.h>

#include "unicam-time.h"

static Window *window;
static TextLayer *term_text_layer, *week_text_layer;
static TextLayer *date_text_layer, *day_text_layer;

static char placeholder_text[] = "PLACEHOLDER";

#define TEXT_LEN 32

static char week_text[TEXT_LEN], date_text[TEXT_LEN], day_text[TEXT_LEN];

// RESOURCES
GBitmap *filled_dot_bitmap, *empty_dot_bitmap;

// Called once per minute
static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) {
    static char tmp_str[TEXT_LEN];

    // Get current date in Uni format
    struct uni_term_date term_date;
    if(NULL == uni_term_make(tick_time, &term_date))
        return;

    // Date: e.g. 3 Nov
    strftime(tmp_str, sizeof(tmp_str), "%h", tick_time);
    snprintf(date_text, sizeof(date_text), "%li %s", tick_time->tm_mday, tmp_str);
    text_layer_set_text(date_text_layer, date_text);

    // Week: e.g. Wk 2
    snprintf(week_text, sizeof(week_text), "Wk %i", term_date.week+1);
    text_layer_set_text(week_text_layer, week_text);

    // Day: e.g. Fri (6)
    strftime(tmp_str, sizeof(tmp_str), "%a", tick_time);
    snprintf(day_text, sizeof(day_text), "%s (%i)", tmp_str, term_date.day+1);
    text_layer_set_text(day_text_layer, day_text);

    // Term name: e.g. Michaelmas
    text_layer_set_text(term_text_layer, uni_term_name_to_string(term_date.name));
}

// Convenience function to initialise a label and set the right defaults.
TextLayer* create_label(GRect frame)
{
    TextLayer* rv = text_layer_create(frame);
    text_layer_set_text_color(rv, GColorWhite);
    text_layer_set_text_alignment(rv, GTextAlignmentCenter);
    text_layer_set_background_color(rv, GColorClear);
    return rv;
}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    term_text_layer = create_label((GRect) { .origin = { 0, 123 }, .size = { bounds.size.w, 20 } });
    text_layer_set_text(term_text_layer, placeholder_text);

    date_text_layer = create_label((GRect) { .origin = { bounds.size.w>>1, 145 }, .size = { bounds.size.w>>1, 20 } });
    text_layer_set_text(date_text_layer, placeholder_text);

    day_text_layer = create_label((GRect) { .origin = { bounds.size.w>>1, 96 }, .size = { bounds.size.w>>1, 20 } });
    text_layer_set_text(day_text_layer, placeholder_text);

    week_text_layer = create_label((GRect) { .origin = { 0, 145 }, .size = { bounds.size.w>>1, 20 } });
    text_layer_set_text(week_text_layer, placeholder_text);

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
}

static void window_unload(Window *window) {
    text_layer_destroy(term_text_layer);
    text_layer_destroy(date_text_layer);
    text_layer_destroy(day_text_layer);
    text_layer_destroy(week_text_layer);
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
