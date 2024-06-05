
// 開啟ArduboyTones測試音效功能.
#include <Arduboy2.h>
#include <ArduboyTones.h>
#include "bitmaps.h"

// 建立Arduboy2物件.
Arduboy2 arduboy;
// 建立音效物件.
ArduboyTones sound(arduboy.audio.enabled);

// make an ArdBitmap instance that will use the given the screen buffer and dimensions
#define ARDBITMAP_SBUF arduboy.getBuffer()
#include <ArdBitmap.h>
ArdBitmap<WIDTH, HEIGHT> ardbitmap;

// 廚師動畫.
byte chefAni[4] = {0, 0, 0, 0};
// 廚師動畫Frame.
byte chefAniFrame = 0;
// 廚師位置.
byte chefPos = 1;
// 廚師位置-備份.
byte chefPosTemp = chefPos;
// 在原地拋接旗標.
bool waitThrow = false;

// 肉動畫.
byte meatAni[4] = {0, 0, 0, 0};
// 肉移動高度.
byte meatHeight[4] = {4, 4, 4, 4};
// 肉移動方向.
// 0:往上.
// 1:往下.
// 2:判斷肉是否掉落.
byte meatDirection[4] = {0, 0, 0, 0};
// 肉移動順序.
// A:2,0,1
// B:2,3,0,1
byte meatMoveA[4] = {2, 0, 1, 0};
byte meatMoveB[4] = {2, 3, 0, 1};
// 肉移動指標.
byte meatMoveP = 0;

// 老鼠動畫Frame.
byte mouseAniFrame = 0;
// 貓動畫Frame.
byte catAniFrame = 1;

// 生命動畫Frame.
byte lifeAniFrame = 0;
byte lifeAniFrameTemp = 0;
// 生命閃爍次數.
byte lifeFalsh = 0;

// 模式動畫Frame.
// 0:A.
// 1:B.
byte modeAniFrame = 0;

// 時脈.
// 0:肉動畫-1.
// 1:肉動畫-2.
// 2:肉動畫-3.
// 3:肉動畫-4.
// 4:老鼠動畫.
// 5:廚師動畫.
// 6:貓干擾.
// 7:水壺.
unsigned long clockSystem[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// 遊戲模式.
// 3:準備開始遊戲-1.
// 4:準備開始遊戲-2.
// 5:遊戲中.
// 11:老鼠拿到肉-暫停N秒.
// 12:老鼠拿到肉-播放動畫.
// 13:老鼠拿到肉-準備繼續遊戲.
// 50:生命歸零.
// 100:GameOver.
// 150:主選單.
// 151:製作人員.
byte gameMode = 150;

// 反轉畫面.
bool invertFlag = true;

// 分數.
unsigned int score = 0;
// 肉移動速度.
unsigned int meatSpeed = 250;

// 貓干擾-是否啟動.
bool interferenceEnable = false;
// 貓干擾-啟動機率.
byte interferenceProbability = 20;
// 貓干擾-持續時間.
int  interferenceTime = 0;

// 音效開關.
bool gameSound = true;

// 游標選到標題.
// 0:開始遊戲.
// 1:製作人員.
byte cursorMode = 0;
// 游標動畫.
bool cursorAni = true;

// 製作人員字移動.
byte staffMoveA = 0;
byte staffMoveB = 128;

// 水壺-未開.
byte kettleOpen = 1;
// 加速計次.
byte accelerate = 0;

// 密技-需先死亡1隻.
// 上上下下左右左右:生命歸零+自動玩.
byte secretTechnique = 0;
// 密技-自動玩.
bool autoRun=false;

//----------------------------------------------------------------------------
// 初始時脈.
//----------------------------------------------------------------------------
void initClock(byte n = 255) {
  if (n == 255) {
    for (byte i = 0; i < (sizeof(clockSystem) / sizeof(unsigned long)); i++)
      clockSystem[i] = millis();
  } else {
    clockSystem[n] = millis();
  }
}

//----------------------------------------------------------------------------
// 檢查啟動時脈.
//----------------------------------------------------------------------------
bool checkClock(byte id, unsigned long t) {
  unsigned long diff = millis() - clockSystem[id];
  if (diff > t) {
    clockSystem[id] = millis() - (diff - t);
    return true;
  }
  return false;
}

// --------------------------------------------------------------
// 加分數與設定肉高度.
// --------------------------------------------------------------
void setScoreAndMeatHeight(byte id) {
  // 亂數設定肉飛的高度(2~4).
  byte ran = random(3) + 2;
  meatHeight[id] = ran;
  //Serial.println(ran);

  // 加分數.
  score++;

  // 速度加快.  
  if(accelerate < 50){
    accelerate++;
  }else{
    meatSpeed = 200;  
  }
  
  // 破關.
  if (score > 999) {
    // 歸0.
    score = 0;
    // 啟動水壺.
    kettleOpen = 1;
    // 初始時脈.
    initClock();
  }
  // 死亡次數歸0.
  if (score == 300 && lifeAniFrame>0){
    //lifeAniFrame = 0;
    lifeReset();            // 生命歸零.
  }  
}

// --------------------------------------------------------------
// 移動肉.
// --------------------------------------------------------------
void moveMeat(byte id) {
  // 遊戲模式(5:遊戲中).
  if (gameMode == 5) {
    //.
    if (checkClock(id, meatSpeed)) {
      // 接肉失敗.
      if (meatDirection[id] == 2 && meatAni[id] == 0 && chefPos != id) {
        // 關閉肉.
        meatAni[id] = 5;
        // 設定老鼠撿到肉.
        setGetMeat(id + 1);
        return;
      }
      // 判斷肉是否掉落狀態.
      if (meatDirection[id] == 2) {
        // 設定肉往上移動.
        meatDirection[id] = 0;
      }

      // 處理讓肉依序移動.
      meatMoveP++;

      // 0:A.
      if (modeAniFrame == 0) {
        if (meatMoveP > 2)
          meatMoveP = 0;

        // 1:B.
      } else {
        if (meatMoveP > 3)
          meatMoveP = 0;
      }

      // 往上.
      if (meatDirection[id] == 0) {
        meatAni[id]++;
        // 判斷肉高度返回.
        if (meatAni[id] > meatHeight[id]) {
          meatAni[id] = meatHeight[id] - 1;
          meatDirection[id] = 1;
        }
        // 往下.
      } else {
        meatAni[id]--;        
        if (meatAni[id] == 0) {
          meatDirection[id] = 2;
          // 密技-自動玩.
          if(autoRun)
            chefPos = id;
        }
      }
      // 初始肉時脈.
      for (byte i = 0; i < 5; i++)
        initClock(i);

      // 位置0的肉不能再0的位置(故意讓玩家有機會接不到)而且要是在往上的時候才會出現貓干擾.
      if (meatAni[0] > 0)
        interference();   // 亂數貓干擾.

      // 音效.
      //sound.noTone();
      sound.tone(NOTE_FS0, 24);
    }

    // 貓干擾-啟動中處理(讓肉固定在位置2).
    if (interferenceEnable && id == 0) {
      // 固定肉位置.
      meatAni[id] = 2;
      // 設定肉往下.
      meatDirection[id] = 1;
    }

  }
}

// --------------------------------------------------------------
// 重新開始遊戲.
// --------------------------------------------------------------
void resetGame() {
  // 初始變數.
  for (byte i = 0; i < 5; i++) {
    // 初始肉移動方向.
    meatDirection[i] = 0;
  }

  // 初始加快分數計次.
  accelerate = 0;
  // 初始肉速度.
  meatSpeed = 250;
    
  // 貓干擾-是否啟動.
  interferenceEnable = false;
  // 貓動畫Frame.
  catAniFrame = 1;
  // 設定位置0的肉位置.
  meatAni[0]  = 2;

  // 3:準備開始遊戲-1.
  gameMode = 3;
  // 設定廚師拋肉.
  setChefThrow();
  // 設定廚師位置.
  chefPos = 1;
  // 初始時脈.
  initClock();
}

// --------------------------------------------------------------
// 設定老鼠拿到肉位置.
// pos:1~4.
// --------------------------------------------------------------
void setGetMeat(byte pos) {
  // 播放音效.
  sound.tone(NOTE_DS4, 100);
  // 設定老鼠位置.
  mouseAniFrame = pos;
  // 老鼠拿到肉-暫停1秒.
  gameMode = 11;
  // 初始時脈.
  initClock(4);
}

// --------------------------------------------------------------
// 更新老鼠拿到肉位置.
// --------------------------------------------------------------
void updateGetMeat() {
  // 遊戲模式.
  // 11:老鼠拿到肉-暫停N秒.
  if (gameMode == 11) {
    secretTechnique = 0;
    if (checkClock(4, 750))
      gameMode = 12;

    // 12:老鼠拿到肉-播放動畫.
  } else if (gameMode == 12) {
    secretTechnique = 0;
    if (checkClock(4, 180)) {
      mouseAniFrame++;
      if (mouseAniFrame > 5) {
        mouseAniFrame = 5;
        gameMode = 13;
        // 初始時脈.
        initClock(4);
      } else {
        // 播放音效.
        sound.tone(NOTE_DS4, 100);
      }
    }

    // 13:老鼠拿到肉-準備繼續遊戲.
  } else if (gameMode == 13) {
    if (checkClock(4, 500)) {
      // 設定老鼠位置.
      mouseAniFrame = 0;
      // 處理扣生命.
      lifeAniFrame++;
      if (lifeAniFrame > 2) {
        //lifeAniFrame = 3;
        // 設定GameOver.
        gameMode = 100;
        // 初始時脈.
        initClock();        
        return;
      }

      //-------------------------------------------------------------
      // 重新開始遊戲.
      //-------------------------------------------------------------
      resetGame();
    }
  }
}

// --------------------------------------------------------------
// 設定廚師拋肉.
// --------------------------------------------------------------
void setChefThrow() {
  // 初始廚師、肉動畫.
  for (byte i = 0; i < 5; i++)
    chefAni[i] = meatAni[i] = 0;

  // 初始廚師時脈.
  initClock(5);
}

// --------------------------------------------------------------
// 更新廚師拋肉.
// --------------------------------------------------------------
void updateChefThrow() {
  // 3:準備開始遊戲-1.
  if (gameMode == 3 ) {
    if (checkClock( 5, 300)) {
      // 初始肉時脈.
      for (byte i = 0; i < 5; i++) {
        chefAni[i] = meatAni[i] = 1;
        initClock(i);
      }
      // 初始廚師時脈.
      initClock(5);
      // 4:準備開始遊戲-2.
      gameMode = 4;
    }

    // 4:準備開始遊戲-2.
  } else  if (gameMode == 4) {
    if (checkClock( 5, 300)) {
      // 初始肉時脈.
      for (byte i = 0; i < 5; i++) {
        // 設定肉動畫.
        meatAni[i] = 2;
        // 初始肉時脈.
        initClock(i);
      }
      // 初始廚師動畫.
      for (byte i = 0; i < 5; i++)
        chefAni[i] = 0;
      // 5:遊戲中.
      gameMode = 5;
    }
  }
}

// --------------------------------------------------------------
// 判斷廚師拋肉.
// --------------------------------------------------------------
void  ifChefThrow(byte pos, bool m = true) {
  // 廚師移動初始動畫設定.
  if (chefPosTemp != chefPos)
    chefAni[pos] = 0;

  // 初始在原地拋接.
  if (waitThrow) {
    if (checkClock( 5, 100)) {
      // 設定肉動畫.
      meatAni[pos] = 1;
      // 設定廚師動畫.
      chefAni[pos] = 1;
      // 關閉原地等待拋肉旗標.
      waitThrow = false;
    }
    return;
  }

  // 恢復廚師初始動作
  if (chefAni[pos] == 1) {
    if (checkClock( 5, 200))
      chefAni[pos] = 0;
  }
  chefPosTemp = chefPos;

  // 判斷是否執行拋肉.
  if (meatDirection[pos] == 2 && meatAni[pos] == 0 && chefPos == pos) {
    // 移到位置直接拋肉.
    if (m) {
      // 設定肉往上移動.
      meatDirection[pos] = 0;
      // 設定肉動畫.
      meatAni[pos] = 1;
      // 設定廚師動畫.
      chefAni[pos] = 1;
      // 在原地等待拋肉.
    } else {
      waitThrow = true;
    }
    // 加分數與設定肉高度.
    setScoreAndMeatHeight(pos);
    // 初始廚師時脈.
    initClock(5);
    // 音效.
    sound.noTone();
    sound.tone(NOTE_GS5, 47);
  }
}

// --------------------------------------------------------------
// 貓干擾.
// --------------------------------------------------------------
void interference() {
  byte ran = 0;

  // 貓干擾-是否啟動.
  // 啟動處理.
  if (interferenceEnable) {
    // 判斷貓縮回.
    if (checkClock( 6, interferenceTime)) {
      // 設定貓縮回.
      catAniFrame = 1;
      // 關閉.
      interferenceEnable = false;
    }
    // 未啟動處理.
  } else {
    // 亂數.
    ran = random(255);
    // 判斷是否啟動.
    if (ran < interferenceProbability && meatAni[0] == 2) {
      // 設定貓出現.
      catAniFrame = 0;

      // 貓干擾-時間(1/1000).
      interferenceTime = random(3000) + 500;
      // 啟動.
      interferenceEnable = true;
      initClock(6);
    }
  }
}

// --------------------------------------------------------------
// 音效開關.
// --------------------------------------------------------------
void setSound(bool s){
  // 音效開關.
  if(s)
    arduboy.audio.on();  
  // 關閉音效(靜音).
  else 
    arduboy.audio.off();  
}

// --------------------------------------------------------------
// 秀數字.
// --------------------------------------------------------------
void showScore(int n, int16_t x, int16_t y){
  if(n==0)
    Sprites::drawSelfMasked( x, y, n0, 0);
  else if(n==1)
    Sprites::drawSelfMasked( x, y, n1, 0);
  else if(n==2)
    Sprites::drawSelfMasked( x, y, n2, 0);
  else if(n==3)
    Sprites::drawSelfMasked( x, y, n3, 0);
  else if(n==4)
    Sprites::drawSelfMasked( x, y, n4, 0);
  else if(n==5)
    Sprites::drawSelfMasked( x, y, n5, 0);
  else if(n==6)
    Sprites::drawSelfMasked( x, y, n6, 0);
  else if(n==7)
    Sprites::drawSelfMasked( x, y, n7, 0);
  else if(n==8)
    Sprites::drawSelfMasked( x, y, n8, 0);
  else if(n==9)
    Sprites::drawSelfMasked( x, y, n9, 0);    
}

// --------------------------------------------------------------
// 模式A-B.
// --------------------------------------------------------------
void initMode(bool a){
  // 遊戲模式-A.
  if(a)      
    modeAniFrame = 0;
  // 遊戲模式-B.
  else      
    modeAniFrame = 1;

  // 生命動畫Frame.
  lifeAniFrame = 0;
  // 初始分數.
  score = 0;
  // 初始肉速度.
  meatSpeed = 250;      

  // 關閉水壺.
  kettleOpen = 0;

  // 重新開始遊戲.
  resetGame();      
}

// --------------------------------------------------------------
// 主選單.
// --------------------------------------------------------------
void mainMenu(){

  //---------------------------------------------------------------
  // 輸入.
  //---------------------------------------------------------------
  // 右移動.
  if (arduboy.justPressed(RIGHT_BUTTON)){
    cursorMode++;
    if(cursorMode>1)
      cursorMode = 1;
  }
  // 左移動.
  if (arduboy.justPressed(LEFT_BUTTON)){
    cursorMode--;
    if(cursorMode > 1)
      cursorMode = 0;    
  }

  //---------------------------------------------------------------
  // 繪圖.
  //---------------------------------------------------------------  
  // 清除畫面.
  arduboy.clear();
  
  // merry_cook.
  ardbitmap.drawCompressed( 32, 6, merry_cook, WHITE, ALIGN_H_LEFT, MIRROR_NONE); 
  // head.
  ardbitmap.drawCompressed( 48,18, head, WHITE, ALIGN_H_LEFT, MIRROR_NONE); 
  // start.
  ardbitmap.drawCompressed( 40,51, start, WHITE, ALIGN_H_LEFT, MIRROR_NONE); 
  // staff.
  ardbitmap.drawCompressed( 72,51, staff, WHITE, ALIGN_H_LEFT, MIRROR_NONE); 

  // 游標動畫。
  if (checkClock( 0, 240))
    cursorAni = !cursorAni;

  // 開始遊戲.
  if(cursorMode == 0){
    // 顯示游標動畫.
    if(cursorAni)
      ardbitmap.drawCompressed( 33,51, cursor, WHITE, ALIGN_H_LEFT, MIRROR_NONE); 
    else
      ardbitmap.drawCompressed( 32,51, cursor, WHITE, ALIGN_H_LEFT, MIRROR_NONE);

    // A.
    if (arduboy.justPressed(A_BUTTON))
      initMode(true);
    // B.
    else if (arduboy.justPressed(B_BUTTON))
      initMode(false);
          
  // 製作人員選單.
  }else if(cursorMode == 1){
    // 顯示游標動畫.
    if(cursorAni)
      ardbitmap.drawCompressed( 65,51, cursor, WHITE, ALIGN_H_LEFT, MIRROR_NONE); 
    else
      ardbitmap.drawCompressed( 64,51, cursor, WHITE, ALIGN_H_LEFT, MIRROR_NONE);

    // A、B.
    if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON))      
      gameMode=151; // 151:製作人員.
  }  
}

// --------------------------------------------------------------
// 製作人員.
// --------------------------------------------------------------
void productionStaff(){
  // 清除畫面.
  arduboy.clear();
  // staff.
  ardbitmap.drawCompressed( 56, 9, staff, WHITE, ALIGN_H_LEFT, MIRROR_NONE); 

  // 移動.
  if (checkClock( 0, 1)){
    staffMoveA+=4;
    if(staffMoveA>32)
      staffMoveA = 32;
      
    staffMoveB-=6;
    if(staffMoveB<72)
      staffMoveB = 72;      
  }
  
  // program(32).  
  ardbitmap.drawCompressed( staffMoveA,24, program, WHITE, ALIGN_H_LEFT, MIRROR_NONE); 
  // graphic(32).
  ardbitmap.drawCompressed( staffMoveA,36, graphic, WHITE, ALIGN_H_LEFT, MIRROR_NONE); 
  // pm(32).
  ardbitmap.drawCompressed( staffMoveA,48, pm, WHITE, ALIGN_H_LEFT, MIRROR_NONE); 

  // channel(72).
  ardbitmap.drawCompressed( staffMoveB,24, channel, WHITE, ALIGN_H_LEFT, MIRROR_NONE);   
  // maxx_kao(72).
  ardbitmap.drawCompressed( staffMoveB,36, maxx_kao, WHITE, ALIGN_H_LEFT, MIRROR_NONE); 
  // chris_ch(72).
  ardbitmap.drawCompressed( staffMoveB,48, chris_ch, WHITE, ALIGN_H_LEFT, MIRROR_NONE); 
  
  // A、B.
  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)){
    // 初始變數.
    staffMoveA = 0;
    staffMoveB = 128;
    // 初始時脈.
    initClock(); 
    // 150:主選單.
    gameMode=150;
  }
}

// --------------------------------------------------------------
// 生命歸零.
// --------------------------------------------------------------
void lifeReset(){
  // 進入生命歸零狀態.
  gameMode = 50;      
  // 紀錄生命數.
  lifeAniFrameTemp = lifeAniFrame;
  // 初始時脈.
  initClock(4);
}

// --------------------------------------------------------------
// gamePlay.
// --------------------------------------------------------------
void gamePlay(){
  // --------------------------------------------------------------
  // 清除畫面.
  //arduboy.clear();

  //---------------------------------------------------------------
  // 輸入.
  //--------------------------------------------------------------- 
  // 遊戲模式(100:GameOver).
  if (gameMode == 100) {
    // A.
    if (arduboy.justPressed(A_BUTTON)) {
      initMode(true);
    }
    // B.
    else if (arduboy.justPressed(B_BUTTON)) {
      initMode(false);
    }

    // 回主目錄.
    if (checkClock( 0, 15000))
      gameMode = 150;
    
  }

  // 遊戲模式(5:遊戲中).
  if (gameMode == 5) {
    
    // 右移動.
    if ((arduboy.justPressed(RIGHT_BUTTON) || arduboy.justPressed(B_BUTTON)) && (chefPos < 3)) {
      chefPos++;
      // 判斷廚師拋肉.
      ifChefThrow(chefPos);
      // 解除自動玩.
      autoRun=false;
    }

    // 左移動.
    if ((arduboy.justPressed(LEFT_BUTTON) ||arduboy.justPressed(A_BUTTON))  && (chefPos > 0)) {
      chefPos--;
      // 判斷廚師拋肉.
      ifChefThrow(chefPos);
      // 解除自動玩.
      autoRun=false;      
    }

    // 密技.
    // 上上下下左右左右:生命歸零+自動玩.
    // 最少失敗1次.
    if(lifeAniFrame>0){
      // 右.
      if (arduboy.justPressed(RIGHT_BUTTON)){
        //6.(右).
        if(secretTechnique==5)
          secretTechnique = 6;
        //8.(右).
        else if(secretTechnique==7){
          lifeReset();            // 生命歸零.
          secretTechnique = 0;
          autoRun=!autoRun;
        }
      // 左.
      }else if (arduboy.justPressed(LEFT_BUTTON)) {
        //5.(左).
        if(secretTechnique==4)
          secretTechnique = 5;
        //7.(左).
        else if(secretTechnique==6)
          secretTechnique = 7;
        else
          secretTechnique = 0;      
      // 上.
      }else if (arduboy.justPressed(UP_BUTTON)) {
        //1.(上).
        if(secretTechnique==0)
          secretTechnique = 1;
        //2.(上).
        else if(secretTechnique==1)
          secretTechnique = 2;
        else
          secretTechnique = 0;        
      // 下.
      }else if (arduboy.justPressed(DOWN_BUTTON)) {      
        //3.(下).
        if(secretTechnique==2)
          secretTechnique = 3;
        //4.(下).
        else if(secretTechnique==3)
          secretTechnique = 4;
        else
          secretTechnique = 0;
      // A、B.
      }else if ( (arduboy.justPressed(A_BUTTON)) || (arduboy.justPressed(B_BUTTON)) ){
        secretTechnique = 0;        
      }      
    }
    //Serial.println(secretTechnique);
        
    //---------------------------------------------------------------
    // 邏輯.
    //---------------------------------------------------------------
    // 更新肉移動順序.
    // 0:A.
    if (modeAniFrame == 0) {
      if (meatMoveA[meatMoveP] == 0)
        moveMeat(0);
      else if (meatMoveA[meatMoveP] == 1)
        moveMeat(1);
      else if (meatMoveA[meatMoveP] == 2)
        moveMeat(2);
      // 1:B.
    } else {
      if (meatMoveB[meatMoveP] == 0)
        moveMeat(0);
      else if (meatMoveB[meatMoveP] == 1)
        moveMeat(1);
      else if (meatMoveB[meatMoveP] == 2)
        moveMeat(2);
      else if (meatMoveB[meatMoveP] == 3)
        moveMeat(3);
    }

    // 判斷廚師拋肉.
    ifChefThrow( chefPos, false);
  
  // 遊戲模式(50:生命歸零).
  }else if (gameMode == 50){
    if (checkClock(4, 140)){
      if(lifeAniFrame==0){
        // 音效.
        sound.tone(NOTE_DS4, 24);        
        // 閃爍次數. 
        lifeFalsh++;
        
        // 閃爍結束.
        //if(lifeFalsh>=5){
          // 生命規零.
        //  lifeAniFrame = 0;
        //  lifeFalsh = 0;
          // 繼續遊戲.
        //  gameMode = 5;          
        // 閃爍大頭(開).
        //}else{                 
          lifeAniFrame = lifeAniFrameTemp;
        //}
      }else{
        // 閃爍大頭(關).
        lifeAniFrame = 0;

        // 閃爍結束.
        if(lifeFalsh>=5){
          // 生命規零.
        //  lifeAniFrame = 0;
          lifeFalsh = 0;
          // 繼續遊戲.
          gameMode = 5;          
        }        
      }
    }
  }

  // 更新老鼠拿到肉位置.
  updateGetMeat();
  // 更新廚師拋肉.
  updateChefThrow();

  //---------------------------------------------------------------
  // 繪圖.
  //---------------------------------------------------------------
  // 清除畫面.
  arduboy.clear();

  // --------------------------------------------------------------
  // 顯示背景.
  // --------------------------------------------------------------
  ardbitmap.drawCompressed(0,  0,   Background, WHITE, ALIGN_H_LEFT, MIRROR_NONE);

  // --------------------------------------------------------------
  // 顯示廚師.
  // --------------------------------------------------------------
  // 0.
  if (chefPos == 0 || (gameMode >= 3 && gameMode <= 4)) {
    // 廚師1-2
    if (chefAni[chefPos] == 1)
      //ardbitmap.drawBitmap( 13, 25, chef_1_2, 24, 24, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
      Sprites::drawSelfMasked( 16, 24, chef_1_2, 0);
    // 廚師1-1
    else  if (chefAni[chefPos] == 0)
      //ardbitmap.drawBitmap( 13, 25, chef_1_1, 24, 24, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
      Sprites::drawSelfMasked( 16, 24, chef_1_1, 0);
  }
  // 1.
  if (chefPos == 1 || (gameMode >= 3 && gameMode <= 4)) {
    // 廚師2-2
    if (chefAni[chefPos] == 1)
      //ardbitmap.drawBitmap( 39, 25, chef_2_2, 24, 24, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
      Sprites::drawSelfMasked( 40, 24, chef_2_2, 0);
    // 廚師2-1
    else  if (chefAni[chefPos] == 0)
      //ardbitmap.drawBitmap( 39, 25, chef_2_1, 24, 24, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
      Sprites::drawSelfMasked( 40, 24, chef_2_1, 0);
  }
  // 2.
  if (chefPos == 2 || (gameMode >= 3 && gameMode <= 4)) {
    // 廚師3-2
    if (chefAni[chefPos] == 1)
      //ardbitmap.drawBitmap( 66, 25, chef_3_2, 24, 24, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
      Sprites::drawSelfMasked( 64, 24, chef_3_2, 0);
    // 廚師3-1
    else  if (chefAni[chefPos] == 0)
      //ardbitmap.drawBitmap( 66, 25, chef_3_1, 24, 24, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
      Sprites::drawSelfMasked( 64, 24, chef_3_1, 0);
  }
  // 3.
  if (chefPos == 3 || ((gameMode >= 3 && gameMode <= 4) && modeAniFrame == 1)) {
    // 廚師4-2
    if (chefAni[chefPos] == 1)
      //ardbitmap.drawBitmap( 90, 25, chef_4_2, 24, 24, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
      Sprites::drawSelfMasked( 88, 24, chef_4_2, 0);
    // 廚師4-1
    else  if (chefAni[chefPos] == 0)
      //ardbitmap.drawBitmap( 90, 25, chef_4_1, 24, 24, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
      Sprites::drawSelfMasked( 88, 24, chef_4_1, 0);
  }

  // --------------------------------------------------------------
  // 顯示肉-0.
  // --------------------------------------------------------------
  // 肉動畫.
  // 廚師-1-3.
  if (meatAni[0] == 0) {
    //ardbitmap.drawBitmap( 25, 37, chef_1_3, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked( 32, 40 - 4, chef_1_3, 0);
    // 廚師-1-4.
  } else if (meatAni[0] == 1) {
    //ardbitmap.drawBitmap( 25, 32, chef_1_4, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked( 32, 32 - 4, chef_1_4, 0);
    // 廚師-1-5.
  } else if (meatAni[0] == 2) {
    //ardbitmap.drawBitmap( 25, 25, chef_1_5, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked( 32, 23, chef_1_5, 0);
    // 廚師-1-6.
  } else if (meatAni[0] == 3) {
    //ardbitmap.drawBitmap( 25, 18, chef_1_6, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked( 32, 16, chef_1_6, 0);
    // 廚師-1-7.
  } else if (meatAni[0] == 4) {
    //ardbitmap.drawBitmap( 25, 11, chef_1_7, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked( 32, 8, chef_1_7, 0);
  }

  // --------------------------------------------------------------
  // 顯示肉-1.
  // --------------------------------------------------------------
  // 肉動畫.
  // 廚師-2-3.
  if (meatAni[1] == 0) {
    //ardbitmap.drawBitmap( 51, 37, chef_1_3, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked( 56, 40 - 4, chef_1_3, 0);
    // 廚師-2-4.
  } else if (meatAni[1] == 1) {
    //ardbitmap.drawBitmap( 51, 32, chef_1_4, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked( 56, 32 - 4, chef_1_4, 0);
    // 廚師-2-5.
  } else if (meatAni[1] == 2) {
    //ardbitmap.drawBitmap( 51, 25, chef_1_5, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked( 56, 23, chef_1_5, 0);
    // 廚師-2-6.
  } else if (meatAni[1] == 3) {
    //ardbitmap.drawBitmap( 51, 18, chef_1_6, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked( 56, 16, chef_1_6, 0);
    // 廚師-2-7.
  } else if (meatAni[1] == 4) {
    //ardbitmap.drawBitmap( 51, 11, chef_1_7, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked( 56, 8, chef_1_7, 0);
  }

  // --------------------------------------------------------------
  // 顯示肉-2.
  // --------------------------------------------------------------
  // 肉動畫.
  // 廚師-3-3.
  if (meatAni[2] == 0) {
    //ardbitmap.drawBitmap( 77, 37, chef_1_3, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked( 80, 40 - 4, chef_1_3, 0);
    // 廚師-3-4.
  } else if (meatAni[2] == 1) {
    //ardbitmap.drawBitmap( 77, 32, chef_1_4, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked( 80, 32 - 4, chef_1_4, 0);
    // 廚師-3-5.
  } else if (meatAni[2] == 2) {
    //ardbitmap.drawBitmap( 77, 25, chef_1_5, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked( 80, 23, chef_1_5, 0);
    // 廚師-3-6.
  } else if (meatAni[2] == 3) {
    //ardbitmap.drawBitmap( 77, 18, chef_1_6, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked( 80, 16, chef_1_6, 0);
    // 廚師-3-7.
  } else if (meatAni[2] == 4) {
    //ardbitmap.drawBitmap( 77, 11, chef_1_7, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked( 80, 8, chef_1_7, 0);
  }

  // --------------------------------------------------------------
  // 顯示肉-3.
  // --------------------------------------------------------------
  // 肉3要B模式才會出現.
  if (modeAniFrame == 1) {
    // 肉動畫.
    // 廚師-4-3.
    if (meatAni[3] == 0) {
      //ardbitmap.drawBitmap(104, 37, chef_1_3, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
      Sprites::drawSelfMasked(104, 40 - 4, chef_1_3, 0);
      // 廚師-4-4.
    } else if (meatAni[3] == 1) {
      //ardbitmap.drawBitmap(104, 32, chef_1_4, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
      Sprites::drawSelfMasked(104, 32 - 4, chef_1_4, 0);
      // 廚師-4-5.
    } else if (meatAni[3] == 2) {
      //ardbitmap.drawBitmap(104, 25, chef_1_5, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
      Sprites::drawSelfMasked(104, 23, chef_1_5, 0);
      // 廚師-4-6.
    } else if (meatAni[3] == 3) {
      //ardbitmap.drawBitmap(104, 18, chef_1_6, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
      Sprites::drawSelfMasked(104, 16, chef_1_6, 0);
      // 廚師-4-7.
    } else if (meatAni[3] == 4) {
      //ardbitmap.drawBitmap(104, 11, chef_1_7, 8, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
      Sprites::drawOverwrite(104, 8, chef_1_7, 0);
    }
  }

  // --------------------------------------------------------------
  // 顯示老鼠.
  // --------------------------------------------------------------
  // 老鼠-1.
  if (mouseAniFrame == 0)
    //ardbitmap.drawBitmap(  3, 56, mouse_1, 16, 16, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked( 0, 48, mouse_1, 0);
  // 老鼠-2.
  else if (mouseAniFrame == 1)
    //ardbitmap.drawBitmap( 18, 54, mouse_2, 16, 16, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked(16, 48, mouse_2, 0);
  // 老鼠-3.
  else if (mouseAniFrame == 2)
    //ardbitmap.drawBitmap( 41, 55, mouse_3, 16, 16, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked(40, 48, mouse_3, 0);
  // 老鼠-4.
  else if (mouseAniFrame == 3)
    //ardbitmap.drawBitmap( 67, 54, mouse_4, 16, 16, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked(64, 48, mouse_4, 0);
  // 老鼠-5.
  else if (mouseAniFrame == 4)
    //ardbitmap.drawBitmap( 97, 54, mouse_5, 16, 16, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked(88, 48, mouse_5, 0);
  // 老鼠-6.
  else if (mouseAniFrame == 5)
    //ardbitmap.drawBitmap( 115, 53, mouse_6, 16, 16, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked(112, 48, mouse_6, 0);


  // --------------------------------------------------------------
  // 顯示貓.
  // --------------------------------------------------------------
  // 貓-1.
  if (catAniFrame == 0)
    Sprites::drawSelfMasked(0, 8, cat_1, 0);
  // 貓-2.
  else if (catAniFrame == 1)
    Sprites::drawSelfMasked(0, 8, cat_2, 0);

  // --------------------------------------------------------------
  // 顯示生命數.
  // --------------------------------------------------------------
  // 生命-3.
  if (lifeAniFrame >= 1)
    //ardbitmap.drawBitmap(  51, 0, life, 12, 12, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked(64, 0, life, 0);
  // 生命-2.
  if (lifeAniFrame >= 2)
    //ardbitmap.drawBitmap(  63, 0, life, 12, 12, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked(72, 0, life, 0);
  // 生命-1.
  if (lifeAniFrame >= 3)
    //ardbitmap.drawBitmap( 75, 0, life, 12, 12, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked(80, 0, life, 0);

  // --------------------------------------------------------------
  // 顯示遊戲模式.
  // --------------------------------------------------------------
  // A.
  if (modeAniFrame == 0)
    //ardbitmap.drawBitmap(  90, 0, game_a, 24, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked(96, 0, game_a, 0);
  // B.
  else
    //ardbitmap.drawBitmap(  90, 0, game_b, 24, 8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    Sprites::drawSelfMasked(96, 0, game_b, 0);

  // --------------------------------------------------------------
  // 水壺.
  // --------------------------------------------------------------
  if (kettleOpen>0 && checkClock( 7, 500)){
    kettleOpen++;
    if(kettleOpen>2) 
      kettleOpen = 1;
  }  
  if (kettleOpen==0 || kettleOpen==1)
    Sprites::drawSelfMasked(0, 24, kettle_1, 0);
  else if (kettleOpen==2)
    Sprites::drawSelfMasked(0, 24, kettle_2, 0);


  // --------------------------------------------------------------
  // 顯示分數.
  // --------------------------------------------------------------
  Sprites::drawSelfMasked(0, 0, scoreFont, 0);
  // 顯示分數.
  //score = 777;
  //arduboy.setCursor(26, 1);
  //arduboy.print(score);

  // 抓出每個位數.
  int a=0, b=0, c=0;
  if(score > 99){
    a = score % 10;         // 個.
    b = score / 10  % 10;   // 十.
    c = score / 100 % 10;   // 百.
    // 秀分數. 
    showScore( a, 40, 0);
    showScore( b, 32, 0);    
    showScore( c, 24, 0);
  }else  if(score > 9){
    a = score % 10;         // 個.
    b = score/ 10  % 10;    // 十.  
    // 秀分數. 
    showScore( a, 31, 0);
    showScore( b, 24, 0);
  }else{
    a = score % 10;         // 個.
    // 秀分數. 
    showScore( a, 24, 0);
  }   
}

// --------------------------------------------------------------
// 初始.
// --------------------------------------------------------------
//Sprites sprites;
void setup() {

  Serial.begin(57600);

  // initiate arduboy instance
  arduboy.begin();
  arduboy.setFrameRate(60);
  arduboy.setTextSize (1);
  arduboy.invert(invertFlag);

  // 初始時脈.
  initClock();
  // 設定廚師拋肉.
  setChefThrow();
  // 開關音效.
  setSound(gameSound);    
}

// --------------------------------------------------------------
// 主迴圈.
// --------------------------------------------------------------
void loop() {

  // pause render until it's time for the next frame
  if (!(arduboy.nextFrame()))
    return;    
  // 按鈕事件.
  arduboy.pollButtons();

  //---------------------------------------------------------------
  // 通用輸入.
  //---------------------------------------------------------------
  // 上.
  if (arduboy.justPressed(UP_BUTTON)) {
    // 反轉畫面.
    //invertFlag = !invertFlag;
    //arduboy.invert(invertFlag);
  }
  // 下.
  if (arduboy.justPressed(DOWN_BUTTON)) {
    // 開關音效.
    gameSound = !gameSound;
    setSound(gameSound);    
  }

  // 150:主選單.
  if(gameMode==150){
    mainMenu();    
  // 151:製作人員.
  }else if(gameMode==151){
    productionStaff();    
  // 3:gamePlay.
  }else{    
    gamePlay();
  }

  //Serial.println(HEIGHT);

  // then we finaly we tell the arduboy to display what we just wrote to the display
  arduboy.display();

}
