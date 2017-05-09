.PHONY: all clean run

CC=gcc
STD="-ansi"

all: blastit

clean:
	-rm -f *.o *~ blastit

ALLEGRO_PKG_LIBS="allegro-5.0 allegro_font-5.0 allegro_image-5.0 allegro_primitives-5.0"

%.o: %.c *.h
	$(CC) $(CFLAGS) `pkg-config --cflags $(ALLEGRO_PKG_LIBS)` -c -g -O0 ${STD} $< -o $@

blastit: blastit-c/game.o blastit-c/gui.o blastit-c/highscores.o blastit-c/level.o blastit-c/main.o
	$(CC) $(LDFLAGS) $^ -o $@ -g `pkg-config --libs --static $(ALLEGRO_PKG_LIBS)`

run: blastit
	./blastit

