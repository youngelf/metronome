
#include "metronome.h"
#include <iostream>
#include <thread>
#include <chrono>

Metronome::Metronome() : tempo(120), beats_per_measure(4), note_value(4), running(false), tick_callback(nullptr) {
    recalculate_beat_duration();
}

void Metronome::recalculate_beat_duration() {
    beat_duration = std::chrono::milliseconds((int)(60000.0 / tempo * (4.0 / note_value)));
}

void Metronome::set_tempo(int bpm) {
    tempo = bpm;
    recalculate_beat_duration();
}

void Metronome::set_time_signature(int beats_per_measure, int note_value) {
    this->beats_per_measure = beats_per_measure;
    this->note_value = note_value;
    recalculate_beat_duration();
}

void Metronome::set_tick_callback(std::function<void(bool)> cb) {
    tick_callback = cb;
}

void Metronome::start() {
    if (running) {
        return;
    }
    running = true;
    std::thread t(&Metronome::tick, this);
    t.detach();
}

void Metronome::stop() {
    running = false;
}

void Metronome::tick() {
    int beat_count = 0;
    while (running) {
        if (tick_callback) {
            tick_callback(beat_count == 0);
        }
        beat_count = (beat_count + 1) % beats_per_measure;
        std::this_thread::sleep_for(beat_duration);
    }
}
