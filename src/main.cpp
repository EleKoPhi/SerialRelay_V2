#include <Arduino.h>

#include <time.h>

#include <U8g2lib.h>
#include <U8x8lib.h>
#include <Wire.h>

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/U8X8_PIN_NONE);

const int size = 8;
byte serialBuffer[size] = {0};
int returnLenght;
uint8_t mode;

uint8_t startIdentifier = 0x0F;
uint8_t endIdentfier = 0xF0;

uint8_t typeIndex = 1;
uint8_t payLoadStartIndex = 3;

const uint8_t writeInformation = 1;
const uint8_t getInformation = 2;
const uint8_t findDevice = 3;

struct channel
{
  int idx;
  bool status;
  int pin;
  int displayPositionX;
  int displayPositionY;
  int mask;
};

const int noChanels = 4;
struct channel c1, c2, c3, c4;
channel channels[noChanels] = {c1, c2, c3, c4};

bool stC1 = false;
int stC1Pin = 13;

bool stC2 = false;
int stC2Pin = 12;

bool stC3 = false;
int stC3Pin = 11;

bool stC4 = false;
int stC4Pin = 10;



String active = "On ";
String inActice = "Off";

void resetSerialBuffer();
void resetSerialBuffer()
{
  while (Serial.available())
  {
    Serial.read();
  }
}

void sendInvalid();
void sendInvalid()
{
  for (int index = 0; index < size; index++)
  {
    Serial.write(byte(0xFF));
  }
}

void InitializeBuffer(byte value);
void InitializeBuffer(byte value)
{
  for (int index = 0; index < size; index++)
  {
    serialBuffer[index] = value;
  }
}

void SendSerialBuffer();
void sendSerialBuffer()
{
  for (int index = 0; index < size; index++)
  {
    Serial.write(serialBuffer[index]);
  }
}

void updateDisplay(bool c1, bool c2, bool c3, bool c4)
{
  u8x8.clear();
  u8x8.setFont(u8x8_font_8x13_1x2_f);

  u8x8.setCursor(0, 0);
  u8x8.print("Chanel 1 : ");
  u8x8.setCursor(11, 0);
  u8x8.print(c1 ? active : inActice);

  u8x8.setCursor(0, 2);
  u8x8.print("Chanel 2 : ");
  u8x8.setCursor(11, 2);
  u8x8.print(c2 ? active : inActice);

  u8x8.setCursor(0, 4);
  u8x8.print("Chanel 3 : ");
  u8x8.setCursor(11, 4);
  u8x8.print(c3 ? active : inActice);

  u8x8.setCursor(0, 6);
  u8x8.print("Chanel 4 : ");
  u8x8.setCursor(11, 6);
  u8x8.print(c4 ? active : inActice);
}

void updateStatus(channel c)
{
    u8x8.setCursor(c.displayPositionX, c.displayPositionY);
    u8x8.print(c.status ? active : inActice);
}

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(0.5);

  channels[0].pin = stC1Pin;
  channels[0].status = LOW;
  channels[0].displayPositionX = 11;
  channels[0].displayPositionY = 0;
  channels[0].mask = 1;
  channels[0].idx = 1;

  channels[1].pin = stC2Pin;
  channels[1].status = LOW;
  channels[1].displayPositionX = 11;
  channels[1].displayPositionY = 2;
  channels[1].mask = 2;
  channels[1].idx = 2;

  channels[2].pin = stC3Pin;
  channels[2].status = LOW;
  channels[2].displayPositionX = 11;
  channels[2].displayPositionY = 4;
  channels[2].mask = 4;
  channels[2].idx = 3;

  channels[3].pin = stC4Pin;
  channels[3].status = LOW;
  channels[3].displayPositionX = 11;
  channels[3].displayPositionY = 6;
  channels[3].mask = 8;
  channels[3].idx = 4;

  for (int index = 0; index < noChanels; index++)
  {
    pinMode(channels[index].pin, OUTPUT);
    digitalWrite(channels[index].pin, LOW);
    updateStatus(channels[index]);
  }
  
  resetSerialBuffer();
  u8x8.begin();
  updateDisplay(false, false, false, false);
}

void loop()
{

  if (Serial.available() >= size)
  {

    returnLenght = Serial.readBytes(serialBuffer, size);

    // Check for valid frame layout
    // If layout is not valid - Return 8x 0xFF
    if (serialBuffer[0] != byte(startIdentifier) or serialBuffer[7] != byte(endIdentfier))
    {
      sendInvalid();
      sendSerialBuffer();
      InitializeBuffer((0xFF));
    }
    else
    {
      switch (serialBuffer[typeIndex])
      {
      case findDevice:
        for (int index = 0; index < size; index++)
        {
          Serial.write(serialBuffer[index]);
        }
        break;

      case writeInformation:

        for (int index = 0; index < noChanels; index++)
        {
          bool _stTemp = channels[index].status;
          channels[index].status = (serialBuffer[payLoadStartIndex] & byte(channels[index].mask)) != 0;
          if (channels[index].status != _stTemp)
          {
            updateStatus(channels[index]);
            digitalWrite(channels[index].pin, channels[index].status);
          }
        }

        for (int index = 0; index < size; index++)
        {
          Serial.write(serialBuffer[index]);
        }

        break;

      default:
        break;
      }
    }
  }
}