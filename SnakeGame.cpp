#include <iostream>
#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

#define MAXSNAKESIZE 100
#define MAXFRAMEX 119
#define MAXFRAMEY 29

class GameObject {
public:
    virtual void Draw() = 0;
    virtual void Erase() = 0;
    virtual ~GameObject() {}
};

class Point : public GameObject {
private:
    int x;
    int y;

public:
    Point() : x(10), y(10) {}

    Point(int x, int y) : x(x), y(y) {}

    void SetPoint(int x, int y) {
        this->x = x;
        this->y = y;
    }

    int GetX() const {
        return x;
    }

    int GetY() const {
        return y;
    }

    void MoveUp() {
        y--;
        if (y < 0)
            y = MAXFRAMEY;
    }

    void MoveDown() {
        y++;
        if (y > MAXFRAMEY)
            y = 0;
    }

    void MoveLeft() {
        x--;
        if (x < 0)
            x = MAXFRAMEX;
    }

    void MoveRight() {
        x++;
        if (x > MAXFRAMEX)
            x = 0;
    }

    void Draw() override {
        mvaddch(y, x, 'O');
    }

    void Draw(char ch) {
        mvaddch(y, x, ch);
    }

    void Erase() override {
        mvaddch(y, x, ' ');
    }

    void CopyPos(Point* p) {
        p->x = x;
        p->y = y;
    }

    bool IsEqual(const Point* p) const {
        return p->x == x && p->y == y;
    }
};

class Fruit : public Point {
public:
    Fruit() : Point(rand() % MAXFRAMEX, rand() % MAXFRAMEY) {}

    void Relocate() {
        SetPoint(rand() % MAXFRAMEX, rand() % MAXFRAMEY);
    }

    void Draw() override {
        mvaddch(GetY(), GetX(), '#');
    }
};

class Snake : public GameObject {
private:
    Point* cell[MAXSNAKESIZE];
    int size;
    char dir;
    Fruit fruit;
    int state;
    int started;
    int blink;
    int score;

    int kbhit() {
        int ch = getch();
        if (ch != ERR) {
            ungetch(ch);
            return 1;
        } else {
            return 0;
        }
    }

    void ClearScreen() {
        clear();
    }

    int SelfCollision() {
        for (int i = 1; i < size; i++)
            if (cell[0]->IsEqual(cell[i]))
                return 1;
        return 0;
    }

    void AddCell(int x, int y) {
        cell[size++] = new Point(x, y);
    }

public:
    Snake() : size(1), dir('l'), state(0), started(0), blink(0), score(0) {
        cell[0] = new Point(20, 20);
        for (int i = 1; i < MAXSNAKESIZE; i++) {
            cell[i] = nullptr;
        }
    }

    ~Snake() {
        for (int i = 0; i < MAXSNAKESIZE; i++) {
            delete cell[i];
        }
    }

    void WelcomeScreen() {
        printw("\n            /^\\/^\\                                             ");
        printw("\n          _|__|  O|                                              ");
        printw("\n \\/     /~     \\_/ \\                                          ");
        printw("\n  \\____|__________/  \\                                         ");
        printw("\n         \\_______      \\                                       ");
        printw("\n                 `\\     \\                 \\                   ");
        printw("\n                   |     |                  \\                   ");
        printw("\n                  /      /                    \\                 ");
        printw("\n                 /     /                       \\\\              ");
        printw("\n               /      /                         \\ \\            ");
        printw("\n              /     /                            \\  \\          ");
        printw("\n            /     /             _----_            \\   \\        ");
        printw("\n           /     /           _-~      ~-_         |   |          ");
        printw("\n          (      (        _-~    _--_    ~-_     _/   |          ");
        printw("\n           \\      ~-____-~    _-~    ~-_    ~-_-~    /          ");
        printw("\n             ~-_           _-~          ~-_       _-~   - Aoda -");
        printw("\n                ~--______-~                ~-___-~               ");
        printw("\n To play the game use :");
        printw("\n 'a' to move left.");
        printw("\n 'd' to move right.");
        printw("\n 'w' to move top.");
        printw("\n 's' to move bottom.");
    }

    int get_score() const {
        return score;
    }

    void Move() {
        ClearScreen();

        if (state == 0) {
            if (!started) {
                WelcomeScreen();
                printw("\n\nPress any key to start");
                refresh();
                getchar();
                started = 1;
                state = 1;
            } else {
                printw("\nGame Over");
                printw("\nYour Score: %d", score);
                printw("\nPress any key to start");
                refresh();
                getchar();
                state = 1;
                size = 1;
                score = 0;
            }
        }

        for (int i = size - 1; i > 0; i--) {
            cell[i - 1]->CopyPos(cell[i]);
        }

        switch (dir) {
            case 'w':
                cell[0]->MoveUp();
                break;
            case 's':
                cell[0]->MoveDown();
                break;
            case 'a':
                cell[0]->MoveLeft();
                break;
            case 'd':
                cell[0]->MoveRight();
                break;
        }

        if (SelfCollision())
            state = 0;

        if (fruit.GetX() == cell[0]->GetX() && fruit.GetY() == cell[0]->GetY()) {
            AddCell(0, 0);
            fruit.Relocate();
            score++;
        }

        attron(COLOR_PAIR(1));
        for (int i = 0; i < size; i++)
            cell[i]->Draw();

        attroff(COLOR_PAIR(1));
        attron(COLOR_PAIR(2));
        if (!blink)
            fruit.Draw();
        blink = !blink;
        attroff(COLOR_PAIR(2));

        mvprintw(MAXFRAMEY + 2, 0, "Score: %d", score);

        refresh();

        usleep(100000);
    }

    void TurnUp() {
        if (dir != 's')
            dir = 'w';
    }

    void TurnDown() {
        if (dir != 'w')
            dir = 's';
    }

    void TurnLeft() {
        if (dir != 'd')
            dir = 'a';
    }

    void TurnRight() {
        if (dir != 'a')
            dir = 'd';
    }

    void Draw() override {}
    void Erase() override {}
};

int main() {
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(FALSE);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);

    srand(time(NULL));
    Snake snake;
    int op = 'l';

    do {
        op = getch();
        switch (op) {
            case 'w':
            case 'W':
                snake.TurnUp();
                break;

            case 's':
            case 'S':
                snake.TurnDown();
                break;

            case 'a':
            case 'A':
                snake.TurnLeft();
                break;

            case 'd':
            case 'D':
                snake.TurnRight();
                break;
        }
        snake.Move();
    } while (op != 'e');

    printw("Your Final Score: %d", snake.get_score());
    printw("\nPress any key to exit");
    refresh();
    getchar();

    endwin();

    return 0;
}
