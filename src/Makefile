CFLAGS=`pkg-config --cflags gtk+-2.0` -O0 -Wall -g
LDFLAGS= `pkg-config --libs gtk+-2.0`

all: grub-menu-test grub-choose-default

grub-choose-default: grub-choose-default-main.o grub-choose-default-button-box.o gcd-error.o grub-menu.o grub-choose-default-window.o
	$(CC) $(LDFLAGS) -o $@ $^

grub-menu-test: grub-menu.o grub-menu-main.o gcd-error.o
	$(CC) $(LDFLAGS) -o $@ $^

grub-menu.o: grub-menu.c grub-menu.h gcd-error.h
grub-menu-main.o: grub-menu-main.c grub-menu.h
gcd-error.o: gcd-error.c gcd-error.h
grub-choose-default-button-box.o: grub-choose-default-button-box.c grub-choose-default-button-box.h
grub-choose-default-main.o: grub-choose-default-main.c grub-choose-default-window.h
grub-choose-default-window.o: grub-choose-default-window.c grub-choose-default-window.h
