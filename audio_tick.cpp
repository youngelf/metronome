
#include "audio_tick.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <cmath>

const int AMPLITUDE = 28000;
const int SAMPLE_RATE = 44100;

Mix_Chunk* generate_beep(double freq, int duration_ms) {
    int num_samples = duration_ms * (SAMPLE_RATE / 1000.0);
    int buffer_size = num_samples * sizeof(Sint16);
    Sint16* buffer = new Sint16[num_samples];

    for (int i = 0; i < num_samples; ++i) {
        double time = (double)i / SAMPLE_RATE;
        buffer[i] = (Sint16)(AMPLITUDE * sin(2.0 * M_PI * freq * time));
    }

    Mix_Chunk* chunk = new Mix_Chunk();
    chunk->allocated = 1;
    chunk->abuf = (Uint8*)buffer;
    chunk->alen = buffer_size;
    chunk->volume = 128;

    return chunk;
}

Mix_Chunk* beep_accent = nullptr;
Mix_Chunk* beep_regular = nullptr;

void audio_setup() {
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(SAMPLE_RATE, MIX_DEFAULT_FORMAT, 2, 1024);
    beep_accent = generate_beep(880.0, 100);  // A5
    beep_regular = generate_beep(440.0, 50);   // A4
}

void on_tick(bool is_accented) {
    if (is_accented) {
        Mix_PlayChannel(-1, beep_accent, 0);
    } else {
        Mix_PlayChannel(-1, beep_regular, 0);
    }
}

void audio_cleanup() {
    Mix_FreeChunk(beep_accent);
    Mix_FreeChunk(beep_regular);
    Mix_CloseAudio();
    SDL_Quit();
}
