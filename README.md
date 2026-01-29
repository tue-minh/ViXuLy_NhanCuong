# ATmega8L + Arduino Mega 2560 (Arduino as ISP) + PlatformIO + OLED SSD1306 + Potentionmeter

- Kết nối **Arduino Mega 2560 làm ISP**
- Burn bootloader cho **ATmega8L**
- Cấu hình fuse
- Tạo project **PlatformIO**
- Hiển thị OLED SSD1306 I2C
- Đọc ADC + Button

---

## Phần cứng sử dụng

- ATmega8L (DIP-28)
- Arduino Mega 2560
- OLED SSD1306 I2C (128x64)
- Biến trở 20k (ADC)
- 4 nút nhấn (button)
- Thạch anh 8 MHz (chỉ dùng khi cứu fuse / test)
- Tụ gốm, điện trở

---

## Kết nối Arduino Mega 2560 làm ISP

### Nạp ArduinoISP cho Mega
Trong Arduino IDE:
- Board : Arduino Mega or Mega 2560
- Processor : ATmega2560
- Programmer : AVRISP mkII (mặc định khi upload sketch)   
- Mở: File → Examples → ArduinoISP → ArduinoISP
- Upload vào Arduino Mega.

### Chống reset cho Mega
- Gắn tụ: Arduino Mega RESET Pin — 10µF — 5V
---
### 2.3 Sơ đồ nối ISP

| Mega 2560 | ATmega8L |
|----------|----------|
| 50 (MISO) | PB4 (pin 18) |
| 51 (MOSI) | PB3 (pin 17) |
| 52 (SCK)  | PB5 (pin 19) |
| 53 (SS)   | RESET (pin 1) |
| 5V        | VCC (pin 7) + AVCC (pin 20) |
| GND       | GND (pin 8, 22) |

---

## Burn bootloader cho ATmega8L (Arduino IDE)

### Cài MiniCore
- Arduino IDE → Preferences → Additional Boards Manager URLs: https://mcudude.github.io/MiniCore/package_MCUdude_MiniCore_index.json
- Boards Manager → cài **MiniCore**
---

### Cấu hình MiniCore
- Board : MiniCore → ATmega8
- Clock : 8 MHz Internal
- BOD : Disabled
- Compiler LTO : Disabled
- Variant : Standard
- ISP Clock : 125 kHz
- Programmer : Arduino as ISP
→ **Burn Bootloader**

---

## User Manual
- Triết áp: Trả về 9 giá trị từ 0 tới 8 tương đương với 8 tốc độ tăng dần của động cơ.
- SW2: Bật động cơ
- SW3: Tắt động cơ
- SW4: Xoay thuận chiều (default)
- SW5: Xoay ngược chiều

| L298N | ATmega8L |
|----------|----------|
| PWM (ENA) | TIM1A (PB1) |
| IN1 | PC2 |
| IN2 | PC3 |
| GND | GND |