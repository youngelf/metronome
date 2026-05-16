
#ifndef METRONOME_H
#define METRONOME_H

#include <functional>
#include <chrono>

class Metronome {
public:
    Metronome();
    void set_tempo(int bpm);
    void set_time_signature(int beats_per_measure, int note_value);
    void set_tick_callback(std::function<void(bool)> cb);
    void start();
    void stop();

private:
    void tick();
    void recalculate_beat_duration();
    int tempo;
    int beats_per_measure;
    int note_value;
    bool running;
    std::function<void(bool)> tick_callback;
    std::chrono::milliseconds beat_duration;
};

#endif // METRONOME_H
