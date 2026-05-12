/*
 * ============================================================
 *   COLORFUL SNAKE GAME - Windows Version (No ncurses!)
 *   Uses: windows.h + conio.h (works on any Windows PC)
 *
 *   COMPILE (in CMD or PowerShell):
 *     gcc snake_game_windows.c -o snake_game.exe
 *
 *   OR in Code::Blocks / Dev-C++ — just paste and run!
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

/* --- Board Settings --------------------------------------- */
#define BOARD_W     40
#define BOARD_H     20
#define MAX_LEN     500

/* --- Directions ------------------------------------------- */
#define UP    0
#define DOWN  1
#define LEFT  2
#define RIGHT 3

/* --- Windows Console Colors ------------------------------- */
#define BLACK        0
#define DARK_BLUE    1
#define DARK_GREEN   2
#define DARK_CYAN    3
#define DARK_RED     4
#define DARK_MAGENTA 5
#define DARK_YELLOW  6
#define LIGHT_GRAY   7
#define DARK_GRAY    8
#define BLUE         9
#define GREEN        10
#define CYAN         11
#define RED          12
#define MAGENTA      13
#define YELLOW       14
#define WHITE        15

/* --- Structs ---------------------------------------------- */
typedef struct { int x, y; } Point;

typedef struct {
    Point body[MAX_LEN];
    int   len;
    int   dir;
    int   colorIdx;
} Snake;

/* --- Globals ---------------------------------------------- */
static Snake snake;
static Point fruit;
static int   score   = 0;
static int   running = 1;

/* Snake color cycle */
static int snakeColors[] = { GREEN, CYAN, MAGENTA, YELLOW, RED, BLUE };
static int NUM_COLORS = 6;

/* --- Console Helpers -------------------------------------- */
static HANDLE hConsole;

static void setColor(int fg, int bg) {
    SetConsoleTextAttribute(hConsole, (WORD)(bg * 16 + fg));
}

static void resetColor(void) {
    setColor(WHITE, BLACK);
}

static void gotoxy(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hConsole, pos);
}

static void hideCursor(void) {
    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(hConsole, &ci);
}

static void showCursor(void) {
    CONSOLE_CURSOR_INFO ci = { 1, TRUE };
    SetConsoleCursorInfo(hConsole, &ci);
}

static void clearScreen(void) {
    system("cls");
}

/* --- Draw Colored Text ------------------------------------ */
static void printColored(const char *text, int fg, int bg) {
    setColor(fg, bg);
    printf("%s", text);
    resetColor();
}

/* --- Draw a Box ------------------------------------------- */
static void drawBox(int x, int y, int w, int h, int color) {
    setColor(color, BLACK);

    /* Top row */
    int i,r;
    gotoxy(x, y);
    printf("+");
    for (i = 0; i < w - 2; i++) {
	printf("-");
    }
    printf("+");

    /* Middle rows */
    for (r = 1; r < h - 1; r++) {
        gotoxy(x,         y + r); printf("|");
        gotoxy(x + w - 1, y + r); printf("|");
    }

    /* Bottom row */
    gotoxy(x, y + h - 1);
    printf("+");
    for ( i = 0; i < w - 2; i++) printf("-");
    printf("+");

    resetColor();
}

/* --- Splash / Title Animation ----------------------------- */
static void splashAnimation(void) {
    clearScreen();
    int i,c,b;
    const char *title[] = {
        "  ____  _   _    _    _  _______   ____    _    __  __ ___ ",
        " / ___|| \\ | |  / \\  | |/ / ____| / ___|  / \\  |  \\/  | ___|",
        " \\___ \\|  \\| | / _ \\ | ' /|  _|  | |  _  / _ \\ | |\\/| |  _| ",
        "  ___) | |\\  |/ ___ \\| . \\| |___  | |_| |/ ___ \\| |  | | |___",
        " |____/|_| \\_/_/   \\_|_|\\_|_____|  \\____/_/   \\_|_|  |_|_____|"
    };

    int colors[] = { GREEN, CYAN, YELLOW, MAGENTA, RED };

    /* Drop title lines one by one */
    for (i = 0; i < 5; i++) {
        gotoxy(2, 3 + i);
        setColor(colors[i], BLACK);
        printf("%s", title[i]);
        resetColor();
        Sleep(150);
    }

    /* Crawling snake animation */
    for (c = 0; c < 55; c++) {
        int col = snakeColors[c % NUM_COLORS];
        setColor(col, BLACK);
        gotoxy(c, 11);
        printf("~oooO");
        gotoxy(c > 0 ? c - 1 : 0, 11);
        printf(" ");
        resetColor();
        Sleep(25);
    }

    /* Blinking prompt */
    for (b = 0; b < 6; b++) {
        gotoxy(18, 13);
        if (b % 2 == 0) setColor(YELLOW, BLACK);
        else             setColor(DARK_GRAY, BLACK);
        printf("~~ Press any key to continue ~~");
        resetColor();
        Sleep(350);
    }

    gotoxy(18, 13);
    setColor(YELLOW, BLACK);
    printf("~~ Press any key to continue ~~");
    resetColor();
    _getch();
}

/* --- Instructions Screen ---------------------------------- */
static void showInstructions(void) {
    clearScreen();
    drawBox(5, 1, 55, 24, CYAN);

    gotoxy(22, 2);
    setColor(YELLOW, BLACK);
    printf("=== HOW TO PLAY ===");
    resetColor();

    const char *lines[] = {
        "",
        "  CONTROLS:",
        "    [W] or [Arrow Up]    ->  Move Up",
        "    [S] or [Arrow Down]  ->  Move Down",
        "    [A] or [Arrow Left]  ->  Move Left",
        "    [D] or [Arrow Right] ->  Move Right",
        "    [P]                  ->  Pause Game",
        "    [Q]                  ->  Quit Game",
        "",
        "  OBJECTIVE:",
        "    Eat the fruit (*) to grow and earn points.",
        "    Avoid hitting yourself!",
        "",
        "  DIFFICULTY:",
        "    Easy   - Walls wrap around, slow speed",
        "    Medium - Walls kill,  medium speed",
        "    Hard   - Walls kill,  blazing speed!",
        "",
        "  BONUS:",
        "    Snake changes colour every time you score!",
        "",
    };

    int n = sizeof(lines) / sizeof(lines[0]);
    int i;
    for (i = 0; i < n; i++) {
        gotoxy(6, 3 + i);
        setColor(CYAN, BLACK);
        printf("%-53s", lines[i]);
        resetColor();
        Sleep(60);
    }

    gotoxy(13, 23);
    setColor(GREEN, BLACK);
    printf("  Press any key to go to the menu...  ");
    resetColor();

    _getch();
}

/* --- Main Menu -------------------------------------------- */
/*  Returns 0=Easy, 1=Medium, 2=Hard, -1=Quit */
static int showMenu(void) {
    const char *opts[]   = { "  EASY  ", "  MEDIUM  ", "  DIFFICULT  ", "  QUIT  " };
    int         colors[] = { GREEN,      YELLOW,       RED,             DARK_GRAY  };
    int nopts = 4, sel = 0;
    int i;

    while (1) {
        clearScreen();
        int i;
        drawBox(10, 1, 44, 18, CYAN);

        gotoxy(22, 2);
        setColor(YELLOW, BLACK);
        printf("S N A K E   G A M E");
        resetColor();

        gotoxy(17, 4);
        setColor(WHITE, BLACK);
        printf("Choose your difficulty:");
        resetColor();

        for (i = 0; i < nopts; i++) {
            gotoxy(19, 6 + i * 2);
            if (i == sel) {
                setColor(BLACK, colors[i]);
                printf("> %s <", opts[i]);
            } else {
                setColor(colors[i], BLACK);
                printf("  %s  ", opts[i]);
            }
            resetColor();
        }

        gotoxy(11, 16);
        setColor(DARK_GRAY, BLACK);
        printf("[W/S or Arrows] Navigate   [Enter] Select");
        resetColor();

        /* Mini crawl animation */
        static int apos = 0;
        gotoxy(11 + (apos % 30), 17);
        setColor(snakeColors[apos % NUM_COLORS], BLACK);
        printf("~oO");
        resetColor();
        apos++;

        Sleep(100);

        /* Input — check arrow keys too */
        if (_kbhit()) {
            int ch = _getch();
            if (ch == 0 || ch == 224) {   /* arrow key prefix */
                ch = _getch();
                if      (ch == 72) sel = (sel - 1 + nopts) % nopts; /* up    */
                else if (ch == 80) sel = (sel + 1) % nopts;          /* down  */
            } else {
                switch (ch) {
                    case 'w': case 'W': sel = (sel - 1 + nopts) % nopts; break;
                    case 's': case 'S': sel = (sel + 1) % nopts;          break;
                    case '\r': case '\n':
                        return (sel == 3) ? -1 : sel;
                }
            }
        }
    }
}

/* --- Place Fruit ------------------------------------------ */
static void placeFruit(void) {
    int ok = 0;
    int i;
    while (!ok) {
        fruit.x = 1 + rand() % (BOARD_W - 2);
        fruit.y = 1 + rand() % (BOARD_H - 2);
        ok = 1;
        for (i = 0; i < snake.len; i++)
            if (snake.body[i].x == fruit.x && snake.body[i].y == fruit.y)
                { ok = 0; break; }
    }
}

/* --- Draw Everything -------------------------------------- */
#define OX 4   /* board offset x on screen */
#define OY 3   /* board offset y on screen */

static void drawAll(const char *lvlName, int wallKill) {
    /* Border */
    int i;
    drawBox(OX, OY, BOARD_W + 2, BOARD_H + 2, wallKill ? RED : GREEN);

    /* Score bar */
    gotoxy(OX, OY - 2);
    setColor(WHITE, BLACK);
    printf("Level: %-10s  Score: %d   [P]=Pause  [Q]=Quit", lvlName, score);
    resetColor();

    /* Fruit */
    gotoxy(OX + 1 + fruit.x, OY + 1 + fruit.y);
    setColor(RED, BLACK);
    printf("*");
    resetColor();

    /* Snake body */
    int cp = snakeColors[snake.colorIdx % NUM_COLORS];
    for ( i = 1; i < snake.len; i++) {
        gotoxy(OX + 1 + snake.body[i].x, OY + 1 + snake.body[i].y);
        setColor(cp, BLACK);
        printf("o");
        resetColor();
    }

    /* Snake head */
    gotoxy(OX + 1 + snake.body[0].x, OY + 1 + snake.body[0].y);
    setColor(BLACK, cp);
    printf("O");
    resetColor();
}

/* --- Eat Animation ---------------------------------------- */
static void eatAnimation(void) {
	int f;
    for (f = 0; f < 3; f++) {
        gotoxy(OX + BOARD_W + 4, OY + 2);
        setColor(f % 2 == 0 ? YELLOW : GREEN, BLACK);
        printf("+10!");
        resetColor();
        Sleep(90);
        gotoxy(OX + BOARD_W + 4, OY + 2);
        printf("    ");
        Sleep(60);
    }
}

/* --- Game Over Animation ---------------------------------- */
static void gameOverAnim(void) {
    int cx = OX + BOARD_W / 2 - 5;
    int cy = OY + BOARD_H / 2;
    int f;
    for (f = 0; f < 8; f++) {
        gotoxy(cx, cy);
        setColor(f % 2 == 0 ? RED : YELLOW, BLACK);
        printf("** GAME OVER! **");
        resetColor();
        Sleep(200);
    }

    gotoxy(cx - 2, cy + 2);
    setColor(WHITE, BLACK);
    printf("Final Score: %d", score);

    gotoxy(cx - 5, cy + 4);
    setColor(GREEN, BLACK);
    printf("[R] = Retry    [Q] = Quit");
    resetColor();
}

/* --- Main Game Loop --------------------------------------- */
/* Returns 1 = retry, 0 = quit */
static int runGame(int lvlIdx) {
    const char *lvlNames[] = { "EASY", "MEDIUM", "DIFFICULT" };
    int speeds[]            = { 180,    100,       55          };
    int wallKills[]         = { 0,      1,         1           };
    int i;

    int speed   = speeds[lvlIdx];
    int wallKill= wallKills[lvlIdx];

    srand((unsigned)time(NULL));

    /* Init snake */
    snake.len      = 4;
    snake.dir      = RIGHT;
    snake.colorIdx = 0;
    for (i = 0; i < snake.len; i++) {
        snake.body[i].x = BOARD_W / 2 - i;
        snake.body[i].y = BOARD_H / 2;
    }

    score   = 0;
    running = 1;
    placeFruit();
    clearScreen();
    hideCursor();

    DWORD lastTime = GetTickCount();

    while (running) {
        /* -- Input (non-blocking) -- */
        if (_kbhit()) {
            int ch = _getch();
            if (ch == 0 || ch == 224) {
                ch = _getch();
                if      (ch == 72 && snake.dir != DOWN)  snake.dir = UP;
                else if (ch == 80 && snake.dir != UP)    snake.dir = DOWN;
                else if (ch == 75 && snake.dir != RIGHT) snake.dir = LEFT;
                else if (ch == 77 && snake.dir != LEFT)  snake.dir = RIGHT;
            } else {
                switch (ch) {
                    case 'w': case 'W': if (snake.dir != DOWN)  snake.dir = UP;    break;
                    case 's': case 'S': if (snake.dir != UP)    snake.dir = DOWN;  break;
                    case 'a': case 'A': if (snake.dir != RIGHT) snake.dir = LEFT;  break;
                    case 'd': case 'D': if (snake.dir != LEFT)  snake.dir = RIGHT; break;
                    case 'p': case 'P': {
                        gotoxy(OX + 12, OY + BOARD_H / 2);
                        setColor(YELLOW, BLACK);
                        printf("-- PAUSED -- Press any key --");
                        resetColor();
                        _getch();
                        break;
                    }
                    case 'q': case 'Q': running = 0; continue;
                }
            }
        }

        /* -- Tick control -- */
        DWORD now = GetTickCount();
        if ((int)(now - lastTime) < speed) {
            Sleep(5);
            continue;
        }
        lastTime = now;

        /* -- Move -- */
        Point newHead = snake.body[0];
        switch (snake.dir) {
            case UP:    newHead.y--; break;
            case DOWN:  newHead.y++; break;
            case LEFT:  newHead.x--; break;
            case RIGHT: newHead.x++; break;
        }

        /* -- Wall check -- */
        if (wallKill) {
            if (newHead.x < 0 || newHead.x >= BOARD_W ||
                newHead.y < 0 || newHead.y >= BOARD_H) {
                running = 0; break;
            }
        } else {
            /* Wrap around */
            if (newHead.x < 0)        newHead.x = BOARD_W - 1;
            if (newHead.x >= BOARD_W) newHead.x = 0;
            if (newHead.y < 0)        newHead.y = BOARD_H - 1;
            if (newHead.y >= BOARD_H) newHead.y = 0;
        }

        /* -- Self collision -- */
        for (i = 0; i < snake.len; i++) {
            if (snake.body[i].x == newHead.x &&
                snake.body[i].y == newHead.y) {
                running = 0; break;
            }
        }
        if (!running) break;

        /* -- Eat fruit? -- */
        int ate = (newHead.x == fruit.x && newHead.y == fruit.y);
        if (ate && snake.len < MAX_LEN) snake.len++;

        /* Shift body */
        for (i = snake.len - 1; i > 0; i--)
            snake.body[i] = snake.body[i - 1];
        snake.body[0] = newHead;

        if (ate) {
            score += 10;
            snake.colorIdx++;
            placeFruit();
            eatAnimation();
        }

        /* -- Erase old tail (last position) -- */
        if (!ate) {
        gotoxy(OX + 1 + snake.body[snake.len - 1].x,
		       OY + 1 + snake.body[snake.len - 1].y);
        printf(" ");
        }
        /* -- Draw -- */
        drawAll(lvlNames[lvlIdx], wallKill);
        if (ate && snake.len< MAX_LEN)snake.len++;
	}

    gameOverAnim();
    showCursor();

    /* Wait for R or Q */
    while (1) {
        int ch = _getch();
        if (ch == 'r' || ch == 'R') return 1;
        if (ch == 'q' || ch == 'Q') return 0;
    }
}
/* --- Entry Point ------------------------------------------ */
int main(void) {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    /* Maximize console window */
    system("mode con: cols=80 lines=30");
    SetConsoleTitle("Snake Game - Windows");
    hideCursor();

    /* Splash + Instructions */
    splashAnimation();
    showInstructions();

    /* Game loop */
    while (1) {
        int lvl = showMenu();
        if (lvl < 0) break;   /* Quit */

        int retry = runGame(lvl);
        if (!retry) break;
    }

    /* Farewell */
    clearScreen();
    gotoxy(28, 12);
    setColor(YELLOW, BLACK);
    printf("Thanks for playing!");
    gotoxy(26, 13);
    setColor(GREEN, BLACK);
    printf("Coded with <3 in C for Windows");
    resetColor();
    Sleep(1800);

    showCursor();
    return 0;
}

