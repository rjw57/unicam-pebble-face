#include "led-bar-layer.h"
#include "toggle-bitmap-layer.h"

#include <pebble.h>

struct LedBarLayer_ {
    Layer                   *base_layer;
    ToggleBitmapLayer      **toggle_layers;
    int                      n_toggle_layers;
    int                      n_rows, n_cols;
    int                      value;
    int                      max_value;
    const GBitmap           *on_bitmap, *off_bitmap;
    GSize                    led_size, gutter_size;
};

inline int max_int(int a, int b)
{
    return (a>b) ? a : b;
}

inline int min_int(int a, int b)
{
    return (a<b) ? a : b;
}

inline void led_bar_layer_update_toggles_(LedBarLayer* led_bar_layer)
{
    // Early out if there're no toggles
    if(NULL == led_bar_layer->toggle_layers)
        return;

    for(int i=0; i<led_bar_layer->n_toggle_layers; ++i)
    {
        toggle_bitmap_layer_set_state(led_bar_layer->toggle_layers[i],
            (i<led_bar_layer->value) ? TOGGLE_BITMAP_LAYER_STATE_ON : TOGGLE_BITMAP_LAYER_STATE_OFF);
    }
}

inline void led_bar_layer_reset_layout_(LedBarLayer* led_bar_layer)
{
    // remove any existing layers
    if(NULL != led_bar_layer->toggle_layers) {
        for(int i=0; i<led_bar_layer->n_toggle_layers; ++i) {
            ToggleBitmapLayer *tbl = led_bar_layer->toggle_layers[i];
            layer_remove_from_parent(toggle_bitmap_layer_get_layer(tbl));
            toggle_bitmap_layer_destroy(tbl);
        }
        free(led_bar_layer->toggle_layers);
        led_bar_layer->toggle_layers = NULL;
    }
    led_bar_layer->n_toggle_layers = 0;

    // how many layers should we add now?
    led_bar_layer->n_toggle_layers =
        max_int(0, min_int(led_bar_layer->max_value, led_bar_layer->n_cols * led_bar_layer->n_rows));

    if(led_bar_layer->n_toggle_layers > 0) {
        GRect frame = (GRect){ .origin = {0, 0}, .size = led_bar_layer->led_size };

        led_bar_layer->toggle_layers = (ToggleBitmapLayer**) malloc(led_bar_layer->n_toggle_layers * sizeof(ToggleBitmapLayer*));
        for(int n_in_row=0, n_added=0; n_added < led_bar_layer->n_toggle_layers; ++n_added, ++n_in_row)
        {
            if(n_in_row >= led_bar_layer->n_cols) {
                n_in_row -= led_bar_layer->n_cols;
                frame.origin.x = 0;
                frame.origin.y += led_bar_layer->led_size.h + led_bar_layer->gutter_size.h;
            }

            ToggleBitmapLayer* tbl = toggle_bitmap_layer_create(frame);
            toggle_bitmap_layer_set_on_bitmap(tbl, led_bar_layer->on_bitmap);
            toggle_bitmap_layer_set_off_bitmap(tbl, led_bar_layer->off_bitmap);
            toggle_bitmap_layer_set_state(tbl, TOGGLE_BITMAP_LAYER_STATE_OFF);
            layer_add_child(led_bar_layer->base_layer, toggle_bitmap_layer_get_layer(tbl));
            led_bar_layer->toggle_layers[n_added] = tbl;

            frame.origin.x += led_bar_layer->led_size.w + led_bar_layer->gutter_size.w;
        }
    }

    led_bar_layer_update_toggles_(led_bar_layer);
}

LedBarLayer* led_bar_layer_create(GRect frame)
{
    /* Create LedBarLayer structure. */
    LedBarLayer* lbl = (LedBarLayer*) malloc(sizeof(LedBarLayer));
    if(lbl == NULL)
        return NULL;

    /* Initialise struct */
    *lbl = (LedBarLayer) {
        .base_layer         = layer_create(frame),
        .toggle_layers      = NULL,
        .n_toggle_layers    = 0,
        .n_rows             = 0,
        .n_cols             = 0,
        .value              = 0,
        .max_value          = 0,
        .led_size           = { 0, 0 },
        .gutter_size        = { 0, 0 },
        .on_bitmap          = NULL,
        .off_bitmap         = NULL,
    };

    return lbl;
}

void led_bar_layer_destroy(LedBarLayer* led_bar_layer)
{
    if(NULL == led_bar_layer)
        return;

    // this will de-allocate the toggle layers
    led_bar_layer_set_geometry(led_bar_layer, 0, 0);

    // destroy the base layer
    layer_destroy(led_bar_layer->base_layer);

    // free the state structure
    free(led_bar_layer);
}

Layer* led_bar_layer_get_layer(const LedBarLayer* led_bar_layer)
{
    return led_bar_layer->base_layer;
}

void led_bar_layer_set_geometry(LedBarLayer* led_bar_layer, int n_rows, int n_cols)
{
    led_bar_layer->n_rows = n_rows;
    led_bar_layer->n_cols = n_cols;
    led_bar_layer_reset_layout_(led_bar_layer);
}

int led_bar_layer_get_n_rows(LedBarLayer* led_bar_layer)
{
    return led_bar_layer->n_rows;
}

int led_bar_layer_get_n_cols(LedBarLayer* led_bar_layer)
{
    return led_bar_layer->n_cols;
}

void led_bar_layer_set_gutter_size(LedBarLayer* led_bar_layer, GSize gutter_size)
{
    led_bar_layer->gutter_size = gutter_size;
    led_bar_layer_reset_layout_(led_bar_layer);
}

GSize led_bar_layer_get_gutter_size(LedBarLayer* led_bar_layer)
{
    return led_bar_layer->gutter_size;
}

void led_bar_layer_set_led_size(LedBarLayer* led_bar_layer, GSize led_size)
{
    led_bar_layer->led_size = led_size;
    led_bar_layer_reset_layout_(led_bar_layer);
}

GSize led_bar_layer_get_led_size(LedBarLayer* led_bar_layer)
{
    return led_bar_layer->led_size;
}

void led_bar_layer_set_value(LedBarLayer* led_bar_layer, int value)
{
    led_bar_layer->value = value;
    led_bar_layer_update_toggles_(led_bar_layer);
}

int led_bar_layer_get_value(LedBarLayer* led_bar_layer)
{
    return led_bar_layer->value;
}

void led_bar_layer_set_max_value(LedBarLayer* led_bar_layer, int max_value)
{
    led_bar_layer->max_value = max_value;
    led_bar_layer_reset_layout_(led_bar_layer);
}

int led_bar_layer_get_max_value(LedBarLayer* led_bar_layer)
{
    return led_bar_layer->max_value;
}

void led_bar_layer_set_on_bitmap(LedBarLayer* led_bar_layer, const GBitmap* bitmap)
{
    led_bar_layer->on_bitmap = bitmap;
    led_bar_layer_reset_layout_(led_bar_layer);
}

const GBitmap* led_bar_layer_get_on_bitmap(const LedBarLayer* led_bar_layer)
{
    return led_bar_layer->on_bitmap;
}

void led_bar_layer_set_off_bitmap(LedBarLayer* led_bar_layer, const GBitmap* bitmap)
{
    led_bar_layer->off_bitmap = bitmap;
    led_bar_layer_reset_layout_(led_bar_layer);
}

const GBitmap* led_bar_layer_get_off_bitmap(const LedBarLayer* led_bar_layer)
{
    return led_bar_layer->off_bitmap;
}
