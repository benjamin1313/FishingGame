
testGame: main.o
	g++ main.o -o FishingGame.exe -O1 -Wall -Wno-missing-braces -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows

main.o: main.cpp
	g++ -c main.cpp


clean:
	rm *.o
