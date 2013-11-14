#include <pebble.h>

#include "unicam-time.h"

static Window *window;
static TextLayer *term_text_layer;
static TextLayer *text_layer;

static char placeholder_text[] = "PLACEHOLDER";

// Called once per minute
static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) {
    static char time_text[] = "00:00"; // Needs to be static because it's used by the system later.

    // Get current date in Uni format
    struct uni_term_date term_date;
    if(NULL == uni_term_make(tick_time, &term_date))
        return;

    // TODO: update UI

    strftime(time_text, sizeof(time_text), "%H:%M", tick_time);
    text_layer_set_text(text_layer, time_text);

    // Set term name
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

    text_layer = create_label((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
    text_layer_set_text(text_layer, placeholder_text);

    term_text_layer = create_label((GRect) { .origin = { 0, 123 }, .size = { bounds.size.w, 20 } });
    text_layer_set_text(term_text_layer, placeholder_text);

    // Ensures time is displayed immediately (will break if NULL tick event accessed).
    // (This is why it's a good idea to have a separate routine to do the update itself.)
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    handle_minute_tick(current_time, MINUTE_UNIT);
    tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);

    layer_add_child(window_layer, text_layer_get_layer(text_layer));
    layer_add_child(window_layer, text_layer_get_layer(term_text_layer));
}

static void window_unload(Window *window) {
    text_layer_destroy(text_layer);
    text_layer_destroy(term_text_layer);
}

static void init(void) {
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
}

int main(void) {
    init();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

    app_event_loop();
    deinit();
}
