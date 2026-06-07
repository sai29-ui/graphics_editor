#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define CANVAS_WIDTH 60
#define CANVAS_HEIGHT 20
#define MAX_SHAPES 100

// Shape definition
typedef enum {
    SHAPE_LINE,
    SHAPE_RECTANGLE,
    SHAPE_TRIANGLE,
    SHAPE_CIRCLE
} ShapeType;

typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;     // Only for triangle
    int radius;     // Only for circle
    char draw_char; // Character to draw shape with ('*', '_', etc.)
} ShapeParams;

typedef struct {
    int id;
    ShapeType type;
    ShapeParams params;
} Shape;

// Global State
char canvas[CANVAS_HEIGHT][CANVAS_WIDTH];
Shape shapes[MAX_SHAPES];
int shape_count = 0;
int next_shape_id = 1;
bool aspect_ratio_correction = true;
char bg_char = ' '; // Canvas background character

// Function declarations for drawing algorithms
void clear_canvas();
void draw_line(int x1, int y1, int x2, int y2, char c);
void draw_rectangle(int x1, int y1, int x2, int y2, char c);
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, char c);
void draw_circle(int xc, int yc, int r, char c, bool correct_aspect);
void render_shapes();

// Helper functions
const char* get_shape_name(ShapeType type) {
    switch (type) {
        case SHAPE_LINE:      return "Line";
        case SHAPE_RECTANGLE: return "Rectangle";
        case SHAPE_TRIANGLE:  return "Triangle";
        case SHAPE_CIRCLE:    return "Circle";
        default:              return "Unknown";
    }
}

void clear_canvas() {
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            canvas[y][x] = bg_char;
        }
    }
}

void draw_line(int x1, int y1, int x2, int y2, char c) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;
    
    while (1) {
        if (x1 >= 0 && x1 < CANVAS_WIDTH && y1 >= 0 && y1 < CANVAS_HEIGHT) {
            canvas[y1][x1] = c;
        }
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void draw_rectangle(int x1, int y1, int x2, int y2, char c) {
    int min_x = x1 < x2 ? x1 : x2;
    int max_x = x1 > x2 ? x1 : x2;
    int min_y = y1 < y2 ? y1 : y2;
    int max_y = y1 > y2 ? y1 : y2;

    // Draw top and bottom sides
    for (int x = min_x; x <= max_x; x++) {
        if (x >= 0 && x < CANVAS_WIDTH) {
            if (min_y >= 0 && min_y < CANVAS_HEIGHT) canvas[min_y][x] = c;
            if (max_y >= 0 && max_y < CANVAS_HEIGHT) canvas[max_y][x] = c;
        }
    }
    // Draw left and right sides
    for (int y = min_y; y <= max_y; y++) {
        if (y >= 0 && y < CANVAS_HEIGHT) {
            if (min_x >= 0 && min_x < CANVAS_WIDTH) canvas[y][min_x] = c;
            if (max_x >= 0 && max_x < CANVAS_WIDTH) canvas[y][max_x] = c;
        }
    }
}

void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, char c) {
    draw_line(x1, y1, x2, y2, c);
    draw_line(x2, y2, x3, y3, c);
    draw_line(x3, y3, x1, y1, c);
}

static void plot_circle_points(int xc, int yc, int x, int y, char c, bool correct_aspect) {
    // Standard terminal character cell aspect ratio is roughly 2:1 (height:width).
    // To make circles look circular rather than vertically stretched, we scale
    // the horizontal offset (X direction) by 2 if correction is enabled.
    int x_scale = correct_aspect ? 2 : 1;

    int points[8][2] = {
        {xc + x * x_scale, yc + y},
        {xc - x * x_scale, yc + y},
        {xc + x * x_scale, yc - y},
        {xc - x * x_scale, yc - y},
        {xc + y * x_scale, yc + x},
        {xc - y * x_scale, yc + x},
        {xc + y * x_scale, yc - x},
        {xc - y * x_scale, yc - x}
    };

    for (int i = 0; i < 8; i++) {
        int px = points[i][0];
        int py = points[i][1];
        if (px >= 0 && px < CANVAS_WIDTH && py >= 0 && py < CANVAS_HEIGHT) {
            canvas[py][px] = c;
        }
    }
}

void draw_circle(int xc, int yc, int r, char c, bool correct_aspect) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    plot_circle_points(xc, yc, x, y, c, correct_aspect);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        plot_circle_points(xc, yc, x, y, c, correct_aspect);
    }
}

void render_shapes() {
    clear_canvas();
    for (int i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        switch (s.type) {
            case SHAPE_LINE:
                draw_line(s.params.x1, s.params.y1, s.params.x2, s.params.y2, s.params.draw_char);
                break;
            case SHAPE_RECTANGLE:
                draw_rectangle(s.params.x1, s.params.y1, s.params.x2, s.params.y2, s.params.draw_char);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(s.params.x1, s.params.y1, s.params.x2, s.params.y2, s.params.x3, s.params.y3, s.params.draw_char);
                break;
            case SHAPE_CIRCLE:
                draw_circle(s.params.x1, s.params.y1, s.params.radius, s.params.draw_char, aspect_ratio_correction);
                break;
        }
    }
}

bool add_shape(ShapeType type, ShapeParams params) {
    if (shape_count >= MAX_SHAPES) {
        return false;
    }
    shapes[shape_count].id = next_shape_id++;
    shapes[shape_count].type = type;
    shapes[shape_count].params = params;
    shape_count++;
    render_shapes();
    return true;
}

bool delete_shape(int index) {
    if (index < 0 || index >= shape_count) {
        return false;
    }
    for (int i = index; i < shape_count - 1; i++) {
        shapes[i] = shapes[i + 1];
    }
    shape_count--;
    render_shapes();
    return true;
}

// ==========================================
// CLI Mode Implementation
// ==========================================

void display_canvas_cli() {
    // Draw top border
    printf(" +");
    for (int x = 0; x < CANVAS_WIDTH; x++) printf("-");
    printf("+\n");

    // Draw canvas rows
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        printf("%2d|", y);
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            putchar(canvas[y][x]);
        }
        printf("|\n");
    }

    // Draw bottom border
    printf(" +");
    for (int x = 0; x < CANVAS_WIDTH; x++) printf("-");
    printf("+\n");
    
    // Draw columns helper x-axis every 5 chars
    printf("  ");
    for (int x = 0; x < CANVAS_WIDTH; x++) {
        if (x % 10 == 0) printf("%d", x / 10);
        else if (x % 5 == 0) printf("+");
        else printf(" ");
    }
    printf("\n  ");
    for (int x = 0; x < CANVAS_WIDTH; x++) {
        printf("%d", x % 10);
    }
    printf("\n\n");
}

void list_shapes_cli() {
    if (shape_count == 0) {
        printf("No objects added yet.\n");
        return;
    }
    printf("=== Shape List ===\n");
    for (int i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        printf("[%d] ID %d: %s (char: '%c') | ", i, s.id, get_shape_name(s.type), s.params.draw_char);
        switch (s.type) {
            case SHAPE_LINE:
                printf("From (%d, %d) to (%d, %d)\n", s.params.x1, s.params.y1, s.params.x2, s.params.y2);
                break;
            case SHAPE_RECTANGLE:
                printf("Corners (%d, %d) and (%d, %d)\n", s.params.x1, s.params.y1, s.params.x2, s.params.y2);
                break;
            case SHAPE_TRIANGLE:
                printf("Vertices (%d, %d), (%d, %d), (%d, %d)\n", s.params.x1, s.params.y1, s.params.x2, s.params.y2, s.params.x3, s.params.y3);
                break;
            case SHAPE_CIRCLE:
                printf("Center (%d, %d), Radius %d\n", s.params.x1, s.params.y1, s.params.radius);
                break;
        }
    }
    printf("==================\n");
}

int read_int_cli(const char* prompt, int min_val, int max_val) {
    int val;
    char term;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &val) == 1) {
            // Check for remaining junk characters
            while ((term = getchar()) != '\n' && term != EOF);
            if (val >= min_val && val <= max_val) {
                return val;
            }
        } else {
            // Clear input buffer on error
            while ((term = getchar()) != '\n' && term != EOF);
        }
        printf("Invalid input! Please enter an integer between %d and %d.\n", min_val, max_val);
    }
}

char read_char_cli(const char* prompt) {
    char c;
    char term;
    printf("%s", prompt);
    // Read first non-whitespace character
    if (scanf(" %c", &c) != 1) {
        c = '*';
    }
    // Clean remaining buffer
    while ((term = getchar()) != '\n' && term != EOF);
    return c;
}

void run_cli_mode() {
    int choice;
    while (1) {
        printf("\n====================================\n");
        printf("     2D Graphics Editor (CLI)       \n");
        printf("====================================\n");
        printf("1. Add Line\n");
        printf("2. Add Rectangle\n");
        printf("3. Add Triangle\n");
        printf("4. Add Circle\n");
        printf("5. Delete Shape\n");
        printf("6. List Shapes\n");
        printf("7. Display Picture\n");
        printf("8. Clear Canvas\n");
        printf("9. Toggle Circle Aspect Ratio Correction (%s)\n", aspect_ratio_correction ? "ON" : "OFF");
        printf("10. Set Background Character (Current: '%c')\n", bg_char);
        printf("11. Exit\n");
        printf("------------------------------------\n");
        
        choice = read_int_cli("Enter your choice (1-11): ", 1, 11);
        printf("\n");

        if (choice == 11) {
            printf("Exiting editor. Goodbye!\n");
            break;
        }

        ShapeParams p;
        memset(&p, 0, sizeof(p));

        switch (choice) {
            case 1: { // Line
                printf("--- Add Line ---\n");
                p.x1 = read_int_cli("Enter x1 (0-59): ", 0, CANVAS_WIDTH - 1);
                p.y1 = read_int_cli("Enter y1 (0-19): ", 0, CANVAS_HEIGHT - 1);
                p.x2 = read_int_cli("Enter x2 (0-59): ", 0, CANVAS_WIDTH - 1);
                p.y2 = read_int_cli("Enter y2 (0-19): ", 0, CANVAS_HEIGHT - 1);
                p.draw_char = read_char_cli("Enter character to draw with ('*' or '_'): ");
                add_shape(SHAPE_LINE, p);
                printf("Line added!\n");
                break;
            }
            case 2: { // Rectangle
                printf("--- Add Rectangle ---\n");
                p.x1 = read_int_cli("Enter x1 corner (0-59): ", 0, CANVAS_WIDTH - 1);
                p.y1 = read_int_cli("Enter y1 corner (0-19): ", 0, CANVAS_HEIGHT - 1);
                p.x2 = read_int_cli("Enter x2 opposite corner (0-59): ", 0, CANVAS_WIDTH - 1);
                p.y2 = read_int_cli("Enter y2 opposite corner (0-19): ", 0, CANVAS_HEIGHT - 1);
                p.draw_char = read_char_cli("Enter character to draw with ('*' or '_'): ");
                add_shape(SHAPE_RECTANGLE, p);
                printf("Rectangle added!\n");
                break;
            }
            case 3: { // Triangle
                printf("--- Add Triangle ---\n");
                p.x1 = read_int_cli("Enter x1 vertex (0-59): ", 0, CANVAS_WIDTH - 1);
                p.y1 = read_int_cli("Enter y1 vertex (0-19): ", 0, CANVAS_HEIGHT - 1);
                p.x2 = read_int_cli("Enter x2 vertex (0-59): ", 0, CANVAS_WIDTH - 1);
                p.y2 = read_int_cli("Enter y2 vertex (0-19): ", 0, CANVAS_HEIGHT - 1);
                p.x3 = read_int_cli("Enter x3 vertex (0-59): ", 0, CANVAS_WIDTH - 1);
                p.y3 = read_int_cli("Enter y3 vertex (0-19): ", 0, CANVAS_HEIGHT - 1);
                p.draw_char = read_char_cli("Enter character to draw with ('*' or '_'): ");
                add_shape(SHAPE_TRIANGLE, p);
                printf("Triangle added!\n");
                break;
            }
            case 4: { // Circle
                printf("--- Add Circle ---\n");
                p.x1 = read_int_cli("Enter center x (0-59): ", 0, CANVAS_WIDTH - 1);
                p.y1 = read_int_cli("Enter center y (0-19): ", 0, CANVAS_HEIGHT - 1);
                p.radius = read_int_cli("Enter radius (1-30): ", 1, 30);
                p.draw_char = read_char_cli("Enter character to draw with ('*' or '_'): ");
                add_shape(SHAPE_CIRCLE, p);
                printf("Circle added!\n");
                break;
            }
            case 5: { // Delete
                if (shape_count == 0) {
                    printf("No shapes to delete!\n");
                } else {
                    list_shapes_cli();
                    int del_idx = read_int_cli("Enter shape index to delete: ", 0, shape_count - 1);
                    delete_shape(del_idx);
                    printf("Shape deleted!\n");
                }
                break;
            }
            case 6: // List
                list_shapes_cli();
                break;
            case 7: // Display
                display_canvas_cli();
                break;
            case 8: // Clear
                shape_count = 0;
                render_shapes();
                printf("Canvas cleared!\n");
                break;
            case 9: // Toggle Correction
                aspect_ratio_correction = !aspect_ratio_correction;
                render_shapes();
                printf("Aspect ratio correction is now %s.\n", aspect_ratio_correction ? "ON" : "OFF");
                break;
            case 10: { // Background character
                char bg = read_char_cli("Enter new background character (e.g. ' ', '_', or '.'): ");
                bg_char = bg;
                render_shapes();
                printf("Background character updated!\n");
                break;
            }
        }
    }
}

// ==========================================
// Ncurses TUI Mode Implementation
// ==========================================

#include <ncurses/ncurses.h>

WINDOW *win_canvas = NULL;
WINDOW *win_menu = NULL;
WINDOW *win_status = NULL;

const char *menu_options[] = {
    "1. Add Line",
    "2. Add Rectangle",
    "3. Add Triangle",
    "4. Add Circle",
    "5. Delete Shape",
    "6. Clear Canvas",
    "7. Toggle Circle Aspect Correction",
    "8. Set Background Character",
    "9. Exit Editor"
};
#define MENU_SIZE (sizeof(menu_options) / sizeof(menu_options[0]))

void update_tui_display(int selected) {
    // 1. Draw Canvas Window
    wclear(win_canvas);
    box(win_canvas, 0, 0);
    // Title
    wattron(win_canvas, A_BOLD | COLOR_PAIR(1));
    mvwprintw(win_canvas, 0, 2, " CANVAS (60x20) ");
    wattroff(win_canvas, A_BOLD | COLOR_PAIR(1));

    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            char ch = canvas[y][x];
            // Highlight drawing characters in a color
            if (ch == '*' || ch == '_') {
                wattron(win_canvas, COLOR_PAIR(2) | A_BOLD);
                mvwaddch(win_canvas, y + 1, x + 1, ch);
                wattroff(win_canvas, COLOR_PAIR(2) | A_BOLD);
            } else {
                mvwaddch(win_canvas, y + 1, x + 1, ch);
            }
        }
    }
    wrefresh(win_canvas);

    // 2. Draw Menu / Shapes Window
    wclear(win_menu);
    box(win_menu, 0, 0);
    wattron(win_menu, A_BOLD | COLOR_PAIR(1));
    mvwprintw(win_menu, 0, 2, " CONTROL CENTER ");
    wattroff(win_menu, A_BOLD | COLOR_PAIR(1));

    // Active shapes display
    mvwprintw(win_menu, 2, 2, "Objects (%d/%d):", shape_count, MAX_SHAPES);
    for (int i = 0; i < 5 && i < shape_count; i++) {
        mvwprintw(win_menu, 3 + i, 2, "[%d] %s ('%c')", i, get_shape_name(shapes[i].type), shapes[i].params.draw_char);
    }
    if (shape_count > 5) {
        mvwprintw(win_menu, 8, 2, "... and %d more", shape_count - 5);
    } else if (shape_count == 0) {
        mvwprintw(win_menu, 3, 4, "(no shapes)");
    }

    // Divider
    mvwprintw(win_menu, 9, 1, "-----------------------------");

    // Menu options listing
    mvwprintw(win_menu, 10, 2, "MENU:");
    for (int i = 0; i < (int)MENU_SIZE; i++) {
        if (i == selected) {
            wattron(win_menu, A_REVERSE | COLOR_PAIR(3));
            mvwprintw(win_menu, 11 + i, 2, " %s ", menu_options[i]);
            wattroff(win_menu, A_REVERSE | COLOR_PAIR(3));
        } else {
            mvwprintw(win_menu, 11 + i, 2, "  %s", menu_options[i]);
        }
    }

    // Correction state display
    mvwprintw(win_menu, 11 + MENU_SIZE + 1, 2, "Aspect Correction: %s", aspect_ratio_correction ? "ON" : "OFF");
    mvwprintw(win_menu, 11 + MENU_SIZE + 2, 2, "Background Char:   '%c'", bg_char);
    wrefresh(win_menu);

    // 3. Draw Status Window
    wclear(win_status);
    box(win_status, 0, 0);
    wattron(win_status, COLOR_PAIR(1));
    mvwprintw(win_status, 1, 2, "Use Up/Down Arrow keys to navigate menu. Press ENTER to select.");
    mvwprintw(win_status, 2, 2, "Editor coordinates: X (0-59), Y (0-19). Canvas redraws instantly.");
    wattroff(win_status, COLOR_PAIR(1));
    wrefresh(win_status);
}

void get_tui_string(const char *prompt, char *buf, int max_len) {
    wclear(win_status);
    box(win_status, 0, 0);
    wattron(win_status, A_BOLD | COLOR_PAIR(3));
    mvwprintw(win_status, 1, 2, "%s", prompt);
    wattroff(win_status, A_BOLD | COLOR_PAIR(3));
    wrefresh(win_status);

    echo();
    curs_set(1);
    mvwgetnstr(win_status, 1, 2 + strlen(prompt), buf, max_len);
    noecho();
    curs_set(0);
}

int get_tui_int(const char *prompt, int min_val, int max_val) {
    char buf[64];
    while (1) {
        get_tui_string(prompt, buf, sizeof(buf) - 1);
        char *endptr;
        long val = strtol(buf, &endptr, 10);
        if (endptr != buf && *endptr == '\0' && val >= min_val && val <= max_val) {
            return (int)val;
        }
        
        // Show error and ask again
        wclear(win_status);
        box(win_status, 0, 0);
        wattron(win_status, COLOR_PAIR(4) | A_BOLD);
        mvwprintw(win_status, 1, 2, "ERROR: Enter an integer from %d to %d. Press any key to retry...", min_val, max_val);
        wattroff(win_status, COLOR_PAIR(4) | A_BOLD);
        wrefresh(win_status);
        wgetch(win_status);
    }
}

char get_tui_char(const char *prompt) {
    char buf[64];
    get_tui_string(prompt, buf, sizeof(buf) - 1);
    if (strlen(buf) == 0) {
        return '*';
    }
    return buf[0];
}

void run_tui_mode() {
    // Initialize ncurses screen
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    // Setup colors if terminal supports it
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_CYAN, COLOR_BLACK);
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        init_pair(4, COLOR_RED, COLOR_BLACK);
    }

    // Windows dimensions:
    // Canvas: height=22, width=62 (inner 20x60 + borders)
    // Menu: height=22, width=32
    // Status: height=4, width=95
    win_canvas = newwin(CANVAS_HEIGHT + 2, CANVAS_WIDTH + 2, 1, 1);
    win_menu = newwin(CANVAS_HEIGHT + 2, 32, 1, CANVAS_WIDTH + 4);
    win_status = newwin(4, CANVAS_WIDTH + 2 + 32 + 1, CANVAS_HEIGHT + 3, 1);

    keypad(win_canvas, TRUE);
    keypad(win_menu, TRUE);
    keypad(win_status, TRUE);

    int selected = 0;
    bool quit = false;

    render_shapes();

    while (!quit) {
        update_tui_display(selected);
        int ch = wgetch(win_menu);

        switch (ch) {
            case KEY_UP:
            case 'w':
            case 'W':
                selected--;
                if (selected < 0) selected = MENU_SIZE - 1;
                break;
            case KEY_DOWN:
            case 's':
            case 'S':
                selected++;
                if (selected >= (int)MENU_SIZE) selected = 0;
                break;
            case 10: // ENTER key
            case ' ':
                // Process selection
                switch (selected) {
                    case 0: { // Add Line
                        ShapeParams p;
                        memset(&p, 0, sizeof(p));
                        p.x1 = get_tui_int("Line: Enter x1 (0-59): ", 0, CANVAS_WIDTH - 1);
                        p.y1 = get_tui_int("Line: Enter y1 (0-19): ", 0, CANVAS_HEIGHT - 1);
                        p.x2 = get_tui_int("Line: Enter x2 (0-59): ", 0, CANVAS_WIDTH - 1);
                        p.y2 = get_tui_int("Line: Enter y2 (0-19): ", 0, CANVAS_HEIGHT - 1);
                        p.draw_char = get_tui_char("Line: Enter character ('*' or '_'): ");
                        add_shape(SHAPE_LINE, p);
                        break;
                    }
                    case 1: { // Add Rectangle
                        ShapeParams p;
                        memset(&p, 0, sizeof(p));
                        p.x1 = get_tui_int("Rect: Enter corner x1 (0-59): ", 0, CANVAS_WIDTH - 1);
                        p.y1 = get_tui_int("Rect: Enter corner y1 (0-19): ", 0, CANVAS_HEIGHT - 1);
                        p.x2 = get_tui_int("Rect: Enter corner x2 (0-59): ", 0, CANVAS_WIDTH - 1);
                        p.y2 = get_tui_int("Rect: Enter corner y2 (0-19): ", 0, CANVAS_HEIGHT - 1);
                        p.draw_char = get_tui_char("Rect: Enter character ('*' or '_'): ");
                        add_shape(SHAPE_RECTANGLE, p);
                        break;
                    }
                    case 2: { // Add Triangle
                        ShapeParams p;
                        memset(&p, 0, sizeof(p));
                        p.x1 = get_tui_int("Triangle: Enter x1 (0-59): ", 0, CANVAS_WIDTH - 1);
                        p.y1 = get_tui_int("Triangle: Enter y1 (0-19): ", 0, CANVAS_HEIGHT - 1);
                        p.x2 = get_tui_int("Triangle: Enter x2 (0-59): ", 0, CANVAS_WIDTH - 1);
                        p.y2 = get_tui_int("Triangle: Enter y2 (0-19): ", 0, CANVAS_HEIGHT - 1);
                        p.x3 = get_tui_int("Triangle: Enter x3 (0-59): ", 0, CANVAS_WIDTH - 1);
                        p.y3 = get_tui_int("Triangle: Enter y3 (0-19): ", 0, CANVAS_HEIGHT - 1);
                        p.draw_char = get_tui_char("Triangle: Enter character ('*' or '_'): ");
                        add_shape(SHAPE_TRIANGLE, p);
                        break;
                    }
                    case 3: { // Add Circle
                        ShapeParams p;
                        memset(&p, 0, sizeof(p));
                        p.x1 = get_tui_int("Circle: Enter center x (0-59): ", 0, CANVAS_WIDTH - 1);
                        p.y1 = get_tui_int("Circle: Enter center y (0-19): ", 0, CANVAS_HEIGHT - 1);
                        p.radius = get_tui_int("Circle: Enter radius (1-30): ", 1, 30);
                        p.draw_char = get_tui_char("Circle: Enter character ('*' or '_'): ");
                        add_shape(SHAPE_CIRCLE, p);
                        break;
                    }
                    case 4: { // Delete Shape
                        if (shape_count == 0) {
                            wclear(win_status);
                            box(win_status, 0, 0);
                            wattron(win_status, COLOR_PAIR(4) | A_BOLD);
                            mvwprintw(win_status, 1, 2, "No shapes to delete! Press any key...");
                            wattroff(win_status, COLOR_PAIR(4) | A_BOLD);
                            wrefresh(win_status);
                            wgetch(win_status);
                        } else {
                            int del_idx = get_tui_int("Delete: Enter shape index to delete: ", 0, shape_count - 1);
                            delete_shape(del_idx);
                        }
                        break;
                    }
                    case 5: // Clear Canvas
                        shape_count = 0;
                        render_shapes();
                        break;
                    case 6: // Toggle Circle Correction
                        aspect_ratio_correction = !aspect_ratio_correction;
                        render_shapes();
                        break;
                    case 7: { // Set Background Character
                        char bg = get_tui_char("Background: Enter new character (e.g. ' ', '.', '_'): ");
                        bg_char = bg;
                        render_shapes();
                        break;
                    }
                    case 8: // Exit
                        quit = true;
                        break;
                }
                break;
        }
    }

    // Clean up
    delwin(win_canvas);
    delwin(win_menu);
    delwin(win_status);
    endwin();
}

// ==========================================
// Automated Test Suite (Self-Test Mode)
// ==========================================

int run_test_suite() {
    printf("Starting 2D Graphics Editor verification tests...\n\n");

    // Test 1: Initialize canvas
    printf("Test 1: Canvas initialization... ");
    clear_canvas();
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            if (canvas[y][x] != ' ') {
                printf("FAIL! Canvas not cleared to spaces.\n");
                return 1;
            }
        }
    }
    printf("PASS\n");

    // Test 2: Add shapes and render
    printf("Test 2: Add shape & verify list size... ");
    ShapeParams p;
    memset(&p, 0, sizeof(p));
    p.x1 = 0; p.y1 = 0; p.x2 = 9; p.y2 = 0; p.draw_char = '*';
    if (!add_shape(SHAPE_LINE, p)) {
        printf("FAIL! Could not add line.\n");
        return 1;
    }
    if (shape_count != 1) {
        printf("FAIL! Shape count should be 1, but is %d.\n", shape_count);
        return 1;
    }
    printf("PASS\n");

    // Test 3: Drawing correctness (Line)
    printf("Test 3: Verify line drawing rasterization... ");
    // The line from (0,0) to (9,0) should write '*' at coordinates (0,0) to (9,0)
    for (int x = 0; x < 10; x++) {
        if (canvas[0][x] != '*') {
            printf("FAIL! Line pixel missing at (x=%d, y=0), found '%c'\n", x, canvas[0][x]);
            return 1;
        }
    }
    if (canvas[0][10] == '*') {
        printf("FAIL! Line drawn out of bounds at (10,0)\n");
        return 1;
    }
    printf("PASS\n");

    // Test 4: Delete shape and re-render
    printf("Test 4: Verify shape deletion... ");
    if (!delete_shape(0)) {
        printf("FAIL! Could not delete shape at index 0.\n");
        return 1;
    }
    if (shape_count != 0) {
        printf("FAIL! Shape count should be 0 after deletion.\n");
        return 1;
    }
    if (canvas[0][0] != ' ') {
        printf("FAIL! Canvas did not clear after deleting the shape.\n");
        return 1;
    }
    printf("PASS\n");

    // Test 5: Triangle rendering
    printf("Test 5: Triangle rendering... ");
    p.x1 = 5; p.y1 = 5;
    p.x2 = 10; p.y2 = 5;
    p.x3 = 5; p.y3 = 10;
    p.draw_char = '_';
    add_shape(SHAPE_TRIANGLE, p);
    // Vertices should be drawn
    if (canvas[5][5] != '_' || canvas[5][10] != '_' || canvas[10][5] != '_') {
        printf("FAIL! Triangle vertex character mismatch.\n");
        return 1;
    }
    printf("PASS\n");

    printf("\nAll 5 verification tests PASSED successfully!\n");
    return 0;
}

// ==========================================
// Main Entry Point
// ==========================================

int main(int argc, char *argv[]) {
    // Check if test mode is requested
    if (argc > 1 && strcmp(argv[1], "--test") == 0) {
        return run_test_suite();
    }

    // Check if TUI mode is explicitly requested
    if (argc > 1 && strcmp(argv[1], "--tui") == 0) {
        run_tui_mode();
        return 0;
    }

    // Default to CLI mode for maximum terminal compatibility (e.g. inside VS Code)
    run_cli_mode();
    return 0;
}
