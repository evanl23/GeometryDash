#include <stdio.h>
#include <ncurses.h> // Library for handling text-based interfaces in the terminal
#include <unistd.h> // Provides access for usleep() to control game speed

#define SCREEN_WIDTH 120
#define LEVEL_HEIGHT 18
#define MAX_LEVEL_LENGTH 5000
#define PLAYER_CHAR '@'
#define GROUND_CHAR '_'
#define SPIKE_CHAR 'A'
#define DOWN_SPIKE 'V'
#define SMALL_SPIKE '*'
#define BEAM '|'
#define EMPTY_CHAR ' '
#define GAME_OVER '%'

#define esc 27
#define space 32

// Color pairs
#define COLOR_PLAYER 1
#define COLOR_GROUND 2
#define COLOR_SPIKE 3
#define COLOR_OBSTACLE 4
#define COLOR_STAR 5
#define COLOR_GOAL 6
#define COLOR_BEAM 7
#define TEXT_COLOR 8

// Game variables
int player_x = 2, player_y = LEVEL_HEIGHT-1;
float velocity = 0.0;
int is_jumping = 0; // Variable for tracking if player if jumping
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

// Initialize colors
void init_colors() {
    start_color();
    
    // Define color pairs
    init_pair(COLOR_PLAYER, COLOR_CYAN, COLOR_BLACK);     // Player - blue
    init_pair(COLOR_GROUND, COLOR_WHITE, COLOR_BLACK);    // Ground - white
    init_pair(COLOR_SPIKE, COLOR_RED, COLOR_BLACK);       // Spikes - red
    init_pair(COLOR_STAR, COLOR_MAGENTA, COLOR_BLACK);    // Stars - magenta
    init_pair(COLOR_GOAL, COLOR_GREEN, COLOR_BLACK);      // Goal - green
    init_pair(TEXT_COLOR, COLOR_YELLOW, COLOR_BLACK);     // Text - yellow
}

// Function to render the game
void render() {
    clear(); // Clears screen 
    
    // Draw the level with colors
    for (int y = 0; y < LEVEL_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH - 2; x++) {
            if (x + level_offset < MAX_LEVEL_LENGTH) {
                char ch = level[y][x + level_offset];
                
                // Set color based on character
                if (ch == GROUND_CHAR || ch == '_' || ch == '|' || ch == '/' || ch == '\\') {
                    attron(COLOR_PAIR(COLOR_GROUND));
                } else if (ch == SPIKE_CHAR || ch == 'A' || ch == '*' || ch == 'V') {
                    attron(COLOR_PAIR(COLOR_SPIKE));
                } else if (ch == '.' || ch == '$') {
                    attron(COLOR_PAIR(COLOR_STAR));
                } else if (ch == GAME_OVER || ch == '%') {
                    attron(COLOR_PAIR(COLOR_GOAL));
                } else {
                    attron(COLOR_PAIR(COLOR_GROUND)); // Default color
                }
                
                mvaddch(y, x, ch);
                attroff(COLOR_PAIR(COLOR_GROUND));
                attroff(COLOR_PAIR(COLOR_SPIKE));
                attroff(COLOR_PAIR(COLOR_STAR));
                attroff(COLOR_PAIR(COLOR_GOAL));
            }
        }
    }
    
    // Draw the player with color
    attron(COLOR_PAIR(COLOR_PLAYER));
    mvaddch(player_y, player_x, PLAYER_CHAR);
    attroff(COLOR_PAIR(COLOR_PLAYER));

    // Draw progress bar
    int progress = ((level_offset * 100) / level_length);
    mvprintw(1, (SCREEN_WIDTH-12)/2, "Progress: %d%%", progress); // Print progress bar
    
    refresh(); // Updates screen with new frame
}

// Function to update game state
void update() {
    if (game_over) return;

    // Apply jump
    // if (is_jumping) {
    //     player_y += -1;
    //     velocity += 0.65;
    //     if (velocity > -1) {is_jumping = 0;}
    // }

    // Apply gravity
    if (level[player_y][player_x + level_offset] == EMPTY_CHAR) {
        velocity += 0.65;
        player_y += (velocity > 1) ? 1 : 0;

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
    else if (level[player_y][player_x + level_offset] == DOWN_SPIKE) {
        game_over = 1;
        success = 0;
    } else if (level[player_y][player_x + level_offset] == GAME_OVER) {
        game_over = 1;
        success = 1;
    }

    // Move the level
    level_offset++;
}

// Main function
int main() {
    initscr(); // Starts ncurses mode
    init_colors(); // Initialize color support

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
        load_level("levels/stereomadness.txt");

        while (!game_over) {
            render();
            
            // Handle input
            int ch = getch();
            if ((ch == ' ' || ch == KEY_UP) && level[player_y][player_x+level_offset] == GROUND_CHAR) {
                velocity = -2.2;
                is_jumping = 1;
                player_y += velocity;
            }

            update();
            usleep(40000); // Control game speed
        }

        // Game Over Message
        if (success) {
            attron(COLOR_PAIR(TEXT_COLOR));
            mvprintw(10, (SCREEN_WIDTH-36)/2, "Level Complete! Hit any key to exit.");
            attroff(COLOR_PAIR(TEXT_COLOR));
            nodelay(stdscr, FALSE); // Waits for key before exiting
            getch(); // Waits for input before closing 
            break;
        } else {
            attron(COLOR_PAIR(TEXT_COLOR));
            mvprintw(10, (SCREEN_WIDTH-64)/2, "Game Over! Hit the space bar to try again, press escape to exit.");
            attroff(COLOR_PAIR(TEXT_COLOR));
            nodelay(stdscr, FALSE); // Waits for key before exiting

            int key;
            do {
                key = getch();
            } while (key != esc && key != space);
            
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
