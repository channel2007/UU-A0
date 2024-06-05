/***********************************************************************************
 * 
 * 程式說明：測試Arduboy2繪圖API.
 * 
 **********************************************************************************/
// 含入Arduboy2.h
#include <Arduboy2.h>

// 宣告arduboy實例.
Arduboy2 arduboy;

//---------------------------------------------------------------------------------
// 初始.
//---------------------------------------------------------------------------------
void setup() {
  // 啟動arduboy實例.
  arduboy.begin();  
  // 設定每秒15帧更新率.
  arduboy.setFrameRate(15);
}

//---------------------------------------------------------------------------------
// 主迴圈.
//---------------------------------------------------------------------------------
void loop() {
  // 等待下一帧時間到才進入繼續執行.
  if (!(arduboy.nextFrame()))
    return;

  // 將畫面清為黑色.
  arduboy.clear();

  // 將畫面填滿白色.
  //arduboy.fillScreen (WHITE);

  // 顯示文字.
  arduboy.print(F("Hi!!"));

  // 畫圓(空心).
  arduboy.drawCircle ( 3, 12, 2);
  // 畫圓(實心).
  arduboy.fillCircle (10, 12, 2);
  
  // 畫線.
  arduboy.drawLine (14, 12, 20, 14);
  // 畫垂直線.
  arduboy.drawFastVLine (32, 10, 6);
  // 畫橫線.
  arduboy.drawFastHLine (35, 13, 5);
  
  // 畫方形(空心).
  arduboy.drawRect (24, 10, 6, 6);  
  // 畫方形(實心).
  arduboy.fillRect (42, 10, 6, 6);
  
  // 畫圓形矩形(空心).
  arduboy.drawRoundRect (50, 8, 16, 10, 4);
  // 畫圓形矩形(實心).
  arduboy.fillRoundRect (68, 8, 16, 10, 4);
  
  // 畫三角形(空心).
  arduboy.drawTriangle (86, 8, 86, 16, 90, 12);
  // 畫三角形(實心).
  arduboy.fillTriangle (92, 8, 92, 16, 96, 12);
  
  // 畫點.
  arduboy.drawPixel (100, 12);
  
  // 更新畫面.
  arduboy.display();  
}
