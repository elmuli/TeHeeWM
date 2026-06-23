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
            }
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
            }
        },
    };
}

void ClayWindow(Clay_ElementId id){
    CLAY(id, {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_GROW(1) },
            .padding = CLAY_PADDING_ALL(0)
        }
    }){};
}

Clay_RenderCommandArray CreateClayLayout(){

    Clay_BeginLayout();

    CLAY(CLAY_ID("MainContainer"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = { .width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_GROW(1) },
            .padding = CLAY_PADDING_ALL(5),
            .childGap = 5
        }
    }){
        for (int k=0;k<containerCount;k++){
            CLAY(containers[k]->clay_id, containers[k]->layoutConfig){
                for (int i=0;i<containers[k]->windowCount;i++){
                    ClayWindow(containers[k]->windows[i]->clay_id);
                }
            }
        }

    };

    return Clay_EndLayout(1.0f);
}

void WM_RenderClay(){

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

}

void removeContainerFromArray(int index){
    if(index<0 || index>=containerCount){
        fprintf(stderr, "Error: Invalid container index %d\n", index);
        return;
    }

    for (int i = index; i < containerCount - 1; i++) {
        containers[i] = containers[i + 1];
    }

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
