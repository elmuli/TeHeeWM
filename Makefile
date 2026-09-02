PKG_CONFIG?=pkg-config

PKGS="wlroots-0.20" wayland-server xkbcommon pango pangocairo cairo
CFLAGS_PKG_CONFIG!=$(PKG_CONFIG) --cflags $(PKGS)
CFLAGS+=$(CFLAGS_PKG_CONFIG)
LIBS!=$(PKG_CONFIG) --libs $(PKGS)

INCL= -I./include/

SRCS = src/main.c src/layout.c src/config_parser.c

all:
	gcc $(SRCS) -Werror $(CFLAGS) $(LDFLAGS) $(LIBS) $(INCL) -I. -DWLR_USE_UNSTABLE -o build/test

clean:
	rm -f build/*

.PHONY: all clean
