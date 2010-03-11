CFLAGS=`pkg-config --cflags gtk+-2.0` -O0 -Wall -g
LDFLAGS= `pkg-config --libs gtk+-2.0`

all: grub-menu

grub-menu: grub-menu.o grub-menu-main.o
grub-menu.o: grub-menu.c grub-menu.h
grub-menu-main.o: grub-menu-main.c grub-menu.h
