#include <Arduino.h>
#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C oled(U8X8_PIN_NONE);

#define SW2 PIN_PD4
#define SW3 PIN_PD5
#define SW4 PIN_PD6
#define SW5 PIN_PD7

#define POTENTIONMETER PIN_PC0

#define L298_ENA PIN_PB1 // OC1A - PWM - TIM1A
#define L298_IN1 PIN_PC2
#define L298_IN2 PIN_PC3

#define DEBOUNCE_TIME 40

uint8_t lastAdcValue = 0;
uint8_t lastBtnState = 0;
uint32_t lastDebounceTime[4] = {0, 0, 0, 0};

bool motorEnable = false;
bool motorDirection = true; // true: forward, false: reverse
uint8_t pwmValue = 0;

uint8_t prevMode = 0xFF;
bool prevMotorEnable = 2;
bool prevMotorDirection = 2;

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
    digitalWrite(L298_IN1, LOW);
    digitalWrite(L298_IN2, LOW);
    analogWrite(L298_ENA, 0);
    return;
  }

  if (motorDirection)
  {
    digitalWrite(L298_IN1, HIGH);
    digitalWrite(L298_IN2, LOW);
  }
  else
  {
    digitalWrite(L298_IN1, LOW);
    digitalWrite(L298_IN2, HIGH);
  }

  analogWrite(L298_ENA, pwmValue);
}

void oledUpdate(uint8_t mode)
{
  oled.clearDisplay();

  char buf[5];
  oled.drawString(0, 0, "MODE:");
  itoa(mode, buf, 10);
  oled.drawString(6, 0, buf);

  oled.drawString(0, 3, motorEnable ? "RUN " : "STOP");
  oled.drawString(0, 5, motorDirection ? "FWD" : "REV");
}

void setup()
{
  oled.begin();
  oled.setPowerSave(0);
  oled.setFont(u8x8_font_8x13_1x2_r);

  pinMode(POTENTIONMETER, INPUT);
  pinMode(SW2, INPUT);
  pinMode(SW3, INPUT);
  pinMode(SW4, INPUT);
  pinMode(SW5, INPUT);

  pinMode(L298_ENA, OUTPUT);
  pinMode(L298_IN1, OUTPUT);
  pinMode(L298_IN2, OUTPUT);

  analogWrite(L298_ENA, 0);
}

void loop()
{
  uint16_t adcValue = analogRead(POTENTIONMETER);
  uint8_t Mode_Control = adcValue / 113;
  if (Mode_Control > 8)
    Mode_Control = 8;

  pwmValue = map(Mode_Control, 0, 8, 0, 255);

  if (buttonPressed(SW2, 0))    motorEnable = true;
  if (buttonPressed(SW3, 1))    motorEnable = false;
  if (buttonPressed(SW4, 2))    motorDirection = true;
  if (buttonPressed(SW5, 3))    motorDirection = false;

  motorUpdate();
  if (Mode_Control != prevMode ||
      motorEnable != prevMotorEnable ||
      motorDirection != prevMotorDirection)
  {
    prevMode = Mode_Control;
    prevMotorEnable = motorEnable;
    prevMotorDirection = motorDirection;

    oledUpdate(Mode_Control);
  }
}