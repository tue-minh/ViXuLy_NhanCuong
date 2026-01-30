#include <Arduino.h>
#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C oled(U8X8_PIN_NONE);

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
uint8_t pwmValue = 0;

uint8_t prevMode = 0xff;
bool prevMotorEnable = 2;
bool prevMotorDirection = 2;

bool buttonPressed(uint8_t pinMask, uint8_t index)
{
  bool state = (PIND & pinMask) ? true : false;

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

  uint8_t Mode_Control = adcValue / 113;
  if (Mode_Control > 8)
    Mode_Control = 8;

  pwmValue = map(Mode_Control, 0, 8, 0, 255);

  if (buttonPressed((1 << SW2_PIN), 0))
    motorEnable = true;
  if (buttonPressed((1 << SW3_PIN), 1))
    motorEnable = false;
  if (buttonPressed((1 << SW4_PIN), 2))
    motorDirection = true;
  if (buttonPressed((1 << SW5_PIN), 3))
    motorDirection = false;

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