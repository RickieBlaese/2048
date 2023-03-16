#include <iostream>
#include <string>
#include <vector>
#include <random>

#include <cmath>
#include <cstdlib>
#include <cinttypes>

#include <ncurses.h>


WINDOW *init_ncurses() {
    WINDOW *win = initscr();
    cbreak();
    keypad(win, true);
    wclear(win);
    return win;
}

void deinit_ncurses(WINDOW *win) {
    wclear(win);
    endwin();
}


enum Direction : std::uint32_t {
    up, down, left, right
};

class Game {
public:
    std::int32_t x = 0, y = 0;
    std::int64_t **cells;

    Game(std::int32_t x, std::int32_t y) : x(x), y(y), cells(new std::int64_t*[x]) {
        for (std::int32_t i = 0; i < x; i++) {
            cells[x] = new std::int64_t[y];
        }
    }

    ~Game() {
        for (std::int32_t i = 0; i < x; i++) {
            delete[] cells[x];
        }
        delete[] cells;
    }

    void move(const Direction& direction) {
        if (direction == Direction::up) {
            for (std::int32_t i = x - 1; i > 0; i--) {
                for (std::int32_t j = 0; j < y; j++) {
                    if (cells[i][j] <= 0) { continue; }
                    if (cells[i - 1][j] == cells[i][j]) {
                        cells[i - 1][j]++;
                    } else if (cells[i - 1][j] == 0) {
                        cells[i - 1][j] = cells[i][j];
                    }
                    cells[i][j] = 0;
                }
            }
        } else if (direction == Direction::down) {
            for (std::int32_t i = 0; i < x - 1; i++) {
                for (std::int32_t j = 0; j < y; j++) {
                    if (cells[i][j] <= 0) { continue; }
                    if (cells[i + 1][j] == cells[i][j]) {
                        cells[i + 1][j]++;
                    } else if (cells[i + 1][j] == 0) {
                        cells[i + 1][j] = cells[i][j];
                    }
                    cells[i][j] = 0;
                }
            }
        } else if (direction == Direction::left) {
            for (std::int32_t j = y - 1; j > 0; j--) {
                for (std::int32_t i = 0; i < x; i++) {
                    if (cells[i][j] <= 0) { continue; }
                    if (cells[i][j - 1] == cells[i][j]) {
                        cells[i][j - 1]++;
                    } else if (cells[i][j - 1] == 0) {
                        cells[i][j - 1] = cells[i][j];
                    }
                    cells[i][j] = 0;
                }
            }
        } else if (direction == Direction::right) {
            for (std::int32_t j = 0; j < x - 1 ; j++) {
                for (std::int32_t i = 0; i < x; i++) {
                    if (cells[i][j] <= 0) { continue; }
                    if (cells[i][j + 1] == cells[i][j]) {
                        cells[i][j + 1]++;
                    } else if (cells[i][j + 1] == 0) {
                        cells[i][j + 1] = cells[i][j];
                    }
                    cells[i][j] = 0;
                }
            }
        }
    }


    void out(WINDOW *win) {
        std::int32_t max_size = 0, this_size = 0;
        for (std::int32_t i = 0; i < x; i++) {
            for (std::int32_t j = 0; j < y; j++) {
                this_size = std::to_string(static_cast<std::int32_t>(std::pow(2, cells[i][j]))).length();
                if (this_size > max_size) {
                    max_size = this_size;
                }
            }
        }

        std::string this_str;
        for (std::int32_t i = 0; i < x; i++) {
            for (std::int32_t j = 0; j < y; j++) {
                this_str = static_cast<std::int32_t>(std::pow(2, cells[i][j]));
                mvwaddstr(win, i * 2, j * max_size + (max_size - this_str.length()) / 2, this_str.c_str());
            }
        }
    }

};


int main() {
    static constexpr std::int32_t max_input_size = 50; /* this is a number so this is more than large enough */

    WINDOW *win = init_ncurses();
    move(0, 0);

    char in[max_input_size];
    std::int32_t x = 0, y = 0;

    addstr("x size: ");
    wgetnstr(win, in, max_input_size);
    x = std::atoi(in);

    addstr("y size: ");
    wgetnstr(win, in, max_input_size);
    y = std::atoi(in);

    Game game(x, y);
    game.out(win);

    chtype chin = '\0';

    while ((chin = getch()) != 'q') {

        switch (chin) {
            case 'w':
            case KEY_UP:
                game.move(Direction::up);
                break;

            case 'a':
            case KEY_LEFT:
                game.move(Direction::left);
                break;

            case 's':
            case KEY_DOWN:
                game.move(Direction::down);
                break;

            case 'd':
            case KEY_RIGHT:
                game.move(Direction::right);
                break;

            default:
                continue;
        }

        wclear(win);
        game.out(win);
        wrefresh(win);
        
    }
    
    deinit_ncurses(win);
    return 0;
                
}
