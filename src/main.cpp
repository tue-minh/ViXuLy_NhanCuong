#include <Arduino.h>
#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C oled(U8X8_PIN_NONE);

#define BTN1 PIN_PD4
#define BTN2 PIN_PD5
#define BTN3 PIN_PD6
#define BTN4 PIN_PD7
#define POTENTIONMETER PIN_PC0

uint16_t readADC() {
  ADCSRA |= (1 << ADSC);             // Start conversion
  while (ADCSRA & (1 << ADSC));      // Wait
  return ADC;
}

void setup() {
  oled.begin();
  oled.setPowerSave(0);
  oled.setFont(u8x8_font_chroma48medium8_r);
  pinMode(POTENTIONMETER, INPUT);
  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);
  pinMode(BTN3, INPUT);
  pinMode(BTN4, INPUT);
}

void loop() {
  char buf[8];
  uint16_t adcValue = analogRead(POTENTIONMETER);
  itoa(adcValue, buf, 10);
  oled.clearDisplay();
  oled.drawString(0, 0, "ADC0:");
  oled.drawString(6, 0, buf);
  if(digitalRead(BTN1)==1)oled.drawString(0, 5, "PD4");
  if(digitalRead(BTN2)==1)oled.drawString(0, 5, "PD5");
  if(digitalRead(BTN3)==1)oled.drawString(0, 5, "PD6");
  if(digitalRead(BTN4)==1)oled.drawString(0, 5, "PD7");
  delay(100);
}