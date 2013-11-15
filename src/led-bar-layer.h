/* A NxM array of virtual LEDs showing an integral number on the interval [0, NxM]. */

#ifndef LED_BAR_H__
#define LED_BAR_H__

#include <pebble.h>

/* An opaque type representing a LedBarLayer */
typedef struct LedBarLayer_ LedBarLayer;

LedBarLayer* led_bar_layer_create(GRect frame);

void led_bar_layer_destroy(LedBarLayer* led_bar_layer);

/* Gets the "root" Layer of the bitmap layer, which is the parent for the sub-
 * layers used for its implementation. */
Layer* led_bar_layer_get_layer(const LedBarLayer* led_bar_layer);

void led_bar_layer_set_geometry(LedBarLayer* led_bar_layer, int n_rows, int n_cols);

int led_bar_layer_get_n_rows(LedBarLayer* led_bar_layer);

int led_bar_layer_get_n_cols(LedBarLayer* led_bar_layer);

void led_bar_layer_set_gutter_size(LedBarLayer* led_bar_layer, GSize gutter_size);

GSize led_bar_layer_get_gutter_size(LedBarLayer* led_bar_layer);

void led_bar_layer_set_led_size(LedBarLayer* led_bar_layer, GSize led_size);

GSize led_bar_layer_get_led_size(LedBarLayer* led_bar_layer);

void led_bar_layer_set_value(LedBarLayer* led_bar_layer, int value);

int led_bar_layer_get_value(LedBarLayer* led_bar_layer);

void led_bar_layer_set_max_value(LedBarLayer* led_bar_layer, int max_value);

int led_bar_layer_get_max_value(LedBarLayer* led_bar_layer);

/* Set the 'on' state bitmap. If NULL then 'on' is empty. N.B. bitmap must now
 * not be destroyed until after the layer is destroyed. */
void led_bar_layer_set_on_bitmap(LedBarLayer* led_bar_layer, const GBitmap* bitmap);

/* Get the 'on' state bitmap. If NULL then 'on' is empty. */
const GBitmap* led_bar_layer_get_on_bitmap(const LedBarLayer* led_bar_layer);

/* Set the 'off' state bitmap. If NULL then 'off' is empty. N.B. bitmap must now
 * not be destroyed until after the layer is destroyed. */
void led_bar_layer_set_off_bitmap(LedBarLayer* led_bar_layer, const GBitmap* bitmap);

/* Get the 'off' state bitmap. If NULL then 'off' is empty. */
const GBitmap* led_bar_layer_get_off_bitmap(const LedBarLayer* led_bar_layer);

#endif /* LED_BAR_H__ */
