#include <Arduino.h>
#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C oled(U8X8_PIN_NONE);

#define BTN1 PIN_PD4
#define BTN2 PIN_PD5
#define BTN3 PIN_PD6
#define BTN4 PIN_PD7
#define POTENTIONMETER PIN_PC0
// #define ADC_SAMPLES 8
#define ADC_THRESHOLD 100

// uint16_t adcBuffer[ADC_SAMPLES];
// uint8_t adcIndex = 0;
// uint32_t adcSum = 0;
uint16_t lastAdcValue = 0xFFFF;
uint8_t lastBtnState = 0xFF;

// uint16_t readAdcFiltered() {
//   uint16_t raw = analogRead(POTENTIONMETER);

//   adcSum -= adcBuffer[adcIndex];
//   adcBuffer[adcIndex] = raw;
//   adcSum += raw;

//   adcIndex++;
//   if (adcIndex >= ADC_SAMPLES) adcIndex = 0;

//   return adcSum / ADC_SAMPLES;
// }

void setup()
{
  oled.begin();
  oled.setPowerSave(0);
  oled.setFont(u8x8_font_8x13_1x2_r);
  pinMode(POTENTIONMETER, INPUT);
  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);
  pinMode(BTN3, INPUT);
  pinMode(BTN4, INPUT);
}

void loop()
{
  uint16_t adcValue = analogRead(POTENTIONMETER);

  uint8_t btnState = 0;
  btnState |= digitalRead(BTN1) << 0;
  btnState |= digitalRead(BTN2) << 1;
  btnState |= digitalRead(BTN3) << 2;
  btnState |= digitalRead(BTN4) << 3;

  // if (adcValue != lastAdcValue || btnState != lastBtnState)
  if (abs(adcValue - lastAdcValue) > ADC_THRESHOLD || btnState != lastBtnState) 
  {
    lastAdcValue = adcValue;
    lastBtnState = btnState;

    char buf[8];
    itoa(adcValue, buf, 10);

    oled.clearDisplay();
    oled.drawString(0, 0, "ADC0:");
    oled.drawString(6, 0, buf);

    uint8_t y = 4;

    if (btnState & (1 << 0))
      oled.drawString(0, y++, "PD4");
    if (btnState & (1 << 1))
      oled.drawString(0, y++, "PD5");
    if (btnState & (1 << 2))
      oled.drawString(0, y++, "PD6");
    if (btnState & (1 << 3))
      oled.drawString(0, y++, "PD7");
  }
}