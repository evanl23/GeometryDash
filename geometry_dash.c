#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>

#define SCREEN_WIDTH 50
#define LEVEL_HEIGHT 10
#define MAX_LEVEL_LENGTH 1000
#define PLAYER_CHAR '@'
#define GROUND_CHAR '_'
#define SPIKE_CHAR 'A'
#define EMPTY_CHAR ' '

// Game variables
int player_x = 2, player_y = LEVEL_HEIGHT-1;
int velocity = 0;
int level_offset = 0;
int level_length = 0; // Total length of level for progress tracking
int game_over = 0;

// Level representation
char level[LEVEL_HEIGHT][MAX_LEVEL_LENGTH + 1];  // Storage for the level
// Load level from file
void load_level(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open level file");
        return;
    }
    
    int i = 0;
    while (fgets(level[i], MAX_LEVEL_LENGTH, file) && i < LEVEL_HEIGHT) {
        int len = 0;
        while (level[i][len] != '\0' && level[i][len] != '\n') len++; // Get line length
        if (len > level_length) level_length = len; // Track longest row as level length
        i++;
    }

    fclose(file);
}

// Function to render the game
void render() {
    clear();
    
    // Draw the level
    for (int y = 0; y < LEVEL_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH - 2; x++) {
            if (x + level_offset < MAX_LEVEL_LENGTH) {
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
    if (player_y < LEVEL_HEIGHT - 1) {
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

    // Load level
    load_level("stereomadness.txt");

    while (!game_over) {
        render();
        
        // Handle input
        int ch = getch();
        if (ch == ' ') {
            velocity = -3.0;
            player_y += velocity/2;
        }

        update();
        usleep(50000); // Control game speed
    }

    // Game Over Message
    mvprintw(LEVEL_HEIGHT + 1, 0, "Game Over! Press any key to exit.");
    nodelay(stdscr, FALSE);
    getch();

    endwin();
    return 0;
}
