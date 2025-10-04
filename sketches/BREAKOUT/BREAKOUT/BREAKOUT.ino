#include <Arduino.h>
#include <U8x8lib.h>

// OLED pins
#define OLED_SCL PB2
#define OLED_SDA PB0
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(OLED_SCL, OLED_SDA, U8X8_PIN_NONE);

// Buttons
#define BTN_LEFT  PB3
#define BTN_RIGHT PB4
#define BTN_START PB1

// Screen
#define WIDTH  16
#define HEIGHT 8
#define PADDLE_W 3
#define MAX_LEVELS 5

// Game variables
uint8_t paddleX = 6;
uint8_t ballX = 8, ballY = 5;
int8_t ballDX = 1, ballDY = -1;
uint8_t level = 0;
bool bricks[HEIGHT-2][WIDTH];
bool running = false;
bool showMenu = true;
bool winScreen = false;
bool loseScreen = false;

// Levels: 1=brick, 0=empty
const uint8_t levels[MAX_LEVELS][HEIGHT-2][WIDTH] PROGMEM = {
  // Level 1
  {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  },
  // Level 2
  {
    {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
    {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  },
  // Level 3
  {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,0,0,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  },
  // Level 4
  {
    {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
    {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
    {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  },
  // Level 5
  {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,0,1,1,1,1,1,0,0,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  }
};

// Load level into bricks array
void loadLevel(uint8_t lvl){
  for(uint8_t y=0; y<HEIGHT-2; y++)
    for(uint8_t x=0; x<WIDTH; x++)
      bricks[y][x] = pgm_read_byte(&(levels[lvl][y][x]));
}

// Reset ball and paddle
void resetBallPaddle(){
  paddleX = 6;
  ballX = 8;
  ballY = 5;
  ballDX = 1;
  ballDY = -1;
}

// Draw everything
void drawGame(){
  u8x8.clear();

  // Draw bricks
  for(uint8_t y=0; y<HEIGHT-2; y++)
    for(uint8_t x=0; x<WIDTH; x++)
      if(bricks[y][x]) u8x8.drawGlyph(x, y, 0xFF); // solid block

  // Draw paddle (line)
  for(uint8_t i=0; i<PADDLE_W; i++)
    u8x8.drawGlyph(paddleX+i, HEIGHT-1, 0xFF);

  // Draw ball
  u8x8.drawGlyph(ballX, ballY, 0xFF);
}

// Show menu or win/lose screen
void drawMenu(){
  u8x8.clear();
  if(showMenu) u8x8.setCursor(2,3), u8x8.print("BREAKOUT");
  else if(winScreen) u8x8.setCursor(4,3), u8x8.print("YOU WIN!");
  else if(loseScreen) u8x8.setCursor(3,3), u8x8.print("YOU LOSE!");
  u8x8.setCursor(1,5); u8x8.print("PB1 TO START");
}

void setup() {
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_START, INPUT_PULLUP);

  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  drawMenu();
}

void loop() {
  bool left = !digitalRead(BTN_LEFT);
  bool right = !digitalRead(BTN_RIGHT);
  bool start = !digitalRead(BTN_START);

  if(!running){
    if(start){
      running = true;
      showMenu = false;
      winScreen = false;
      loseScreen = false;
      level = 0;
      loadLevel(level);
      resetBallPaddle();
    }
    drawMenu();
    delay(50);
    return;
  }

  // Paddle move
  if(left && paddleX>0) paddleX--;
  if(right && paddleX<WIDTH-PADDLE_W) paddleX++;

  // Ball move
  ballX += ballDX;
  ballY += ballDY;

  // Wall collision
  if(ballX==0 || ballX==WIDTH-1) ballDX *= -1;
  if(ballY==0) ballDY *= -1;

  // Paddle collision
  if(ballY==HEIGHT-1 && ballX>=paddleX && ballX<paddleX+PADDLE_W){
    ballDY = -1;
    if(ballX==paddleX) ballDX=-1;
    else if(ballX==paddleX+PADDLE_W-1) ballDX=1;
  }

  // Brick collision
  if(ballY<HEIGHT-2 && bricks[ballY][ballX]){
    bricks[ballY][ballX]=0;
    ballDY*=-1;
  }

  // Ball lost
  if(ballY>=HEIGHT){
    running = false;
    loseScreen = true;
  }

  // Check if level cleared
  bool cleared=true;
  for(uint8_t y=0;y<HEIGHT-2;y++)
    for(uint8_t x=0;x<WIDTH;x++)
      if(bricks[y][x]) cleared=false;

  if(cleared){
    level++;
    if(level>=MAX_LEVELS){
      running=false;
      winScreen=true;
    } else {
      loadLevel(level);
      resetBallPaddle();
    }
  }

  drawGame();
  delay(80); // snappy speed
}
