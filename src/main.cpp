#include <Arduino.h>
#include "Tiny4kOLED.h"

#define SW2_PIN 4            // PD4
#define SW3_PIN 5            // PD5
#define SW4_PIN 6            // PD6
#define SW5_PIN 7            // PD7
#define POTENTIONMETER_PIN 0 // PC0

#define ENA_PIN 1 // PB1 (OC1A)
#define IN1_PIN 2 // PC2
#define IN2_PIN 3 // PC3

#define DEBOUNCE_TIME 40

#define SW2_READ() ((PIND & (1 << SW2_PIN)))
#define SW3_READ() ((PIND & (1 << SW3_PIN)))
#define SW4_READ() ((PIND & (1 << SW4_PIN)))
#define SW5_READ() ((PIND & (1 << SW5_PIN)))

uint8_t lastAdcValue = 0;
uint8_t lastBtnState = 0;
uint32_t lastDebounceTime[4] = {0, 0, 0, 0};

bool motorEnable = false;
bool motorDirection = true; // true: forward, false: reverse
bool forward = true;
bool reverse = false;
uint8_t pwmValue = 0;

uint8_t prevPWM = 255;

uint8_t currentDisplayMode = 1;
bool currentDisplayMotorEnable = true;
bool currentDisplayMotorDirection = false;

bool buttonPressed(uint8_t pin, uint8_t index)
{
  bool state = digitalRead(pin);

  if (state && !(lastBtnState & (1 << index)))
  {
    if (millis() - lastDebounceTime[index] > DEBOUNCE_TIME)
    {
      lastDebounceTime[index] = millis();
      lastBtnState |= (1 << index);
      return true;
    }
  }

  if (!state)
    lastBtnState &= ~(1 << index);

  return false;
}

void motorUpdate()
{
  if (!motorEnable)
  {
    // off: IN1 = LOW, IN2 = LOW
    PORTC &= ~((1 << IN1_PIN) | (1 << IN2_PIN));
    OCR1A = 0;
    return;
  }

  if (motorDirection)
  {
    // forward: IN1 = HIGH, IN2 = LOW
    PORTC |= (1 << IN1_PIN);
    PORTC &= ~(1 << IN2_PIN);
  }
  else
  {
    // reverse: IN1 = LOW, IN2 = HIGH
    PORTC &= ~(1 << IN1_PIN);
    PORTC |= (1 << IN2_PIN);
  }

  OCR1A = pwmValue;
}

void updateDisplay(uint8_t mode)
{
  if (mode != currentDisplayMode)
  {
    currentDisplayMode = mode;
    oled.setCursor(56, 0); // Cột 6, dòng 0
    oled.print(" ");       // Xóa 2 ký tự
    oled.setCursor(56, 0);
    oled.print(mode);
  }

  if (motorEnable != currentDisplayMotorEnable)
  {
    currentDisplayMotorEnable = motorEnable;
    oled.setCursor(56, 2);
    if (motorEnable)
    {
      oled.print("RUN ");
    }
    else
    {
      oled.print("IDLE");
    }
  }

  if (motorDirection != currentDisplayMotorDirection)
  {
    currentDisplayMotorDirection = motorDirection;
    oled.setCursor(56, 4);
    if (motorDirection)
    {
      oled.print("FWD");
    }
    else
    {
      oled.print("REV");
    }
  }

  if (motorEnable && pwmValue != prevPWM)
  {
    oled.setCursor(56, 6);
    oled.print("   ");
    oled.setCursor(56, 6);
    oled.print(pwmValue);
    oled.setCursor(80, 6);
    oled.print("/255");
  }
  if (!motorEnable)
  {
    oled.setCursor(56, 6); // Cột 5, dòng 3
    oled.print("       "); // Xóa 3 ký tự
  }
}

void setup()
{
  oled.begin(128, 64, sizeof(tiny4koled_init_128x64br), tiny4koled_init_128x64br);
  oled.setFont(FONT8X16);
  oled.on();
  oled.clear();

  oled.setCursor(0, 0);
  oled.print(F("MODE: "));
  oled.setCursor(0, 2);
  oled.print(F("STATE: "));
  oled.setCursor(0, 4);
  oled.print(F("DIR: "));
  oled.setCursor(0, 6);
  oled.print(F("PWM: "));

  // ADC
  ADMUX = (1 << REFS0);
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128

  // IO
  DDRD &= ~((1 << SW2_PIN) | (1 << SW3_PIN) | (1 << SW4_PIN) | (1 << SW5_PIN));
  PORTD &= ~((1 << SW2_PIN) | (1 << SW3_PIN) | (1 << SW4_PIN) | (1 << SW5_PIN));

  DDRC |= (1 << IN1_PIN) | (1 << IN2_PIN);
  DDRB |= (1 << ENA_PIN);

  DDRC &= ~(1 << POTENTIONMETER_PIN);

  // Timer1
  TCCR1A = (1 << COM1A1) | (1 << WGM10);
  TCCR1B = (1 << WGM12) | (1 << CS10);
  OCR1A = 0;

  // init
  PORTC &= ~((1 << IN1_PIN) | (1 << IN2_PIN));
}

void loop()
{
  ADCSRA |= (1 << ADSC);
  while (ADCSRA & (1 << ADSC))
    ;

  uint16_t adcValue = ADC;
  uint8_t Mode_Control = adcValue / 204;
  if (Mode_Control > 4)
    Mode_Control = 4;

  switch (Mode_Control)
  {
  case 0:
    pwmValue = 0;
    break;
  case 1:
    pwmValue = 51;
    break;
  case 2:
    pwmValue = 102;
    break;
  case 3:
    pwmValue = 153;
    break;
  case 4:
    pwmValue = 204;
    break;
  }

  if (buttonPressed(SW2_PIN, 0))
    motorEnable = true;
  if (buttonPressed(SW3_PIN, 1))
    motorEnable = false;
  if (buttonPressed(SW4_PIN, 2))
  {
    forward = true;
    reverse = false;
  }
  if (buttonPressed(SW5_PIN, 3))
  {
    forward = false;
    reverse = true;
  }

  motorDirection = forward && !reverse;

  motorUpdate();
  updateDisplay(Mode_Control);
}
