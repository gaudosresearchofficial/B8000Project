#include <U8x8lib.h>

// Software I2C (choose pins: SDA = PB0, SCL = PB2)
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 2, /* data=*/ 0, /* reset=*/ U8X8_PIN_NONE);

// Button pins (Player 1 and Player 2)
const int buttonP1 = 3; // PB3
const int buttonP2 = 4; // PB4

int scoreP1 = 0;
int scoreP2 = 0;
bool gameStarted = false;
bool gameOver = false;

void setup() {
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  pinMode(buttonP1, INPUT_PULLUP);
  pinMode(buttonP2, INPUT_PULLUP);
}

void loop() {
  if (!gameStarted) {
    showMainMenu();
    if (digitalRead(buttonP2) == LOW) {
      gameStarted = true;
      delay(300);
    }
  } else if (gameOver) {
    showWinScreen();
  } else {
    playGame();
  }
}

void showMainMenu() {
  u8x8.clear();
  u8x8.drawString(0, 1, " CLICK N' MISS ");
  u8x8.drawString(0, 3, "BY MALCOLM FARRUGIA");
  u8x8.drawString(0, 6, "P2 PRESS TO START");
  delay(100);
}

void showWinScreen() {
  u8x8.clear();
  if (scoreP1 >= 10) {
    u8x8.drawString(0, 2, "PLAYER 1 WINS!");
  } else if (scoreP2 >= 10) {
    u8x8.drawString(0, 2, "PLAYER 2 WINS!");
  }
  u8x8.drawString(0, 5, "RESET TO PLAY AGAIN");
  delay(100);
}

void playGame() {
  if (digitalRead(buttonP1) == LOW) {
    scoreP1++;
    delay(150);
  }
  if (digitalRead(buttonP2) == LOW) {
    scoreP2++;
    delay(150);
  }

  if (scoreP1 >= 10 || scoreP2 >= 10) {
    gameOver = true;
    return;
  }

  u8x8.clear();
  u8x8.setCursor(0, 1);
  u8x8.print("P1: ");
  u8x8.print(scoreP1);

  u8x8.setCursor(0, 3);
  u8x8.print("P2: ");
  u8x8.print(scoreP2);

  delay(100);
}
