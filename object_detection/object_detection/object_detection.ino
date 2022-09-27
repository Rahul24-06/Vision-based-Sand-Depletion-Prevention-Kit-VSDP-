#include "Seeed_Arduino_GroveAI.h"
#include <Wire.h>
#include "Free_Fonts.h" // Include the header file attached to this sketch

#include "SPI.h"
#include "TFT_eSPI.h"

// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

unsigned long drawTime = 0;
GroveAI ai(Wire);
uint8_t state = 0;
int xpos =  0, ypos = 40;
void setup()
{
  Wire.begin();
  Serial.begin(115200);
  Serial.println("begin");

  xpos = tft.width() / 2; // Half the screen width
  ypos = 50;

  tft.begin();
  tft.setRotation(3);
  tft.setFreeFont(FSSB18);
  // Numbers, floats and strings can be drawn relative to a datum
  tft.fillRect(0, 0, 320, 240, TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setFreeFont(FSS12);
  tft.setTextDatum(TL_DATUM);
  tft.drawString("VDSP", 120, 10, 4); // Font 4 for fast drawing with background

  tft.fillRect(20, 55, 140, 58, TFT_GREEN);
  tft.setTextColor(TFT_BLACK, TFT_GREEN);
  tft.drawString(" SECTOR A ", 25, 75, GFXFF);

  if (ai.begin(ALGO_OBJECT_DETECTION, MODEL_EXT_INDEX_1)) // Object detection and externel model 1 - Truck/Excavator
  {
    Serial.print("Version: ");
    Serial.println(ai.version());
    Serial.print("ID: ");
    Serial.println( ai.id());
    Serial.print("Algo: ");
    Serial.println( ai.algo());
    Serial.print("Model: ");
    Serial.println(ai.model());
    Serial.print("Confidence: ");
    Serial.println(ai.confidence());
    state = 1;
  }

  else
  {
    Serial.println("Algo begin failed.");
  }
}

void loop()
{
  if (state == 1)
  {
    uint32_t tick = millis();
    if (ai.invoke()) // begin invoke
    {
      while (1) // wait for invoking finished
      {
        CMD_STATE_T ret = ai.state();
        if (ret == CMD_STATE_IDLE)
        {
          break;
        }
        delay(20);
      }

      uint8_t len = ai.get_result_len(); // receive how many excavator, truck, and wheel loader detected
      if (len)
      {
        int time1 = millis() - tick;
        //Serial.print("Time consuming: ");
        //Serial.println(time1);
        Serial.println("Excavator, Human, truck, or wheel loader detected");
        //Serial.println(len);
        object_detection_t data;       //get data

        for (int i = 0; i < len; i++)
        {
          //Serial.println("result:detected");
          //Serial.print("Detecting and calculating: ");
          //Serial.println(i+1);
          ai.get_result(i, (uint8_t*)&data, sizeof(object_detection_t)); //get result

          //Serial.print("confidence:");
          //Serial.print(data.confidence);
          //Serial.println();
        }
        tft.fillRect(20, 130, 180, 160, TFT_WHITE);
        tft.setTextDatum(TL_DATUM);
        tft.setTextColor(TFT_RED, TFT_WHITE);
        tft.drawString(" Unauthorized entry", 20, 150, GFXFF);
      }
      else
      {
        Serial.println("No identification");

        tft.fillRect(20, 130, 205   , 160, TFT_WHITE);
        tft.setTextDatum(TL_DATUM);
        tft.setTextColor(TFT_BLACK, TFT_WHITE);
        tft.drawString(" Zone is safe", 20, 150, GFXFF);
      }
    }
    else
    {
      delay(1000);
      Serial.println("Invoke Failed.");
    }
  }

  else
  {
    state == 0;
  }
}
