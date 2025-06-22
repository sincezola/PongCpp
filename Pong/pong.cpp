#include <iostream>
#include <time.h>
#include <conio.h>
#include <cstdlib>
#include <windows.h>

using namespace std;

enum eDir {
    STOP = 0,
    LEFT = 1,
    UPLEFT = 2,
    DOWNLEFT = 3,
    RIGHT = 4,
    UPRIGHT = 5,
    DOWNRIGHT = 6
};

void vClearScreen() {
#ifdef _WIN32
    COORD topLeft = { 0, 0 };
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(console, topLeft);
#else
    system("clear");
#endif
}

class cBall {
private:
    int x, y;
    int originalX, originalY;
    eDir direction;

public:
    cBall(int posX, int posY) {
        originalX = posX;
        originalY = posY;
        x = posX; y = posY;
        direction = STOP;
    }

    void changeDirection(eDir d) {
        direction = d;
    }

    void randomDirection() {
        direction = (eDir)((rand() % 6) + 1);
        if (direction == STOP) {
            randomDirection();
        }
    }

    inline int getX() { return x; }
    inline int getY() { return y; }
    inline eDir getDirection() { return direction; }

    void Move() {
        switch (direction) {
            case STOP:
                break;
            case LEFT:
                x--;
                break;
            case RIGHT:
                x++;
                break;
            case UPLEFT:
                x--; y--;
                break;
            case DOWNLEFT:
                x--; y++;
                break;
            case UPRIGHT:
                x++; y--;
                break;
            case DOWNRIGHT:
                x++; y++;
                break;
            default:
                break;
        }
    }

    friend ostream & operator << (ostream& o, const cBall& c) {
        o << "Ball [" << c.x << "," << c.y << "][" << c.direction << "]";
        return o;
    }

    void Reset() {
        x = originalX;
        y = originalY;
        direction = STOP;
    }
};

class cPaddle {
private:
    int x, y;
    int originalX, originalY;
public:
    cPaddle() {
        x = y = 0;
    }

    cPaddle(int posX, int posY) : cPaddle() {
        originalX = posX;
        originalY = posY;

        x = posX;
        y = posY;
    }

    inline void Reset() { x = originalX; y = originalY; }

    inline int getX() { return x; }
    inline int getY() { return y; }

    inline void moveUp() { y--; }
    inline void moveDown() { y++;}

    friend ostream & operator << (ostream& o, const cPaddle& c) {
        o << "Paddle [" << c.x << "," << c.y << "]";
        return o;
    }
};

class cGameManager {
private:
    int width, height;
    int score1, score2;
    char up1, down1, up2, down2;
    bool quit;
    bool ballStarted;

    cBall* ball;
    cPaddle* player1;
    cPaddle* player2;
public:
    cGameManager(int w, int h) {
        srand(time(NULL));

        quit = false;
        ballStarted = false;

        up1 = 'w'; up2 = 'i';
        down1 = 's'; down2 = 'k';
        score1 = score2 = 0;

        width = w; height = h;

        ball = new cBall(w / 2, h / 2);

        player1 = new cPaddle(1, h / 2 - 2);
        player2 = new cPaddle(w - 2, h / 2 - 2);
    }

    ~cGameManager() {
        delete ball;
        delete player1;
        delete player2;
    }

    void ScoreUp(cPaddle* player) {
        if (player == player1)
            score1++;
        else if (player == player2)
            score2++;

        ball->Reset();
        player1->Reset();
        player2->Reset();
        ballStarted = false;
    }

    void Draw() {
        vClearScreen();

        for (int i = 0; i < width + 2; i++) {
            cout << "\xB2";
        }

        cout << endl;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int ballx = ball->getX();
                int bally = ball->getY();

                int player1x = player1->getX();
                int player2x = player2->getX();

                int player1y = player1->getY();
                int player2y = player2->getY();

                if (j == 0) {
                    cout << "\xB2"; // First char on screen
                }

                if (ballx == j && bally == i)
                    cout << "O"; // Ball
                else if (player1x == j && (i >= player1y && i < player1y + 4))
                    cout << "\xDB"; // Player 1
                else if (player2x == j && (i >= player2y && i < player2y + 4))
                    cout << "\xDB"; // Player 2
                else
                    cout << " "; // Nothing

                if (j == width - 1) {
                    cout << "\xB2";
                }
            }
            cout << endl;
        }

        for (int i = 0; i < width + 2; i++) {
            cout << "\xB2";
        }

        cout << endl;

        cout << "Score 1: " << score1 << endl;
        cout << "Score 2: " << score2 << endl;
    }

    void Input() {
        if (ballStarted) {
            ball->Move();
        }

        int player1y = player1->getY();
        int player2y = player2->getY();

        if (_kbhit()) {
            char curr = _getch();

            if (curr == up1) {
                if(player1y > 0)
                    player1->moveUp();
                ballStarted = true;
            } else if (curr == up2) {
                if(player2y > 0)
                    player2->moveUp();
                ballStarted = true;
            } else if (curr == down1) {
                if(player1y + 4 < height)
                    player1->moveDown();
                ballStarted = true;
            } else if (curr == down2) {
                if(player2y + 4 < height)
                    player2->moveDown();
                ballStarted = true;
            } else if (curr == 'q' || curr == 'Q') {
                quit = true;
            }

            if (!ballStarted && (curr == up1 || curr == up2 || curr == down1 || curr == down2)) {
                 ballStarted = true;
                 ball->randomDirection();
            }
            if (ballStarted && ball->getDirection() == STOP) {
                 ball->randomDirection();
            }
        }
    }

    void Logic() {
        int ballx = ball->getX();
        int bally = ball->getY();

        int player1x = player1->getX();
        int player2x = player2->getX();

        int player1y = player1->getY();
        int player2y = player2->getY();

        // Left paddle
        if (ballx == player1x + 1 && (bally >= player1y && bally < player1y + 4)) {
            if (bally == player1y) {
                ball->changeDirection(UPRIGHT);
            } else if (bally == player1y + 1) {
                ball->changeDirection(UPRIGHT);
            } else if (bally == player1y + 2) {
                ball->changeDirection(DOWNRIGHT);
            } else if (bally == player1y + 3) {
                ball->changeDirection(DOWNRIGHT);
            }
        }

        // Right paddle
        if (ballx == player2x - 1 && (bally >= player2y && bally < player2y + 4)) {
            if (bally == player2y) {
                ball->changeDirection(UPLEFT);
            } else if (bally == player2y + 1) {
                ball->changeDirection(UPLEFT);
            } else if (bally == player2y + 2) {
                ball->changeDirection(DOWNLEFT);
            } else if (bally == player2y + 3) {
                ball->changeDirection(DOWNLEFT);
            }
        }

        // Bottom wall
        if (bally == height - 1) {
            if (ball->getDirection() == DOWNRIGHT) ball->changeDirection(UPRIGHT);
            else if (ball->getDirection() == DOWNLEFT) ball->changeDirection(UPLEFT);
        }

        // Top wall
        if (bally == 0) {
            if (ball->getDirection() == UPRIGHT) ball->changeDirection(DOWNRIGHT);
            else if (ball->getDirection() == UPLEFT) ball->changeDirection(DOWNLEFT);
        }

        if (ballx == width - 1)
            ScoreUp(player1);

        if (ballx == 0)
            ScoreUp(player2);
    }

    void Run() {
        while (!quit) {
            Draw();
            Input();
            Logic();
            Sleep(50);
        }
    }
};

int main() {
    cGameManager c(40, 20);

    c.Run();

    return 0;
}
