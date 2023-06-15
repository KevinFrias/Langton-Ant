clear
g++ GameOfLife.cpp -O3 -o game `pkg-config --cflags --libs gtk+-3.0` -lsfml-graphics -lsfml-window -lsfml-system
./game
rm game
