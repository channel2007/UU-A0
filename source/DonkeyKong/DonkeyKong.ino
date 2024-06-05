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

// 時脈.
// 0:小金剛跳的時間.
// 1:鳥、夾子移動時間.
// 2:key時脈.
// 3:拿key時脈.
// 4:跌落、取得Key動畫.
// 5:水果、失敗閃爍.
// 6:籠子、大金剛.
unsigned long clockSystem[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// 反轉畫面.
bool invertFlag = true;

// 拿key判斷(0:無 1:上跳 2:左走(跌落) 3:前跳，取Key 4: 跌落動畫 5:前跳 6:成功取得Key動畫).
byte keyGet = 0;

// key方向.
bool keydirect=true;
// key位置.
byte keyPos=10;

// 波次.
bool wave = false;
// 鳥出現位置.
byte bird1[2] = {2, 0};
byte bird2[2] = {0, 0};
// 夾子.
byte clip1[2] = {2, 0};
byte clip2[2] = {0, 0};

// 小金剛狀態(0:下 1:爬樹藤 2:上);
byte state = 0;
// 小金剛位置(0~10);
byte pos = 0;
// 小金剛跳狀態(0:無 1:跳起 2:跳起中).
byte junp=0;

// 動畫格數.
byte frame=0;

// 水果是否掉落(0:無 1:掉落中 2:以掉落).
byte fruitDown=0;

// 籠子(4).
short cage = 0;
byte cageFlash[4]={1,1,1,1};

// 模式(0:無 1:大金剛笑，畫面暫停)
byte mode=1;

// 分數.
short score=0;
byte  scoreTemp=0;

// 鳥出現倒數次數.
byte birdReciprocal = 255;
// 夾子出現倒數次數.
byte clipReciprocal = 255;

// 小金剛失敗閃爍(0:無 1:重新開始 >1:閃爍).
byte die = 0;

// 夾子出現數量.
byte clipNumber = 2;
// 鳥出現數量.
byte birdNumber = 2;

// 生命數量。
byte life = 0;

// 0:遊戲中 1:GameOver.
byte gameMode=1;

// 音效開關.
bool soundOn = true;


//----------------------------------------------------------------------------
// 初始時脈.
//----------------------------------------------------------------------------
void initClock(byte n = 255) {
  if (n == 255) {
    for (byte i = 0; i < (sizeof(clockSystem) / sizeof(unsigned long)); i++)
      clockSystem[i] = millis();
  }else{
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

//----------------------------------------------------------------------------
// 播放音效.
//----------------------------------------------------------------------------
void playSound(uint16_t freq, uint16_t dur = 0) {
  // 播放音效.
  if(soundOn){
    sound.noTone();
    sound.tone(freq, dur);
  }  
}

// --------------------------------------------------------------
// 刪除小金剛出現範圍的夾子.
// --------------------------------------------------------------
void DelClip(){
  // 關閉夾子.
  for(byte i=0; i<2; i++){
    if(clip1[i]>10)
      clip1[i]=0;
    if(clip2[i]>10)
      clip2[i]=0;          
  }  
}

// --------------------------------------------------------------
// 復活.
// --------------------------------------------------------------
void Revival(){
  mode=1;
  die=8;
  initClock(5);   
}

// --------------------------------------------------------------
// 小金剛移動判斷是否觸碰敵人.
// --------------------------------------------------------------
bool MonkeyMove(bool bird, bool before){
  byte a;

  // 鳥.
  if(bird){

  // 夾子.
  }else{
    for(byte i=0; i<2; i++){    
      
      a = pos+clip1[i];
      if(before){
        if(a==13 && junp==0){
          // 復活.
          Revival();  
          return true;      
        }        
      }else{
        if(a==12 && junp==0){
          // 復活.
          Revival();  
          return true;      
        }        
      }
      
      a = pos+clip2[i];
      if(before){
        if(a==13 && junp==0){
          // 復活.
          Revival();  
          return true;      
        }        
      }else{
        if(a==12 && junp==0){
          // 復活.
          Revival();  
          return true;      
        }        
      }      
    }    
  }    
  return false;
}

// --------------------------------------------------------------
// 敵人移動判斷是否觸碰小金剛.
// --------------------------------------------------------------
bool EnemyMove(bool bird, byte ps){  
  byte a;
  
  // 鳥.
  if(bird){
    // 樹藤上判斷.
    if(ps==8 && pos==(ps-2) && (state==1 || junp>0)){
      // 復活.
      Revival();        
      return true;
    // 其他位置判斷.      
    }else if(ps<9 && pos==(ps-3) && (state==1 || junp>0)){
      // 復活.
      Revival();        
      return true;
    }                      
    
  // 夾子.
  }else{
    // 過濾掉在右邊樹藤上.
    if(ps!=7 && pos<11){
      a = ps+pos;
      
      if(a==13 && junp==0){
        // 復活.
        Revival();        
        return true;

      // 跳過夾子，加分數.
      }if(a==13 && junp==2){
        scoreTemp+=1;
      }           
    }
  }
  return false;
}

// --------------------------------------------------------------
// 出現敵人.
// --------------------------------------------------------------
void EnemiesAppear(){
  byte n=0;
  int ran=0;
  
  //------------------------------------------------------------
  // 夾子.
  if(clipReciprocal == 255){
    // 判斷夾子在場景中的數量.
    for(byte i=0; i<2; i++){
      if(clip1[i]>0)
        n++;
      if(clip2[i]>0)
        n++;
    }
    // 少於數量.
    if(n<clipNumber){
      ran = random(1, 3);
      clipReciprocal=ran;
    }    
  }else{
    // 處理夾子出現.
    clipReciprocal--;
    if(clipReciprocal==0){
      for(byte i=0; i<2; i++){
        if(clip1[i]==0){
          clip1[i]=1;
          return;
        }
        if(clip2[i]==0){
          clip2[i]=1;
          return;        
        }
      }      
      clipReciprocal=255;  
    }
  }
  
  //------------------------------------------------------------
  // 鳥.
  n=0;
  if(birdReciprocal == 255){
    // 判斷鳥在場景中的數量.
    for(byte i=0; i<2; i++){
      if(bird1[i]>0)
        n++;
      if(bird2[i]>0)
        n++;
    }
    // 少於數量.
    if(n<birdNumber){
      ran = random(1, 3);
      birdReciprocal=ran;
    }    
  }else{
    // 處理鳥出現.
    birdReciprocal--;
    if(birdReciprocal==0){
      for(byte i=0; i<2; i++){
        if(bird1[i]==0){
          bird1[i]=1;
          return;
        }
        if(bird2[i]==0){
          bird2[i]=1;
          return;        
        }
      }      
      birdReciprocal=255;  
    }
  }
   
}

// --------------------------------------------------------------
// 輸入更新.
// --------------------------------------------------------------
void InputUpdate(){
  
  if (arduboy.justPressed(A_BUTTON) && arduboy.justPressed(DOWN_BUTTON)){
    soundOn = !soundOn;
    return;
  }
  
  // 遊戲中.
  if(gameMode==0){
    //.
    if(mode==1){return;}
  
    // 播放動畫中不接受輸入.
    // 初始動畫時脈.
    if(keyGet>3){
      // .
      if(checkClock(4, 600)){
        frame++;
  
        // 跌落動畫.
        if(keyGet==4 && frame>2){
          keyGet=0;
          pos=0;
          frame=0;
          // 小金剛狀態(0:下 1:爬樹藤 2:上);
          state = 0;        
          // 刪除小金剛出現範圍的夾子.
          DelClip();

        // 跌落動畫音效.
        }else if(keyGet==4 && frame==2){
          // 播放音效.
          playSound(NOTE_A1, 50);
          /*
          if(soundOn){
            sound.noTone();
            sound.tone(NOTE_A1, 50);        
          }            
          */
        // 前跳.
        }else if(keyGet==5){
          //.
          initClock(4);
  
          // 拿到Key.
          if(keyPos==10 || keyPos==11){
            keyGet=6;                    
          // 無拿到Key.
          }else{
            keyGet=4;
          }
            
          pos=12;
          frame=0;
  
        // 取得Key(閃爍籠子).
        }else if(keyGet==6 && frame==2){
          // 解開籠子.
          if(cage<4){
            cageFlash[cage]=0;
            // 加分數.
            scoreTemp=5;          
          }
          cage++;
          
          // 過關.
          if(cage>3){
            frame--;
            mode=1;
            // 加分數.
            scoreTemp=10;
          }        
          // 初始籠子狀態.
          for(int i=0; i<cage; i++)
            cageFlash[i]=0;          
          initClock(6);
          
        // 取得Key.
        }else if(keyGet==6 && frame>2){
          // 復原籠子.
          for(int i=0; i<4; i++)
            cageFlash[i]=1;            
          // 初始變數.
          keyGet=0;
          pos=0;
          frame=0;
          fruitDown = 0;
          // 小金剛狀態(0:下 1:爬樹藤 2:上);
          state = 0;
          // 刪除小金剛出現範圍的夾子.
          DelClip();
          
        }      
      }            
      return;
    }
    
    //------------------------------------------------------------
    // 判斷跌落或拿Key.
    if(checkClock(3, 150)){
      // (0:無 1:上跳 2:左走(跌落) 3:前跳，取Key 4: 跌落動畫 5:前跳 6:成功取得Key動畫).
      // 1:上跳.
      if(keyGet == 1){
        junp = 2;
        initClock(0);
        return;
        
      // 2:左走.
      }else if(keyGet == 2){
        initClock(4);
        keyGet=4;
        pos=12;
        frame=0;      
        return;
        
      // 3:前跳(判斷有無拿到Key).
      }else if(keyGet == 3){      
        initClock(4);      
        keyGet = 5;
        pos=11;
        return;
      }            
    }  
  
    //------------------------------------------------------------
    // 右移動.
    if (arduboy.justPressed(RIGHT_BUTTON)){
      // 小金剛狀態.
      // 0:下.
      if(state == 0){
        pos++;
        if(pos>5)
          pos=5;
        // 小金剛移動判斷是否觸碰敵人.
        if(MonkeyMove(false, true))
              pos--;    
              
      // 2:上.
      }else if(state == 2){
        // 上層右邊極限，過濾掉上層樹藤上右邊極限.
        if((pos>7)&&!(pos==8 &&junp==1))
          pos--;
        MonkeyMove(false, false);
      }
      // 播放音效.
      playSound(NOTE_AS4, 50);
      /*
      if(soundOn){
        sound.noTone();
        sound.tone(NOTE_AS4, 50);
      }
      */
    //------------------------------------------------------------
    // 左移動.
    }else if (arduboy.justPressed(LEFT_BUTTON)){
      // 小金剛狀態
      // 0:下.
      if(state == 0){
        // 小金剛位置.
        if(pos>0)
          pos--;
        // 小金剛移動判斷是否觸碰敵人.
        MonkeyMove(false,false);
              
          
      // 2:上.
      }else if(state == 2){
        // .
        if(pos==10){
          // 判斷按鍵狀態.
          if(keyGet<3 && keyGet!=2){
            // 拿key判斷(0:無 1:跳 2:左走(跌落) 3:取Key)
            if(keyGet== 0)
              keyGet = 2;
            else if(keyGet== 1)
              keyGet = 3;          
            // 初始時脈.
            initClock(3);            
          }        
        }
        // 小金剛位置.
        pos++;
        if(pos>10)
          pos=10;
  
        // 小金剛移動判斷是否觸碰敵人.
        if(pos<=10 && MonkeyMove(false,true))
          pos--;
        
        // 水果是否掉落(0:無 1:掉落中 2:以掉落).
        if(junp==1 && pos==9 && fruitDown==0){
          initClock(5);
          fruitDown = 1;
        }
      }
      // 播放音效.
      playSound(NOTE_AS4, 50);
      /*
      if(soundOn){
        sound.noTone();
        sound.tone(NOTE_AS4, 50);
      }
      */
    //------------------------------------------------------------
    // 上移動.
    }else if (arduboy.justPressed(UP_BUTTON)){
      // 小金剛狀態.
      // 1:爬樹藤.
      if(state == 1){
        pos++; 
        state=2;

        // 播放音效.
        playSound(NOTE_AS4, 50);
        /*
        if(soundOn){
          sound.noTone();
          sound.tone(NOTE_AS4, 50);
        } 
        */       
      }
  
    //------------------------------------------------------------
    // 下移動.
    }else if (arduboy.justPressed(DOWN_BUTTON)){
      // 小金剛狀態.
      // 1:爬樹藤.
      if(state == 1 && junp==0){
        pos--; 
        state=0;
        // 播放音效.
        playSound(NOTE_AS4, 50);
        /*
        if(soundOn){
          sound.noTone();
          sound.tone(NOTE_AS4, 50);
        }
        */
      // 2:上，可爬下樹藤.
      }else if(state == 2 && pos==7 && junp==0){
        pos--; 
        state=1;
        // 播放音效.
        playSound(NOTE_AS4, 50);
        /*
        if(soundOn){
          sound.noTone();
          sound.tone(NOTE_AS4, 50);
        }
        */
      // 從樹藤下來.
      }else if(junp==1 &&(pos==0||pos==2||pos==3||pos==8)){
        junp=0;
        // 播放音效.
        playSound(NOTE_AS4, 50);
        /*
        if(soundOn){
          sound.noTone();
          sound.tone(NOTE_AS4, 50);
        } 
        */       
      }
              
    }
    // A、B(跳).
    if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)){
      // 過濾掉第7個位置不能跳.
      if(pos!=6&&pos!=7){
        // 下-最右邊，準備往上跳.
        if(state == 0 && pos==5){
          pos++; 
          state=1;
          
        // 其他位置準備跳.
        }else{        
          // 跳.
          if((junp==0||junp==1) && pos!=10){
            junp = 1;
            // 水果是否掉落(0:無 1:掉落中 2:以掉落).
            if(pos==9 && fruitDown==0){
              initClock(5);
              fruitDown = 1;
            }
          // 判斷拿key.
          }else{
            // 判斷按鍵狀態.
            if(keyGet<3 && keyGet!=1){
              // 拿key判斷(0:無 1:跳 2:左走(跌落) 3:取Key)
              if(keyGet== 0)
                keyGet = 1;
              else if(keyGet== 2)
                keyGet = 3;          
              // 初始時脈.
              initClock(3);            
            }
          }
        }
        // 播放音效.
        playSound(NOTE_AS4, 50);
        /*
        if(soundOn){
          sound.noTone();
          sound.tone(NOTE_AS4, 50);
        } 
        */       
      }        
    }
    
    // 跳-初始空中停留時間.
    if(junp==1&&(pos==1||pos==4||pos==9||pos==10)){
      junp = 2;
      initClock(0);
    }

  // GameOver.
  }else if(gameMode==1){
    // A、B(開始遊戲).
    if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)){
      // 重新開始遊戲.
      replayGame();      
    }    
  }    
}

//----------------------------------------------------------------------------
// 重新開始遊戲.
//----------------------------------------------------------------------------
void replayGame() {
  // 籠子(4).
  cage = 0;

  // 初始變數.
  for(byte i=0; i<2; i++){
    // 鳥出現位置.
    bird1[i] = 0;
    bird2[i] = 0;
    // 夾子.
    clip1[i] = 0;
    clip2[i] = 0;        
  }
  bird1[0] = 2;
  clip1[0] = 2;

  // 初始變數.
  for(byte i=0; i<4; i++)
    cageFlash[i]= 1;
  
  // 模式(0:無 1:大金剛笑，畫面暫停)
  mode=0;      
  // 水果是否掉落(0:無 1:掉落中 2:以掉落).
  fruitDown=0;

  // 分數.
  score=0;
  scoreTemp=0;
        
  // 小金剛失敗閃爍(0:無 1:重新開始 >1:閃爍).
  die = 0;
  
  // 夾子出現數量.
  clipNumber = 2;
  // 鳥出現數量.
  birdNumber = 2;
  
  // 生命數量。
  life = 3;

  // 初始變數.
  state = 0;
  pos = 0;
  junp=0;

  // 初始時脈.
  initClock();

  // 0:遊戲中 1:GameOver.
  gameMode=0;       
}

// --------------------------------------------------------------
// 秀數字.
// --------------------------------------------------------------
void showScore(int n, int16_t x, int16_t y){
  if(n==0)    
    ardbitmap.drawCompressed(x, y,N0, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
  else if(n==1)
    ardbitmap.drawCompressed(x, y,N1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
  else if(n==2)
    ardbitmap.drawCompressed(x, y,N2, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
  else if(n==3)
    ardbitmap.drawCompressed(x, y,N3, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
  else if(n==4)
    ardbitmap.drawCompressed(x, y,N4, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
  else if(n==5)
    ardbitmap.drawCompressed(x, y,N5, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
  else if(n==6)
    ardbitmap.drawCompressed(x, y,N6, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
  else if(n==7)
    ardbitmap.drawCompressed(x, y,N7, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
  else if(n==8)
    ardbitmap.drawCompressed(x, y,N8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
  else if(n==9)
    ardbitmap.drawCompressed(x, y,N9, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
}

// --------------------------------------------------------------
// 繪圖更新.
// --------------------------------------------------------------
void DrawUpdate() {
  //---------------------------------------------------------------
  // 繪圖.
  //---------------------------------------------------------------
  // 清除畫面.
  arduboy.clear();

  // --------------------------------------------------------------
  // 顯示背景.
  // --------------------------------------------------------------
  ardbitmap.drawCompressed(0,  0,   Background, WHITE, ALIGN_H_LEFT, MIRROR_NONE);

  //if((die==0)||(die>0 && checkClock(5, 500) )){}

  // 閃爍小金剛.
  if(die>0){
    // 閃爍.
    if(checkClock(5, 350)){
      die--;  

      // 播放音效.
      if(soundOn && die!=6 && die!=5){
        sound.noTone();
        sound.tone(NOTE_A1, 50);        
      }      
    }
          
    // 初始遊戲.
    if(die==1){
      // 初始變數.
      die=0;
      mode=0;
      state = 0;
      pos = 0;
      junp=0;

      // 扣生命.
      life--;
      // Game Over.
      if(life==0){
        mode = 1; 
        gameMode=1;
      }     

      // 刪除小金剛出現範圍的夾子.
       DelClip();              
    }      
  }
  if((die%2)==0){
    // 金剛(下)-1-1.
    if(pos==0 && junp==0)
      ardbitmap.drawCompressed( 15,48, MonkeyDown_1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛(下)-2-2.
    else if(pos==1 && junp==0)
      ardbitmap.drawCompressed( 35,48, MonkeyDown_2, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛(下)-1-3.
    else if(pos==2 && junp==0)
      ardbitmap.drawCompressed( 52,48, MonkeyDown_1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛(下)-2-4.
    else if(pos==3 && junp==0)
      ardbitmap.drawCompressed( 71,48, MonkeyDown_2, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛(下)-1-5.
    else if(pos==4 && junp==0)
      ardbitmap.drawCompressed( 88,48, MonkeyDown_1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛(下)-2-6.
    else if(pos==5 && junp==0)
      ardbitmap.drawCompressed(107,48, MonkeyDown_2, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛(上)-6.
    else if(pos==6 && junp==0)
      ardbitmap.drawCompressed(109,37, MonkeyUp_6, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛(下)-1-7.
    else if(pos==7 && junp==0)   
      ardbitmap.drawCompressed(106,17, MonkeyDown_1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛(下)-2-8.
    else if(pos==8 && junp==0)
      ardbitmap.drawCompressed( 89,17, MonkeyDown_2, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛(下)-1-9.
    else if(pos==9 && junp==0)
      ardbitmap.drawCompressed( 70,17, MonkeyDown_1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛(下)-2-10.
    else if(pos==10 && junp==0)
      ardbitmap.drawCompressed( 53,17, MonkeyDown_2, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
      
    // 金剛(上)-1.
    else if(pos==0 && junp==1)
      ardbitmap.drawCompressed( 16,36, MonkeyUp_1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛(上)-2.
    else if(pos==1 && (junp==1||junp==2))
      ardbitmap.drawCompressed( 33,37, MonkeyUp_2, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛(上)-3.
    else if(pos==2 && junp==1)
      ardbitmap.drawCompressed( 53,36, MonkeyUp_3, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛(上)-4.
    else if(pos==3 && junp==1)
      ardbitmap.drawCompressed( 73,36, MonkeyUp_4, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛(上)-5.
    else if(pos==4 && (junp==1||junp==2))
      ardbitmap.drawCompressed( 90,37, MonkeyUp_5, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛(上)-7.
    else if(pos==8 && junp==1)
      ardbitmap.drawCompressed( 88, 8, MonkeyUp_7, WHITE, ALIGN_H_LEFT, MIRROR_NONE);  
    // 金剛(上)-8.
    else if(pos==9 && (junp==1||junp==2))
      ardbitmap.drawCompressed( 69, 7, MonkeyUp_8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛(上)-9.
    else if(pos==10 && (junp==1||junp==2))
      ardbitmap.drawCompressed( 51, 7, MonkeyUp_9, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
    // 金剛(下)-2-11.
    else if(pos==11)
      ardbitmap.drawCompressed( 38, 6, MonkeyDown_3, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
  }
  
  for(byte i=0; i<2; i++){    
    //----------------------------------------------------------------------------
    // 顯示夾子.
    // 夾子-1-1.
    if(clip1[i]==1 || clip2[i]==1)
      ardbitmap.drawCompressed( 33, 24, Clip_1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 夾子-1-2.
    if(clip1[i]==2 || clip2[i]==2)
      ardbitmap.drawCompressed( 48, 27, Clip_1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 夾子-2-3.
    if(clip1[i]==3 || clip2[i]==3)      
      ardbitmap.drawCompressed( 65, 29, Clip_2, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 夾子-1-4.
    if(clip1[i]==4 || clip2[i]==4)
      ardbitmap.drawCompressed( 84, 27, Clip_1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 夾子-2-5.
    if(clip1[i]==5 || clip2[i]==5)
      ardbitmap.drawCompressed(101, 29, Clip_2, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 夾子-3-6.
    if(clip1[i]==6 || clip2[i]==6)
      ardbitmap.drawCompressed(119, 28, Clip_3, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
    // 夾子-2-7.
    if(clip1[i]==7 || clip2[i]==7)
      ardbitmap.drawCompressed(119, 60, Clip_2, WHITE, ALIGN_H_LEFT, MIRROR_HORIZONTAL);
    // 夾子-1-8.
    if(clip1[i]==8 || clip2[i]==8)
      ardbitmap.drawCompressed(100, 58, Clip_1, WHITE, ALIGN_H_LEFT, MIRROR_HORIZONTAL);
    // 夾子-2-9.
    if(clip1[i]==9 || clip2[i]==9)
      ardbitmap.drawCompressed( 83, 60, Clip_2, WHITE, ALIGN_H_LEFT, MIRROR_HORIZONTAL);
    // 夾子-1-10.
    if(clip1[i]==10 || clip2[i]==10)
      ardbitmap.drawCompressed( 64, 58, Clip_1, WHITE, ALIGN_H_LEFT, MIRROR_HORIZONTAL);
    // 夾子-2-11.
    if(clip1[i]==11 || clip2[i]==11)
      ardbitmap.drawCompressed( 47, 60, Clip_2, WHITE, ALIGN_H_LEFT, MIRROR_HORIZONTAL);
    // 夾子-1-12.
    if(clip1[i]==12 || clip2[i]==12)
      ardbitmap.drawCompressed( 28, 58, Clip_1, WHITE, ALIGN_H_LEFT, MIRROR_HORIZONTAL);
    // 夾子-2-13.
    if(clip1[i]==13 || clip2[i]==13)
      ardbitmap.drawCompressed( 11, 60, Clip_2, WHITE, ALIGN_H_LEFT, MIRROR_HORIZONTAL);

    //----------------------------------------------------------------------------
    // 顯示鳥.
    // 鳥-1-1.
    if(bird1[i]==1 || bird2[i]==1)
      ardbitmap.drawCompressed(  0, 39, Bird_1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);  
    // 鳥-2-2.
    if(bird1[i]==2 || bird2[i]==2)
      ardbitmap.drawCompressed(  8, 43, Bird_2, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 鳥-3-3.
    if(bird1[i]==3 || bird2[i]==3)
      ardbitmap.drawCompressed( 29, 42, Bird_3, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 鳥-2-4.
    if(bird1[i]==4 || bird2[i]==4)
      ardbitmap.drawCompressed( 46, 43, Bird_4, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 鳥-3-5.
    if(bird1[i]==5 || bird2[i]==5)
      ardbitmap.drawCompressed( 66, 41, Bird_3, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 鳥-2-6.
    if(bird1[i]==6 || bird2[i]==6)
      ardbitmap.drawCompressed( 85, 43, Bird_4, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 鳥-3-7.
    if(bird1[i]==7 || bird2[i]==7)
      ardbitmap.drawCompressed(104, 42, Bird_3, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 鳥-3-8.
    if(bird1[i]==8 || bird2[i]==8)
      ardbitmap.drawCompressed(121, 32, Bird_5, WHITE, ALIGN_H_LEFT, MIRROR_NONE);

  }  

  // 取得Key所以不要顯示.
  if(keyGet!=6){
    // Key-1.
    if(keyPos==10)
      ardbitmap.drawCompressed( 35, 1, Key, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // Key-2.
    else if(keyPos==11)
      ardbitmap.drawCompressed( 41, 1, Key, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // Key-3.
    else if(keyPos==12)
      ardbitmap.drawCompressed( 47, 1, Key, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // Key-4.
    else if(keyPos==13)
      ardbitmap.drawCompressed( 53, 1, Key, WHITE, ALIGN_H_LEFT, MIRROR_NONE);       
  }

  // .
  if(keyGet == 4){
    // 金剛-掉落-1.
    if(frame==0)
      ardbitmap.drawCompressed( 16,24, MonkeyDie_1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛-掉落-2.
    else if(frame==1)
      ardbitmap.drawCompressed(  6,38, MonkeyDie_2, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛-掉落-3.
    else if(frame==2){
      ardbitmap.drawCompressed(  0,50, MonkeyDie_3, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
    }
  }
  // .
  if(keyGet == 6){
    // 金剛-取得-1.
    if(frame==0)
      ardbitmap.drawCompressed( 27, 1, MonkeyGet_1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛-取得-2.
    else if(frame==1)
      ardbitmap.drawCompressed( 23, 5, MonkeyGet_2, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 金剛-取得-3.
    else if(frame==2)
      ardbitmap.drawCompressed( 16,24, MonkeyDie_1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
  }
  
  // 水果是否掉落(0:無 1:掉落中 2:以掉落).
  if(fruitDown==0){
    // 水果-1.
    ardbitmap.drawCompressed( 67, 5,Fruit_1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
  }else if(fruitDown==1){
    // 水果-2.
    if(frame==1)
      ardbitmap.drawCompressed( 68,24,Fruit_2, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 水果-3.
    else if(frame==2)
      ardbitmap.drawCompressed( 68,37,Fruit_3, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
    // 水果-4.
    else if(frame==3)
      ardbitmap.drawCompressed( 68,53,Fruit_4, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
  }
  
  // 籠子-1.
  if(cageFlash[0]==1)
    ardbitmap.drawCompressed( 12, 1, Cage_2, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
  // 籠子-2.
  if(cageFlash[1]==1)
    ardbitmap.drawCompressed(  1, 1, Cage_1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
  // 籠子-3.
  if(cageFlash[2]==1)
    ardbitmap.drawCompressed(  1,13, Cage_3, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
  // 籠子-4.
  if(cageFlash[3]==1)
    ardbitmap.drawCompressed( 12,13, Cage_4, WHITE, ALIGN_H_LEFT, MIRROR_NONE);

  // 大金剛笑.
  if(cage>4){    
    if(cage%2)      
      ardbitmap.drawCompressed( 1, 3, BigMonkey_1, WHITE, ALIGN_H_LEFT, MIRROR_NONE);   // 大金剛-1.
    else    
      ardbitmap.drawCompressed( 1, 3, BigMonkey_2, WHITE, ALIGN_H_LEFT, MIRROR_NONE);   // 大金剛-2.    
  }else{
    // 大金剛-2.
    ardbitmap.drawCompressed( 1, 3, BigMonkey_2, WHITE, ALIGN_H_LEFT, MIRROR_NONE);    
  }

  // 加分數.
  if(checkClock(7, 50)){
    if(scoreTemp>0){
      scoreTemp--;
      score++;
      if(score>999)
        score=999;

      // 播放音效.
      playSound(NOTE_D8, 50);
      /*
      if(soundOn){
        sound.noTone();
        sound.tone(NOTE_D8, 50);
      } 
      */       
    }  
  }
  // 抓出每個位數.
  int a=0, b=0, c=0;  
  if(score > 99){
    a = score % 10;         // 個.
    b = score / 10  % 10;   // 十.
    c = score / 100 % 10;   // 百.
  }else  if(score > 9){
    a = score % 10;         // 個.
    b = score/ 10  % 10;    // 十.  
  }else{
    a = score % 10;         // 個.
  }   
  // 秀分數. 
  showScore( a, 121, 1);
  showScore( b, 113, 1);    
  showScore( c, 105, 1);    
    
  // 蓋住-頭-1.
  if(life>2)
    ardbitmap.drawCompressed( 85, 2,Head, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
  // 蓋住-頭-2.
  if(life>1)
    ardbitmap.drawCompressed( 90, 2,Head, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
  // 蓋住-頭-3.
  if(life>0)
    ardbitmap.drawCompressed( 95, 2,Head, WHITE, ALIGN_H_LEFT, MIRROR_NONE);

  // 數字0-1
  //ardbitmap.drawCompressed(105, 1,N9, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
  // 數字0-2
  //ardbitmap.drawCompressed(113, 1,N8, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
  // 數字0-3
  //ardbitmap.drawCompressed(121, 1,N7, WHITE, ALIGN_H_LEFT, MIRROR_NONE);
  
  // then we finaly we tell the arduboy to display what we just wrote to the display
  arduboy.display();
}

// --------------------------------------------------------------
// 初始.
// --------------------------------------------------------------
void setup() {
  Serial.begin(57600);

  // initiate arduboy instance
  arduboy.begin();
  arduboy.setFrameRate(60);
  arduboy.setTextSize (1);
  arduboy.invert(invertFlag);

  // 初始時脈.
  initClock();
  
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
  
  //------------------------------------------------------------  
  // 籠子動畫.  
  if(checkClock(6, 500)){
    // GameOver不要處理籠子閃爍.
    if(gameMode!=1){
      // 閃爍籠子.
      if(cage<4){
        // 籠子-1.
        if(cage>0)
          cageFlash[0]=!cageFlash[0];
        // 籠子-2.
        if(cage>1)
          cageFlash[1]=!cageFlash[1];
        // 籠子-3.
        if(cage>2)
          cageFlash[2]=!cageFlash[2];
        // 籠子-4.
        if(cage>3)
          cageFlash[3]=!cageFlash[3];
          
      // 大猩猩笑.
      }else{
        cage++;
        // 籠子復原重新開始遊戲.
        if(cage>=8){
          cage=-1;
          mode=0;        
          initClock(4);
        }
      }      
    }
  }

  //------------------------------------------------------------  
  // 水果動畫.
  // 水果是否掉落(0:無 1:掉落中 2:以掉落).
  if(fruitDown==1){
    if(checkClock(5, 200)){
      if(scoreTemp==0){
        //.
        frame++;
        // 夾子-1.
        if(frame==1){
          for(byte i=0; i<2; i++){
            if(clip1[i]==3){
              clip1[i] = 0;
              scoreTemp = 3;
            }
            if(clip2[i]==3){
              clip2[i] = 0;
              scoreTemp = 3;
            }
          }
        }
        // 鳥-1.
        if(frame==2){
          for(byte i=0; i<2; i++){
            if(bird1[i]==5){
              bird1[i] = 0;
              scoreTemp = 9;
            }
            if(bird2[i]==5){
              bird2[i] = 0; 
              scoreTemp = 3;
            }         
          }
        }
        // 夾子-2.
        if(frame==3){
          for(byte i=0; i<2; i++){                    
            if(clip1[i]==10){
              clip1[i] = 0;          
              scoreTemp = 3;
            }
            if(clip2[i]==10){
              clip2[i] = 0;
              scoreTemp = 3;
            }
          }
        }
  
        // 結束.
        if(frame>3){
          frame = 0; 
          fruitDown=2; 
          // 初始時脈.
          initClock();        
        }           
      }      
    }
    // 繪圖更新.
    DrawUpdate();
    return;
  }
      
  //------------------------------------------------------------  
  // key動畫.
  if(checkClock(2, 700)){
    // GameOver.
    if(gameMode!=1){
      // 小金剛失敗Key不要動.
      if(die==0){
        if(keydirect){
          keyPos++;  
          if(keyPos>13){
            keyPos=12;
            keydirect = !keydirect;
          }
        }else{
          keyPos--;
          if(keyPos<10){
            keyPos=11;
            keydirect = !keydirect;
          }       
        }      
      }          
    }
  }
  
  //------------------------------------------------------------  
  // 鳥、夾子動畫.
  if(checkClock(1, 500)){
    //.
    if(mode!=1){
      for(byte i=0; i<2; i++){
        // 判斷移動波次.
        if(wave){
          //-----------------------------------------
          // 判斷出現的夾子才移動.
          if(clip1[i]>0){
            clip1[i]++;
            
            // 小金剛失敗.
            if(EnemyMove(false,clip1[i]))
                clip1[i]=0;
                        
            if(clip1[i]>13)
              clip1[i]=0;
          }
          //-----------------------------------------
          // 判斷出現的鳥才移動.
          if(bird1[i]>0){
            bird1[i]++;

            // 小金剛失敗.
            if(EnemyMove(true,bird1[i]))
              bird1[i]=0;
              
            if(bird1[i]>8)
              bird1[i]=0;
          }                
        }else{
          //-----------------------------------------
          // 判斷出現的夾子才移動.
          if(clip2[i]>0){
            clip2[i]++;
            
            // 小金剛失敗.
            if(EnemyMove(false,clip2[i]))
                clip2[i]=0;
                                    
            if(clip2[i]>13)
              clip2[i]=0;
          }
          //-----------------------------------------
          // 判斷出現的鳥才移動.
          if(bird2[i]>0){
            bird2[i]++;

            // 小金剛失敗.
            if(EnemyMove(true, bird2[i]))
              bird2[i]=0;
              
            if(bird2[i]>8)
              bird2[i]=0;
          }
        }
      }    
      // 出現敵人.
      EnemiesAppear();
      // 反轉波次.
      wave = !wave;
      
      // 播放音效.
      playSound(NOTE_F0, 100);
      /*
      if(soundOn){
        sound.noTone();
        sound.tone(NOTE_F0, 100);
      }
      */
    }
  }
  
  //------------------------------------------------------------
  // 空中停留中排除輸入.
  //if(junp != 2 || keyGet<3){
  if(junp != 2){
    // 輸入更新.
    InputUpdate();    
  }else{
    // 等待掉落時間.
    if(checkClock(0, 1000)){
      // 跳躍中被鳥碰觸，不要再讓小金剛掉落.
      if( mode!=1){
        junp = 0;
        keyGet=0;        
      }
    }    
  }

  //Serial.println(junp);
  
  //------------------------------------------------------------
  // 繪圖更新.
  DrawUpdate();
    
}
