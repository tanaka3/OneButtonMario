#include "switch_tinyusb_custom.h"

#define SW_SATRT_GPIO 19
#define SW_LEFT_GPIO 18
#define SW_B_GPIO 17
#define SW_A_GPIO 16

Adafruit_USBD_HID G_usb_hid;
NSGamepad Gamepad(&G_usb_hid);
HID_NSGamepadReport_Data_t beforeData;

bool resetFlg = false;

void setup() {

  Serial.begin(115200);

  Gamepad.begin();
  // wait until device mounted
  while( !USBDevice.mounted() ) delay(1);

  pinMode(SW_SATRT_GPIO, INPUT_PULLUP);
  pinMode(SW_LEFT_GPIO, INPUT_PULLUP);
  pinMode(SW_B_GPIO, INPUT_PULLUP);
  pinMode(SW_A_GPIO, INPUT_PULLUP);

  pinMode(25, OUTPUT);
  digitalWrite(25, HIGH);   
}

void loop() {

  Gamepad.reset();

  // 他のキーが押下されている場合、スタートが無効となるため
  // スタートが押されている場合は、他のボタンを無視する
  if(digitalRead(SW_SATRT_GPIO) == LOW){

    // スタート押下状態で、他のボタンが押されている場合は
    // ゲームをリセットする
    if(digitalRead(SW_A_GPIO) == LOW && 
      digitalRead(SW_STOP_GPIO) == LOW ){

      if(resetFlg){
        return;
      }

      //リセットフラグを有効し、複数回の実行を防ぐ
      resetFlg = true;
      resetControll();

    }
    else{
      resetFlg = false;
      Gamepad.press(NSButton_Plus);
    }
  }
  else{
    resetFlg = false;

    //キーロックが有効の場合は、Bダッシュ状態とする
    if(digitalRead(SW_B_GPIO) == LOW){
      Gamepad.press(NSButton_B);
    }

    if(digitalRead(SW_A_GPIO) == LOW){
      Gamepad.press(NSButton_A);
    }    

    bool up = false;
    bool down = false;
    // 強制的に移動させる
    bool right = true;
    // 右と左が同時に押されている場合は、無効の判断になる（なので止まる）
    bool left = stopButton.read() == LOW;

    Gamepad.dPad(up, down, left, right);

  }

  // 前回と入力が同じ場合は、ボタン操作を送信しない
  if(!Gamepad.compareTo(beforeData)){
    Gamepad.SendReport();
    beforeData = Gamepad.getReportData();
  }
}

/**
 * @brief ファミコンのリセット操作 
 * ZL＋ZR押してメニューのリセットを選ぶ操作
 */
void resetControll(){

  Gamepad.reset();
  Gamepad.press(NSButton_LeftThrottle);
  Gamepad.press(NSButton_RightThrottle);  
  Gamepad.SendReport();
  delay(100);

  for(int i=0; i<3; i++){
    Gamepad.reset();
    Gamepad.dPad(true, false, false, false);
    Gamepad.SendReport();
    delay(150);
    Gamepad.reset();
    Gamepad.SendReport();
    delay(150);
  }
  
  Gamepad.reset();
  Gamepad.press(NSButton_A);
  Gamepad.SendReport();
}
