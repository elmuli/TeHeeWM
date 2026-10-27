#include <stdlib.h>
#include <stdio.h>

#include "lib/clay.h"

int getCpuPr();
void getTime(char**);

#define STRING_ARENA_SIZE (5 * 1024)

typedef struct {
    char  buf[STRING_ARENA_SIZE];
    int   used;
} StringArena;

static StringArena g_frame_arena;

static inline void frame_arena_reset(void) {
    g_frame_arena.used = 0;
}

static inline Clay_String arena_fmt(const char *fmt, ...) {
    char *dst = g_frame_arena.buf + g_frame_arena.used;
    int   remaining = STRING_ARENA_SIZE - g_frame_arena.used;

    va_list args;
    va_start(args, fmt);
    int written = vsnprintf(dst, remaining, fmt, args);
    va_end(args);

    if (written < 0 || written >= remaining) {
        written = remaining - 1;
    }

    g_frame_arena.used += written + 1;

    return (Clay_String){ .chars = dst, .length = written };
}
