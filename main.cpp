#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "letters.h"

const int SCREEN_WIDTH = 1920;  // same as monitor width
const int SCREEN_HEIGHT = 1080; // same as monitor height
const int BRICK_COLS = 48;
const int BRICK_WIDTH = SCREEN_WIDTH / BRICK_COLS;
const int BRICK_HEIGHT = SCREEN_HEIGHT / 30;
int highScore = 0;
const char *HIGH_SCORE_FILE = "highscore.dat";

// Use custom Font
Font myFont, myFont2, myFont3;

const int LETTER_GAP = 1; // Gap between letters (in bricks)

// Timer variables
float gameTimer = 10.0f;  // 10 second timer
bool timerActive = false; // Only start timer when game begins

int score = 0;
int lives = 4;
bool gameOver = false;
bool gameWon = false;
bool isPaused = false;
bool shouldExit = false;
bool inInterface = true;
char userInput[6] = {0}; // 5 chars + null terminator
int inputPos = 0;
float OffsetX = -800.0f;
float ROT = 30.0f;
bool PinballMode = false;
bool backgroundMusicPlaying = false;

Sound wallHitSound;
Sound paddleHitSound;
Sound explosionSound;
Sound freezeSound;
Sound meltSound;
Sound eatSound;
Sound gameOverSound;
Sound gameWonSound;
Sound ShockSound;
Sound LifeLossSound;
Sound BuzzerSound;
Sound IntroSound;
Sound BackgroundSound;

bool gameOverSoundPlayed = false;
bool gameWonSoundPlayed = false;

float currentRotation = 30.0f;   // Initial rotation angle
float currentRotation2 = -30.0f; // Initial rotation angle for right side
float rotationTime = 0.1f;
float elapsedTime = 0.0f;
bool isRotating = false;
float elapsedTime2 = 0.0f;
bool isRotating2 = false;

// New state for collision cooldown
float collisionCooldown = 0.0f;    // Time left before the ball can collide again (in seconds)
const float collisionDelay = 1.0f; // 1.0 seconds cooldown after collision

struct GifAnimation
{
    Image image;
    Texture2D texture;
    int totalFrames;
    int currentFrame = 0;
    int frameCounter = 0;
    int frameDelay = 8;

    void Update()
    {
        frameCounter++;
        if (frameCounter >= frameDelay)
        {
            currentFrame = (currentFrame + 1) % totalFrames;
            int offset = image.width * image.height * 4 * currentFrame;
            UpdateTexture(texture, ((unsigned char *)image.data) + offset);
            frameCounter = 0;
        }
    }

    void Draw(Rectangle dest)
    {
        DrawTexturePro(
            texture,
            (Rectangle){0, 0, (float)image.width, (float)image.height},
            dest,
            (Vector2){0, 0},
            0.0f,
            WHITE);
    }

    void Load(const char *path, int delay = 8)
    {
        image = LoadImageAnim(path, &totalFrames);
        texture = LoadTextureFromImage(image);
        frameDelay = delay;
    }

    void Unload()
    {
        UnloadTexture(texture);
        UnloadImage(image);
    }
};

GifAnimation FireAnim, WaterAnim, ElectricAnim, IceAnim, WindAnim, EarthAnim, Intro, FireBallAnim, WaterBallAnim, DiscoAnim, rollAnim;
bool gifPlaying = true;
bool gifFinished = false;
int gifFrameCounter = 0;

Texture2D cry;
Texture2D cool;
Texture2D heart;
Texture2D Icon;
Texture2D PaddleIcon;
Texture2D Gradient;
Image icon;
Texture2D JumpPadLeft;
Texture2D JumpPadRight;
Texture2D Mock;

const int (*letterMap[128])[12][9] = {NULL};
void InitLetterMap()
{
    letterMap['A'] = &letterA;
    letterMap['B'] = &letterB;
    letterMap['C'] = &letterC;
    letterMap['D'] = &letterD;
    letterMap['E'] = &letterE;
    letterMap['F'] = &letterF;
    letterMap['G'] = &letterG;
    letterMap['H'] = &letterH;
    letterMap['I'] = &letterI;
    letterMap['J'] = &letterJ;
    letterMap['K'] = &letterK;
    letterMap['L'] = &letterL;
    letterMap['M'] = &letterM;
    letterMap['N'] = &letterN;
    letterMap['O'] = &letterO;
    letterMap['P'] = &letterP;
    letterMap['Q'] = &letterQ;
    letterMap['R'] = &letterR;
    letterMap['S'] = &letterS;
    letterMap['T'] = &letterT;
    letterMap['U'] = &letterU;
    letterMap['V'] = &letterV;
    letterMap['W'] = &letterW;
    letterMap['X'] = &letterX;
    letterMap['Y'] = &letterY;
    letterMap['Z'] = &letterZ;

    letterMap['0'] = &digit0;
    letterMap['1'] = &digit1;
    letterMap['2'] = &digit2;
    letterMap['3'] = &digit3;
    letterMap['4'] = &digit4;
    letterMap['5'] = &digit5;
    letterMap['6'] = &digit6;
    letterMap['7'] = &digit7;
    letterMap['8'] = &digit8;
    letterMap['9'] = &digit9;

    letterMap['a'] = &letter_a;
    letterMap['b'] = &letter_b;
    letterMap['c'] = &letter_c;
    letterMap['d'] = &letter_d;
    letterMap['e'] = &letter_e;
    letterMap['f'] = &letter_f;
    letterMap['g'] = &letter_g;
    letterMap['h'] = &letter_h;
    letterMap['i'] = &letter_i;
    letterMap['j'] = &letter_j;
    letterMap['k'] = &letter_k;
    letterMap['l'] = &letter_l;
    letterMap['m'] = &letter_m;
    letterMap['n'] = &letter_n;
    letterMap['o'] = &letter_o;
    letterMap['p'] = &letter_p;
    letterMap['q'] = &letter_q;
    letterMap['r'] = &letter_r;
    letterMap['s'] = &letter_s;
    letterMap['t'] = &letter_t;
    letterMap['u'] = &letter_u;
    letterMap['v'] = &letter_v;
    letterMap['w'] = &letter_w;
    letterMap['x'] = &letter_x;
    letterMap['y'] = &letter_y;
    letterMap['z'] = &letter_z;

    letterMap['!'] = &symbol_exclamation;
    letterMap['?'] = &symbol_question;
    letterMap['.'] = &symbol_period;
    letterMap[','] = &symbol_comma;
    letterMap['-'] = &symbol_hyphen;
    letterMap['_'] = &symbol_underscore;
    letterMap['+'] = &symbol_plus;
    letterMap['='] = &symbol_equals;
    letterMap['/'] = &symbol_slash;
    letterMap['\\'] = &symbol_backslash;
    letterMap['|'] = &symbol_pipe;
    letterMap['*'] = &symbol_asterisk;
    letterMap['#'] = &symbol_hash;
    letterMap['$'] = &symbol_dollar;
    letterMap['%'] = &symbol_percent;
    letterMap['&'] = &symbol_ampersand;
    letterMap['('] = &symbol_paren_open;
    letterMap[')'] = &symbol_paren_close;
    letterMap['@'] = &symbol_at;
    letterMap[':'] = &symbol_colon;
    letterMap[';'] = &symbol_semicolon;
    letterMap['"'] = &symbol_quotation;
    letterMap['\''] = &symbol_apostrophe;
    letterMap['['] = &symbol_bracket_open;
    letterMap[']'] = &symbol_bracket_close;
    letterMap['{'] = &symbol_brace_open;
    letterMap['}'] = &symbol_brace_close;
    letterMap['~'] = &symbol_tilde;
    letterMap['^'] = &symbol_caret;
    letterMap['<'] = &symbol_less;
    letterMap['>'] = &symbol_greater;
    letterMap['`'] = &symbol_backtick;
}

enum Element
{
    Fire,
    Water,
    Ice,
    None,
    Wind,
    Electric
};

struct Paddle
{
    Rectangle paddle;
} Paddle = {{SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT - 30, 150, 10}};

typedef struct Line
{
    Vector2 start;
    Vector2 end;
} Line;

Line line1, line2;

Vector2 originalBallSpeed = {350.0f, 350.0f};
int dirx;
int diry;

const int trailLength = 20; // Length of the trail

struct Ball
{
    Vector2 Position;
    Vector2 ballSpeed;
    float Radius;
    bool onPaddle;
    Element element;
    Vector2 trail[10]; // Array to hold previous positions for trail effect
} ball = {{SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50}, {350.0f, 350.0f}, 10.0f, true, Fire, {}};

struct Brick
{
    Rectangle brick;
    Element element;
    bool active;
};

Brick bricks[700];
int brickCount = 0;

void LoadAssets()
{

    Intro.Load("Gifs/Intro.gif", 3);

    myFont = LoadFont("fonts/square_block.ttf");
    myFont2 = LoadFont("fonts/Carre.ttf");
    myFont3 = LoadFont("fonts/zh-cn.ttf");

    wallHitSound = LoadSound("Sounds/wall-hit.wav");
    paddleHitSound = LoadSound("Sounds/paddle-hit.wav");
    explosionSound = LoadSound("Sounds/explosion.wav");
    freezeSound = LoadSound("Sounds/freeze.wav");
    meltSound = LoadSound("Sounds/melt.wav");
    eatSound = LoadSound("Sounds/eat.wav");
    ShockSound = LoadSound("Sounds/Shock.wav");
    LifeLossSound = LoadSound("Sounds/LifeLoss.wav");
    gameOverSound = LoadSound("Sounds/gameover.wav");
    gameWonSound = LoadSound("Sounds/win.wav");
    BuzzerSound = LoadSound("Sounds/buzzer.wav");
    IntroSound = LoadSound("Sounds/Intro.wav");
    BackgroundSound = LoadSound("Sounds/Background.wav");

    PaddleIcon = LoadTexture("Icons/paddle1.png");

    cry = LoadTexture("Icons/cry.png");
    cool = LoadTexture("Icons/cool.png");
    heart = LoadTexture("Icons/heart.png");
    icon = LoadImage("Icons/icon.png");
    Gradient = LoadTexture("Icons/gradient.png");
    Mock = LoadTexture("Icons/mock.png");
    Icon = LoadTextureFromImage(icon);

    FireAnim.Load("Gifs/FireAnim.gif", 2);
    WaterAnim.Load("Gifs/WaterAnim.gif", 20);
    ElectricAnim.Load("Gifs/ElectricAnim.gif", 1);
    IceAnim.Load("Gifs/IceAnim.gif", 32);
    WindAnim.Load("Gifs/WindAnim.gif", 3);
    EarthAnim.Load("Gifs/EarthAnim.gif", 16);
    FireBallAnim.Load("Gifs/fireball1.gif", 2);
    WaterBallAnim.Load("Gifs/waterball.gif", 10);
    DiscoAnim.Load("Gifs/disco.gif", 7);
    rollAnim.Load("Gifs/roll.gif", 5);

    JumpPadLeft = LoadTexture("Icons/JumpPadLeft.png");
    JumpPadRight = LoadTexture("Icons/JumpPadRight.png");
}

void UnloadAssets()
{
    UnloadFont(myFont);
    UnloadFont(myFont2);
    UnloadFont(myFont3);

    UnloadSound(wallHitSound);
    UnloadSound(paddleHitSound);
    UnloadSound(explosionSound);
    UnloadSound(freezeSound);
    UnloadSound(meltSound);
    UnloadSound(eatSound);
    UnloadSound(ShockSound);
    UnloadSound(LifeLossSound);
    UnloadSound(gameOverSound);
    UnloadSound(gameWonSound);
    UnloadSound(BuzzerSound);
    UnloadSound(IntroSound);
    UnloadSound(BackgroundSound);

    UnloadTexture(PaddleIcon);
    UnloadTexture(cry);
    UnloadTexture(cool);
    UnloadTexture(Gradient);
    UnloadTexture(heart);
    UnloadTexture(Icon);
    UnloadTexture(JumpPadLeft);
    UnloadTexture(JumpPadRight);
    UnloadTexture(Mock);

    UnloadImage(icon);
    FireAnim.Unload();
    WaterAnim.Unload();
    ElectricAnim.Unload();
    IceAnim.Unload();
    WindAnim.Unload();
    EarthAnim.Unload();
    FireBallAnim.Unload();
    WaterBallAnim.Unload();
    DiscoAnim.Unload();
    rollAnim.Unload();
}

void LoadHighScore()
{
    FILE *file = fopen(HIGH_SCORE_FILE, "rb");
    if (file != NULL)
    {
        fread(&highScore, sizeof(int), 1, file);
        fclose(file);
    }
    else
    {
        highScore = 0; // Default high score if file doesn't exist
    }
}

void SaveHighScore()
{
    FILE *file = fopen(HIGH_SCORE_FILE, "wb");
    if (file != NULL)
    {
        fwrite(&highScore, sizeof(int), 1, file);
        fclose(file);
    }
}

void UpdateHighScore()
{
    if (score > highScore)
    {
        highScore = score;
        SaveHighScore();
    }
}

bool CheckWinCondition()
{
    for (int i = 0; i < brickCount; i++)
    {
        if (bricks[i].active && bricks[i].element != None)
        {
            return false; // Found an elemental brick that's still active and elemental
        }
    }
    return true; // Only non-elemental bricks or no bricks left
}

Color GetElementColor(Element e)
{
    switch (e)
    {
    case Fire:
        return RED;
    case Water:
        return BLUE;
    case Ice:
        return SKYBLUE;
    case Electric:
        return YELLOW;
    case Wind:
        return GREEN;
    default:
        return GRAY;
    }
}

// Function to draw a letter
void DrawLetter(int startX, int startY, const int (*letter)[12][9])
{
    int initialBrick = brickCount;
    for (int row = 0; row < LETTER_HEIGHT; row++)
    {
        for (int col = 0; col < LETTER_WIDTH; col++)
        {
            if ((*letter)[row][col] == 1)
            {
                Rectangle brick = {
                    (float)startX + col * BRICK_WIDTH,
                    (float)startY + row * BRICK_HEIGHT,
                    BRICK_WIDTH,
                    BRICK_HEIGHT};
                Element element = (Element)((3 * row + col) % 4); // Random element
                bricks[brickCount++] = {brick, element, true};
            }
        }
    }
    int finalBrick = brickCount;
    int total = finalBrick - initialBrick;
    if (total >= 5)
    {
        for (int i = 1; i <= 6; i++)
        {
            int index = initialBrick + (i * total) / 7;
            if (i % 3 == 0)
                bricks[index].element = (Element)(4);
            else
                bricks[index].element = (Element)(5);
        }
    }
}

void InitBricks(char *userInput, int inputLength)
{
    brickCount = 0;
    int totalWidth = (inputLength * LETTER_WIDTH * BRICK_WIDTH + (inputLength - 1) * LETTER_GAP * BRICK_WIDTH);

    // Calculate starting X position to center the letters
    int startX = (SCREEN_WIDTH - totalWidth) / 2;
    int startY = 2 * BRICK_HEIGHT; // Vertical position

    // Side-wall bricks
    if (inputLength < 5)
    {
        for (int row = 0; row < LETTER_HEIGHT + 2; row++)
        {
            float y = startY + row * BRICK_HEIGHT;

            for (int i = 0; i < 3; i++)
            {
                float xLeft = i * BRICK_WIDTH;
                float xRight = SCREEN_WIDTH - (i + 1) * BRICK_WIDTH;

                Element elementLeft = (Element)((row + i) % 6);

                bricks[brickCount++] = {{xLeft, y, BRICK_WIDTH, BRICK_HEIGHT}, elementLeft, true};
                bricks[brickCount++] = {{xRight, y, BRICK_WIDTH, BRICK_HEIGHT}, elementLeft, true};
            }
        }
    }

    // Draw each letter with proper gaps
    for (int i = 0; i < inputLength; i++)
    {
        DrawLetter(startX + (LETTER_WIDTH + LETTER_GAP) * i * BRICK_WIDTH,
                   startY,
                   letterMap[(unsigned char)userInput[i]]);
    }

    // Bottom line of bricks
    int LINE_Y_OFFSET = (LETTER_HEIGHT + 4) * BRICK_HEIGHT;

    for (int col = 0; col < BRICK_COLS; col++)
    {
        float x = col * BRICK_WIDTH;

        for (int layer = 0; layer < 3; layer++)
        {
            float y = LINE_Y_OFFSET + layer * BRICK_HEIGHT;
            Element element;

            switch (layer)
            {
            case 0:
                element = (col % 6 == 2) ? Wind : (Element)((col + 1) % 4);
                break;
            case 1:
                element = (col % 6 == 4) ? Electric : (Element)((col + 1) % 3);
                break;
            case 2:
                element = (col % 6 == 0) ? Wind : (Element)(col % 4);
                break;
            }

            bricks[brickCount++] = {{x, y, BRICK_WIDTH, BRICK_HEIGHT}, element, true};
        }
    }
}

void React(Brick *brick, Element reactor);

void destroyAdjacent(Brick *B)
{
    PlaySound(explosionSound); // Play explosion sound
    Vector2 center = {B->brick.x + B->brick.width / 2, B->brick.y + B->brick.height / 2};
    for (int i = 0; i < brickCount; i++)
    {
        Brick *b = &bricks[i];
        if (!b->active)
            continue;
        Vector2 otherCenter = {b->brick.x + b->brick.width / 2, b->brick.y + b->brick.height / 2};
        float dx = fabs(center.x - otherCenter.x);
        float dy = fabs(center.y - otherCenter.y);
        if (dx <= BRICK_WIDTH * 1 && dy <= BRICK_HEIGHT * 1)
        {
            b->active = false;
            score += 5;
        }
    }
}

void reactAdjacent(Brick *brick, Element reactor)
{
    if (!brick || !brick->active)
        return;

    Vector2 center = {brick->brick.x + brick->brick.width / 2,
                      brick->brick.y + brick->brick.height / 2};

    // Limit reaction chain depth
    static int reactionDepth = 0;
    const int MAX_REACTION_DEPTH = 8;

    if (reactionDepth >= MAX_REACTION_DEPTH)
        return;

    reactionDepth++;

    for (int i = 0; i < brickCount; i++)
    {
        Brick *b = &bricks[i];
        if (!b->active || b == brick)
            continue;

        Vector2 otherCenter = {b->brick.x + b->brick.width / 2,
                               b->brick.y + b->brick.height / 2};
        float dx = fabs(center.x - otherCenter.x);
        float dy = fabs(center.y - otherCenter.y);

        if (dx <= BRICK_WIDTH && dy <= BRICK_HEIGHT)
        {
            React(b, reactor);
        }
    }

    reactionDepth--;
}

void reactFire(Brick *brick)
{
    Element BRICK_ELEMENT = brick->element;
    switch (BRICK_ELEMENT)
    {
    case Ice:
        PlaySound(meltSound);
        brick->element = Water;
        break;
    case Water:
        PlaySound(eatSound);
        brick->active = false;
        score += 5;
        break;
    case Electric:
        destroyAdjacent(brick);
        break;
    case Wind:
        reactAdjacent(brick, Fire);
        brick->active = false;
        score += 5;
        break;
    case None:
        PlaySound(wallHitSound);
        break;
    default:
        break;
    }
}

void reactIce(Brick *brick)
{
    Element BRICK_ELEMENT = brick->element;
    switch (BRICK_ELEMENT)
    {
    case Water:
        brick->element = Ice;
        PlaySound(freezeSound);
        break;
    case Fire:
    case Electric:
        PlaySound(eatSound);
        brick->active = false;
        score += 5;
        break;
    case Wind:
        reactAdjacent(brick, Ice);
        brick->active = false;
        score += 5;
        break;
    case None:
        brick->element = Ice;
        PlaySound(freezeSound);
        break;
    default:
        break;
    }
}

void reactWater(Brick *brick)
{
    Element BRICK_ELEMENT = brick->element;
    switch (BRICK_ELEMENT)
    {
    case Fire:
        PlaySound(eatSound);
        brick->active = false;
        score += 5;
        break;
    case Ice:
        PlaySound(wallHitSound);
        break;
    case Electric:
        PlaySound(ShockSound);
        reactAdjacent(brick, Electric);
        break;
    case Wind:
        reactAdjacent(brick, Water);
        brick->active = false;
        score += 5;
        break;
    case None:
        PlaySound(wallHitSound);
        break;
    default:
        break;
    }
}

void React(Brick *brick, Element reactor)
{
    if (!brick->active || brick->element == reactor)
    {
        PlaySound(wallHitSound);
        return;
    }

    Element originalElement = brick->element;

    switch (reactor)
    {
    case Fire:
        reactFire(brick);
        break;
    case Water:
        reactWater(brick);
        break;
    case Ice:
        reactIce(brick);
        break;
    default:
        break;
    }
    // Check if the brick's element has changed
    if (brick->element != originalElement)
    {
        return;
    }
    if (CheckWinCondition())
    {
        gameWon = true;
        ball.ballSpeed = {0, 0}; // Stop the ball

        UpdateHighScore();

        // Stop background music on win
        if (backgroundMusicPlaying)
        {
            StopSound(BackgroundSound);
            backgroundMusicPlaying = false;
        }
    }
}

void UpdateBall(float deltaTime)
{
    // Update trail positions
    for (int i = trailLength - 1; i > 0; i--)
    {
        ball.trail[i] = ball.trail[i - 1];
    }
    ball.trail[0] = ball.Position;

    // Decrease the collisionCooldown timer if it's greater than 0
    if (collisionCooldown > 0.0f)
    {
        collisionCooldown -= deltaTime; // Reduce by the frame time
        if (collisionCooldown < 0.0f)
        {
            collisionCooldown = 0.0f; // Prevent going negative
        }
    }

    if (!gameOver && !gameWon)
    {
        // Update the timer
        if (timerActive && !isPaused)
        {
            gameTimer -= deltaTime;

            // Reset timer when it reaches 0
            if (gameTimer <= 0.0f)
            {
                gameTimer = 10.0f;
                PlaySound(BuzzerSound);     // play a sound when timer resets
                PinballMode = !PinballMode; // Toggle Pinball mode
            }
        }
        else if (!timerActive)
        {
            gameTimer = 10.0f; // Keep showing full timer when inactive
        }

        if (ball.onPaddle)
        {
            ball.Position.x = Paddle.paddle.x + Paddle.paddle.width / 2;
            ball.Position.y = Paddle.paddle.y - ball.Radius - 5;

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE)) // Example input to release the ball
            {
                ball.onPaddle = false;
                timerActive = true; // Start the timer when game begins
                gameTimer = 10.0f;  // Ensure timer starts at 10
            }
        }
        else
        {

            // increase ball speed over time by 30% of orinigal speed every second untill it reaches double speed
            dirx = (ball.ballSpeed.x > 0) ? 1 : -1;
            diry = (ball.ballSpeed.y > 0) ? 1 : -1;
            if (fabs(ball.ballSpeed.x) < originalBallSpeed.x * 2)
            {
                ball.ballSpeed.x += dirx * originalBallSpeed.x * 0.25f * deltaTime;
            }
            if (fabs(ball.ballSpeed.y) < originalBallSpeed.y * 2)
            {
                ball.ballSpeed.y += diry * originalBallSpeed.y * 0.25f * deltaTime;
            }

            float angle = atan2f(ball.ballSpeed.y, ball.ballSpeed.x) * RAD2DEG;

            // reduce ball speed to normal speed when it reaches bottom 90% of screen
            if (ball.Position.y > SCREEN_HEIGHT * 0.9f && ball.ballSpeed.y > 0)
            {
                ball.ballSpeed.x = cosf(angle * DEG2RAD) * 350.0f;
                ball.ballSpeed.y = sinf(angle * DEG2RAD) * 350.0f;
            }

            // Increase Ball radius with proportion to height
            ball.Radius = 10.0f + (1 - ball.Position.y / (SCREEN_HEIGHT - 30)) * 5.0f; // Max radius 15 at top, min radius 10 at bottom

            ball.Position.x += ball.ballSpeed.x * deltaTime;
            ball.Position.y += ball.ballSpeed.y * deltaTime;

            // Check for collision with walls
            if (ball.Position.x + ball.Radius >= SCREEN_WIDTH)
            {
                PlaySound(wallHitSound); // Play wall hit sound
                ball.ballSpeed.x = -abs(ball.ballSpeed.x);
                ball.Position.x = SCREEN_WIDTH - ball.Radius; // Prevent sticking
            }
            else if (ball.Position.x - ball.Radius <= 0)
            {
                PlaySound(wallHitSound); // Play wall hit sound
                ball.ballSpeed.x = abs(ball.ballSpeed.x);
                ball.Position.x = ball.Radius;
            }

            if (ball.Position.y - ball.Radius <= 0)
            {
                PlaySound(wallHitSound); // Play wall hit sound
                ball.ballSpeed.y = abs(ball.ballSpeed.y);
                ball.Position.y = ball.Radius;
            }

            if (!PinballMode)
            {
                // Check for collision with paddle
                if (CheckCollisionCircleRec(ball.Position, ball.Radius, Paddle.paddle))
                {
                    PlaySound(paddleHitSound);
                    dirx = (ball.ballSpeed.x > 0) ? 1 : -1;
                    diry = (ball.ballSpeed.y > 0) ? 1 : -1;
                    // Change Ball speed to normal
                    ball.ballSpeed = {dirx * 350.0f, diry * 350.0f};

                    // Calculate relative intersection point (-1 to 1)
                    float relativeIntersect = (Paddle.paddle.x + Paddle.paddle.width / 2) - ball.Position.x;
                    float normalizedIntersect = relativeIntersect / (Paddle.paddle.width / 2);

                    // Calculate bounce angle (limit to 75 degrees)
                    float bounceAngle = normalizedIntersect * (75 * DEG2RAD);

                    // Update ball speed
                    float speed = sqrtf(ball.ballSpeed.x * ball.ballSpeed.x + ball.ballSpeed.y * ball.ballSpeed.y);
                    ball.ballSpeed.x = -sin(bounceAngle) * speed;
                    ball.ballSpeed.y = -cos(bounceAngle) * speed;

                    ball.Position.y = Paddle.paddle.y - ball.Radius - 1;
                }
            }
            else
            {
                // Check for collisions with line1
                if (CheckCollisionCircleLine(ball.Position, ball.Radius, line1.start, line1.end))
                {
                    if (collisionCooldown > 0.0f)
                    {
                        // If cooldown is active or if line is not rotating, ignore the collision
                        return;
                    }

                    PlaySound(paddleHitSound);

                    // Calculate the normal of the line
                    Vector2 lineDir = Vector2Normalize(Vector2Subtract(line1.start, line1.end));
                    Vector2 lineNormal = {lineDir.y, -lineDir.x};

                    // Reflect the ball's velocity off the line
                    ball.ballSpeed = Vector2Reflect(ball.ballSpeed, lineNormal);

                    // make ball faster by 50%
                    ball.ballSpeed.x = ball.ballSpeed.x * 1.5f;
                    ball.ballSpeed.y = ball.ballSpeed.y * 1.5f;

                    // Start the collision cooldown
                    collisionCooldown = collisionDelay;
                }

                // Check for collisions with line2
                if (CheckCollisionCircleLine(ball.Position, ball.Radius, line2.start, line2.end))
                {
                    if (collisionCooldown > 0.0f)
                    {
                        // If cooldown is active, ignore the collision
                        return;
                    }

                    PlaySound(paddleHitSound);

                    // Calculate the normal of the line
                    Vector2 lineDir = Vector2Normalize(Vector2Subtract(line2.end, line2.start));
                    Vector2 lineNormal = {-lineDir.y, lineDir.x};

                    // Reflect the ball's velocity off the line
                    ball.ballSpeed = Vector2Reflect(ball.ballSpeed, lineNormal);

                    // make ball faster by 50%
                    ball.ballSpeed.x = ball.ballSpeed.x * 1.5f;
                    ball.ballSpeed.y = ball.ballSpeed.y * 1.5f;

                    // Start the collision cooldown
                    collisionCooldown = collisionDelay;
                }
            }

            // Brick collisions
            for (int i = 0; i < brickCount; i++)
            {
                Brick *b = &bricks[i];
                if (!b->active)
                    continue;

                if (CheckCollisionCircleRec(ball.Position, ball.Radius, b->brick))
                {
                    float angle = atan2f(ball.ballSpeed.y, ball.ballSpeed.x) * RAD2DEG;

                    if (ball.element == Fire && b->element == Fire)
                    {
                        continue;
                    }

                    if (ball.element == Water && b->element == Water)
                    {
                        // If both ball and brick are Fire, ignore collision but decrease speed very slightly
                        ball.ballSpeed.x = cosf(angle * DEG2RAD) * 300.0f;
                        ball.ballSpeed.y = sinf(angle * DEG2RAD) * 300.0f;
                        continue;
                    }
                    if (ball.element == Ice && b->element == Water)
                    {
                        ball.ballSpeed.x = cosf(angle * DEG2RAD) * 150.0f;
                        ball.ballSpeed.y = sinf(angle * DEG2RAD) * 150.0f;
                    }

                    if (ball.element == Water && b->element == Ice)
                    {
                        ball.element = Ice;
                        ball.ballSpeed.x = cosf(angle * DEG2RAD) * 150.0f;
                        ball.ballSpeed.y = sinf(angle * DEG2RAD) * 150.0f;
                    }

                    // Determine which side was hit
                    float ballLeft = ball.Position.x - ball.Radius;
                    float ballRight = ball.Position.x + ball.Radius;
                    float ballTop = ball.Position.y - ball.Radius;
                    float ballBottom = ball.Position.y + ball.Radius;

                    float brickLeft = b->brick.x;
                    float brickRight = b->brick.x + b->brick.width;
                    float brickTop = b->brick.y;
                    float brickBottom = b->brick.y + b->brick.height;

                    // Calculate penetration depths
                    float overlapLeft = ballRight - brickLeft;
                    float overlapRight = brickRight - ballLeft;
                    float overlapTop = ballBottom - brickTop;
                    float overlapBottom = brickBottom - ballTop;

                    // Find smallest overlap (side with least penetration)
                    bool fromLeft = overlapLeft < overlapRight;
                    bool fromTop = overlapTop < overlapBottom;

                    float minOverlapX = fromLeft ? overlapLeft : overlapRight;
                    float minOverlapY = fromTop ? overlapTop : overlapBottom;

                    // Collision response based on side
                    if (minOverlapX < minOverlapY)
                    {
                        // Horizontal collision
                        if (fromLeft)
                        {
                            ball.Position.x = brickLeft - ball.Radius;
                            ball.ballSpeed.x = -abs(ball.ballSpeed.x); // Ensure bounce left
                        }
                        else
                        {
                            ball.Position.x = brickRight + ball.Radius;
                            ball.ballSpeed.x = abs(ball.ballSpeed.x); // Ensure bounce right
                        }
                    }
                    else
                    {
                        // Vertical collision
                        if (fromTop)
                        {
                            ball.Position.y = brickTop - ball.Radius;
                            ball.ballSpeed.y = -abs(ball.ballSpeed.y); // Ensure bounce up
                        }
                        else
                        {
                            ball.Position.y = brickBottom + ball.Radius;
                            ball.ballSpeed.y = abs(ball.ballSpeed.y); // Ensure bounce down
                        }
                    }

                    React(b, ball.element);
                    if (CheckWinCondition())
                    {
                        gameWon = true;
                        ball.ballSpeed = {0, 0}; // Stop the ball
                    }
                }
            }

            // Check for game over
            if (ball.Position.y + ball.Radius >= SCREEN_HEIGHT)
            {
                PlaySound(LifeLossSound);
                lives--;
                timerActive = false;
                if (lives <= 0)
                {
                    gameOver = true;
                    ball.ballSpeed = {0, 0}; // Stop the ball

                    UpdateHighScore();

                    // Stop background music on game over
                    if (backgroundMusicPlaying)
                    {
                        StopSound(BackgroundSound);
                        backgroundMusicPlaying = false;
                    }
                }
                else
                {
                    // Reset ball position to paddle
                    ball.onPaddle = true;
                    ball.ballSpeed = {350.0f, 350.0f}; // Reset ball speed for the next life
                }
            }
        }
    }
}

void HandleInput(float deltaTime)
{
    if (isPaused)
        return;
    static float lastMouseX = GetMousePosition().x; // remember last frame
    float mouseX = GetMousePosition().x;
    float deltaX = mouseX - lastMouseX; // difference

    // Apply movement based on delta
    if (fabs(deltaX) > 0.1f)
    {                                     // threshold to ignore noise
        Paddle.paddle.x += deltaX * 1.2f; // scale factor = sensitivity
    }

    // Keyboard movement
    if (IsKeyDown(KEY_LEFT))
        Paddle.paddle.x -= 800 * deltaTime;
    if (IsKeyDown(KEY_RIGHT))
        Paddle.paddle.x += 800 * deltaTime;

    // Clamp paddle inside screen
    if (Paddle.paddle.x < 0)
        Paddle.paddle.x = 0;
    if (Paddle.paddle.x + Paddle.paddle.width > SCREEN_WIDTH)
        Paddle.paddle.x = SCREEN_WIDTH - Paddle.paddle.width;

    // Update memory for next frame
    lastMouseX = mouseX;

    // Clamp paddle within screen bounds
    if (Paddle.paddle.x < 0)
        Paddle.paddle.x = 0;
    else if (Paddle.paddle.x + Paddle.paddle.width > SCREEN_WIDTH)
        Paddle.paddle.x = SCREEN_WIDTH - Paddle.paddle.width;

    if (IsKeyPressed(KEY_ONE))
    {
        ball.element = Fire;
    }
    if (IsKeyPressed(KEY_TWO))
    {
        ball.element = Ice;
    }
    if (IsKeyPressed(KEY_THREE))
    {
        ball.element = Water;
    }
    if (IsKeyPressed(KEY_F))
    {
        ToggleFullscreen();
    }

    // press 'O' to instantly win the game (for testing purposes)
    if (IsKeyPressed(KEY_O))
    {
        for (int i = 0; i < brickCount; i++)
        {
            if (bricks[i].active && bricks[i].element != None)
            {
                bricks[i].active = false;
            }
        }
        gameWon = true;
        ball.ballSpeed = {0, 0}; // Stop the ball
        StopSound(BackgroundSound);
        backgroundMusicPlaying = false;
    }

    if (IsKeyPressed(KEY_TAB))
    {
        isPaused = !isPaused;
        if (isPaused)
        {
            PauseSound(BackgroundSound);
        }
        else
        {
            ResumeSound(BackgroundSound);
        }
    }

    // Handle the rotation logic when the LEFT key is pressed
    if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_UP)) && !isRotating && PinballMode)
    {
        isRotating = true;
        elapsedTime = 0.0f; // Reset the timer
    }
    if ((IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) || IsKeyPressed(KEY_DOWN)) && !isRotating2 && PinballMode)
    {
        isRotating2 = true;
        elapsedTime2 = 0.0f; // Reset the timer
    }

    // If the rotation is in progress, interpolate between the current and target rotation angles
    if (isRotating)
    {
        // Interpolate the rotation angle smoothly over time
        elapsedTime += deltaTime;
        if (elapsedTime <= rotationTime)
        {
            // Calculate the interpolated rotation angle
            currentRotation = Lerp(ROT, (-1 * ROT), elapsedTime / rotationTime);
        }
        else
        {
            // Once the rotation is done, switch the target angle back
            currentRotation = (-1 * ROT);
            isRotating = false;
            elapsedTime = 0.0f;
        }
    }
    else
    {
        // After the rotation, interpolate back to the original state
        if (elapsedTime <= rotationTime)
        {
            elapsedTime += deltaTime;
            currentRotation = Lerp((-1 * ROT), ROT, elapsedTime / rotationTime);
        }
    }

    // If the rotation is in progress, interpolate between the current and target rotation angles
    if (isRotating2)
    {
        // Interpolate the rotation angle smoothly over time
        elapsedTime2 += deltaTime;
        if (elapsedTime2 <= rotationTime)
        {
            // Calculate the interpolated rotation angle
            currentRotation2 = Lerp(ROT, (-1 * ROT), elapsedTime2 / rotationTime);
        }
        else
        {
            // Once the rotation is done, switch the target angle back
            currentRotation2 = (-1 * ROT);
            isRotating2 = false;
            elapsedTime2 = 0.0f;
        }
    }
    else
    {
        // After the rotation, interpolate back to the original state
        if (elapsedTime2 <= rotationTime)
        {
            elapsedTime2 += deltaTime;
            currentRotation2 = Lerp((-1 * ROT), ROT, elapsedTime2 / rotationTime);
        }
    }

    // Update the line's rotation based on the current rotation angle
    line1.start = {Paddle.paddle.x - 10, Paddle.paddle.y - 15};
    line1.end = {Paddle.paddle.x - 10 + Paddle.paddle.width / 2 * cosf(currentRotation * DEG2RAD),
                 Paddle.paddle.y - 15 + Paddle.paddle.width / 2 * sinf(currentRotation * DEG2RAD)};

    // Update line2 opposite to line1 on paddle
    line2.start = {Paddle.paddle.x + 10 + Paddle.paddle.width, Paddle.paddle.y - 15};
    line2.end = {Paddle.paddle.x + 10 + Paddle.paddle.width - Paddle.paddle.width / 2 * cosf(currentRotation2 * DEG2RAD), Paddle.paddle.y - 15 + Paddle.paddle.width / 2 * sinf(currentRotation2 * DEG2RAD)};
}

void Interface()
{
    DrawTextureEx(Icon, (Vector2){(float)(SCREEN_WIDTH / 2 - Icon.width / 4), (float)(SCREEN_HEIGHT / 2 - Icon.height / 1.75 - 25)}, 0.0f, 0.5f, WHITE);
    DrawTextEx(myFont, "REACT-O-BALL", (Vector2){(float)(SCREEN_WIDTH / 2 - MeasureTextEx(myFont, "REACT-O-BALL", 60, 1).x / 2), (float)(SCREEN_HEIGHT / 2 - 50)}, 60, 1, DARKGRAY);

    // Draw disco gif along the bottom of the screen

    for (int i = 0; i < 6; i++)
    {
        DiscoAnim.Draw((Rectangle){(float)(i * SCREEN_WIDTH / 6), (float)SCREEN_HEIGHT * 0.75f, (float)SCREEN_WIDTH / 6, (float)SCREEN_HEIGHT / 4});
    }
    // Display high score
    char highScoreText[50];
    sprintf(highScoreText, "Best Score: %d", highScore);
    DrawTextEx(myFont3, highScoreText, (Vector2){SCREEN_WIDTH / 2 - MeasureTextEx(myFont3, highScoreText, 35, 1).x / 2, SCREEN_HEIGHT / 2 + 15}, 35, 1, GOLD);

    DrawTextEx(myFont, "Enter Your Text:", (Vector2){SCREEN_WIDTH / 2 - MeasureTextEx(myFont, "Enter Your Text:", 40, 1).x / 2, SCREEN_HEIGHT / 2 + 60}, 40, 1, DARKGRAY);

    // Display current input (or placeholders)
    if (inputPos == 0)
    {
        DrawText("___", SCREEN_WIDTH / 2 - MeasureText("___", 60) / 2, SCREEN_HEIGHT / 2 + 120, 60, LIGHTGRAY);
    }
    else
    {
        // Draw the current letters
        char displayText[6] = {0}; // To hold the input letters + one space for underscore

        // Copy the entered letters into the displayText array
        for (int i = 0; i < inputPos; i++)
        {
            displayText[i] = userInput[i]; // Display letters
        }

        // Display the current letters (centered)
        DrawTextEx(myFont3, displayText, (Vector2){SCREEN_WIDTH / 2 - MeasureTextEx(myFont3, displayText, 60, 1).x / 2, SCREEN_HEIGHT / 2 + 120}, 60, 1, DARKGRAY);
    }
}

void DrawGame()
{
    ClearBackground(WHITE);

    if (gameOver || gameWon)
    {
        if (gameOver && !gameOverSoundPlayed)
        {
            PlaySound(gameOverSound);
            gameOverSoundPlayed = true;
        }
        else if (gameWon && !gameWonSoundPlayed)
        {
            PlaySound(gameWonSound);
            gameWonSoundPlayed = true;
        }
        // Draw rollanim on left and right side of screen if win
        if (gameWon)
        {
            rollAnim.Draw((Rectangle){0.0f, 75.0f, (float)SCREEN_WIDTH / 2.5f, (float)SCREEN_HEIGHT - 150});
            rollAnim.Draw((Rectangle){(float)(SCREEN_WIDTH - SCREEN_WIDTH / 2.5f), 75.0f, (float)SCREEN_WIDTH / 2.5f, (float)SCREEN_HEIGHT - 150});
        }
        // Draw smile or cry texture depending on win or game over
        DrawTextureEx(gameOver ? Mock : cool, (Vector2){(float)(SCREEN_WIDTH / 2 - (gameOver ? Mock.width : cool.width) / 4), (float)(SCREEN_HEIGHT / 2 - (gameOver ? Mock.height : cool.height) / 1.75)}, 0.0f, 0.5f, WHITE);

        DrawTextEx(myFont, gameOver ? "Game Over!" : "You Win!", (Vector2){SCREEN_WIDTH / 2 - MeasureTextEx(myFont, gameOver ? "Game Over!" : "You Win!", 45, 1).x / 2, SCREEN_HEIGHT / 2 - 20}, 45, 1, (gameOver ? RED : GREEN));
        DrawTextEx(myFont, "Press R to Restart", (Vector2){SCREEN_WIDTH / 2 - MeasureTextEx(myFont, "Press R to Restart", 35, 1).x / 2, SCREEN_HEIGHT / 2 + 50}, 35, 1, DARKGRAY);
        // Display score
        char scoreText[50];
        sprintf(scoreText, "Final Score: %d", score);
        DrawTextEx(myFont3, scoreText, (Vector2){SCREEN_WIDTH / 2 - MeasureTextEx(myFont3, scoreText, 45, 1).x / 2, SCREEN_HEIGHT / 2 + 100}, 45, 1, BLACK);

        // Add high score display
        char highScoreText[50];
        sprintf(highScoreText, "Best Score: %d", highScore);
        Color highScoreColor = (score == highScore) ? GOLD : GRAY;
        DrawTextEx(myFont3, highScoreText, (Vector2){SCREEN_WIDTH / 2 - MeasureTextEx(myFont3, highScoreText, 35, 1).x / 2, SCREEN_HEIGHT / 2 + 150}, 35, 1, highScoreColor);

        // Show "NEW HIGH SCORE!" if applicable
        if (score == highScore && score > 0)
        {
            DrawTextEx(myFont, "NEW HIGH SCORE!", (Vector2){SCREEN_WIDTH / 2 - MeasureTextEx(myFont, "NEW HIGH SCORE!", 30, 1).x / 2, SCREEN_HEIGHT / 2 + 200}, 30, 1, GOLD);
        }

        if (IsKeyPressed(KEY_R))
        {
            gameOver = false;
            gameWon = false;
            lives = 4;
            score = 0;
            InitBricks(userInput, inputPos);
            ball.Position = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50};
            ball.ballSpeed = (Vector2){350.0f, 350.0f};
            ball.onPaddle = true;
            ball.element = Fire;
            gameOverSoundPlayed = false;
            gameWonSoundPlayed = false;

            // Reset timer and Pinball mode
            timerActive = false;
            gameTimer = 10.0f;
            PinballMode = false;

            // stop any playing sounds
            StopSound(gameOverSound);
            StopSound(gameWonSound);
            gameOverSoundPlayed = false;
            gameWonSoundPlayed = false;

            // Background music
            if (!backgroundMusicPlaying)
            {
                PlaySound(BackgroundSound);
                backgroundMusicPlaying = true;
            }
        }
    }
    else
    {
        // Draw the background gradient fit to screen height
        DrawTextureEx(Gradient, (Vector2){OffsetX, 0}, 0.0f, (float)SCREEN_HEIGHT / Gradient.height, WHITE);

        // Draw the paddle or JumpPad
        if (!PinballMode)
        {
            // Draw paddle icon exactly as the paddle
            DrawTextureEx(PaddleIcon, (Vector2){(float)(Paddle.paddle.x + Paddle.paddle.width / 2 - PaddleIcon.width * 0.07f), (float)(Paddle.paddle.y - PaddleIcon.height * 0.025f)}, 0.0f, 0.14f, WHITE);
            // DrawRectangleRec(Paddle.paddle, WHITE);
        }
        else
        {
            DrawTextureEx(JumpPadLeft, (Vector2){line1.start.x - 5, line1.start.y - 5}, currentRotation, 0.08f, WHITE);
            DrawTextureEx(JumpPadRight, (Vector2){line2.end.x + 5, line2.end.y - 5}, -currentRotation2, 0.08f, WHITE);
        }

        // DrawLine(line1.start.x, line1.start.y, line1.end.x, line1.end.y, RED);
        // DrawLine(line2.end.x, line2.end.y, line2.start.x, line2.start.y, RED);

        // Draw the bricks
        for (int i = 0; i < brickCount; i++)
        {
            Brick *brick = &bricks[i];
            if (!brick->active)
                continue;

            switch (brick->element)
            {
            case Fire:
                FireAnim.Draw(brick->brick);
                break;
            case Water:
                WaterAnim.Draw(brick->brick);
                break;
            case Electric:
                ElectricAnim.Draw(brick->brick);
                break;
            case Ice:
                IceAnim.Draw(brick->brick);
                break;
            case Wind:
                WindAnim.Draw(brick->brick);
                break;
            case None:
                EarthAnim.Draw(brick->brick);
                break;
            default:
                break;
            }
            DrawRectangleLinesEx(brick->brick, 0.5f, RAYWHITE);
        }

        // Draw ball Trail
        if (!ball.onPaddle) // Don't draw trail when ball is on paddle
        {
            for (int i = 0; i < trailLength; i++)
            {
                float alpha = (float)(trailLength - i) / trailLength; // Fade out
                Color trailColor;
                switch (ball.element)
                {
                case Fire:
                    trailColor = (Color){255, 69, 0, (unsigned char)(alpha * 100)}; // Orange with fading alpha
                    break;
                case Water:
                    trailColor = (Color){0, 191, 255, (unsigned char)(alpha * 100)}; // Deep Sky Blue with fading alpha
                    break;
                case Ice:
                    trailColor = (Color){135, 206, 250, (unsigned char)(alpha * 100)}; // Light Sky Blue with fading alpha
                    break;
                default:
                    trailColor = (Color){192, 192, 192, (unsigned char)(alpha * 100)}; // Silver with fading alpha
                    break;
                }
                DrawCircle(ball.trail[i].x, ball.trail[i].y, ball.Radius * (alpha * 0.7f), trailColor);
            }
        }

        // Draw future ball Trajectory
        if (ball.Position.y > SCREEN_HEIGHT * 0.75f && ball.ballSpeed.y > 0 && !PinballMode) // only show when ball is in the bottom 85% of the screen
        {
            Vector2 futurePos = ball.Position;
            Vector2 futureSpeed = ball.ballSpeed;
            for (int i = 0; i < 300; i += 5) // Increment by 5 pixels for smoother line
            {
                futurePos.x += futureSpeed.x * (5.0f / sqrtf(futureSpeed.x * futureSpeed.x + futureSpeed.y * futureSpeed.y));
                futurePos.y += futureSpeed.y * (5.0f / sqrtf(futureSpeed.x * futureSpeed.x + futureSpeed.y * futureSpeed.y));

                // Check for wall collisions
                if (futurePos.x + ball.Radius >= SCREEN_WIDTH || futurePos.x - ball.Radius <= 0)
                {
                    futureSpeed.x = -futureSpeed.x;
                }
                if (futurePos.y - ball.Radius <= 0)
                {
                    futureSpeed.y = -futureSpeed.y;
                }

                // Check for paddle collision
                if (CheckCollisionCircleRec(futurePos, ball.Radius, Paddle.paddle))
                {
                    float relativeIntersect = (Paddle.paddle.x + Paddle.paddle.width / 2) - futurePos.x;
                    float normalizedIntersect = relativeIntersect / (Paddle.paddle.width / 2);
                    float bounceAngle = normalizedIntersect * (75 * DEG2RAD);
                    float speed = sqrtf(futureSpeed.x * futureSpeed.x + futureSpeed.y * futureSpeed.y);
                    futureSpeed.x = -sin(bounceAngle) * speed;
                    futureSpeed.y = -cos(bounceAngle) * speed;
                    futurePos.y = Paddle.paddle.y - ball.Radius - 1; // Adjust position to avoid sticking
                }

                // Draw the trajectory point gradually fading out
                if (i % 10 == 0) // Draw every 15 pixels for clarity in raywhite
                {
                    DrawCircle(futurePos.x, futurePos.y, ball.Radius * 0.3f, Fade(RAYWHITE, 1.0f - (float)i / 300.0f));
                }
            }
        }

        // Draw the ball
        switch (ball.element)
        {
        case Fire:
            DrawCircle(ball.Position.x, ball.Position.y, ball.Radius + 1, BLACK);
            FireBallAnim.Draw((Rectangle){ball.Position.x - ball.Radius, ball.Position.y - ball.Radius, ball.Radius * 2, ball.Radius * 2});
            break;

        case Water:
            DrawCircle(ball.Position.x, ball.Position.y, ball.Radius + 1, WHITE);
            WaterBallAnim.Draw((Rectangle){ball.Position.x - ball.Radius, ball.Position.y - ball.Radius, ball.Radius * 2, ball.Radius * 2});
            break;

        case Ice:
            DrawCircle(ball.Position.x, ball.Position.y, ball.Radius + 1, BLUE);
            DrawCircle(ball.Position.x, ball.Position.y, ball.Radius - 2, WHITE);
            break;

        default:
            break;
        }

        if (ball.onPaddle)
        {
            ball.Radius = 10.0f; // Reset ball radius when on paddle
        }

        // Display Score
        char scoreText[50];
        sprintf(scoreText, "%d", score);
        DrawTextEx(myFont2, scoreText, (Vector2){10, 10}, 40, 1, BLACK);
        DrawTextEx(myFont2, scoreText, (Vector2){12, 12}, 40, 1, WHITE);

        // Display high score
        char highScoreText1[20], highScoreText2[20];
        sprintf(highScoreText1, "Best:");
        sprintf(highScoreText2, "%d", highScore);
        DrawTextEx(myFont3, highScoreText1, (Vector2){MeasureTextEx(myFont2, "00000000", 40, 1).x, 9}, 40, 1, BLACK);
        DrawTextEx(myFont3, highScoreText1, (Vector2){MeasureTextEx(myFont2, "00000000", 40, 1).x + 2, 11}, 40, 1, WHITE);
        DrawTextEx(myFont2, highScoreText2, (Vector2){MeasureTextEx(myFont3, highScoreText1, 40, 1).x + MeasureTextEx(myFont2, "00000000", 40, 1).x, 14}, 35, 1, BLACK);
        DrawTextEx(myFont2, highScoreText2, (Vector2){MeasureTextEx(myFont3, highScoreText1, 40, 1).x + MeasureTextEx(myFont2, "00000000", 40, 1).x + 2, 16}, 35, 1, GREEN);

        // Draw lives
        for (int i = 1; i <= lives; i++)
        {
            DrawTextureEx(heart, (Vector2){(float)(SCREEN_WIDTH - heart.width * i * 0.05), 8.0f}, 0.0f, 0.04f, WHITE);
        }

        // Draw a stylish timer bar
        float barWidth = 200.0f;
        float barHeight = 30.0f;
        float barX = SCREEN_WIDTH / 2 - barWidth / 2;
        float barY = 20.0f;
        // Background bar
        DrawRectangleRounded((Rectangle){barX, barY, barWidth, barHeight}, 1.0f, 30, LIGHTGRAY);

        // Progress bar color based on timer state
        Color progressColor;
        if (!timerActive)
        {
            progressColor = GRAY; // Gray when inactive
        }
        else
        {
            progressColor = gameTimer > 3.0f ? (Color){57, 173, 72, 255} : RED;
        }

        DrawRectangleRounded((Rectangle){barX, barY, (gameTimer / 10.0f) * barWidth, barHeight}, 1.0f, 30, progressColor);

        // Border color based on state
        Color borderColor;
        if (!timerActive)
        {
            borderColor = GRAY;
        }
        else
        {
            borderColor = gameTimer > 3.0f ? DARKGREEN : (Color){139, 69, 19, 255};
        }

        DrawRectangleRoundedLinesEx((Rectangle){barX - 2, barY - 2, barWidth + 4, barHeight + 4}, 1.0f, 1.0f, 2, borderColor);

        // Timer text
        char timeText[10];
        sprintf(timeText, "%.1f", gameTimer);
        Color textColor = timerActive ? BLACK : DARKGRAY;
        DrawTextEx(myFont3, timeText, (Vector2){barX + barWidth / 2 - MeasureTextEx(myFont3, timeText, 25, 1).x / 2, barY + barHeight / 2 - MeasureTextEx(myFont3, timeText, 25, 1).y / 2}, 25, 1, textColor);
    }
}

void PauseScreen()
{
    // Create a semi-transparent overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.4f));

    // Create the background for the pause box
    DrawRectangleRounded((Rectangle){SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, 0.3f, 20, Fade(RAYWHITE, 0.9f));

    // Define button width and height
    float buttonHeight = SCREEN_HEIGHT / 14;
    float buttonWidth = SCREEN_WIDTH / 2 - buttonHeight * 2;

    // Calculate the startY to vertically center the buttons
    float startY = SCREEN_HEIGHT / 2;

    // Calculate the startX to horizontally center the buttons
    float startX = (SCREEN_WIDTH - buttonWidth) / 2;

    // Define the buttons' rectangles
    Rectangle continueButton = {startX, (startY - 2.5f * buttonHeight), buttonWidth, buttonHeight};
    Rectangle restartButton = {startX, (startY - 0.5f * buttonHeight), buttonWidth, buttonHeight};
    Rectangle exitButton = {startX, (startY + 1.5f * buttonHeight), buttonWidth, buttonHeight};

    // Draw the buttons
    DrawRectangleRounded(continueButton, 0.8f, 10, WHITE);
    DrawTextEx(myFont, "Continue", (Vector2){continueButton.x + (buttonWidth - MeasureTextEx(myFont, "Continue", 40, 1).x) / 2, continueButton.y + (buttonHeight - 35) / 2}, 40, 1, DARKGRAY);

    DrawRectangleRounded(restartButton, 0.8f, 10, WHITE);
    DrawTextEx(myFont, "Restart", (Vector2){restartButton.x + (buttonWidth - MeasureTextEx(myFont, "Restart", 40, 1).x) / 2, restartButton.y + (buttonHeight - 35) / 2}, 40, 1, DARKGRAY);

    DrawRectangleRounded(exitButton, 0.8f, 10, WHITE);
    DrawTextEx(myFont, "Exit", (Vector2){exitButton.x + (buttonWidth - MeasureTextEx(myFont, "Exit", 40, 1).x) / 2, exitButton.y + (buttonHeight - 35) / 2}, 40, 1, DARKGRAY);

    // Handle mouse hover and click for each button
    Vector2 mouse = GetMousePosition();

    // Highlight the buttons on hover
    if (CheckCollisionPointRec(mouse, continueButton))
    {
        DrawRectangleRoundedLinesEx(continueButton, 0.8f, 10, 2.0f, (Color){0, 128, 0, 255}); // medium Green color on hover
        DrawTextEx(myFont, "Continue", (Vector2){continueButton.x + (buttonWidth - MeasureTextEx(myFont, "Continue", 40, 1).x) / 2, continueButton.y + (buttonHeight - 35) / 2}, 40, 1, (Color){0, 128, 0, 255});
    }
    if (CheckCollisionPointRec(mouse, restartButton))
    {
        DrawRectangleRoundedLinesEx(restartButton, 0.8f, 10, 2.0f, (Color){0, 0, 255, 255}); // Blue color on hover
        DrawTextEx(myFont, "Restart", (Vector2){restartButton.x + (buttonWidth - MeasureTextEx(myFont, "Restart", 40, 1).x) / 2, restartButton.y + (buttonHeight - 35) / 2}, 40, 1, (Color){0, 0, 255, 255});
    }
    if (CheckCollisionPointRec(mouse, exitButton))
    {
        DrawRectangleRoundedLinesEx(exitButton, 0.8f, 10, 2.0f, (Color){255, 0, 0, 255}); // Red color on hover
        DrawTextEx(myFont, "Exit", (Vector2){exitButton.x + (buttonWidth - MeasureTextEx(myFont, "Exit", 40, 1).x) / 2, exitButton.y + (buttonHeight - 35) / 2}, 40, 1, (Color){255, 0, 0, 255});
    }

    // Click detection
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (CheckCollisionPointRec(mouse, continueButton))
        {
            // Resume the game
            isPaused = false;

            // Resume background music
            if (backgroundMusicPlaying)
            {
                ResumeSound(BackgroundSound);
            }
        }
        else if (CheckCollisionPointRec(mouse, restartButton))
        {
            // Restart the game
            isPaused = false;
            gameOver = false;
            gameWon = false;
            lives = 4;
            score = 0;
            InitBricks(userInput, inputPos);
            // Initialize ball position
            ball.onPaddle = true;
            // reset timer and Pinball mode
            timerActive = false;
            gameTimer = 10.0f;
            PinballMode = false;
        }
        else if (CheckCollisionPointRec(mouse, exitButton))
        {
            // Exit the game
            shouldExit = true;
        }
    }
}

void RestrictCursor()
{
    // Get the current mouse position
    Vector2 mousePos = GetMousePosition();

    // Manually confine the mouse inside the window
    if (mousePos.x < 0)
    {
        mousePos.x = 0;
    }
    else if (mousePos.x > SCREEN_WIDTH)
    {
        mousePos.x = SCREEN_WIDTH;
    }

    if (mousePos.y < 0)
    {
        mousePos.y = 0;
    }
    else if (mousePos.y > SCREEN_HEIGHT)
    {
        mousePos.y = SCREEN_HEIGHT;
    }

    // Update the mouse position to keep it within the window bounds
    SetMousePosition(mousePos.x, mousePos.y);
}

void UpdateBackground(float deltaTime)
{
    float targetOffsetX = 0.0f, time = 0.0f;
    switch (ball.element)
    {
    case Fire:
        targetOffsetX = -10.0f;
        break;
    case Water:
        targetOffsetX = -2500.0f;
        break;
    case Ice:
        targetOffsetX = -1300.0f;
        break;
    default:
        break;
    }

    if (OffsetX != targetOffsetX)
    {
        time += deltaTime;

        if (time >= 0.5f)
        {
            time = 0.5f;
        }

        OffsetX = OffsetX + (targetOffsetX - OffsetX) * 2.5 * time;
    }
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "React-O-Ball");
    SetTargetFPS(60);

    InitAudioDevice();

    InitLetterMap();
    LoadAssets();
    LoadHighScore();
    SetWindowIcon(icon);

    srand(time(NULL));
    PlaySound(IntroSound);
    while (WindowShouldClose() == false)
    {
        float deltaTime = GetFrameTime();

        // Handle GIF intro playback
        if (gifPlaying)
        {

            // Update the GIF animation only if not finished
            if (!gifFinished)
            {

                Intro.Update();

                // Check if we've reached the last frame
                if (Intro.currentFrame >= Intro.totalFrames - 1)
                {
                    gifFinished = true;
                }
            }

            // Check for mouse click to proceed
            if (GetKeyPressed() || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                gifPlaying = false;
                Intro.Unload();
                inInterface = true;
                StopSound(IntroSound);
            }

            // Draw the GIF frame
            BeginDrawing();
            ClearBackground(WHITE);

            Intro.Draw((Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});

            EndDrawing();

            continue; // Skip the rest of the game loop while GIF is playing
        }

        FireAnim.Update();
        WaterAnim.Update();
        ElectricAnim.Update();
        IceAnim.Update();
        WindAnim.Update();
        EarthAnim.Update();
        FireBallAnim.Update();
        WaterBallAnim.Update();
        DiscoAnim.Update();
        rollAnim.Update();

        BeginDrawing();
        ClearBackground(WHITE);

        if (inInterface)
        {
            Interface();
            if (!backgroundMusicPlaying)
            {
                PlaySound(BackgroundSound);
                backgroundMusicPlaying = true;
            }

            // Handle text input
            int key = GetCharPressed();

            while (key > 0)
            {
                if (inputPos < 5 && key != KEY_SPACE)
                {
                    PlaySound(wallHitSound);
                    userInput[inputPos] = key;
                    inputPos++;
                }
                key = GetCharPressed();
            }

            // Handle backspace
            if (IsKeyPressed(KEY_BACKSPACE) && inputPos > 0)
            {
                PlaySound(wallHitSound);
                inputPos--;
                userInput[inputPos] = 0;
            }

            if (IsKeyPressed(KEY_ENTER) && inputPos >= 2) // Ensure at least 2 letters
            {
                InitBricks(userInput, inputPos);
                if (IsKeyPressed(KEY_ENTER))
                    inInterface = false;
            }
        }
        else
        {
            HandleInput(deltaTime);
            if (!isPaused)
            {
                UpdateBall(deltaTime);
                if (backgroundMusicPlaying && !IsSoundPlaying(BackgroundSound) && !gameOver && !gameWon)
                {
                    PlaySound(BackgroundSound);
                }
                HideCursor();
                RestrictCursor();
                UpdateBackground(deltaTime);
            }
            DrawGame();
            if (isPaused)
            {
                ShowCursor();
                PauseScreen();
            }
        }

        EndDrawing();
        if (shouldExit)
            break;
    }

    UnloadAssets();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}