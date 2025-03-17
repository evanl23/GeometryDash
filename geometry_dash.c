#include <ncurses.h>
#include <unistd.h>

#define LEVEL_WIDTH 50
#define LEVEL_HEIGHT 10
#define PLAYER_CHAR '@'
#define GROUND_CHAR '_'
#define SPIKE_CHAR '^'
#define EMPTY_CHAR ' '

// Game variables
int player_x = 2, player_y = 5;
int velocity = 0;
int level_offset = 0;
int level_length = LEVEL_WIDTH - 2; // Total length of level for progress tracking
int game_over = 0;

// Level representation
char level[LEVEL_HEIGHT][LEVEL_WIDTH + 1] = {
    "_________________________________________________",
    "_________________________________________________",
    "                                                 ",
    "                                                 ",
    "                         ^                       ",
    "                    ^^^                          ",
    "                                                 ",
    "_________________________________________________",
    "_________________________________________________",
    "                                                 "
};

// Function to render the game
void render() {
    clear();
    
    // Draw the level
    for (int y = 0; y < LEVEL_HEIGHT; y++) {
        for (int x = 0; x < LEVEL_WIDTH - 2; x++) {
            if (x + level_offset < LEVEL_WIDTH) {
                mvaddch(y, x, level[y][x + level_offset]);
            }
        }
    }
    
    // Draw the player
    mvaddch(player_y, player_x, PLAYER_CHAR);

    // Draw progress bar
    int progress = ((level_offset * 100) / level_length);
    mvprintw(LEVEL_HEIGHT, 0, "Progress: %d%%", progress);
    
    refresh();
}

// Function to update game state
void update() {
    if (game_over) return;

    // Apply gravity
    if (player_y < LEVEL_HEIGHT - 2) {
        velocity += 1;
        player_y += (velocity > 1) ? 1 : 0;
    }

    // Collision detection
    if (level[player_y][player_x + level_offset] == SPIKE_CHAR) {
        game_over = 1;
    }

    // Move the level
    level_offset++;
    if (level_offset >= level_length) {
        game_over = 1;
    }
}

// Main function
int main() {
    initscr();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    while (!game_over) {
        render();
        
        // Handle input
        int ch = getch();
        if (ch == ' ') {
            velocity = -2;
            player_y += velocity;
        }

        update();
        usleep(100000); // Control game speed
    }

    // Game Over Message
    mvprintw(LEVEL_HEIGHT + 1, 0, "Game Over! Press any key to exit.");
    nodelay(stdscr, FALSE);
    getch();

    endwin();
    return 0;
}
