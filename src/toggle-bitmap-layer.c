#include "toggle-bitmap-layer.h"

#include <pebble.h>

struct ToggleBitmapLayer_ {
    BitmapLayer             *base_layer;
    ToggleBitmapLayerState   state;
    const GBitmap           *on_bitmap, *off_bitmap;
};

inline void toggle_bitmap_layer_reset_bitmap_(ToggleBitmapLayer* toggle_bitmap_layer)
{
    switch(toggle_bitmap_layer_get_state(toggle_bitmap_layer)) {
        case TOGGLE_BITMAP_LAYER_STATE_ON:
            bitmap_layer_set_bitmap(
                    toggle_bitmap_layer_get_bitmap_layer(toggle_bitmap_layer),
                    toggle_bitmap_layer_get_on_bitmap(toggle_bitmap_layer));
            break;
        case TOGGLE_BITMAP_LAYER_STATE_OFF:
            bitmap_layer_set_bitmap(
                    toggle_bitmap_layer_get_bitmap_layer(toggle_bitmap_layer),
                    toggle_bitmap_layer_get_off_bitmap(toggle_bitmap_layer));
            break;
    };
}

ToggleBitmapLayer* toggle_bitmap_layer_create(GRect frame)
{
    /* Create ToggleBitmapLayer structure. */
    ToggleBitmapLayer* tbl = (ToggleBitmapLayer*) malloc(sizeof(ToggleBitmapLayer));
    if(tbl == NULL)
        return NULL;

    /* Initialise struct */
    *tbl = (ToggleBitmapLayer) {
        .base_layer = bitmap_layer_create(frame),
        .state      = TOGGLE_BITMAP_LAYER_STATE_ON,
        .on_bitmap  = NULL,
        .off_bitmap = NULL,
    };

    return tbl;
}

void toggle_bitmap_layer_destroy(ToggleBitmapLayer* toggle_bitmap_layer)
{
    if(NULL == toggle_bitmap_layer)
        return;

    bitmap_layer_destroy(toggle_bitmap_layer->base_layer);
    free(toggle_bitmap_layer);
}

Layer* toggle_bitmap_layer_get_layer(const ToggleBitmapLayer* toggle_bitmap_layer)
{
    return bitmap_layer_get_layer(toggle_bitmap_layer->base_layer);
}

BitmapLayer* toggle_bitmap_layer_get_bitmap_layer(const ToggleBitmapLayer* toggle_bitmap_layer)
{
    return toggle_bitmap_layer->base_layer;
}

void toggle_bitmap_layer_toggle_state(ToggleBitmapLayer* toggle_bitmap_layer)
{
    toggle_bitmap_layer_set_state(toggle_bitmap_layer,
        (toggle_bitmap_layer_get_state(toggle_bitmap_layer) == TOGGLE_BITMAP_LAYER_STATE_ON) ?
            TOGGLE_BITMAP_LAYER_STATE_OFF : TOGGLE_BITMAP_LAYER_STATE_ON
    );
}

void toggle_bitmap_layer_set_state(ToggleBitmapLayer* toggle_bitmap_layer, ToggleBitmapLayerState state)
{
    if(state == toggle_bitmap_layer_get_state(toggle_bitmap_layer)) {
        /* early out if this is a NOP. */
        return;
    }

    toggle_bitmap_layer->state = state;
    toggle_bitmap_layer_reset_bitmap_(toggle_bitmap_layer);
}

ToggleBitmapLayerState toggle_bitmap_layer_get_state(const ToggleBitmapLayer* toggle_bitmap_layer)
{
    return toggle_bitmap_layer->state;
}

void toggle_bitmap_layer_set_on_bitmap(ToggleBitmapLayer* toggle_bitmap_layer, const GBitmap* bitmap)
{
    toggle_bitmap_layer->on_bitmap = bitmap;
    if(toggle_bitmap_layer_get_state(toggle_bitmap_layer) == TOGGLE_BITMAP_LAYER_STATE_ON)
        toggle_bitmap_layer_reset_bitmap_(toggle_bitmap_layer);
}

const GBitmap* toggle_bitmap_layer_get_on_bitmap(const ToggleBitmapLayer* toggle_bitmap_layer)
{
    return toggle_bitmap_layer->on_bitmap;
}

void toggle_bitmap_layer_set_off_bitmap(ToggleBitmapLayer* toggle_bitmap_layer, const GBitmap* bitmap)
{
    toggle_bitmap_layer->off_bitmap = bitmap;
    if(toggle_bitmap_layer_get_state(toggle_bitmap_layer) == TOGGLE_BITMAP_LAYER_STATE_OFF)
        toggle_bitmap_layer_reset_bitmap_(toggle_bitmap_layer);
}

const GBitmap* toggle_bitmap_layer_get_off_bitmap(const ToggleBitmapLayer* toggle_bitmap_layer)
{
    return toggle_bitmap_layer->off_bitmap;
}
