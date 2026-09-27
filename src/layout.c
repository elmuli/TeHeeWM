#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_buffer.h>
#include <wlr/interfaces/wlr_buffer.h>

#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <cairo/cairo.h>
#include <drm_fourcc.h>

#include "main.h"
#define CLAY_IMPLEMENTATION
#include "lib/clay.h"


Clay_ElementDeclaration containerLayoutConfigVertical(){
    printf("padding %d\n", wm_config->windowPadding);
    return (Clay_ElementDeclaration){
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = {wm_config->windowPadding,
                wm_config->windowPadding,
                wm_config->windowPadding,
                wm_config->windowPadding
            },
            .childGap = wm_config->windowGap
        },
        .border = { .width = { wm_config->containerBorderSize[0], 
                                wm_config->containerBorderSize[1], 
                                wm_config->containerBorderSize[2], 
                                wm_config->containerBorderSize[3], 0}, 
                    .color = {wm_config->containerBorderColor[0], 
                                wm_config->containerBorderColor[1], 
                                wm_config->containerBorderColor[2], 
                                wm_config->containerBorderColor[3]}
        },
    };
}

Clay_ElementDeclaration containerLayoutConfigHorizontal(){
    printf("padding %d\n", wm_config->windowPadding);
    return (Clay_ElementDeclaration){
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = {wm_config->windowPadding,
                wm_config->windowPadding,
                wm_config->windowPadding,
                wm_config->windowPadding
            },
            .childGap = wm_config->windowGap
        },
        .border = { .width = { wm_config->containerBorderSize[0], 
                                wm_config->containerBorderSize[1], 
                                wm_config->containerBorderSize[2], 
                                wm_config->containerBorderSize[3], 0}, 
                    .color = {wm_config->containerBorderColor[0], 
                                wm_config->containerBorderColor[1], 
                                wm_config->containerBorderColor[2], 
                                wm_config->containerBorderColor[3]}
        },
    };
}

Clay_TextElementConfig normalTextConfig = (Clay_TextElementConfig){
    .fontId = 1,
    .fontSize = 40, 
    .textColor = (Clay_Color){255, 255, 255, 255},
    .wrapMode = CLAY_TEXT_WRAP_NEWLINES
};

void ClayWindow(Clay_ElementId id){
    CLAY(id, {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_GROW(1) },
            .padding = CLAY_PADDING_ALL(wm_config->containerPadding),
            .childGap = wm_config->containerGap
        },
        .border = { .width = { wm_config->windowBorderSize[0], 
                                wm_config->windowBorderSize[1], 
                                wm_config->windowBorderSize[2], 
                                wm_config->windowBorderSize[3], 0}, 
                    .color = {wm_config->windowBorderColor[0], 
                                wm_config->windowBorderColor[1], 
                                wm_config->windowBorderColor[2], 
                                wm_config->windowBorderColor[3]}
        },
    }){};
}

Clay_RenderCommandArray CreateClayLayout(){

    Clay_BeginLayout();

    CLAY(CLAY_ID("MainContainer"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_GROW(1) },
            .padding = CLAY_PADDING_ALL(5),
            .childGap = 5
        }
    }){
        CLAY(CLAY_ID("testContainer"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_FIXED(50)},
                .padding = CLAY_PADDING_ALL(3),
                .childGap = 2
            },
            .backgroundColor = (Clay_Color){100, 10, 50, 255}
        }){
            CLAY_TEXT(CLAY_STRING("test"), normalTextConfig);
        };
        CLAY(CLAY_ID("WindowContainer"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_GROW(1)},
                .padding = CLAY_PADDING_ALL(0),
                .childGap = 0
            },
        }){
            for (int k=0;k<containerCount;k++){
                CLAY(containers[k]->clay_id, containers[k]->layoutConfig){
                    for (int i=0;i<containers[k]->windowCount;i++){
                        ClayWindow(containers[k]->windows[i]->clay_id);
                    }
                }
            }
        };

    };

    return Clay_EndLayout(1.0f);
}

void removeContainerFromArray(int index){
    if(index<0 || index>=containerCount){
        fprintf(stderr, "Error: Invalid container index %d\n", index);
        return;
    }

    for (int i = index; i < containerCount - 1; i++) {
        containers[i] = containers[i + 1];
    }

    printf("container count: %d\n", containerCount);
    containerCount--;
}

void removeWindowFromArray(int index, int parentIndex) {
    printf("removing a window %i, from container %i\n", index, parentIndex);
    struct container *container = containers[parentIndex];
    if (index < 0 || index >= container->windowCount) {
        fprintf(stderr, "Error: Invalid window index %d\n", index);
        return;
    }

    for (int i = index; i < container->windowCount - 1; i++) {
        container->windows[i] = container->windows[i + 1];
    }

    container->windowCount--;

    container->windows[container->windowCount]->toplevel = NULL;
    if (container->windowCount == 0){
        removeContainerFromArray(parentIndex);
    }
}

void CreateContainer(Clay_ElementDeclaration config){
    printf("creating a container\n");
    containers[containerCount] = malloc(sizeof(struct container));
    printf("container allocated\n");
    containers[containerCount]->layoutConfig = config;
    containers[containerCount]->clay_id = CLAY_IDI("Container%i", containerCount);
    printf("clay data created\n");
    containers[containerCount]->windowCount = 0;
    containers[containerCount]->windows = malloc(sizeof(struct window*)*40);
    printf("windwow list allocated\n");
    selectedContainerIndex = containerCount;
    containerCount++;
    printf("new container created\n");
}

void focus_next_container(int currentFocus){
    if(currentFocus >= containerCount-1){
        selectedContainerIndex = 0;
    }else{
        selectedContainerIndex = currentFocus+1;
    }
}

static struct wm_clay_border *FindClayBorder(wm_clay_ui *ui, uint32_t id){
    wm_clay_border *border;
    wl_list_for_each(border, &ui->borders, link){
        if(border->clay_id == id){
            return border;
        }
    }
    return NULL;
}

static struct wm_clay_rectangle *FindClayRectangle(wm_clay_ui *ui, int32_t id){
    wm_clay_rectangle *rectangle;
    wl_list_for_each(rectangle, &ui->rectangles, link){
        if(rectangle->clay_id == id){
            return rectangle;
        }
    }
    return NULL;
}

static struct wm_clay_border *CreateClayBorder(wm_clay_ui *ui, uint32_t id){
    wm_clay_border *border = calloc(1, sizeof(*border));
    border->clay_id = id;
    float transparent[4] = {255,0,0,255};
    for(int i=0;i<4;i++){
        border->rect[i] = wlr_scene_rect_create(ui->tree, 1, 1, transparent);
    }
    wl_list_insert(&ui->borders, &border->link);
    return border;
}

static struct wm_clay_rectangle *CreateClayRectangle(wm_clay_ui *ui, uint32_t id){
    wm_clay_rectangle *rectangle = calloc(1, sizeof(*rectangle));
    rectangle->clay_id = id;
    float transparent[4] = {255,0,0,255};
    rectangle->rect = wlr_scene_rect_create(ui->tree, 1, 1, transparent);
    wl_list_insert(&ui->rectangles, &rectangle->link);
    return rectangle;
}

static void DestroyClayBorder(wm_clay_border *border){
    for(int i=0;i<4;i++){
        wlr_scene_node_destroy(&border->rect[i]->node);
    }
    wl_list_remove(&border->link);
    free(border);
}

static void DestroyClayRectangle(wm_clay_rectangle *rectangle){
    wlr_scene_node_destroy(&rectangle->rect->node);
    wl_list_remove(&rectangle->link);
    free(rectangle);
}

static void DrawClayBorder(wm_clay_ui *ui, Clay_RenderCommand *cmd){
    Clay_BoundingBox box = cmd->boundingBox;
    //printf("boundingBox: x: %d, y: %d, width: %d, height: %d\n", (int)box.x, (int)box.y, (int)box.width, (int)box.height);
    Clay_BorderRenderData clay_border = cmd->renderData.border;

    wm_clay_border *border = FindClayBorder(ui, cmd->id);
    if(!border){
        border = CreateClayBorder(ui, cmd->id);
    }
    border->seenThisFrame = true;

    float border_color[4] = {
        clay_border.color.r / 255.0f,
        clay_border.color.g / 255.0f,
        clay_border.color.b / 255.0f,
        clay_border.color.a / 255.0f,
    };

    //printf("border width: %d, box x: %d, box y: %d, \n", clay_border.width.top, box.x, box.y);
    struct {int w, x, y, rw, rh; } edges[4] = {
        {clay_border.width.top, (int)(box.x), (int)box.y, (int)box.width, clay_border.width.top},

        {clay_border.width.right,  (int)(box.x + box.width - clay_border.width.right), (int)box.y,
            clay_border.width.right, (int)box.height },

        {clay_border.width.bottom, (int)box.x, (int)(box.y + box.height - clay_border.width.bottom),
            (int)box.width, clay_border.width.bottom },

        {clay_border.width.left, (int)box.x, (int)box.y, clay_border.width.left, (int)box.height },
    };

    for (int i=0;i<4;i++){
        //printf("edge%i: w:%d, x:%d, y:%d, rw:%d, rh:%d\n",i, edges[i].w, edges[i].x, edges[i].y, edges[i].rw, edges[i].rh);
        if(edges[i].w > 0){
            wlr_scene_rect_set_size(border->rect[i], edges[i].rw, edges[i].rh);
            wlr_scene_node_set_position(&border->rect[i]->node, edges[i].x, edges[i].y);
            wlr_scene_rect_set_color(border->rect[i], border_color);
            wlr_scene_node_set_enabled(&border->rect[i]->node, true);
        }else{
            wlr_scene_node_set_enabled(&border->rect[i]->node, false);
        }
    }
}

static void DrawClayRectangle(wm_clay_ui *ui, Clay_RenderCommand *cmd){
    Clay_BoundingBox box = cmd->boundingBox;
    Clay_RectangleRenderData config = cmd->renderData.rectangle;

    wm_clay_rectangle *rectangle = FindClayRectangle(ui, cmd->id);
    if(!rectangle){
        rectangle = CreateClayRectangle(ui, cmd->id);
    }
    rectangle->seenThisFrame = true;

    float rect_color[4] = {
        config.backgroundColor.r / 255.0f,
        config.backgroundColor.g / 255.0f,
        config.backgroundColor.b / 255.0f,
        config.backgroundColor.a / 255.0f,
    };

    wlr_scene_rect_set_size(rectangle->rect, box.width, box.height);
    wlr_scene_node_set_position(&rectangle->rect->node, box.x, box.y);
    wlr_scene_rect_set_color(rectangle->rect, rect_color);
    wlr_scene_node_set_enabled(&rectangle->rect->node, true);
}

struct text_pixel_buffer {
    struct wlr_buffer base;

    unsigned char *data;
    uint32_t format;
    size_t stride;
};

static void text_pixel_buffer_destroy(struct wlr_buffer *wlr_buffer) {
    struct text_pixel_buffer *buffer =
        wl_container_of(wlr_buffer, buffer, base);

    free(buffer->data);
    free(buffer);
}

static bool text_pixel_buffer_begin_data_ptr_access(
    struct wlr_buffer *wlr_buffer,
    uint32_t flags,
    void **data,
    uint32_t *format,
    size_t *stride) {

    struct text_pixel_buffer *buffer =
        wl_container_of(wlr_buffer, buffer, base);

    *data = buffer->data;
    *format = buffer->format;
    *stride = buffer->stride;

    return true;
}

static void text_pixel_buffer_end_data_ptr_access(
    struct wlr_buffer *wlr_buffer) {
    // Nothing to do.
}

static const struct wlr_buffer_impl text_pixel_buffer_impl = {
    .destroy = text_pixel_buffer_destroy,
    .begin_data_ptr_access = text_pixel_buffer_begin_data_ptr_access,
    .end_data_ptr_access = text_pixel_buffer_end_data_ptr_access,
};


static struct wlr_buffer *text_pixel_buffer_create(
    unsigned char *data,
    uint32_t format,
    int width,
    int height,
    size_t stride) {

    struct text_pixel_buffer *buffer = calloc(1, sizeof(*buffer));
    if (!buffer) {
        return NULL;
    }

    buffer->data = data;
    buffer->format = format;
    buffer->stride = stride;

    wlr_buffer_init(
        &buffer->base,
        &text_pixel_buffer_impl,
        width,
        height
    );

    return &buffer->base;
}

static struct wlr_scene_buffer *CreateTextNode(wm_clay_ui *ui, const char *text, int length, double fontSize, Clay_Color color) {

    float r = color.r / 255.0f;
    float g = color.g / 255.0f;
    float b = color.b / 255.0f;
    float a = color.a / 255.0f;

    /*
     * We only need a tiny Cairo surface to create a Cairo/Pango context
     * for measuring the text.
     */
    cairo_surface_t *measure_surface =
        cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1, 1);

    cairo_t *measure_cr = cairo_create(measure_surface);

    PangoLayout *layout =
        pango_cairo_create_layout(measure_cr);

    PangoFontDescription *desc =
        pango_font_description_from_string("DejaVu Sans");

    /*
     * fontSize is assumed to be pixels.
     *
     * set_size() uses points.
     * set_absolute_size() uses Pango units directly.
     */
    pango_font_description_set_absolute_size(
        desc,
        fontSize * PANGO_SCALE
    );

    pango_layout_set_font_description(layout, desc);
    pango_layout_set_text(layout, text, length);

    int w = 0;
    int h = 0;

    pango_layout_get_pixel_size(layout, &w, &h);

    g_object_unref(layout);
    cairo_destroy(measure_cr);
    cairo_surface_destroy(measure_surface);

    if (w <= 0 || h <= 0) {
        pango_font_description_free(desc);
        return NULL;
    }

    /*
     * Render the actual text.
     */
    cairo_surface_t *surface =
        cairo_image_surface_create(
            DRM_FORMAT_ARGB8888,
            w,
            h
        );

    cairo_status_t status =
        cairo_surface_status(surface);

    if (status != CAIRO_STATUS_SUCCESS) {
        pango_font_description_free(desc);
        cairo_surface_destroy(surface);
        return NULL;
    }

    cairo_t *cr = cairo_create(surface);

    if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
        pango_font_description_free(desc);
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
        return NULL;
    }

    /*
     * Make sure the background is transparent.
     */
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

    layout = pango_cairo_create_layout(cr);

    pango_layout_set_font_description(layout, desc);
    pango_layout_set_text(layout, text, length);

    cairo_set_source_rgba(cr, r, g, b, a);

    pango_cairo_show_layout(cr, layout);

    cairo_surface_flush(surface);

    int stride =
        cairo_image_surface_get_stride(surface);

    unsigned char *surface_data =
        cairo_image_surface_get_data(surface);

    /*
     * Cairo owns surface_data, so copy it into memory owned by
     * our wlr_buffer.
     */
    size_t data_size = (size_t)stride * (size_t)h;

    unsigned char *copy = malloc(data_size);

    if (!copy) {
        g_object_unref(layout);
        pango_font_description_free(desc);
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
        return NULL;
    }

    memcpy(copy, surface_data, data_size);

    struct wlr_buffer *buf =
        text_pixel_buffer_create(
            copy,
            WL_SHM_FORMAT_ARGB8888,
            w,
            h,
            stride
        );

    /*
     * We no longer need the Cairo/Pango objects.
     */
    g_object_unref(layout);
    pango_font_description_free(desc);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    if (!buf) {
        free(copy);
        return NULL;
    }

    struct wlr_scene_buffer *scene_buf =
        wlr_scene_buffer_create(ui->tree, buf);

    /*
     * wlr_scene_buffer_create() takes its own reference.
     */
    wlr_buffer_drop(buf);

    return scene_buf;
}

static void DrawText(
    wm_clay_ui *ui,
    Clay_RenderCommand *cmd) {

    Clay_TextRenderData *text =
        &cmd->renderData.text;

    printf("text: %s\n", text->stringContents.chars);

    struct wlr_scene_buffer *scene_buf =
        CreateTextNode(
            ui,
            text->stringContents.chars,
            text->stringContents.length,
            text->fontSize,
            text->textColor
        );

    if (!scene_buf) {
        return;
    }

    wlr_scene_node_set_position(
        &scene_buf->node,
        cmd->boundingBox.x,
        cmd->boundingBox.y
    );
}

static inline Clay_Dimensions MeasureText(
    Clay_StringSlice text,
    Clay_TextElementConfig *config,
    void *userData)
{
    if (!text.chars || text.length <= 0) {
        return (Clay_Dimensions){0, 0};
    }

    cairo_surface_t *surface =
        cairo_image_surface_create(
            CAIRO_FORMAT_ARGB32,
            1,
            1
        );

    cairo_t *cr = cairo_create(surface);

    PangoLayout *layout =
        pango_cairo_create_layout(cr);

    PangoFontDescription *desc =
        pango_font_description_from_string("Sans");

    pango_font_description_set_absolute_size(
        desc,
        config->fontSize * PANGO_SCALE
    );

    pango_layout_set_font_description(layout, desc);

    pango_layout_set_text(
        layout,
        text.chars,
        text.length
    );

    int w = 0;
    int h = 0;

    pango_layout_get_pixel_size(
        layout,
        &w,
        &h
    );

    pango_font_description_free(desc);
    g_object_unref(layout);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    return (Clay_Dimensions){
        .width = (float)w,
        .height = (float)h
    };
}


void WM_RenderClay(wm_clay_ui *ui, Clay_RenderCommandArray *commandArray){

    Clay_SetMeasureTextFunction(*MeasureText, ui);

    for (int k=0;k<containerCount;k++){
        container *container = containers[k];
        for(int i=0;i<container->windowCount;i++){
            window *window = container->windows[i];
            if (window->toplevel != NULL){
                wm_toplevel *toplevel = window->toplevel;
                Clay_ElementData element = Clay_GetElementData(window->clay_id);
                window->sizex = element.boundingBox.width;
                window->sizey = element.boundingBox.height;
                window->posx = element.boundingBox.x;
                window->posy = element.boundingBox.y;
                //printf("set window %i: pos %d,%d and size %dx%d\n",i,window->posx,window->posy,window->sizex,window->sizey);
                wlr_xdg_toplevel_set_size(toplevel->xdg_toplevel, window->sizex, window->sizey);
                wlr_scene_node_set_position(&toplevel->scene_tree->node, window->posx, window->posy);
            }else{
                printf("toplevel NULL, not drawing\n");
            }
        }
    }

    wm_clay_border *border, *tmp;
    wl_list_for_each(border, &ui->borders, link){
        border->seenThisFrame = false;
    }

    if(commandArray != NULL){
        for(int32_t i=0;i<commandArray->length;i++){
            Clay_RenderCommand *cmd = Clay_RenderCommandArray_Get(commandArray, i);
            switch (cmd->commandType) {
            case CLAY_RENDER_COMMAND_TYPE_BORDER:
                DrawClayBorder(ui, cmd);
                break;
            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE:
                DrawClayRectangle(ui, cmd);
                break;
            case CLAY_RENDER_COMMAND_TYPE_TEXT:
                DrawText(ui, cmd);
                break;
            default:
                break;
            }
        }
    }

    wl_list_for_each_safe(border, tmp, &ui->borders, link) {
        if(!border->seenThisFrame){
            DestroyClayBorder(border);
        }
    }

}

void ClayUiInit(wm_clay_ui *ui, struct wlr_scene_tree *parent){
    ui->tree = wlr_scene_tree_create(parent);
    wl_list_init(&ui->borders);
    wl_list_init(&ui->rectangles);
}

void ClayUiCleanup(wm_clay_ui *ui){
    wm_clay_border *border, *tmp;
    wl_list_for_each_safe(border, tmp, &ui->borders, link) {
        DestroyClayBorder(border);
    }
        wlr_scene_node_destroy(&ui->tree->node);
}

