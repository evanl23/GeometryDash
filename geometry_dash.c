#include <stdio.h>
#include <ncurses.h> // Library for handling text-based interfaces in the terminal
#include <unistd.h> // Provides access for usleep() to control game speed

#define SCREEN_WIDTH 120
#define LEVEL_HEIGHT 20
#define MAX_LEVEL_LENGTH 2000
#define PLAYER_CHAR '@'
#define GROUND_CHAR '_'
#define SPIKE_CHAR 'A'
#define SMALL_SPIKE '*'
#define BEAM '|'
#define EMPTY_CHAR ' '

#define esc 27
#define space 32

// Game variables
int player_x = 2, player_y = LEVEL_HEIGHT-1;
float velocity = 0.0;
int level_offset = 0;
int level_length = 0; // Total length of level for progress tracking
int game_over = 0;
int success = 0;

// Level representation
char level[LEVEL_HEIGHT][MAX_LEVEL_LENGTH + 1];  // Storage for the level
// Load level from file
void load_level(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open level file");
        return;
    }

    for (int x = 0; x < LEVEL_HEIGHT; x++) {
        for (int y = 0; y < MAX_LEVEL_LENGTH; y++) {
            level[x][y] = EMPTY_CHAR;
        }
        level[x][MAX_LEVEL_LENGTH] = '\0';
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
    clear(); // Clears screen 
    
    // Draw the level
    for (int y = 0; y < LEVEL_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH - 2; x++) {
            if (x + level_offset < MAX_LEVEL_LENGTH) {
                mvaddch(y, x, level[y][x + level_offset]); // Draws level
            }
        }
    }
    
    // Draw the player
    mvaddch(player_y, player_x, PLAYER_CHAR); // Draws player

    // Draw progress bar
    int progress = ((level_offset * 100) / level_length);
    mvprintw(LEVEL_HEIGHT, 0, "Progress: %d%%", progress); // Print progress bar
    
    refresh(); // Updates screen with new frame
}

// Function to update game state
void update() {
    if (game_over) return;

    // Apply gravity
    if (level[player_y][player_x + level_offset] == EMPTY_CHAR) {
        velocity += 0.80;
        player_y += (velocity > 1) ? 1 : 0;

        // // Stop the player when hitting the ground
        // if (player_y >= LEVEL_HEIGHT) {
        //     player_y = LEVEL_HEIGHT;  // Lock position to ground
        //     velocity = 0;  // Stop movement
        // }
    } else {
        velocity = 0;
    }

    // Collision detection
    if (level[player_y][player_x + level_offset] == SPIKE_CHAR) {
        game_over = 1;
        success = 0;
    }
    else if (level[player_y][player_x + level_offset] == SMALL_SPIKE) {
        game_over = 1;
        success = 0;
    }
    else if (level[player_y][player_x + level_offset] == BEAM) {
        game_over = 1;
        success = 0;
    }

    // Move the level
    level_offset++;
    if (level_offset >= level_length) {
        game_over = 1;
        success = 1;
    }
}

// Main function
int main() {
    initscr(); // Starts ncurses mode

    while(1) {
        noecho(); // Prevents input from being displayed on screen 
        curs_set(FALSE); // Hides curser
        nodelay(stdscr, TRUE); // Enables non-blocking input
        keypad(stdscr, TRUE); // Allows keyboard input

        // Game variables
        player_x = 2, player_y = LEVEL_HEIGHT-1;
        velocity = 0.0;
        level_offset = 0;
        level_length = 0; // Total length of level for progress tracking
        game_over = 0;
        success = 0;

        // Load level
        load_level("stereomadness.txt");

        while (!game_over) {
            render();
            
            // Handle input
            int ch = getch();
            if (ch == ' ' && level[player_y][player_x+level_offset] == GROUND_CHAR) {
                velocity = -2.5;
                player_y += velocity;
                
            }

            update();
            usleep(40000); // Control game speed
        }

        // Game Over Message
        if (success) {
            mvprintw(LEVEL_HEIGHT + 1, 0, "Level Complete! Hit any key to exit.");
            nodelay(stdscr, FALSE); // Waits for key before exiting
            getch(); // Waits for input before closing 
            break;
        } else {
            mvprintw(LEVEL_HEIGHT + 1, 0, "Game Over! Hit the space bar to try again, press escape to exit.");
            nodelay(stdscr, FALSE); // Waits for key before exiting
            char key = getch();
            if (key == esc) {
                break;
            } else if (key == space) {
                continue;
            }
        }
    }
    endwin();
    return 0;
}

// Blocks 4 _ wide, 4 _ in bew