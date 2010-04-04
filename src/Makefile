CFLAGS=`pkg-config --cflags gtk+-2.0` -O0 -Wall -g
LDFLAGS= `pkg-config --libs gtk+-2.0`

all: gchd-test grub-choose-default

grub-choose-default: grub-choose-default-main.o grub-choose-default-button-box.o grub-choose-default-window.o gchd-error.o gchd-menu.o gchd.o gchd-unix.o
	$(CC) $(LDFLAGS) -o $@ $^

gchd-test: gchd-test.o gchd-menu.o gchd-error.o gchd.o gchd-unix.o
	$(CC) $(LDFLAGS) -o $@ $^

gchd.o: gchd.c gchd.h gchd-internal.h
gchd-menu.o: gchd-menu.c gchd-menu.h gchd-error.h
gchd-unix.o: gchd-unix.c gchd-unix.h gchd-internal.h
gchd-error.o: gchd-error.c gchd-error.h

gchd-test.o: gchd-test.c gchd.h

grub-choose-default-button-box.o: grub-choose-default-button-box.c grub-choose-default-button-box.h
grub-choose-default-main.o: grub-choose-default-main.c grub-choose-default-window.h
grub-choose-default-window.o: grub-choose-default-window.c grub-choose-default-window.h

clean:
	rm *.o grub-choose-default gchd-test
