// свой список режимов

// ************************ НАСТРОЙКИ ************************
#define SMOOTH_CHANGE 1     // плавная смена режимов через чёрный
#define SHOW_FULL_TEXT 1    // не переключать режим, пока текст не покажется весь
#define SHOW_TEXT_ONCE 1    // показывать бегущий текст только 1 раз

// подключаем внешние файлы с картинками
//#include "bitmap2.h"
#include "Screen.h"

/*
   Режимы:
    clockRoutine();       // часы на чёрном фоне

   Эффекты:
    sparklesRoutine();    // случайные цветные гаснущие вспышки
    snowRoutine();        // снег
    matrixRoutine();      // "матрица"
    starfallRoutine();    // звездопад (кометы)
    ballRoutine();        // квадратик
    ballsRoutine();       // шарики
    rainbowRoutine();     // радуга во всю матрицу
    fireRoutine();        // огонь
    analyzerRoutine();    //псевдоанализатор частот
    cyclonRoutine();      // циклон
    lightBallsRoutine();  // пейнтбол
    swirlRoutine();       // водоворот
    lightersRoutine();    // светлячки
    pacificaRoutine();    // прикольное безумие
    shadowsRoutine();     // тени
    paletteRoutine()      // палитра
    prizmataRoutine();    // синусы
    munchRoutine();       // вышиванка
    fire2Routine();       // огонь2 - камин
    arrowsRoutine();      // стрелки

  Крутые эффекты "шума":
    madnessNoise();       // цветной шум во всю матрицу
    cloudNoise();         // облака
    lavaNoise();          // лава
    plasmaNoise();        // плазма
    rainbowNoise();       // радужные переливы
    rainbowStripeNoise(); // полосатые радужные переливы
    zebraNoise();         // зебра
    forestNoise();        // шумящий лес
    oceanNoise();         // морская вода

  Игры:
    snakeRoutine();     // змейка
    tetrisRoutine();    // тетрис
    mazeRoutine();      // лабиринт
    runnerRoutine();    // бегалка прыгалка
    flappyRoutine();    // flappy bird
    arkanoidRoutine();  // арканоид
    lifeRoutine();      // жызнь

  Бегущая строка:
    fillString("Ваш текст", цвет);    // цвет вида 0x00ff25 или CRGB::Red и проч. цвета
    fillString("Ваш текст", 1);       // радужный перелив текста
    fillString("Ваш текст", 2);       // каждая буква случайным цветом!

  Рисунки и анимации:
    loadImage(<название массива>);    // основная функция вывода картинки
    imageRoutine1();                  // пример использования
    animation1();                     // пример анимации



*/

// не забудьте указать количество режимов для корректного переключения с последнего на первый
#define MODES_AMOUNT 42  // количество кастомных режимов (которые переключаются сами или кнопкой)

// ************************* СВОЙ СПИСОК РЕЖИМОВ ************************
// список можно менять, соблюдая его структуру. Можно удалять и добавлять эффекты, ставить их в
// любой последовательности или вообще оставить ОДИН. Удалив остальные case и break. Cтруктура оч простая:
// case <номер>: <эффект>;
//  break;

void customModes() {
  switch (thisMode) {
    case 0: fillString("С НАСТУПАЮЩИМ НОВЫМ ГОДОМ!!!", CRGB::Red);
      break;
    case 1: fillString("Гирлянда", 1);
      break;
    case 2: fillString("RGB LED", 2);
      break;
    case 3: madnessNoise();
      break;
    case 4: cloudNoise();
      break;
    case 5: lavaNoise();
      break;
    case 6: plasmaNoise();
      break;
    case 7: rainbowNoise();
      break;
    case 8: rainbowStripeNoise();
      break;
    case 9: zebraNoise();
      break;
    case 10: forestNoise();
      break;
    case 11: oceanNoise();
      break;
    case 12: snowRoutine();
      break;
    case 13: sparklesRoutine();
      break;
    case 14: matrixRoutine();
      break;
    case 15: starfallRoutine();
      break;
    case 16: ballRoutine();
      break;
    case 17: ballsRoutine();
      break;
    case 18: rainbowRoutine(0);
      break;
    case 19: cyclonRoutine();
      break;
    case 20: rainbowRoutine(1);
      break;
    case 21: clockRoutine();
      break;
    case 22: analyzerRoutine();
      break;
    case 23: lightBallsRoutine();
      break;
    case 24: swirlRoutine();
      break;
    case 25: lightersRoutine();
      break;
    case 26: pacificaRoutine();
      break;
    case 27: shadowsRoutine();
      break;
    case 28: paletteRoutine();
      break;
    case 29: prizmataRoutine();
      break;
    case 30: munchRoutine();
      break;
    case 31: rainRoutine();
      break;
    case 32: fire2Routine();
      break;
    case 33: rainbowRoutine(2);
      break;
    case 34: snakeRoutine();
      break;
    case 35: tetrisRoutine();
      break;
    case 36: runnerRoutine();
      break;
    case 37: arkanoidRoutine();
      break;
    case 38: lifeRoutine();
      break;
    case 39: imageRoutinePicahu();
      break;
    case 40: clockRoutine();
      break;
    case 41: rainbowRoutine(3);
      break;
  }

}


#define MC_CLOCK                 0         // Режим отображения часов (когда "Ночные часы" или просто "Часы" на черном фоне, а не как оверлей поверх других эффектов
#define MC_FILL_COLOR            1
#define MC_SNOW                  2
#define MC_BALL                  3
#define MC_RAINBOW               4
#define MC_PAINTBALL             5
#define MC_FIRE                  6
#define MC_MATRIX                7
#define MC_BALLS                 8
#define MC_STARFALL              9
#define MC_SPARKLES             10
#define MC_NOISE_MADNESS        11
#define MC_NOISE_CLOUD          12
#define MC_NOISE_LAVA           13
#define MC_NOISE_PLASMA         14
#define MC_NOISE_RAINBOW        15
#define MC_NOISE_RAINBOW_STRIP  16
#define MC_NOISE_ZEBRA          17
#define MC_NOISE_FOREST         18
#define MC_NOISE_OCEAN          19
#define MC_COLORS               20
#define MC_LIGHTERS             21
#define MC_SWIRL                22
#define MC_CYCLON               23
#define MC_FLICKER              24
#define MC_PACIFICA             25
#define MC_SHADOWS              26
#define MC_MAZE                 27
#define MC_SNAKE                28
#define MC_TETRIS               29
#define MC_ARKANOID             30
#define MC_PALETTE              31
#define MC_ANALYZER             32
#define MC_PRIZMATA             33
#define MC_MUNCH                34
#define MC_RAIN                 35
#define MC_FIRE2                36
#define MC_ARROWS               37
#define MC_LIFE                 38


// функция загрузки картинки в матрицу. должна быть здесь, иначе не работает =)
void loadImage(uint16_t (*frame)[8]) {
  for (byte i = 0; i < 8; i++)   
    for (byte j = 0; j < 8; j++)
      drawPixelXY(i, j, gammaCorrection(expandColor((pgm_read_word(&(frame[8 - j - 1][i]))))));
  // да, тут происходит лютенький копец, а именно:
  // 1) pgm_read_word - восстанавливаем из PROGMEM (флэш памяти) цвет пикселя в 16 битном формате по его координатам
  // 2) expandColor - расширяем цвет до 24 бит (спасибо adafruit)
  // 3) gammaCorrection - проводим коррекцию цвета для более корректного отображения
}
timerMinim gifTimer(4000);

// ********************** ПРИМЕРЫ ВЫВОДА КАРТИНОК ***********************

// Внимание! Если размер матрицы не совпадает с исходным размером матрицы в скетче
// (если вы только что  его скачали), то нужно удалить/закомментировать данные функции!
//

// показать картинку
void imageRoutinePicahu() {
  if (loadingFlag) {
    loadingFlag = false;
    loadImage(frame99);
  }
}


void releaseEffectResources(uint8_t aMode) {  
  switch (aMode) {
    case 28:             paletteRoutineRelease(); break;
    case 0:            analyzerRoutineRelease(); break;
    case 32:               fire2RoutineRelease(); break;
    case 43:                lifeRoutineRelease(); break;
  }
}

// ********************* ОСНОВНОЙ ЦИКЛ РЕЖИМОВ *******************
#if (SMOOTH_CHANGE == 1)
byte fadeMode = 4;
boolean modeDir;
#endif

static void nextMode() {
#if (SMOOTH_CHANGE == 1)
  fadeMode = 0;
  modeDir = true;
#else
  nextModeHandler();
#endif
}
static void prevMode() {
#if (SMOOTH_CHANGE == 1)
  fadeMode = 0;
  modeDir = false;
#else
  prevModeHandler();
#endif
}
void nextModeHandler() {
  releaseEffectResources(thisMode);
  thisMode++;
  if (thisMode >= MODES_AMOUNT) thisMode = 0;
  loadingFlag = true;
  gamemodeFlag = false;
  FastLED.clear();
  FastLED.show();
}
void prevModeHandler() {
  releaseEffectResources(thisMode);
  thisMode--;
  if (thisMode < 0) thisMode = MODES_AMOUNT - 1;
  loadingFlag = true;
  gamemodeFlag = false;
  FastLED.clear();
  FastLED.show();
}

int fadeBrightness;
#if (SMOOTH_CHANGE == 1)
void modeFader() {
  if (fadeMode == 0) {
    fadeMode = 1;
  } else if (fadeMode == 1) {
    if (changeTimer.isReady()) {
      fadeBrightness -= 40;
      if (fadeBrightness < 0) {
        fadeBrightness = 0;
        fadeMode = 2;
      }
      FastLED.setBrightness(fadeBrightness);
    }
  } else if (fadeMode == 2) {
    fadeMode = 3;
    if (modeDir) nextModeHandler();
    else prevModeHandler();
  } else if (fadeMode == 3) {
    if (changeTimer.isReady()) {
      fadeBrightness += 40;
      if (fadeBrightness > globalBrightness) {
        fadeBrightness = globalBrightness;
        fadeMode = 4;
      }
      FastLED.setBrightness(fadeBrightness);
    }
  }
}
#endif

boolean loadFlag2;
void customRoutine() {
  if (!BTcontrol) {
    if (!gamemodeFlag) {
      if (effectTimer.isReady()) {
#if (OVERLAY_CLOCK == 1 && USE_CLOCK == 1)
        if (overlayAllowed()) {
          if (!loadingFlag && !gamemodeFlag && needUnwrap() && modeCode != 0) clockOverlayUnwrap(CLOCK_X, CLOCK_Y);
          if (loadingFlag) loadFlag2 = true;
        }
#endif

        customModes();                // режимы крутятся, пиксели мутятся

#if (OVERLAY_CLOCK == 1 && USE_CLOCK == 1)
        if (overlayAllowed()) {
          if (!gamemodeFlag && modeCode != 0) clockOverlayWrap(CLOCK_X, CLOCK_Y);
          if (loadFlag2) {
            setOverlayColors();
            loadFlag2 = false;
          }
        }
#endif
        loadingFlag = false;
        FastLED.show();
      }
    } else {
      customModes();
    }
    btnsModeChange();
#if (SMOOTH_CHANGE == 1)
    modeFader();
#endif
  }

  if (idleState) {
    if (fullTextFlag && SHOW_TEXT_ONCE) {
      fullTextFlag = false;
      autoplayTimer = millis();
      nextMode();
    }
    if (millis() - autoplayTimer > autoplayTime && AUTOPLAY) {    // таймер смены режима
      if (modeCode == 0 && SHOW_FULL_TEXT) {    // режим текста
        if (fullTextFlag) {
          fullTextFlag = false;
          autoplayTimer = millis();
          nextMode();
        }
      } else {
        autoplayTimer = millis();
        nextMode();
      }
    }
  } else {
    if (idleTimer.isReady()) {      // таймер холостого режима
      idleState = true;
      autoplayTimer = millis();
      gameDemo = true;

      gameSpeed = DEMO_GAME_SPEED;
      gameTimer.setInterval(gameSpeed);

      loadingFlag = true;
      BTcontrol = false;
      FastLED.clear();
      FastLED.show();
    }
  }
}

void timeSet(boolean type, boolean dir) {    // type: 0-часы, 1-минуты, dir: 0-уменьшить, 1-увеличить
  if (type) {
    if (dir) hrs++;
    else hrs--;
  } else {
    if (dir) mins++;
    else mins--;
    if (mins > 59) {
      mins = 0;
      hrs++;
    }
    if (mins < 0) {
      mins = 59;
      hrs--;
    }
  }
  if (hrs > 23) hrs = 0;
  if (hrs < 0) hrs = 23;
}

void btnsModeChange() {
#if (USE_BUTTONS == 1)
  if (bt_set.clicked()) {
    if (gamemodeFlag) gameDemo = !gameDemo;
    if (gameDemo) {
      gameSpeed = DEMO_GAME_SPEED;
      gameTimer.setInterval(gameSpeed);
      AUTOPLAY = true;
    } else {
      gameSpeed = D_GAME_SPEED;
      gameTimer.setInterval(gameSpeed);
      AUTOPLAY = false;
    }
  }
  if (bt_set.holded()) {
    if (modeCode == 2)
      mazeMode = !mazeMode;
    if (modeCode == 1) {    // вход в настройку часов
      clockSet = !clockSet;
      AUTOPLAY = false;
      secs = 0;
#if (USE_CLOCK == 1)
      if (!clockSet) rtc.adjust(DateTime(2014, 1, 21, hrs, mins, 0)); // установка нового времени в RTC
#endif
    }
  }

  // timeSet type: 0-часы, 1-минуты, dir: 0-уменьшить, 1-увеличить

  if (gameDemo) {
    if (bt_right.clicked()) {
      if (!clockSet) {
        autoplayTimer = millis();
        nextMode();
      } else {
        timeSet(1, 1);
      }
    }

    if (bt_left.clicked()) {
      if (!clockSet) {
        autoplayTimer = millis();
        prevMode();
      } else {
        timeSet(1, 0);
      }
    }

    if (bt_up.clicked()) {
      if (!clockSet) {
        AUTOPLAY = true;
        autoplayTimer = millis();
      } else {
        timeSet(0, 1);
      }
    }
    if (bt_down.clicked()) {
      if (!clockSet) {
        AUTOPLAY = false;
      } else {
        timeSet(0, 0);
      }
    }

    if (bt_right.holding())
      if (changeTimer.isReady()) {
        if (!clockSet) {
          effects_speed -= 2;
          if (effects_speed < 30) effects_speed = 30;
          effectTimer.setInterval(effects_speed);
        } else {
          timeSet(1, 1);
        }
      }
    if (bt_left.holding())
      if (changeTimer.isReady()) {
        if (!clockSet) {
          effects_speed += 2;
          if (effects_speed > 300) effects_speed = 300;
          effectTimer.setInterval(effects_speed);
        } else {
          timeSet(1, 0);
        }
      }
    if (bt_up.holding())
      if (changeTimer.isReady()) {
        if (!clockSet) {
          globalBrightness += 2;
          if (globalBrightness > 255) globalBrightness = 255;
          fadeBrightness = globalBrightness;
          FastLED.setBrightness(globalBrightness);
        } else {
          timeSet(0, 1);
        }
      }
    if (bt_down.holding())
      if (changeTimer.isReady()) {
        if (!clockSet) {
          globalBrightness -= 2;
          if (globalBrightness < 0) globalBrightness = 0;
          fadeBrightness = globalBrightness;
          FastLED.setBrightness(globalBrightness);
        } else {
          timeSet(0, 0);
        }
      }
  }
#endif
}
