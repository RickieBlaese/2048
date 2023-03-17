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
            cells[i] = new std::int64_t[y];
        }
    }

    ~Game() {
        for (std::int32_t i = 0; i < x; i++) {
            delete[] cells[i];
        }
        delete[] cells;
    }

    void move(const Direction& direction) {
        if (direction == Direction::up) {
            for (std::int32_t i = 1; i < x; i++) {
                for (std::int32_t j = 0; j < y; j++) {
                    if (cells[i][j] <= 0) { continue; }
                    std::int32_t z = i - 1;
                    while (z >= 0) {
                        if (cells[z][j] > 0) { break; }
                        z--;
                    }
                    if (z < 0) { z = 0; }
                    if (cells[z][j] == cells[i][j]) {
                        cells[z][j]++;
                        cells[i][j] = 0;
                    } else if (cells[z][j] <= 0) {
                        cells[z][j] = cells[i][j];
                        cells[i][j] = 0;
                    } else if (cells[z + 1][j] <= 0) {
                        cells[z + 1][j] = cells[i][j];
                        cells[i][j] = 0;
                    }
                }
            }

        } else if (direction == Direction::down) {
            for (std::int32_t i = x - 2; i > 0; i--) {
                for (std::int32_t j = 0; j < y; j++) {
                    if (cells[i][j] <= 0) { continue; }
                    std::int32_t z = i + 1;
                    while (z < x) {
                        if (cells[z][j] > 0) { break; }
                        z++;
                    }
                    if (z >= x) { z = x - 1; }
                    if (cells[z][j] == cells[i][j]) {
                        cells[z][j]++;
                        cells[i][j] = 0;
                    } else if (cells[z][j] <= 0) {
                        cells[z][j] = cells[i][j];
                        cells[i][j] = 0;
                    } else if (cells[z - 1][j] <= 0) {
                        cells[z - 1][j] = cells[i][j];
                        cells[i][j] = 0;
                    }

                }
            }

        } else if (direction == Direction::left) {
            for (std::int32_t j = 1; j < y; j++) {
                for (std::int32_t i = 0; i < x; i++) {
                    if (cells[i][j] <= 0) { continue; }
                    std::int32_t z = j - 1;
                    while (z >= 0) {
                        if (cells[i][z] > 0) { break; }
                        z--;
                    }
                    if (z < 0) { z = 0; }
                    if (cells[i][z] == cells[i][j]) {
                        cells[i][z]++;
                        cells[i][j] = 0;
                    } else if (cells[i][z] <= 0) {
                        cells[i][z] = cells[i][j];
                        cells[i][j] = 0;
                    } else if (cells[i][z + 1] <= 0) {
                        cells[i][z + 1] = cells[i][j];
                        cells[i][j] = 0;
                    }

                }
            }

        } else if (direction == Direction::right) {
            for (std::int32_t j = y - 2; j > 0; j--) {
                for (std::int32_t i = 0; i < x; i++) {
                    if (cells[i][j] <= 0) { continue; }
                    std::int32_t z = j + 1;
                    while (z < y) {
                        if (cells[i][z] > 0) { break; }
                        z++;
                    }
                    if (z >= y) { z = y - 1; }
                    if (cells[i][z] == cells[i][j]) {
                        cells[i][z]++;
                        cells[i][j] = 0;
                    } else if (cells[i][z] <= 0) {
                        cells[i][z] = cells[i][j];
                        cells[i][j] = 0;
                    } else if (cells[i][z - 1] <= 0) {
                        cells[i][z - 1] = cells[i][j];
                        cells[i][j] = 0;
                    }

                }
            }
        }
    }


    void out(WINDOW *win) {
        std::size_t max_size = 0, this_size = 0;
        for (std::int32_t i = 0; i < x; i++) {
            for (std::int32_t j = 0; j < y; j++) {
                this_size = std::to_string(static_cast<std::int64_t>(std::pow(2, cells[i][j]))).length();
                if (this_size > max_size) {
                    max_size = this_size;
                }
            }
        }

        for (std::int32_t i = 0; i < x; i++) {
            for (std::int32_t j = 0; j < y; j++) {
                if (cells[i][j] == 0) { continue; }
                std::string this_str = std::to_string(static_cast<std::int64_t>(std::pow(2, cells[i][j])));
                mvwaddstr(win, i, j * (max_size + 2) + (max_size + 2 - this_str.length()) / 2, this_str.c_str());
            }
        }
    }

};


int main() {
    static constexpr std::int32_t max_input_size = 50; /* this is a number input str so this is more than large enough */
    static constexpr std::int32_t min_random_spawn_count = 1;
    static constexpr std::int32_t max_random_spawn_count = 3;
    static constexpr std::int32_t min_random_spawn = 1;
    static constexpr std::int32_t max_random_spawn = 4;

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

    /* now we are down with full string input */
    curs_set(0);
    noecho();

    Game game(x, y);

    std::random_device device{};
    std::default_random_engine engine(device());

    const std::int32_t rscount = std::uniform_int_distribution<>(min_random_spawn_count, max_random_spawn_count)(engine);
    std::uniform_int_distribution<> xdist(0, x - 1);
    std::uniform_int_distribution<> ydist(0, y - 1);
    std::uniform_int_distribution<> powerdist(min_random_spawn, max_random_spawn);

    for (std::int32_t i = 0; i < rscount + 2; i++) {
        game.cells[xdist(engine)][ydist(engine)] = powerdist(engine);
    }

    wclear(win);
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

        /*
        std::int32_t exist_count = 0;
        for (std::int32_t i = 0; i < x; i++) {
            for (std::int32_t j = 0; j < y; j++) {
                if (game.cells[i][j] > 0) { exist_count++; }
                if (exist_count >= x * y) {
                    break;
                }
            }
        }

        std::int32_t rx = 0, ry = 0;
        if (exist_count < x * y) {
            for (std::int32_t i = 0; i < rscount; i++) {
                rx = xdist(engine);
                ry = ydist(engine);
                while (game.cells[rx][ry] > 0) {
                    rx = xdist(engine);
                    ry = ydist(engine);
                }

                game.cells[rx][ry] = powerdist(engine);
            }
        }
        */

        wclear(win);
        game.out(win);
        wrefresh(win);

    }

    deinit_ncurses(win);
    return 0;

}
