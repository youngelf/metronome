
#include "metronome.h"
#include <iostream>
#include <thread>
#include <chrono>

#ifdef USE_AUDIO
#include "audio_tick.h"
#else
void on_tick(bool is_accented) {
    if (is_accented) {
        std::cout << "BEEP\a" << std::endl;
    } else {
        std::cout << "boop\a" << std::endl;
    }
}
#endif

int main(int argc, char *argv[]) {
    Metronome metronome;

#ifdef USE_AUDIO
    audio_setup();
#endif

    if (argc == 4) {
        try {
            int tempo = std::stoi(argv[1]);
            int beats_per_measure = std::stoi(argv[2]);
            int note_value = std::stoi(argv[3]);
            metronome.set_tempo(tempo);
            metronome.set_time_signature(beats_per_measure, note_value);
            std::cout << "Starting metronome with tempo " << tempo << " and time signature " << beats_per_measure << "/" << note_value << std::endl;
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: Please provide numbers for tempo, beats per measure, and note value." << std::endl;
            return 1;
        } catch (const std::out_of_range& e) {
            std::cerr << "Argument out of range." << std::endl;
            return 1;
        }
    } else {
        metronome.set_tempo(120);
        metronome.set_time_signature(4, 4);
        std::cout << "Starting metronome with default tempo 120 and time signature 4/4." << std::endl;
        std::cout << "Usage: " << argv[0] << " <tempo> <beats_per_measure> <note_value>" << std::endl;
    }

    metronome.set_tick_callback(on_tick);
    metronome.start();

    std::cout << "Metronome started. Press enter to stop." << std::endl;
    std::cin.get();

    metronome.stop();
    std::cout << "Metronome stopped." << std::endl;

#ifdef USE_AUDIO
    audio_cleanup();
#endif

    return 0;
}
