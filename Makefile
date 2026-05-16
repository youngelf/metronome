
CXX = g++
CXXFLAGS = -std=c++11 -Wall $(shell pkg-config --cflags dbus-1)
LDFLAGS = -lpthread
NCURSES_LDFLAGS = -lncurses
SDL_LDFLAGS = -lSDL2 -lSDL2_mixer

# On Debian/Ubuntu, run:
# sudo apt-get install libsdl2-dev libsdl2-mixer-dev libncurses5-dev

all: metronome

metronome: main.o metronome.o
	$(CXX) $(CXXFLAGS) -o metronome main.o metronome.o $(LDFLAGS)

main.o: main.cpp metronome.h
	$(CXX) $(CXXFLAGS) -c main.cpp

metronome.o: metronome.cpp metronome.h
	$(CXX) $(CXXFLAGS) -c metronome.cpp

metronome_audio: audio_main.o metronome.o audio_tick.o
	$(CXX) $(CXXFLAGS) -o metronome_audio audio_main.o metronome.o audio_tick.o $(LDFLAGS) $(SDL_LDFLAGS)

audio_main.o: main.cpp metronome.h audio_tick.h
	$(CXX) $(CXXFLAGS) -DUSE_AUDIO -c main.cpp -o audio_main.o

audio_tick.o: audio_tick.cpp audio_tick.h
	$(CXX) $(CXXFLAGS) -c audio_tick.cpp

metronome_ncurses: ncurses_main.o metronome.o audio_tick.o
	$(CXX) $(CXXFLAGS) -o metronome_ncurses ncurses_main.o metronome.o audio_tick.o $(LDFLAGS) $(SDL_LDFLAGS) $(NCURSES_LDFLAGS) $(shell pkg-config --libs dbus-1)

ncurses_main.o: ncurses_main.cpp metronome.h audio_tick.h
	$(CXX) $(CXXFLAGS) -c ncurses_main.cpp

clean:
	rm -f *.o metronome metronome_audio metronome_ncurses
