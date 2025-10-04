#include <Arduino.h>
#include <U8x8lib.h>

// ------------ Pins ------------
#define BTN1_PIN PB3
#define BTN2_PIN PB4

// OLED: SSD1306 128x64, SW I2C PB2=SCL, PB0=SDA
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(PB2, PB0, U8X8_PIN_NONE);

// ------------ Helpers ------------
static inline uint8_t rd(uint8_t p){ return !digitalRead(p); } // active low
static inline void waitRelease(){ while(rd(BTN1_PIN)||rd(BTN2_PIN)){} delay(20); }
static inline void clr(){ u8x8.clear(); }
static inline void center(uint8_t y,const char*s){
  int8_t x=(16-(int)strlen(s))/2; if(x<0)x=0; u8x8.drawString(x,y,s);
}
static inline void msg(const char*a,const char*b){ clr(); center(2,a); center(5,b); }

// ------------ Menu ------------
const char gameNames[][12] PROGMEM={
 "Breakout","Runner","PongCPU",
 "Invaders","Flappy","Reflex"
};
enum {G_BREAKOUT,G_RUNNER,G_PONG,G_INVADERS,G_FLAPPY,G_REFLEX,G_COUNT};

uint8_t menu(){
  uint8_t sel=0;
  for(;;){
    clr();
    for(uint8_t i=0;i<G_COUNT;i++){
      char name[12]; strcpy_P(name,gameNames[i]);
      if(i==sel){ u8x8.drawString(0,i,">"); u8x8.drawString(2,i,name);}
      else u8x8.drawString(2,i,name);
    }
    if(rd(BTN1_PIN)){ sel=(sel+1)%G_COUNT; waitRelease();}
    if(rd(BTN2_PIN)){ waitRelease(); return sel;}
    delay(70);
  }
}

// ------------ Games ------------

// 1. Breakout
void gameBreakout(){
  int paddle=7; int ballX=7,ballY=5; int dx=1,dy=-1;
  bool bricks[16]; for(int i=0;i<16;i++) bricks[i]=true;
  for(;;){
    clr();
    for(int i=0;i<16;i++) if(bricks[i]) u8x8.drawGlyph(i,0,'#');
    u8x8.drawGlyph(paddle,7,'='); u8x8.drawGlyph(paddle+1,7,'=');
    u8x8.drawGlyph(ballX,ballY,'O');
    if(rd(BTN1_PIN)&&paddle>0){ paddle--; waitRelease();}
    if(rd(BTN2_PIN)&&paddle<14){ paddle++; waitRelease();}
    ballX+=dx; ballY+=dy;
    if(ballX<=0||ballX>=15) dx=-dx;
    if(ballY<=0){ if(bricks[ballX]){ bricks[ballX]=false; dy=-dy;} else dy=-dy;}
    if(ballY==6 && ballX>=paddle && ballX<=paddle+1) dy=-dy;
    if(ballY>=7){ msg("Game Over","BT2=again"); while(!rd(BTN2_PIN)){} waitRelease(); return;}
    delay(120);
  }
}

// 2. Runner
void gameRunner(){
  int lane=1;
  for(;;){
    int obs=random(0,3); 
    for(int x=15;x>=0;x--){
      clr();
      if(obs==1) u8x8.drawGlyph(x,2,'^');
      if(obs==2) u8x8.drawGlyph(x,4,'^');
      u8x8.drawGlyph(1,lane?4:2,'@');
      if(rd(BTN1_PIN)) { lane=0; waitRelease(); }
      if(rd(BTN2_PIN)) { lane=1; waitRelease(); }
      if(x==1 && ((obs==1&&lane==0)||(obs==2&&lane==1))){
        msg("Crash!","BT2=again"); while(!rd(BTN2_PIN)){} waitRelease(); return;
      }
      delay(120);
    }
  }
}

// 3. Pong vs CPU
void gamePong(){
  int player=3,cpu=3,ballX=7,ballY=3,dx=1,dy=1;
  for(;;){
    clr();
    u8x8.drawGlyph(0,player,'|'); u8x8.drawGlyph(15,cpu,'|');
    u8x8.drawGlyph(ballX,ballY,'O');
    if(rd(BTN1_PIN)&&player>0){player--; waitRelease();}
    if(rd(BTN2_PIN)&&player<7){player++; waitRelease();}
    if(ballY>cpu) cpu++; else if(ballY<cpu) cpu--;
    ballX+=dx; ballY+=dy;
    if(ballY<=0||ballY>=7) dy=-dy;
    if(ballX==1 && ballY==player){ dx=1; }
    if(ballX==14 && ballY==cpu) dx=-1;
    if(ballX<=0||ballX>=15){ msg("Game Over","BT2=again"); while(!rd(BTN2_PIN)){} waitRelease(); return;}
    delay(120);
  }
}

// 4. Invaders Mini
void gameInvaders(){
  bool aliens[8]; for(int i=0;i<8;i++) aliens[i]=true;
  int ship=3;
  for(;;){
    clr();
    for(int i=0;i<8;i++) if(aliens[i]) u8x8.drawGlyph(i,0,'W');
    u8x8.drawGlyph(ship,7,'A');
    if(rd(BTN1_PIN)&&ship>0){ship--; waitRelease();}
    if(rd(BTN2_PIN)){
      for(int y=6;y>=0;y--){
        clr();
        for(int i=0;i<8;i++) if(aliens[i]) u8x8.drawGlyph(i,0,'W');
        u8x8.drawGlyph(ship,7,'A');
        u8x8.drawGlyph(ship,y,'|');
        if(y==0 && aliens[ship]){ aliens[ship]=false; }
        delay(80);
      }
      waitRelease();
    }
    bool any=false; for(int i=0;i<8;i++) if(aliens[i]) any=true;
    if(!any){ msg("You Win!","BT2=again"); while(!rd(BTN2_PIN)){} waitRelease(); return;}
    delay(150);
  }
}

// 5. Flappy Dot
void gameFlappy(){
  int y=3;
  for(;;){
    int gap=random(1,6);
    for(int x=15;x>=0;x--){
      clr();
      for(int i=0;i<8;i++){ if(i!=gap&&i!=gap+1) u8x8.drawGlyph(x,i,'|'); }
      u8x8.drawGlyph(2,y,'@');
      if(rd(BTN1_PIN)&&y>0){ y--; waitRelease();}
      else if(y<7) y++;
      if(x==2 && (y!=gap&&y!=gap+1)){ msg("Crash!","BT2=again"); while(!rd(BTN2_PIN)){} waitRelease(); return;}
      delay(120);
    }
  }
}

// 6. Reflex Duel
void gameReflex(){
  clr(); center(2,"Wait...");
  delay(500+random(800,2000));
  bool want=(random(0,2)==0);
  clr(); center(2,want?"BT1!":"BT2!");
  while(!rd(BTN1_PIN)&&!rd(BTN2_PIN)){}
  bool good=(want?rd(BTN1_PIN):rd(BTN2_PIN));
  clr(); center(3,good?"Correct":"Wrong");
  delay(1500);
}

// ------------ Setup / Loop ------------
void setup(){
  pinMode(BTN1_PIN,INPUT_PULLUP);
  pinMode(BTN2_PIN,INPUT_PULLUP);
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  randomSeed(analogRead(1));
}

void loop(){
  switch(menu()){
    case G_BREAKOUT: gameBreakout(); break;
    case G_RUNNER: gameRunner(); break;
    case G_PONG: gamePong(); break;
    case G_INVADERS: gameInvaders(); break;
    case G_FLAPPY: gameFlappy(); break;
    case G_REFLEX: gameReflex(); break;
  }
}
