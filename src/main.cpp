#include <ESP32Servo.h>
#include <HCSR04.h>
#include <TM1637Display.h>

byte triggerPin = 4;
byte echoPin = 2;
#define SERVO_PIN 12
#define MAX_WIDTH 2500
#define MIN_WIDTH 500
#define LED_PIN 15
#define TM1637_CLK 17
#define TM1637_DIO 16
#define TEST_DELAY 2000

const uint8_t SEG_DONE[] = {
    SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,         // d
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, // O
    SEG_C | SEG_E | SEG_G,                         // n
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G          // E
};
double *distances = NULL;
// 定义 servo 对象
Servo my_servo;

TM1637Display display(TM1637_CLK, TM1637_DIO);

bool parking = false;

void setup()
{
  Serial.begin(9600);
  // 初始化超声波传感器
  HCSR04.begin(triggerPin, echoPin);
  // 分配硬件定时器
  ESP32PWM::allocateTimer(0);
  // 设置频率
  my_servo.setPeriodHertz(50);
  // 关联 servo 对象与 GPIO 引脚，设置脉宽范围
  my_servo.attach(SERVO_PIN, MIN_WIDTH, MAX_WIDTH);
  // 初始化 LED 引脚
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  // 初始化数码管
  distances = new double[1];
  *distances = 20;
  display.clear();
  display.showNumberDec(0, true);
  display.setBrightness(0x0f);
}

void servoDown()
{
  for (int i = 180; i >= 0; i--)
  {
    my_servo.write(i);
    delay(15);
  }
}

void servoUp()
{
  for (int i = 0; i <= 180; i++)
  {
    my_servo.write(i);
    delay(15);
  }
}

void countDown()
{
  for (int i = 10; i >= 0; i--)
  {
    display.showNumberDec(i, true);
    digitalWrite(LED_PIN, LOW);
    delay(500);
    digitalWrite(LED_PIN, HIGH);
    delay(500);
  }
}

void loop()
{
  float temperature = 15;
  distances = HCSR04.measureDistanceCm(temperature);
  Serial.print("Distance: ");
  Serial.println(*distances);

  if (distances[0] < 15 && !parking)
  {
    parking = true;
    servoDown();
    digitalWrite(LED_PIN, HIGH);
  }
  else if (distances[0] > 15 && parking)
  {
    countDown();
    servoUp();
    digitalWrite(LED_PIN, LOW);
    parking = false;
  }
  delay(250);
}
