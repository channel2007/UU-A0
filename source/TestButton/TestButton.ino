/***********************************************************************************
 * 
 * 程式說明：測試Arduboy2按鈕API.
 * 
 **********************************************************************************/
// 含入Arduboy2.h
#include <Arduboy2.h>

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64

// 宣告arduboy實例.
Arduboy2 arduboy;

// 顯示位置.
byte x = 64;
byte y = 32;

String str = "O";

//---------------------------------------------------------------------------------
// 初始.
//---------------------------------------------------------------------------------
void setup() {
  // 啟動arduboy實例.
  arduboy.begin();  
  // 設定每秒30帧更新率.
  arduboy.setFrameRate(30);
}

//---------------------------------------------------------------------------------
// 主迴圈.
//---------------------------------------------------------------------------------
void loop() {
  // 等待下一帧時間到才進入繼續執行.
  if (!(arduboy.nextFrame()))
    return;

  // 保存按鈕狀態以供justPressed、justReleased使用.
  arduboy.pollButtons();    

  // 將畫面清為黑色.
  arduboy.clear();

  // [按下持續執行]右方向鍵(右移動).
  if(arduboy.pressed(RIGHT_BUTTON) && (x < SCREEN_WIDTH)) {
    x++;
  }
  // [按下持續執行]左方向鍵(左移動).
  if(arduboy.pressed(LEFT_BUTTON) && (x > 0)) {
    x--;
  }
  // [按下持續執行]下方向鍵(下移動).
  if(arduboy.pressed(DOWN_BUTTON) && (y < SCREEN_HEIGHT)) {
    y++;
  }
  // [按下持續執行]上方向鍵(上移動).
  if(arduboy.pressed(UP_BUTTON) && (y > 0)) {
    y--;
  }
  // [按下執行1次]按下A按鈕(變換O字串).
  if(arduboy.justPressed(A_BUTTON)) {
    str = "O";
  }
  // [放開執行1次]按下B按鈕(變換X字串).
  if(arduboy.justReleased(B_BUTTON)) {
    str = "X";
  }

  // 顯示字串座標.
  arduboy.setCursor(0, 0);
  arduboy.print(F("x="));
  arduboy.print(x);
  arduboy.print(F(" y="));
  arduboy.print(y);

  // 設定字串位置.
  arduboy.setCursor(x, y);
  // 顯示字串.
  arduboy.print(str);

  // 更新畫面.
  arduboy.display();  
}
