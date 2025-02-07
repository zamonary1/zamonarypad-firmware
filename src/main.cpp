#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else

#include <Arduino.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

#include <projectDefs.h>
#include <aLED.h>
#include <usb.h>
#include <serialStruff.h>


//#define NUM_LEDS 2

// ESP32 Touch keyboard


unsigned int value1;
unsigned int value2;
unsigned int button1_sensitivity;
unsigned int button2_sensitivity;
unsigned long millis_sleep_timer = 0;
unsigned int i = 1;
const unsigned short sleep_timeout = 10000;  //10 seconds
bool touch1detected = false;
bool touch2detected = false;

bool button_1_pressed = false;
bool button_2_pressed = false;

long time_leds_updated_last;

int bootanim_col_r;
int bootanim_col_g;
int bootanim_col_b;
int click_col_r;
int click_col_g;
int click_col_b;

int btn1val;
int btn2val;

static portMUX_TYPE _spinlock = portMUX_INITIALIZER_UNLOCKED;

// struct{
//   uint16_t &btn1val;
//   uint16_t &btn2val;
//   uint16_t &button1_sensitivity;
//   uint16_t &sbutton2_sensitivity;
// } ButtonData_t;


void handleSerial( void * pvParameters ){
  // Save the original priority
  

  while(1){
    if (Serial.available() > 0) {
      // char inputString[48] = Serial.readStringUntil('\n');  // read untill the next string
      
      // taskENTER_CRITICAL(&_spinlock);

      char inputString[64];
      Serial.readStringUntil('\n').toCharArray(inputString, 63);
      responseSerial(inputString, btn1val, btn2val, button1_sensitivity, button2_sensitivity);
      
      // taskEXIT_CRITICAL(&_spinlock);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void press_button_1(){ //using these functions is keeping the load off USB
  if (!button_1_pressed){    //less identical packets sent = less waiting for mcu = more poll rate
  key_press('z');
  button_1_pressed = true;
  }
}
void release_button_1(){ //using these functions is keeping the load off USB
  if (button_1_pressed){    //less identical packets sent = less waiting for mcu = more poll rate
  key_release('z');
  button_1_pressed = false;
  }
}

void press_button_2(){ //using these functions is keeping the load off USB
  if (!button_2_pressed){    //less identical packets sent = less waiting for mcu = more poll rate
  key_press('x');
  button_2_pressed = true;
  }
}
void release_button_2(){ //using these functions is keeping the load off USB
  if (button_2_pressed){    //less identical packets sent = less waiting for mcu = more poll rate
  key_release('x');
  button_2_pressed = false;
  }
}

void handleButtons( void * pvParameters ){
  (void) pvParameters;

  while(1){
    
    btn1val = touchRead(btn_1_pin);
    btn2val = touchRead(btn_2_pin);

    if (btn1val > button1_sensitivity) press_button_1();
    else if (btn1val < button1_sensitivity - read_data_rounding) release_button_1();

    if (btn2val > button2_sensitivity) press_button_2();
    else if (btn2val < button2_sensitivity - read_data_rounding) release_button_2();

    vTaskDelay(0.5 / portTICK_PERIOD_MS); //0.5ms delay
  }
}

void handle_led( void * pvParameters ){ //this function gets called evxzzery xzxzxxxxxxxxxxxxxxxxxxxxxxxxxxxxzxzxzxzxzloop() cyczzxxzle and controls the behavior of LEDs.

  (void) pvParameters;

  while(1){
    if (btn1val>button1_sensitivity){
      led1_rgb(click_col_r, click_col_g, click_col_b);
    } else if (btn1val<button1_sensitivity-read_data_rounding) {
      led1_rgb(0, 0, 0);
    }

    if (btn2val>button2_sensitivity){
      led2_rgb(click_col_r, click_col_g, click_col_b);
    } else if (btn2val<button2_sensitivity-read_data_rounding) {
      led2_rgb(0, 0, 0);
    }



    led_update();


    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
  //  time_leds_updated_last=millis();
  //}
  
}

void firstLaunchInit(){

  for(uint8_t i = 0; i<3; i++){
    led1_rgb(255, 255, 255);
    led2_rgb(255, 255, 255);
    led_update();
    delay(100);

    led1_rgb(0, 0, 0);
    led2_rgb(0, 0, 0);
    led_update();
    delay(100);
  }

  delay(3000);

  int btn1val = touchRead(btn_1_pin);
  int btn2val = touchRead(btn_2_pin);

  EEPROM.put(eeprombtn1sens, btn1val+10000);
  EEPROM.put(eeprombtn2sens, btn2val+10000);

  EEPROM.put(eepromstartanim_colr, 254 ); //color of boot animation
  EEPROM.put(eepromstartanim_colg, 0   );
  EEPROM.put(eepromstartanim_colb, 0   );

  EEPROM.put(eepromclick_colr,     0   ); //click indication color
  EEPROM.put(eepromclick_colg,     20  );
  EEPROM.put(eepromclick_colb,     15  );
  
  EEPROM.put(0, 100);//completed initial setup, skip on next boot
  EEPROM.commit();
  led1_rgb(0, 255, 0);
  led2_rgb(0, 255, 0);
  led_update();
  delay(500);

  led1_rgb(0, 0, 0);
  led2_rgb(0, 0, 0);
  led_update();

}

void getEEPROMvars(){
  if (EEPROM.read(0) != 100) firstLaunchInit();

  EEPROM.get(eeprombtn1sens, button1_sensitivity);
  EEPROM.get(eeprombtn2sens, button2_sensitivity);

  EEPROM.get(eepromstartanim_colr, bootanim_col_r);
  EEPROM.get(eepromstartanim_colg, bootanim_col_g);
  EEPROM.get(eepromstartanim_colb, bootanim_col_b);

  EEPROM.get(eepromclick_colr, click_col_r);
  EEPROM.get(eepromclick_colg, click_col_g);
  EEPROM.get(eepromclick_colb, click_col_b);
}

void printStartInfo(){
  Serial.println(button1_sensitivity);
  Serial.println(button2_sensitivity);
  Serial.print(bootanim_col_r);
  Serial.print(' ');
  Serial.print(bootanim_col_g);
  Serial.print(' ');
  Serial.println(bootanim_col_b);
  Serial.print(click_col_r);
  Serial.print(' ');
  Serial.print(click_col_g);
  Serial.print(' ');
  Serial.print(click_col_b);

  Serial.print("Max FreeRTOS priority: " + configMAX_PRIORITIES);
}

void setup() {
  Serial.begin(115200);
  delay(200);  // give me some time to bring up serial monitor

  usb_init();

  pinMode(leds_pin, OUTPUT);

  led_init();

  EEPROM.begin(100);

  getEEPROMvars();



  for (unsigned char i = 0; i<255; i+=1){
    uint8_t col_r = bootanim_col_r * i / 255;
    uint8_t col_g = bootanim_col_g * i / 255;
    uint8_t col_b = bootanim_col_b * i / 255;
    led1_rgb(col_r, col_g, col_b);
    led2_rgb(col_r, col_g, col_b);
    led_update();
    delay(2);
  }

  for (unsigned char i = 255; i>0; i-=1){
    uint8_t col_r = bootanim_col_r * i / 255;
    uint8_t col_g = bootanim_col_g * i / 255;
    uint8_t col_b = bootanim_col_b * i / 255;
    led1_rgb(col_r, col_g, col_b);
    led2_rgb(col_r, col_g, col_b);
    led_update();
    delay(2);
  }

  printStartInfo();

  
  xTaskCreate(&handleButtons, //Function name
      "Touch buttons update", //Task display name
        64 * alloc_word_size, //Stack size, 32 words or 256 bytes
                        NULL, //Passed parameters
                           2, //Priority of task
                       NULL); //task's handle  


  xTaskCreate(&handle_led, //Function name
           "aRGB handler", //Task display name
     32 * alloc_word_size, //Stack size, 32 words or 128 bytes
                     NULL, //Passed parameters
                        2, //Priority of task
                    NULL); //task's handle  


  xTaskCreate(&handleSerial, //Function name
           "Serial console", //Task display name
       64 * alloc_word_size, //Stack size, 64 words or 256 bytes
                       NULL, //Passed parameters
                          3, //Priority of task
                      NULL); //task's handle  



}
//
void loop() {


  // handleSerial(NULL);

  // delay(100);
}


#endif
