
#include <ncurses.h>
#include "metronome.h"
#include "audio_tick.h"
#include <thread>
#include <string>
#include <dbus/dbus.h>

Metronome metronome;
bool is_running = false;
DBusConnection* dbus_conn;

// This function will be run in a separate thread
void metronome_thread_func() {
    metronome.start();
}

bool handle_dbus_message() {
    dbus_connection_read_write(dbus_conn, 0);
    DBusMessage* msg = dbus_connection_pop_message(dbus_conn);

    if (msg == NULL) {
        return false;
    }

    if (dbus_message_is_signal(msg, "org.example.Metronome", "Toggle")) {
        if (is_running) {
            metronome.stop();
            is_running = false;
        } else {
            metronome.start();
            is_running = true;
        }
        dbus_message_unref(msg);
        return true;
    }
    dbus_message_unref(msg);
    return false;
}

int main(int argc, char *argv[]) {
    int tempo = 120;
    int beats_per_measure = 4;
    int note_value = 4;

    // Allow setting tempo/time signature from command line
    if (argc == 4) {
        tempo = std::stoi(argv[1]);
        beats_per_measure = std::stoi(argv[2]);
        note_value = std::stoi(argv[3]);
    }

    metronome.set_tempo(tempo);
    metronome.set_time_signature(beats_per_measure, note_value);
    metronome.set_tick_callback(on_tick);

    audio_setup();

    // D-Bus setup
    DBusError err;
    dbus_error_init(&err);
    dbus_conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "D-Bus connection error: %s\n", err.message);
        dbus_error_free(&err);
    }
    if (dbus_conn != NULL) {
        dbus_bus_add_match(dbus_conn, "type='signal',interface='org.example.Metronome'", &err);
        if (dbus_error_is_set(&err)) {
            fprintf(stderr, "D-Bus match error: %s\n", err.message);
            dbus_error_free(&err);
        }
    }

    initscr();             // Start curses mode
    cbreak();              // Line buffering disabled
    noecho();              // Don't echo() while we do getch
    keypad(stdscr, TRUE);  // Enable function keys
    nodelay(stdscr, TRUE); // Make getch non-blocking

    bool quit = false;
    bool dirty = true; // Flag to indicate that the screen needs to be redrawn
    while (!quit) {
        if (dbus_conn != NULL) {
            if (handle_dbus_message()){
                dirty = true;
            }
        }

        if (dirty)
        {
            // Print the UI
            mvprintw(0, 0, "--- Metronome TUI ---");
            mvprintw(2, 0, "Tempo:          %d BPM", tempo);
            mvprintw(3, 0, "Time Signature: %d/%d", beats_per_measure, note_value);
            mvprintw(6, 0, "--- Controls ---");
            mvprintw(7, 0, "  [Space]   Play/Pause");
            mvprintw(8, 0, "  [q]       Quit");
            mvprintw(9, 0, "  [+ = > .] Increase Tempo (+5)");
            mvprintw(10, 0, "  [- _ < ,] Decrease Tempo (-5)");
            mvprintw(12, 0, "D-Bus signal: org.example.Metronome.Toggle");
            
            // Update status
            mvprintw(4, 0, "Status:         %s", is_running ? "Running" : "Paused");

            // Refresh the screen
            refresh();
            dirty = false;
        }

        // Handle input
        int ch = getch();
        switch (ch) {
            case 'q':
                if (is_running) {
                    metronome.stop();
                }
                quit = true;
                break;
            case ' ':
                if (is_running) {
                    metronome.stop();
                    is_running = false;
                } else {
                    metronome.start();
                    is_running = true;
                }
                dirty = true;
                break;
            case KEY_UP:
            case '+':
            case '=':
            case '>':
            case '.':
                tempo += 5;
                metronome.set_tempo(tempo);
                dirty = true;
                break;
            case KEY_DOWN:
            case ',':
            case '<':
            case '-':
            case '_':
                if (tempo > 20) { // Prevent tempo from going too low
                    tempo -= 5;
                    metronome.set_tempo(tempo);
                }
                dirty = true;
                break;
        }

        // A short sleep to prevent the loop from spinning too fast
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // Clean up
    if (dbus_conn != NULL) {
        dbus_connection_close(dbus_conn);
    }
    endwin(); // End curses mode
    audio_cleanup();

    return 0;
}
