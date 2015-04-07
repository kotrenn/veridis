OBJS = main.o common.o geometry.o vector2d.o surface.o\
       datamanager.o resourcemanager.o datfile.o timer.o\
       manager.o consolemanager.o console.o\
       editormanager.o actorselector.o\
       camera.o level.o tile.o\
       actor.o actorfactory.o bullet.o particle.o item.o
FLAGS = -O0 -g -Wall -Wextra
LINKS = -lm -lmingw32 -lSDLmain -lSDL -lSDL_image -lSDL_mixer -lSDL_ttf

veridis: ${OBJS}
	g++ -o veridis ${OBJS} ${FLAGS} ${LINKS}

.cpp.o: $<
	-g++ ${FLAGS} -c -o $@ $<