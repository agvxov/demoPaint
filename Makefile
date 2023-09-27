main:
	g++ main.cpp -o mypaint.out $$(pkg-config --cflags --libs sdl2) # -ggdb
