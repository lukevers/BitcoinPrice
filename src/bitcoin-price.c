#include <pebble.h>

typedef enum {
  BtcPrice = 0
} AppKey;

static Window *s_main_window;

static TextLayer *price_text_layer;
static TextLayer *loading_text_layer;

static StatusBarLayer *status_bar_layer;

static GColor background_color;
static GColor foreground_color;

static void vertically_center_text_layer(TextLayer *text_layer) {
    GRect frame = layer_get_frame(text_layer_get_layer(text_layer));
    GSize content_size = text_layer_get_content_size(text_layer);

    float y = frame.origin.y + (frame.size.h - content_size.h - 5) / 2;

    layer_set_frame(
        text_layer_get_layer(text_layer),
        GRect(frame.origin.x, y, frame.size.w, content_size.h + 5)
    );
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
    Tuple *price_tupel = dict_find(iter, BtcPrice);

    if (price_tupel) {
        text_layer_set_text(price_text_layer, price_tupel->value->cstring);
        vertically_center_text_layer(price_text_layer);
        if (loading_text_layer) {
            text_layer_destroy(loading_text_layer);
            loading_text_layer = false;
        }
    }
}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // Status Bar Layer
    status_bar_layer = status_bar_layer_create();
    status_bar_layer_set_colors(status_bar_layer, background_color, foreground_color);
    layer_add_child(window_layer, status_bar_layer_get_layer(status_bar_layer));

    // Price Text Layer
    price_text_layer = text_layer_create(PBL_IF_ROUND_ELSE(grect_inset(bounds, GEdgeInsets(20, 0)), bounds));
    text_layer_set_background_color(price_text_layer, background_color);
    text_layer_set_font(price_text_layer, fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS));
    text_layer_set_text_alignment(price_text_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(price_text_layer));

    // Loading Text Layer
    loading_text_layer = text_layer_create(PBL_IF_ROUND_ELSE(grect_inset(bounds, GEdgeInsets(20, 0)), bounds));
    text_layer_set_background_color(loading_text_layer, background_color);
    text_layer_set_font(loading_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
    text_layer_set_text(loading_text_layer, "Loading...");
    text_layer_set_text_alignment(loading_text_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(loading_text_layer));
    vertically_center_text_layer(loading_text_layer);

#if defined(PBL_ROUND)
    text_layer_enable_screen_text_flow_and_paging(price_text_layer, 5);
    text_layer_enable_screen_text_flow_and_paging(loading_text_layer, 5);
#endif
}

static void window_unload(Window *window) {
    text_layer_destroy(price_text_layer);
    status_bar_layer_destroy(status_bar_layer);

    // Only destroy if it has not yet been destroyed
    if (loading_text_layer) {
        text_layer_destroy(loading_text_layer);
    }
}

static void init() {
    s_main_window = window_create();
    background_color = GColorChromeYellow;
    foreground_color = GColorBlack;

    window_set_background_color(s_main_window, background_color);
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });

    window_stack_push(s_main_window, true);

    app_message_register_inbox_received(inbox_received_handler);
    app_message_open(64, 64);
}

static void deinit() {
    window_destroy(s_main_window);
}

int main() {
    init();
    app_event_loop();
    deinit();
}
