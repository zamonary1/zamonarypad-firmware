#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <projectDefs.h>
#include "serialStruff.h"

bool stringStartsWith(char *in_string, char *start_string) {
    size_t len_in = strlen(in_string);
    size_t len_start = strlen(start_string);

    if (len_start > len_in) {
        return false;
    }

    return strncmp(in_string, start_string, len_start) == 0;
}

char *getSubstring(char *in_string, char *buf, uint8_t pos = 0, uint8_t len = 50){

  	int s_len = strlen(in_string);
  	
  	if ( s_len-pos < len ) len = s_len-pos; 
    // Char array to store the substring
    //char ss[16];

    // using strncpy
    strncpy(buf, in_string + pos, len);

    // Null terminate the substring
    buf[len] = '\0';


    return 0;

} 

int getSubstringInt(char *in_string, uint8_t pos = 0, uint8_t len = 50){
  char buf[6];
  getSubstring(in_string, buf, pos, len);
  int out;
  sscanf(buf, "%d", &out);

  return out;
}

void printJsonDoc(JsonDocument& doc, uint8_t len = 64){
  char stringResponse[len];
  serializeJson(doc, stringResponse, len);
  Serial.println(stringResponse);
}

void responseSerial(char inputString[48],
                    uint16_t btn1val,
                    uint16_t btn2val,
                    uint16_t button1_sensitivity,
                    uint16_t button2_sensitivity)
{
  if (0 == strcmp(inputString, "read"))
  {
    JsonDocument response;
    response["button1val"] = btn1val;
    response["button2val"] = btn2val;
    printJsonDoc(response);
  }

  else if (0 == strcmp(inputString, "readmore"))
  {
    JsonDocument response;
    response["button1val"] = btn1val;
    response["button2val"] = btn2val;
    response["button1sens"] = button1_sensitivity;
    response["button2sens"] = button2_sensitivity;
    response["millis"] = millis();
    printJsonDoc(response, 128);
  }

  else if (stringStartsWith(inputString, "wrbtn1"))
  {
    int argument = getSubstringInt(inputString, 6);
    // getSubstringInt(inputString, argument, 6)
    EEPROM.put(eeprombtn1sens, argument);
    EEPROM.commit();
    button1_sensitivity = argument;
    /*      #ifdef interrupts
            touchDetachInterrupt(btn_1_pin);
            touchAttachInterrupt(btn_1_pin, gotTouch1, argument);xxxxzzzz
          #endif */
    JsonDocument response;
    response["button1val"] = touchRead(btn_1_pin);
    response["button2val"] = touchRead(btn_2_pin);
    response["status"] = "success";
    printJsonDoc(response);
  }

  else if (stringStartsWith(inputString, "wrbtn2"))
  {
    int argument = getSubstringInt(inputString, 6);

    EEPROM.put(eeprombtn2sens, argument);
    EEPROM.commit();
    button2_sensitivity = argument;
    /*      #ifdef interrupts
            touchDetachInterrupt(btn_2_pin);
            touchAttachInterrupt(btn_2_pin, gotTouch2, argument);
          #endif */
    JsonDocument response;
    response["button1val"] = touchRead(btn_1_pin);
    response["button2val"] = touchRead(btn_2_pin);
    response["status"] = "success";
    printJsonDoc(response);
  }

  else if (stringStartsWith(inputString, "wrbanim"))
  {
    uint8_t argument_r = getSubstringInt(inputString,  8, 3);
    uint8_t argument_g = getSubstringInt(inputString, 12, 3);
    uint8_t argument_b = getSubstringInt(inputString, 16, 3);

    EEPROM.put(eepromstartanim_colr, argument_r);
    EEPROM.put(eepromstartanim_colg, argument_g);
    EEPROM.put(eepromstartanim_colb, argument_b);
    EEPROM.commit();

    JsonDocument response;
    response["button1val"] = touchRead(btn_1_pin);
    response["button2val"] = touchRead(btn_2_pin);
    response["status"] = "success";
    printJsonDoc(response);
  }

  else if (stringStartsWith(inputString, "wrcfbck"))
  {
    // uint8_t argument_r = getSubstring(inputString, 8, 3).toInt();
    // uint8_t argument_g = getSubstring(inputString, 12, 3).toInt();
    // uint8_t argument_b = getSubstring(inputString, 16, 3).toInt();
    uint8_t argument_r = getSubstringInt(inputString,  8, 3);
    uint8_t argument_g = getSubstringInt(inputString, 12, 3);
    uint8_t argument_b = getSubstringInt(inputString, 16, 3);

    EEPROM.put(eepromclick_colr, argument_r);
    EEPROM.put(eepromclick_colg, argument_g);
    EEPROM.put(eepromclick_colb, argument_b);
    EEPROM.commit();

    JsonDocument response;
    response["button1val"] = touchRead(btn_1_pin);
    response["button2val"] = touchRead(btn_2_pin);
    response["status"] = "success";
    printJsonDoc(response);
  }

  else if (0 == strcmp(inputString, "setup"))
  {
    EEPROM.put(0, 255); // make it seem like it's first boot
    EEPROM.commit();
    ESP.restart();
  }

  else if (0 == strcmp(inputString, "r"))
  {
    ESP.restart();
  }

  else if (0 == strcmp(inputString, "hello"))
  {
    JsonDocument response;
    response["response"] = "Hello!";
    response["millis"] = millis();
    printJsonDoc(response);
  }

  else
  {
    // unknown command
    Serial.print("unknown: \"");
    Serial.print(inputString);
    Serial.println("\"");
  }
}