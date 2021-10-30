/*
  Скетч к проекту "Bluetooth управляемая RGB матрица"
  Гайд по постройке матрицы: https://alexgyver.ru/matrix_guide/
  Страница проекта (схемы, описания): https://alexgyver.ru/GyverMatrixBT/
  Исходники на GitHub: https://github.com/AlexGyver/GyverMatrixBT/
  Нравится, как написан код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2018
  https://AlexGyver.ru/
*/

// Версия прошивки 1.6, совместима с приложением версии 1.5 и выше
/*
  Более удачно сконфигурированы таймеры
  Пофикшен баг в тетрисе
  Добавлена возможность отключить bluetooth и режим текста
*/

// ******************************** НАСТРОЙКИ ********************************
// чем больше матрица и количество частиц (эффекты), тем выше шанс того, что всё зависнет!
// Данный код стабильно работает на матрице 16х16 (256 диодов). Больше - на ваш страх и риск

#define BT_MODE 0           // использовать блютус (0 нет, 1 да)
#define USE_FONTS 1         // использовать буквы (бегущая строка) (0 нет, 1 да). Можно отключить для экономии памяти!

#define D_TEXT_SPEED 100    // скорость бегущего текста по умолчанию
#define D_EFFECT_SPEED 100  // скорость эффектов по умолчанию 
#define D_GAME_SPEED 100    // скорость игр по умолчанию 

#define AUTOPLAY_PERIOD 10  // время между авто сменой режимов (секунды)
#define IDLE_TIME 10        // время бездействия кнопок (в секундах) после которого запускается автосмена режимов и демо в играх

#define LED_PIN 6           // пин ленты
#define BRIGHTNESS 60       // стандартная маскимальная яркость (0-255)

#define WIDTH 16            // ширина матрицы
#define HEIGHT 16           // высота матрицы

#define MATRIX_TYPE 0       // тип матрицы: 0 - зигзаг, 1 - последовательная
#define CONNECTION_ANGLE 0  // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
#define STRIP_DIRECTION 0   // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
// шпаргалка по настройке матрицы здесь! https://alexgyver.ru/matrix_guide/

#define SCORE_SIZE 1        // размер символов счёта в игре. 0 - маленький для 8х8 (шрифт 3х5), 1 - большой (шрифт 5х7)
#define FONT_TYPE 1			    // (0 / 1) два вида маленького шрифта

#define GLOBAL_COLOR_1 CRGB::Green    // основной цвет №1 для игр
#define GLOBAL_COLOR_2 CRGB::Orange   // основной цвет №2 для игр
// поддерживает 150 цветов, названия можно посмотреть
// в библиотеке FastLED, файл pixeltypes.h, строка 590
// также цвет можно задавать в формате HEX (0XFF25AB)
// в любом онлайн колорпикере или в фотошопе!

#define USE_BUTTONS 0       // использовать физические кнопки управления играми (0 нет, 1 да)

#define BUTT_UP 3           // кнопка вверх
#define BUTT_DOWN 5         // кнопка вниз
#define BUTT_LEFT 2         // кнопка влево
#define BUTT_RIGHT 4        // кнопка вправо

// ******************************** ДЛЯ РАЗРАБОТЧИКОВ ********************************
#define DEBUG 0
#define NUM_LEDS WIDTH * HEIGHT

#include "FastLED.h"
CRGB leds[NUM_LEDS];
String runningText = "";

byte buttons = 4;   // 0 - верх, 1 - право, 2 - низ, 3 - лево, 4 - не нажата
byte globalBrightness = BRIGHTNESS;
byte globalSpeed = 200;
uint32_t globalColor = 0x00ff00;   // цвет при запуске зелёный
byte breathBrightness;
boolean loadingFlag;
byte frameNum;
boolean gameDemo = false;
boolean idleState = false;  // флаг холостого режима работы
boolean BTcontrol = false;  // флаг контроля с блютус. Если false - управление с кнопок

#include "GyverTimer.h"

#if (USE_FONTS == 1)
#include "fonts.h"
#endif

GTimer_ms effectTimer(D_EFFECT_SPEED);
GTimer_ms gameTimer(D_GAME_SPEED);
GTimer_ms scrollTimer(D_TEXT_SPEED);
GTimer_ms idleTimer(IDLE_TIME * 1000);

void setup() {
  Serial.begin(9600);

  // настройки ленты
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  randomSeed(analogRead(0) + analogRead(1));    // пинаем генератор случайных чисел
}

void loop() {
  customRoutine();
  bluetoothRoutine();
}
