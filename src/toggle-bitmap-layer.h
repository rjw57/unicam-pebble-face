/* A Layer which displays one of two bitmaps depending on a toggle state. */

#ifndef TOGGLE_BITMAP_LAYER_H__
#define TOGGLE_BITMAP_LAYER_H__

#include <pebble.h>

/* An enumeration of possible states which a ToggleBitmapLayer can be in. */
typedef enum {
    TOGGLE_BITMAP_LAYER_STATE_ON,
    TOGGLE_BITMAP_LAYER_STATE_OFF,
} ToggleBitmapLayerState;

/* An opaque type representing a ToggleBitmapLayer */
typedef struct ToggleBitmapLayer_ ToggleBitmapLayer;

/* Create a new ToggleBitmapLayer with the following defaults:
 *
 * - state: TOGGLE_BITMAP_ON
 * - on_bitmap: NULL (none)
 * - off_bitmap: NULL (none)
 */
ToggleBitmapLayer* toggle_bitmap_layer_create(GRect frame);

/* Destroy an existing ToggleBitmapLayer. */
void toggle_bitmap_layer_destroy(ToggleBitmapLayer* toggle_bitmap_layer);

/* Gets the "root" Layer of the bitmap layer, which is the parent for the sub-
 * layers used for its implementation. */
Layer* toggle_bitmap_layer_get_layer(const ToggleBitmapLayer* toggle_bitmap_layer);

/* Toggle the state of the layer. I.e. on -> off or off -> on. */
void toggle_bitmap_layer_toggle_state(ToggleBitmapLayer* toggle_bitmap_layer);

/* Set the state of the layer explicitly. */
void toggle_bitmap_layer_set_state(ToggleBitmapLayer* toggle_bitmap_layer, ToggleBitmapLayerState state);

/* Get the current state of the layer. */
ToggleBitmapLayerState toggle_bitmap_layer_get_state(const ToggleBitmapLayer* toggle_bitmap_layer);

/* Set the 'on' state bitmap. If NULL then 'on' is empty. N.B. bitmap must now
 * not be destroyed until after the layer is destroyed. */
void toggle_bitmap_layer_set_on_bitmap(ToggleBitmapLayer* toggle_bitmap_layer, const GBitmap* bitmap);

/* Get the 'on' state bitmap. If NULL then 'on' is empty. */
const GBitmap* toggle_bitmap_layer_get_on_bitmap(const ToggleBitmapLayer* toggle_bitmap_layer);

/* Set the 'off' state bitmap. If NULL then 'off' is empty. N.B. bitmap must now
 * not be destroyed until after the layer is destroyed. */
void toggle_bitmap_layer_set_off_bitmap(ToggleBitmapLayer* toggle_bitmap_layer, const GBitmap* bitmap);

/* Get the 'off' state bitmap. If NULL then 'off' is empty. */
const GBitmap* toggle_bitmap_layer_get_off_bitmap(const ToggleBitmapLayer* toggle_bitmap_layer);

#endif /* TOGGLE_BITMAP_LAYER_H__ */
