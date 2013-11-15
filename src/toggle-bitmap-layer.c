#include "toggle-bitmap-layer.h"

#include <pebble.h>

typedef struct {
    ToggleBitmapLayerState   state;
    const GBitmap           *on_bitmap, *off_bitmap;
} ToggleBitmapLayerPrivate;

inline static ToggleBitmapLayerPrivate* priv(ToggleBitmapLayer* l) {
    return (ToggleBitmapLayerPrivate*) layer_get_data(toggle_bitmap_layer_get_layer(l));
}

/* const version of priv() */
inline static const ToggleBitmapLayerPrivate* priv_c(const ToggleBitmapLayer* l) {
    return (const ToggleBitmapLayerPrivate*) layer_get_data(toggle_bitmap_layer_get_layer(l));
}

static void toggle_bitmap_layer_update_proc(Layer* layer, GContext* ctx)
{
    /* Extract private data */
    ToggleBitmapLayerPrivate* p = priv((ToggleBitmapLayer*) layer);

    /* Which bitmap should we draw? */
    const GBitmap* bm = NULL;
    switch(p->state) {
        case TOGGLE_BITMAP_LAYER_STATE_ON:
            bm = p->on_bitmap;
            break;
        case TOGGLE_BITMAP_LAYER_STATE_OFF:
            bm = p->off_bitmap;
            break;
    };

    /* If no bitmap, early-out. */
    if(NULL == bm)
        return;

    /* Draw the bitmap within our bounds. */
    graphics_draw_bitmap_in_rect(ctx, bm, layer_get_bounds(layer));
}

ToggleBitmapLayer* toggle_bitmap_layer_create(GRect frame)
{
    /* Create a layer with extra storage */
    ToggleBitmapLayer* tbl = (ToggleBitmapLayer*) layer_create_with_data(frame, sizeof(ToggleBitmapLayerPrivate));
    if(tbl == NULL)
        return NULL;

    /* Initialise struct */
    *priv(tbl) = (ToggleBitmapLayerPrivate) {
        .state      = TOGGLE_BITMAP_LAYER_STATE_ON,
        .on_bitmap  = NULL,
        .off_bitmap = NULL,
    };

    /* Set update proc */
    layer_set_update_proc((Layer*) tbl, toggle_bitmap_layer_update_proc);

    return (ToggleBitmapLayer*) tbl;
}

void toggle_bitmap_layer_destroy(ToggleBitmapLayer* toggle_bitmap_layer)
{
    /* Pretty simple, no? */
    layer_destroy(toggle_bitmap_layer_get_layer(toggle_bitmap_layer));
}

Layer* toggle_bitmap_layer_get_layer(const ToggleBitmapLayer* toggle_bitmap_layer)
{
    /* Even simpler than toggle_bitmap_layer_destroy, perhaps? */
    return (Layer*) toggle_bitmap_layer;
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

    priv(toggle_bitmap_layer)->state = state;
    layer_mark_dirty(toggle_bitmap_layer_get_layer(toggle_bitmap_layer));
}

ToggleBitmapLayerState toggle_bitmap_layer_get_state(const ToggleBitmapLayer* toggle_bitmap_layer)
{
    return priv_c(toggle_bitmap_layer)->state;
}

void toggle_bitmap_layer_set_on_bitmap(ToggleBitmapLayer* toggle_bitmap_layer, const GBitmap* bitmap)
{
    priv(toggle_bitmap_layer)->on_bitmap = bitmap;
    layer_mark_dirty(toggle_bitmap_layer_get_layer(toggle_bitmap_layer));
}

const GBitmap* toggle_bitmap_layer_get_on_bitmap(const ToggleBitmapLayer* toggle_bitmap_layer)
{
    return priv_c(toggle_bitmap_layer)->on_bitmap;
}

void toggle_bitmap_layer_set_off_bitmap(ToggleBitmapLayer* toggle_bitmap_layer, const GBitmap* bitmap)
{
    priv(toggle_bitmap_layer)->off_bitmap = bitmap;
    layer_mark_dirty(toggle_bitmap_layer_get_layer(toggle_bitmap_layer));
}

const GBitmap* toggle_bitmap_layer_get_off_bitmap(const ToggleBitmapLayer* toggle_bitmap_layer)
{
    return priv_c(toggle_bitmap_layer)->off_bitmap;
}
