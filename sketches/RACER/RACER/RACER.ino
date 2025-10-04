#include <U8x8lib.h>

#define WIDTH 8
#define HEIGHT 8
#define MAX_OBS 3
#define BTN_LEFT 2
#define BTN_RIGHT 3

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/*clock=*/ SCL, /*data=*/ SDA, /*reset=*/ U8X8_PIN_NONE);

struct Obstacle {
  uint8_t x, y;
};

Obstacle obs[MAX_OBS];
uint8_t playerX = WIDTH/2;
const uint8_t playerY = HEIGHT - 1;

void setup() {
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);

  u8x8.begin();
  u8x8.setFont(u8x8_font_5x8_r);  // Simple font

  // Initialize obstacles
  for (int i = 0; i < MAX_OBS; i++) {
    obs[i].x = random(0, WIDTH);
    obs[i].y = random(-HEIGHT, 0);
  }
}

void drawFrame() {
  u8x8.clear();

  // Draw obstacles
  for (int i = 0; i < MAX_OBS; i++) {
    if (obs[i].y < HEIGHT) {
      u8x8.setCursor(obs[i].x, obs[i].y);
      u8x8.print("#");  // simple block
    }
  }

  // Draw player
  u8x8.setCursor(playerX, playerY);
  u8x8.print("@");  // player symbol
}

void updateObstacles() {
  for (int i = 0; i < MAX_OBS; i++) {
    obs[i].y++;
    if (obs[i].y >= HEIGHT) {
      obs[i].y = 0;
      obs[i].x = random(0, WIDTH);
    }
  }
}

void loop() {
  // Move player
  if (!digitalRead(BTN_LEFT) && playerX > 0) playerX--;
  if (!digitalRead(BTN_RIGHT) && playerX < WIDTH - 1) playerX++;

  updateObstacles();
  drawFrame();
  delay(200);
}
