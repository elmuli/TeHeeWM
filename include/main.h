#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/log.h>
#include <xkbcommon/xkbcommon.h>

#include "lib/clay.h"

#ifndef MAIN_H
#define MAIN_H

typedef struct wm_server wm_server;
typedef struct wm_output wm_output;
typedef struct wm_toplevel wm_toplevel;
typedef struct wm_popup wm_popup;
typedef struct wm_keyboard wm_keyboard;

struct wm_server{
	struct wl_display *wl_display;
	struct wlr_backend *backend;
	struct wlr_renderer *renderer;
	struct wlr_allocator *allocator;
	struct wlr_scene *scene;
	struct wlr_scene_output_layout *scene_layout;

	struct wlr_xdg_shell *xdg_shell;
	struct wl_listener new_xdg_toplevel;
	struct wl_listener new_xdg_popup;
	struct wl_list toplevels;

	struct wlr_cursor *cursor;
	struct wlr_xcursor_manager *cursor_mgr;
	struct wl_listener cursor_motion;
	struct wl_listener cursor_motion_absolute;
	struct wl_listener cursor_button;
	struct wl_listener cursor_axis;
	struct wl_listener cursor_frame;

	struct wlr_seat *seat;
	struct wl_listener new_input;
	struct wl_listener request_set_selection;
	struct wl_listener pointer_focus_change;
	struct wl_list keyboards;

	struct wlr_output_layout *output_layout;
	struct wl_list outputs;
	struct wl_listener new_output;
};

struct wm_output {
	struct wl_list link;
	struct wm_server *server;
	struct wlr_output *wlr_output;
	struct wl_listener frame;
	struct wl_listener request_state;
	struct wl_listener destroy;
};

struct wm_toplevel {
	struct wl_list link;
	struct wm_server *server;
	struct wlr_xdg_toplevel *xdg_toplevel;
	struct wlr_scene_tree *scene_tree;
	struct wl_listener map;
	struct wl_listener unmap;
	struct wl_listener commit;
	struct wl_listener destroy;
	struct wl_listener request_move;
	struct wl_listener request_resize;
	struct wl_listener request_maximize;
	struct wl_listener request_fullscreen;
};

struct wm_popup {
	struct wlr_xdg_popup *xdg_popup;
	struct wl_listener commit;
	struct wl_listener destroy;
};

struct wm_keyboard {
	struct wl_list link;
	struct wm_server *server;
	struct wlr_keyboard *wlr_keyboard;

	struct wl_listener modifiers;
	struct wl_listener key;
	struct wl_listener destroy;
};

typedef struct keybind{
    xkb_keysym_t key_sym;
    char *cmd;
    int cmd_len;
    void (*action)(void *self, wm_server *action);
} keybind;

typedef struct config{
    int windowGap;
    int windowPadding;
    int containerGap;
    int containerPadding;
    keybind **binds;
    uint32_t mod_key;
    size_t keybind_count;
} config;

extern config *wm_config;

config *ReadConfigFile(const char*);

void exit_wm(void *self, wm_server *server);
void exec_cmd(void *self, wm_server *server);
void cycle_toplevel(void *self, wm_server *server);
void change_container(void *self, wm_server *server);
void create_vertical_container(void *self, wm_server *server);
void create_horizontal_container(void *self, wm_server *server);

#define MAX_WINDOW_COUNT 

typedef struct window{
    Clay_ElementId clay_id;
    struct wm_toplevel* toplevel;
    int posx, posy;
    int sizex, sizey;
    int parentContainerIndex;
} window;

typedef struct container{
    Clay_ElementDeclaration layoutConfig;
    Clay_ElementId clay_id;
    int windowCount;
    struct window **windows;
} container;

Clay_ElementDeclaration containerLayoutConfigVertical();
Clay_ElementDeclaration containerLayoutConfigHorizontal();
extern struct container **containers;

extern int containerCount;
extern int selectedContainerIndex;

extern const char *socket;

Clay_RenderCommandArray CreateClayLayout();
void WM_RenderClay();
void CreateContainer(Clay_ElementDeclaration);
void focus_next_container(int);
void removeWindowFromArray(int, int);
void removeContainerFromArray(int);

#endif // !MAIN_H

