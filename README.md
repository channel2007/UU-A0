# 前言
UU-A0資源收集

# 目錄說明
* doc ==> 文件目錄
  * Tutorial_1.pdf ==> Arduino Handheld 遊戲開發(一)-函數庫安裝篇
  * Tutorial_2.pdf ==> ArduinoHandheld 遊戲開發(二)-測試程式教學
  * UU-A0_Manual_v1.1.pdf ==> UU-A0掌機使用說明書
  * UU-A1_Prog_Manual_v1.0.pdf ==> UU-A1燒錄器使用說明書
* img ==> 圖片目錄
* source ==> 範例、遊戲原始碼
  * DonkeyKong ==> 大金剛原始碼
  * MerryCook ==> 廚師原始碼
  * TestButton ==> 按鈕測試原始碼
  * TestDraw ==> 測試原始碼
* tools ==> 工具目錄
  * uua1prog.zip ==> UU-A1燒錄器執行程式 

# 線路圖
<a href="https://github.com/channel2007/UU-A0/blob/master/img/CircuitDiagram.jpg" target="_blank"><img src="https://github.com/channel2007/UU-A0/blob/master/img/CircuitDiagram.jpg" alt="abupload" width="640" height="480" border="10" /></a>

# 準備零件
* Arduino Pro Micro x 1
* 0.96吋OLED顯示螢幕 x 1
* 麵包板 x 1
* 麵包板線 x N
* 輕觸開關12*12*4.3MM x 2
* 輕觸開關 6*6*4.3MM x 4
* 有源蜂鳴器 x 1

# 安裝函數庫
* [Arduboy2](https://github.com/MLXXXp/Arduboy2)
* [ArduboyTones](https://github.com/MLXXXp/ArduboyTones)

# 燒錄軟體
* [ArduManFX](推薦)(https://github.com/tuxinator2009/ArduManFX/releases)
  * 支援Windows、MacOS、linux
  * <a href="https://github.com/tuxinator2009/ArduManFX/releases" target="_blank"><img src="https://github.com/channel2007/UU-A0/blob/master/img/ArduManFX.jpg" alt="ArduManFX" width="640" height="480" border="10" /></a>
* [ArduboyUploader](https://github.com/eried/ArduboyUploader/releases/tag/1.4)
  * 支援Windows
  * <a href="https://github.com/eried/ArduboyUploader/releases/tag/1.4" target="_blank"><img src="https://github.com/channel2007/UU-A0/blob/master/img/abupload.jpg" alt="abupload" width="640" height="480" border="10" /></a>
* [arduboy_toolset](https://github.com/randomouscrap98/arduboy_toolset/releases/tag/v0.7.1)
  * 支援Windows
  * <a href="https://github.com/randomouscrap98/arduboy_toolset/releases/tag/v0.7.1" target="_blank"><img src="https://github.com/channel2007/UU-A0/blob/master/img/arduboy_toolset.jpg" alt="abupload" width="640" height="480" border="10" /></a>   
* [xLoader](https://github.com/binaryupdates/xLoader)
  * 支援Windows
  * <a href="https://github.com/binaryupdates/xLoader" target="_blank"><img src="https://github.com/channel2007/UU-A0/blob/master/img/XLoader.jpg" alt="abupload" width="640" height="480" border="10" /></a>   
* [Arby Utility](https://github.com/obono/ArbyUtility?tab=readme-ov-file)
  * 支援Android
  * <a href="https://github.com/obono/ArbyUtility?tab=readme-ov-file" target="_blank"><img src="https://github.com/channel2007/UU-A0/blob/master/img/ArbyUtility.jpg" alt="abupload" width="640" height="480" border="10" /></a>

# 網路資源
* [【線上開發工具】ProjectABE](https://felipemanga.github.io/ProjectABE/)
* [【線上玩】ArduFXTest](https://bateske.github.io/ArduFXTest/)
* [【線上玩】arduboy.ried](https://arduboy.ried.cl/)
* [【Arduboy】arduboy](https://www.arduboy.com/?spm=a2c4e.11153940.blogcont658194.6.45b54ecbV1huX1)
* [【論壇】arduboy](https://community.arduboy.com/)
 
# 網路教學
* [【UU-A系列】Arduhandheld外殼](https://www.facebook.com/notes/687881295267364/)
* [【UU-A系列】阿杜掌機(Arduhandheld)遊戲開發(二) - 測試程式教學](https://www.facebook.com/notes/683344359271378/)
* [【UU-A系列】阿杜掌機(Arduhandheld)遊戲開發(一) - 函數庫安裝篇](https://www.facebook.com/notes/1042182786225668/)
* [【UU-A系列】好用的Arduboy Manager燒錄與管理工具](https://www.facebook.com/notes/796706577772025/)
* [【UU-A系列】阿杜掌機教學資源](https://medium.com/@uupgrade/arduino-pro-micro%E6%8E%8C%E6%A9%9F%E8%B3%87%E6%BA%90-4a99ceb9bd20)
* [【UU-A系列】遊戲開發(1)-線上開發](https://uupgrade.medium.com/uu-a%E7%B3%BB%E5%88%97-%E9%81%8A%E6%88%B2%E9%96%8B%E7%99%BC-1-%E7%B7%9A%E4%B8%8A%E9%96%8B%E7%99%BC-19f8f091bb55)
* [【UU-A系列】遊戲開發(2)-基本框架](https://uupgrade.medium.com/uu-a%E7%B3%BB%E5%88%97-%E9%81%8A%E6%88%B2%E9%96%8B%E7%99%BC-2-%E5%9F%BA%E6%9C%AC%E6%A1%86%E6%9E%B6-2ec7a07dc4df)

# 關於機器變磚塊問題
* 如果發現燒錄某個遊戲後機器在也不能再燒錄其他遊戲，那就表示機器變磚塊了，以下提供兩種方法解決機器磚塊問題：
  * 方法一：參考以下網址製作可以恢復變磚機器的裝置
    * [【UU-A系列】Arduhandheld變磚處理大法](https://www.facebook.com/notes/906590089746956/)
  * 方法二：購賣UU-A1燒錄器(在無限升級賣貨便賣場內有賣)
    * [進入無限升級賣貨便賣場](https://myship.7-11.com.tw/general/detail/GM2406077577313)
    * <a href="https://myship.7-11.com.tw/general/detail/GM2406077577313" target="_blank"><img src="https://github.com/channel2007/UU-A0/blob/master/img/uu_a1_boot.jpg" alt="abupload" width="640" height="480" border="10" /></a>         

# 官方粉絲團
[無限升級](https://www.facebook.com/unlimited.upgrade)
