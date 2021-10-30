// эффекты

// **************** НАСТРОЙКИ ЭФФЕКТОВ ****************
// эффект "синусоиды" - ОТКЛЮЧЕН
#define WAVES_AMOUNT 2    // количество синусоид

// эффект "шарики"
#define BALLS_AMOUNT 3    // количество "шариков"
#define CLEAR_PATH 1      // очищать путь
#define BALL_TRACK 1      // (0 / 1) - вкл/выкл следы шариков
#define DRAW_WALLS 0      // режим с рисованием препятствий для шаров (не работает на ESP и STM32)
#define TRACK_STEP 70     // длина хвоста шарика (чем больше цифра, тем хвост короче)

// эффект "квадратик"
#define BALL_SIZE 3       // размер шара
#define RANDOM_COLOR 1    // случайный цвет при отскоке

// эффект "огонь"
#define SPARKLES 1        // вылетающие угольки вкл выкл
#define HUE_ADD 0         // добавка цвета в огонь (от 0 до 230) - меняет весь цвет пламени

// эффект "кометы"
#define TAIL_STEP 100     // длина хвоста кометы
#define SATURATION 150    // насыщенность кометы (от 0 до 255)
#define STAR_DENSE 60     // количество (шанс появления) комет

// эффект "конфетти"
#define DENSE 3           // плотность конфетти
#define BRIGHT_STEP 70    // шаг уменьшения яркости

// эффект "снег"
#define SNOW_DENSE 10     // плотность снегопада

// --------------------- ДЛЯ РАЗРАБОТЧИКОВ ----------------------

// *********** "дыхание" яркостью ***********
boolean brightnessDirection;
void brightnessRoutine() {
  if (brightnessDirection) {
    breathBrightness += 2;
    if (breathBrightness > globalBrightness - 1) {
      brightnessDirection = false;
    }
  } else {
    breathBrightness -= 2;
    if (breathBrightness < 1) {
      brightnessDirection = true;
    }
  }
  FastLED.setBrightness(breathBrightness);
}

// *********** смена цвета активных светодиодов (рисунка) ***********
byte hue;
void colorsRoutine() {
  hue += 4;
  for (int i = 0; i < NUM_LEDS; i++) {
    if (getPixColor(i) > 0) leds[i] = CHSV(hue, 255, 255);
  }
}

// *********** снегопад 2.0 ***********
void snowRoutine() {
  modeCode = 12;
  // сдвигаем всё вниз
  for (byte x = 0; x < WIDTH; x++) {
    for (byte y = 0; y < HEIGHT - 1; y++) {
      drawPixelXY(x, y, getPixColorXY(x, y + 1));
    }
  }

  for (byte x = 0; x < WIDTH; x++) {
    // заполняем случайно верхнюю строку
    // а также не даём двум блокам по вертикали вместе быть
    if (getPixColorXY(x, HEIGHT - 2) == 0 && (random(0, SNOW_DENSE) == 0))
      drawPixelXY(x, HEIGHT - 1, 0xE0FFFF - 0x101010 * random(0, 4));
    else
      drawPixelXY(x, HEIGHT - 1, 0x000000);
  }
}

// ***************************** БЛУДНЫЙ КУБИК *****************************
int coordB[2];
int8_t vectorB[2];
CRGB ballColor;

void ballRoutine() {
  if (loadingFlag) {
    for (byte i = 0; i < 2; i++) {
      coordB[i] = WIDTH / 2 * 10;
      vectorB[i] = random(8, 20);
      ballColor = CHSV(random(0, 9) * 28, 255, 255);
    }
    modeCode = 16;
    loadingFlag = false;
  }
  for (byte i = 0; i < 2; i++) {
    coordB[i] += vectorB[i];
    if (coordB[i] < 0) {
      coordB[i] = 0;
      vectorB[i] = -vectorB[i];
      if (RANDOM_COLOR) ballColor = CHSV(random(0, 9) * 28, 255, 255);
      //vectorB[i] += random(0, 6) - 3;
    }
  }
  if (coordB[0] > (WIDTH - BALL_SIZE) * 10) {
    coordB[0] = (WIDTH - BALL_SIZE) * 10;
    vectorB[0] = -vectorB[0];
    if (RANDOM_COLOR) ballColor = CHSV(random(0, 9) * 28, 255, 255);
    //vectorB[0] += random(0, 6) - 3;
  }
  if (coordB[1] > (HEIGHT - BALL_SIZE) * 10) {
    coordB[1] = (HEIGHT - BALL_SIZE) * 10;
    vectorB[1] = -vectorB[1];
    if (RANDOM_COLOR) ballColor = CHSV(random(0, 9) * 28, 255, 255);
    //vectorB[1] += random(0, 6) - 3;
  }
  FastLED.clear();
  for (byte i = 0; i < BALL_SIZE; i++)
    for (byte j = 0; j < BALL_SIZE; j++)
      leds[getPixelNumber(coordB[0] / 10 + i, coordB[1] / 10 + j)] = ballColor;
}

// *********** радуга активных светодиодов (рисунка) ***********
void rainbowColorsRoutine() {
  hue++;
  for (byte i = 0; i < WIDTH; i++) {
    CHSV thisColor = CHSV((byte)(hue + i * float(255 / WIDTH)), 255, 255);
    for (byte j = 0; j < HEIGHT; j++)
      if (getPixColor(getPixelNumber(i, j)) > 0) drawPixelXY(i, j, thisColor);
  }
}

// ***************************** РАДУГА *****************************

uint8_t rainbow_type = 0;

void rainbowRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    // modeCode = MC_RAINBOW;
    rainbow_type = (specialTextEffectParam >= 0) ? specialTextEffectParam : effectScaleParam2[MC_RAINBOW];
    // Если авто - генерировать один из типов - 1-Вертикальная радуга, 2-Горизонтальная радуга, 3-Диагональная радуга, 4-Вращающаяся радуга
    if (rainbow_type == 0 || rainbow_type > 4) {
      rainbow_type = random8(1,4);
    }     
    FastLED.clear();  // очистить
  }

  switch (rainbow_type) {
    case 1:  rainbowVertical(); break;
    case 2:  rainbowHorizontal(); break;
    case 3:  rainbowDiagonal(); break;
    default: rainbowRotate(); break;
  }
}

// *********** радуга дигональная ***********

void rainbowDiagonal() {
  uint8_t effectBrightness = getBrightnessCalculated(globalBrightness, effectContrast[thisMode]);
  hue += 2;
  for (uint8_t x = 0; x < WIDTH; x++) {
    for (uint8_t y = 0; y < HEIGHT; y++) {
      float dx = (WIDTH >= HEIGHT)
         ? (float)(WIDTH / HEIGHT * x + y)
         : (float)(HEIGHT / WIDTH * y + x);
      CRGB thisColor = CHSV((uint8_t)(hue + dx * (float)(255 / maxDim)), 255, effectBrightness);
      drawPixelXY(x, y, thisColor); 
    }
  }
}

// *********** радуга горизонтальная ***********

void rainbowHorizontal() {
  uint8_t effectBrightness = getBrightnessCalculated(globalBrightness, effectContrast[thisMode]);
  hue += 2;
  for (uint8_t j = 0; j < HEIGHT; j++) {
    CHSV thisColor = CHSV((uint8_t)(hue + j * map8(getEffectScaleParamValue(MC_RAINBOW),1,WIDTH)), 255, effectBrightness);
    for (uint8_t i = 0; i < WIDTH; i++)
      drawPixelXY(i, j, thisColor);
  }
}

// *********** радуга вертикальная ***********

void rainbowVertical() {
  uint8_t effectBrightness = getBrightnessCalculated(globalBrightness, effectContrast[thisMode]);
  hue += 2;
  for (uint8_t i = 0; i < WIDTH; i++) {
    CHSV thisColor = CHSV((uint8_t)(hue + i * map8(getEffectScaleParamValue(MC_RAINBOW),1,HEIGHT)), 255, effectBrightness);
    for (uint8_t j = 0; j < HEIGHT; j++)
      drawPixelXY(i, j, thisColor);
  }
}

// *********** радуга вращающаяся ***********

void rainbowRotate() {
  uint32_t ms = millis();
  int32_t  yHueDelta32 = ((int32_t)cos16( ms * (27/1) ) * (350 / WIDTH));
  int32_t  xHueDelta32 = ((int32_t)cos16( ms * (39/1) ) * (310 / HEIGHT));

  uint8_t  lineStartHue = ms / 65536;
  int8_t   yHueDelta8   = yHueDelta32 / 32768;
  int8_t   xHueDelta8   = xHueDelta32 / 32768;
  
  uint8_t effectBrightness = getBrightnessCalculated(globalBrightness, effectContrast[thisMode]);

  for(uint8_t y = 0; y < HEIGHT; y++) {
    lineStartHue += yHueDelta8;
    uint8_t pixelHue = lineStartHue;      
    for(uint8_t x = 0; x < WIDTH; x++) {
      pixelHue += xHueDelta8;
      leds[ XY(x, y)]  = CHSV( pixelHue, 255, effectBrightness);
    }
  }
}

// ********************** огонь **********************
unsigned char matrixValue[8][16];
uint8_t *line;
int pcnt = 0;

//these values are substracetd from the generated values to give a shape to the animation
const unsigned char valueMask[8][16] PROGMEM = {
  {32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 , 32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 },
  {64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 , 64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 },
  {96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 , 96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 },
  {128, 64 , 32 , 0  , 0  , 32 , 64 , 128, 128, 64 , 32 , 0  , 0  , 32 , 64 , 128},
  {160, 96 , 64 , 32 , 32 , 64 , 96 , 160, 160, 96 , 64 , 32 , 32 , 64 , 96 , 160},
  {192, 128, 96 , 64 , 64 , 96 , 128, 192, 192, 128, 96 , 64 , 64 , 96 , 128, 192},
  {255, 160, 128, 96 , 96 , 128, 160, 255, 255, 160, 128, 96 , 96 , 128, 160, 255},
  {255, 192, 160, 128, 128, 160, 192, 255, 255, 192, 160, 128, 128, 160, 192, 255}
};

//these are the hues for the fire,
//should be between 0 (red) to about 25 (yellow)
const unsigned char hueMask[8][16] PROGMEM = {
  {1 , 11, 19, 25, 25, 22, 11, 1 , 1 , 11, 19, 25, 25, 22, 11, 1 },
  {1 , 8 , 13, 19, 25, 19, 8 , 1 , 1 , 8 , 13, 19, 25, 19, 8 , 1 },
  {1 , 8 , 13, 16, 19, 16, 8 , 1 , 1 , 8 , 13, 16, 19, 16, 8 , 1 },
  {1 , 5 , 11, 13, 13, 13, 5 , 1 , 1 , 5 , 11, 13, 13, 13, 5 , 1 },
  {1 , 5 , 11, 11, 11, 11, 5 , 1 , 1 , 5 , 11, 11, 11, 11, 5 , 1 },
  {0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 , 0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 },
  {0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 , 0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 },
  {0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 }
};

void fireRoutine() {
  if (loadingFlag) {
    modeCode = 20;
    loadingFlag = false;
    FastLED.clear();
    generateLine();
    memset(matrixValue, 0, sizeof(matrixValue));
  }
  if (pcnt >= 100) {
    shiftUp();
    generateLine();
    pcnt = 0;
  }
  drawFrame(pcnt);
  pcnt += 30;
}


// Randomly generate the next line (matrix row)

void generateLine() {
  for (uint8_t x = 0; x < WIDTH; x++) {
    line[x] = random(64, 255);
  }
}

//shift all values in the matrix up one row

void shiftUp() {
  for (uint8_t y = HEIGHT - 1; y > 0; y--) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      uint8_t newX = x;
      if (x > 15) newX = x - 15;
      if (y > 7) continue;
      matrixValue[y][newX] = matrixValue[y - 1][newX];
    }
  }

  for (uint8_t x = 0; x < WIDTH; x++) {
    uint8_t newX = x;
    if (x > 15) newX = x - 15;
    matrixValue[0][newX] = line[newX];
  }
}

// draw a frame, interpolating between 2 "key frames"
// @param pcnt percentage of interpolation

void drawFrame(int pcnt) {
  int nextv;

  //each row interpolates with the one before it
  for (unsigned char y = HEIGHT - 1; y > 0; y--) {
    for (unsigned char x = 0; x < WIDTH; x++) {
      uint8_t newX = x;
      if (x > 15) newX = x - 15;
      if (y < 8) {
        nextv =
          (((100.0 - pcnt) * matrixValue[y][newX]
            + pcnt * matrixValue[y - 1][newX]) / 100.0)
          - pgm_read_byte(&(valueMask[y][newX]));

        CRGB color = CHSV(
                       HUE_ADD + pgm_read_byte(&(hueMask[y][newX])), // H
                       255, // S
                       (uint8_t)max(0, nextv) // V
                     );

        leds[getPixelNumber(x, y)] = color;
      } else if (y == 8 && SPARKLES) {
        if (random(0, 20) == 0 && getPixColorXY(x, y - 1) != 0) drawPixelXY(x, y, getPixColorXY(x, y - 1));
        else drawPixelXY(x, y, 0);
      } else if (SPARKLES) {

        // старая версия для яркости
        if (getPixColorXY(x, y - 1) > 0)
          drawPixelXY(x, y, getPixColorXY(x, y - 1));
        else drawPixelXY(x, y, 0);

      }
    }
  }

  //first row interpolates with the "next" line
  for (unsigned char x = 0; x < WIDTH; x++) {
    uint8_t newX = x;
    if (x > 15) newX = x - 15;
    CRGB color = CHSV(
                   HUE_ADD + pgm_read_byte(&(hueMask[0][newX])), // H
                   255,           // S
                   (uint8_t)(((100.0 - pcnt) * matrixValue[0][newX] + pcnt * line[newX]) / 100.0) // V
                 );
    leds[getPixelNumber(newX, 0)] = color;
  }
}

// **************** МАТРИЦА *****************
void matrixRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    modeCode = 14;
    FastLED.clear();
  }
  for (byte x = 0; x < WIDTH; x++) {
    // заполняем случайно верхнюю строку
    uint32_t thisColor = getPixColorXY(x, HEIGHT - 1);
    if (thisColor == 0)
      drawPixelXY(x, HEIGHT - 1, 0x00FF00 * (random(0, 10) == 0));
    else if (thisColor < 0x002000)
      drawPixelXY(x, HEIGHT - 1, 0);
    else
      drawPixelXY(x, HEIGHT - 1, thisColor - 0x002000);
  }

  // сдвигаем всё вниз
  for (byte x = 0; x < WIDTH; x++) {
    for (byte y = 0; y < HEIGHT - 1; y++) {
      drawPixelXY(x, y, getPixColorXY(x, y + 1));
    }
  }
}


// ********************************* ШАРИКИ *********************************
int coord[BALLS_AMOUNT][2];
int8_t vector[BALLS_AMOUNT][2];
CRGB ballColors[BALLS_AMOUNT];

void ballsRoutine() {
  if (loadingFlag) {
    modeCode = 17;
    loadingFlag = false;
    for (byte j = 0; j < BALLS_AMOUNT; j++) {
      int sign;

      // забиваем случайными данными
      coord[j][0] = WIDTH / 2 * 10;
      random(0, 2) ? sign = 1 : sign = -1;
      vector[j][0] = random(4, 15) * sign;
      coord[j][1] = HEIGHT / 2 * 10;
      random(0, 2) ? sign = 1 : sign = -1;
      vector[j][1] = random(4, 15) * sign;
      ballColors[j] = CHSV(random(0, 9) * 28, 255, 255);
    }
  }


  if (!BALL_TRACK)    // если режим БЕЗ следов шариков
    FastLED.clear();  // очистить
  else {              // режим со следами
    fader(TRACK_STEP);
  }

  // движение шариков
  for (byte j = 0; j < BALLS_AMOUNT; j++) {

    // отскок от нарисованных препятствий
    if (DRAW_WALLS) {
      uint32_t thisColor = getPixColorXY(coord[j][0] / 10 + 1, coord[j][1] / 10);
      if (thisColor == globalColor/* && vector[j][0] > 0*/) {
        vector[j][0] = -vector[j][0];
      }
      thisColor = getPixColorXY(coord[j][0] / 10 - 1, coord[j][1] / 10);
      if (thisColor == globalColor/* && vector[j][0] < 0*/) {
        vector[j][0] = -vector[j][0];
      }
      thisColor = getPixColorXY(coord[j][0] / 10, coord[j][1] / 10 + 1);
      if (thisColor == globalColor/* && vector[j][1] > 0*/) {
        vector[j][1] = -vector[j][1];
      }
      thisColor = getPixColorXY(coord[j][0] / 10, coord[j][1] / 10 - 1);
      if (thisColor == globalColor/* && vector[j][1] < 0*/) {
        vector[j][1] = -vector[j][1];
      }
    }

    // движение шариков
    for (byte i = 0; i < 2; i++) {
      coord[j][i] += vector[j][i];
      if (coord[j][i] < 0) {
        coord[j][i] = 0;
        vector[j][i] = -vector[j][i];
      }
    }

    if (coord[j][0] > (WIDTH - 1) * 10) {
      coord[j][0] = (WIDTH - 1) * 10;
      vector[j][0] = -vector[j][0];
    }
    if (coord[j][1] > (HEIGHT - 1) * 10) {
      coord[j][1] = (HEIGHT - 1) * 10;
      vector[j][1] = -vector[j][1];
    }
    leds[getPixelNumber(coord[j][0] / 10, coord[j][1] / 10)] =  ballColors[j];
  }
}


/*
  // ******************************** СИНУСОИДЫ *******************************
  #define DEG_TO_RAD 0.017453
  int t;
  byte w[WAVES_AMOUNT];
  byte phi[WAVES_AMOUNT];
  byte A[WAVES_AMOUNT];
  CRGB waveColors[WAVES_AMOUNT];

  void wavesRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    for (byte j = 0; j < WAVES_AMOUNT; j++) {
      // забиваем случайными данными
      w[j] = random(17, 25);
      phi[j] = random(0, 360);
      A[j] = HEIGHT / 2 * random(4, 11) / 10;
      waveColors[j] = CHSV(random(0, 9) * 28, 255, 255);
    }
  }
  if (effectTimer.isReady()) {

    // сдвигаем все пиксели вправо
    for (int i = WIDTH - 1; i > 0; i--)
      for (int j = 0; j < HEIGHT; j++)
        drawPixelXY(i, j, getPixColorXY(i - 1, j));

    // увеличиваем "угол"
    t++;
    if (t > 360) t = 0;

    // заливаем чёрным левую линию
    for (byte i = 0; i < HEIGHT; i++) {
      drawPixelXY(0, i, 0x000000);
    }

    // генерируем позицию точки через синус
    for (byte j = 0; j < WAVES_AMOUNT; j++) {
      float value = HEIGHT / 2 + (float)A[j] * sin((float)w[j] * t * DEG_TO_RAD + (float)phi[j] * DEG_TO_RAD);
      leds[getPixelNumber(0, (byte)value)] = waveColors[j];
    }
  }
  }
*/

// функция плавного угасания цвета для всех пикселей
void fader(byte step) {
  for (byte i = 0; i < WIDTH; i++) {
    for (byte j = 0; j < HEIGHT; j++) {
      fadePixel(i, j, step);
    }
  }
}
void fadePixel(byte i, byte j, byte step) {     // новый фейдер
  int pixelNum = getPixelNumber(i, j);
  if (getPixColor(pixelNum) == 0) return;

  if (leds[pixelNum].r >= 30 ||
      leds[pixelNum].g >= 30 ||
      leds[pixelNum].b >= 30) {
    leds[pixelNum].fadeToBlackBy(step);
  } else {
    leds[pixelNum] = 0;
  }
}

/*
  void fadePixel(byte i, byte j, byte step) {     // старый фейдер
  // измеряяем цвет текущего пикселя
  uint32_t thisColor = getPixColorXY(i, j);

  // если 0, то пропускаем действия и переходим к следующему
  if (thisColor == 0) return;

  // разбиваем цвет на составляющие RGB
  byte rgb[3];
  rgb[0] = (thisColor >> 16) & 0xff;
  rgb[1] = (thisColor >> 8) & 0xff;
  rgb[2] = thisColor & 0xff;

  // ищем максимум
  byte maximum = max(max(rgb[0], rgb[1]), rgb[2]);
  float coef = 0;

  // если есть возможность уменьшить
  if (maximum >= step)
    // уменьшаем и находим коэффициент уменьшения
    coef = (float)(maximum - step) / maximum;

  // далее все цвета умножаем на этот коэффициент
  for (byte i = 0; i < 3; i++) {
    if (rgb[i] > 0) rgb[i] = (float)rgb[i] * coef;
    else rgb[i] = 0;
  }
  leds[getPixelNumber(i, j)] = CRGB(rgb[0], rgb[1], rgb[2]);
  }
*/

// ********************* ЗВЕЗДОПАД ******************
void starfallRoutine() {
  modeCode = 15;
  // заполняем головами комет левую и верхнюю линию
  for (byte i = HEIGHT / 2; i < HEIGHT; i++) {
    if (getPixColorXY(0, i) == 0
        && (random(0, STAR_DENSE) == 0)
        && getPixColorXY(0, i + 1) == 0
        && getPixColorXY(0, i - 1) == 0)
      leds[getPixelNumber(0, i)] = CHSV(random(0, 200), SATURATION, 255);
  }
  for (byte i = 0; i < WIDTH / 2; i++) {
    if (getPixColorXY(i, HEIGHT - 1) == 0
        && (random(0, STAR_DENSE) == 0)
        && getPixColorXY(i + 1, HEIGHT - 1) == 0
        && getPixColorXY(i - 1, HEIGHT - 1) == 0)
      leds[getPixelNumber(i, HEIGHT - 1)] = CHSV(random(0, 200), SATURATION, 255);
  }

  // сдвигаем по диагонали
  for (byte y = 0; y < HEIGHT - 1; y++) {
    for (byte x = WIDTH - 1; x > 0; x--) {
      drawPixelXY(x, y, getPixColorXY(x - 1, y + 1));
    }
  }

  // уменьшаем яркость левой и верхней линии, формируем "хвосты"
  for (byte i = HEIGHT / 2; i < HEIGHT; i++) {
    fadePixel(0, i, TAIL_STEP);
  }
  for (byte i = 0; i < WIDTH / 2; i++) {
    fadePixel(i, HEIGHT - 1, TAIL_STEP);
  }
}

// рандомные гаснущие вспышки
void sparklesRoutine() {
  modeCode = 13;
  for (byte i = 0; i < DENSE; i++) {
    byte x = random(0, WIDTH);
    byte y = random(0, HEIGHT);
    if (getPixColorXY(x, y) == 0)
      leds[getPixelNumber(x, y)] = CHSV(random(0, 255), 255, 255);
  }
  fader(BRIGHT_STEP);
}


// ****************************** ANALYZER *****************************

// цвета высоты полос спектра.
#define COLOR1    HUE_GREEN
#define COLOR2    HUE_YELLOW
#define COLOR3    HUE_ORANGE
#define COLOR4    HUE_RED
#define MAX_COLOR HUE_RED // цвет точек максимума

// анимация
#define SMOOTH 0.3        // плавность движения столбиков (0 - 1)

// точки максимума
#define MAX_DOTS 1        // включить/выключить отрисовку точек максимума (1 вкл, 0 выкл)
#define FALL_DELAY 50     // скорость падения точек максимума (задержка, миллисекунды)
#define FALL_PAUSE 700    // пауза перед падением точек максимума, миллисекунды

uint32_t gainTimer, fallTimer;
uint8_t  maxValue;
bool     fallFlag;

uint32_t *timeLevel;
uint8_t  *posOffset;   // Массив данных для отображения на матрице
int16_t  *maxLevel;
uint8_t  *posLevel_old;

uint8_t st = 0;
uint8_t phase = 0;          // фаза эффекта
// -------------------------------------------------------------------------------------
void analyzerRoutine(){

  static int16_t MAX_LEVEL = (HEIGHT + HEIGHT / 4);
  static uint8_t SIN_WIDTH = (WIDTH / 8);

  if (loadingFlag) {
    // modeCode = MC_ANALYZER;
    loadingFlag = false;

    if (timeLevel    == NULL) { timeLevel    = new uint32_t[WIDTH]; }
    if (posOffset    == NULL) { posOffset    = new uint8_t[WIDTH]; }
    if (maxLevel     == NULL) { maxLevel     = new int16_t[WIDTH]; }
    if (posLevel_old == NULL) { posLevel_old = new uint8_t[WIDTH]; }
    
    for (uint8_t i = 0; i < WIDTH; i++) {
      maxLevel[i] = 0;
      posLevel_old[i] = 0;
    }

    st = 0;
    phase = 0;
    FastLED.clear();
  }
  
  if (phase == 0) {
    // Движение волны слева направо
    for (uint8_t i = 0; i < WIDTH; i++) {
      posOffset[i] = (i < st || i >= st + SIN_WIDTH - (SIN_WIDTH / 4))
        ? 0
        : map8(sin8(map(i, st,st + SIN_WIDTH, 0,255)), 1, HEIGHT + HEIGHT / 2);
    }
  } else 

  if (phase == 2) {
    // Движение волны справа налево
    for (uint8_t i = 0; i < WIDTH; i++) {
        posOffset[i] = (i < WIDTH - st || i > WIDTH - st + SIN_WIDTH)
          ? 0
          : map8(sin8(map(i, WIDTH - st, WIDTH - st + SIN_WIDTH, 0, 255)), 1, HEIGHT + HEIGHT / 2);
    }
  } else

  if (phase == 1 || phase == 3) {
    // Пауза, даем "отстояться" пикам
    for (uint8_t i = 0; i < WIDTH; i++) {
      posOffset[i] = 0;
    }    
  } else
  
  if (phase >= 4) {
    // Случайные двиижения - "музыка"
    for (uint8_t i = 0; i < WIDTH; i++) {
      posOffset[i] = random8(1,MAX_LEVEL);    
    }
  }

  st++;
  if (st >= WIDTH && phase < 4) {    
    phase++;
    st = phase % 2 == 1 ? WIDTH / 2 : 0;
  }
    
  maxValue = 0;
  FastLED.clear();  // очистить матрицу
  
  for (uint8_t pos = 0; pos < WIDTH; pos++) {    // для каждого столбца матрицы
    uint8_t posLevel = posOffset[pos];

    // найти максимум из пачки тонов
    if (posLevel > maxValue) maxValue = posLevel;

    // фильтрация длины столбиков, для их плавного движения
    posLevel = posLevel * SMOOTH + posLevel_old[pos] * (1 - SMOOTH);
    posLevel_old[pos] = posLevel;

    // преобразовать значение величины спектра в диапазон 0..HEIGHT с учётом настроек
    posLevel = constrain(posLevel, 1, HEIGHT - 1);

    if (posLevel > 0) {
      for (uint8_t j = 0; j < posLevel; j++) {
        CHSV color;
        if      (j < map( 5, 0,16, 0,HEIGHT)) color = CHSV(COLOR1, 255, globalBrightness);
        else if (j < map(10, 0,16, 0,HEIGHT)) color = CHSV(COLOR2, 255, globalBrightness);
        else if (j < map(13, 0,16, 0,HEIGHT)) color = CHSV(COLOR3, 255, globalBrightness);
        else if (j < map(15, 0,16, 0,HEIGHT)) color = CHSV(COLOR4, 255, globalBrightness);

        drawPixelXY(pos, j, color);
      }
    }

    if (posLevel > 0 && posLevel > maxLevel[pos]) {    // если для этой полосы есть максимум, который больше предыдущего
      maxLevel[pos] = posLevel;                        // запомнить его
      timeLevel[pos] = millis();                       // запомнить время
    }

    // если точка максимума выше нуля (или равна ему) - включить пиксель
    if (maxLevel[pos] >= 0 && MAX_DOTS) {
      drawPixelXY(pos, maxLevel[pos], CHSV(MAX_COLOR, 255, globalBrightness));
    }

    if (fallFlag) {                                           // если падаем на шаг
      if ((uint32_t)millis() - timeLevel[pos] > FALL_PAUSE) {     // если максимум держался на своей высоте дольше FALL_PAUSE
        if (maxLevel[pos] >= 0) maxLevel[pos]--;              // уменьшить высоту точки на 1
        // внимание! Принимает минимальное значение -1 !
      }
    }
  }

  fallFlag = 0;                                 // сбросить флаг падения
  if (millis() - fallTimer > FALL_DELAY) {      // если настало время следующего падения
    fallFlag = 1;                               // поднять флаг
    fallTimer = millis();
  }
}

void analyzerRoutineRelease() {
  if (posLevel_old != NULL) { delete[] posLevel_old; posLevel_old = NULL; }
  if (maxLevel != NULL)     { delete[] maxLevel;     maxLevel = NULL; }
  if (posOffset != NULL)    { delete[] posOffset;    posOffset = NULL; }
  if (timeLevel != NULL)    { delete[] timeLevel;    timeLevel = NULL; }
}

// ------------- ПЕЙНТБОЛ -------------

uint8_t USE_SEGMENTS_PAINTBALL = 0;
uint8_t BorderWidth = 0;
uint8_t dir_mx, seg_num, seg_size, seg_offset, seg_offset_x, seg_offset_y;
int16_t idx;

void lightBallsRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    // modeCode = MC_PAINTBALL;
    FastLED.clear();  // очистить
    dir_mx = WIDTH > HEIGHT ? 0 : 1;                                   // 0 - квадратные сегменты расположены горизонтально, 1 - вертикально
    seg_num = dir_mx == 0 ? (WIDTH / HEIGHT) : (HEIGHT / WIDTH);     // вычисляем количество сегментов, умещающихся на матрице
    seg_size = dir_mx == 0 ? HEIGHT : WIDTH;                           // Размер квадратного сегмента (высота и ширина равны)
    seg_offset = ((dir_mx == 0 ? WIDTH : HEIGHT) - seg_size * seg_num) / (seg_num + 1); // смещение от края матрицы и между сегментами    
    BorderWidth = 0;
    USE_SEGMENTS_PAINTBALL = effectScaleParam2[MC_PAINTBALL];
  }
  
  // Если совсем задержки нет - матрица мерцает от постоянного обновления
  delay(5);
  
  uint8_t effectBrightness = getBrightnessCalculated(globalBrightness, effectContrast[thisMode]);

  // Apply some blurring to whatever's already on the matrix
  // Note that we never actually clear the matrix, we just constantly
  // blur it repeatedly.  Since the blurring is 'lossy', there's
  // an automatic trend toward black -- by design.
  uint8_t blurAmount = map(effectBrightness, 32,255, 65,91);
  uint8_t actualBrightness = map(effectBrightness, 32,255, 125,250);
  blur2d(leds, WIDTH, HEIGHT, blurAmount);

  // The color of each point shifts over time, each at a different speed.
  uint32_t ms = millis();

  uint8_t  cnt = map8(255-getEffectScaleParamValue(MC_PAINTBALL),1,4);  // 1..4 шариков
  float spd = (map8(255-getEffectSpeedValue(MC_PAINTBALL), 50, 100) / 100.0) / (USE_SEGMENTS_PAINTBALL != 0 ? 1 : (float)seg_num);

  // Отрисовка режима происходит на максимальной скорости. Значение effectSpeed влияет на параметр BPM функции beatsin8
  // The easiest way to construct this is to multiply a floating point BPM value (e.g. 120.3) by 256, (e.g. resulting in 30796 in this case), and pass that as the 16-bit BPM argument.
  uint8_t m1 = ( 91.0 * spd) + 0.51;
  uint8_t m2 = (109.0 * spd) + 0.51;
  uint8_t m3 = ( 73.0 * spd) + 0.51;
  uint8_t m4 = (123.0 * spd) + 0.51;

  // Для неквадратных - вычленяем квадратные сегменты, которые равномерно распределяем по ширине / высоте матрицы 

  if (USE_SEGMENTS_PAINTBALL != 0) {
    uint8_t  i = beatsin8(m1, 0, seg_size - BorderWidth - 1);
    uint8_t  j = beatsin8(m2, 0, seg_size - BorderWidth - 1);
    uint8_t  k = beatsin8(m3, 0, seg_size - BorderWidth - 1);
    uint8_t  m = beatsin8(m4, 0, seg_size - BorderWidth - 1);

    uint8_t d1 = ms / 29;
    uint8_t d2 = ms / 41;
    uint8_t d3 = ms / 73;
    uint8_t d4 = ms / 97;
    
    for (uint8_t ii = 0; ii < seg_num; ii++) {
      delay(0); // Для предотвращения ESP8266 Watchdog Timer      
      uint8_t cx = dir_mx == 0 ? (seg_offset * (ii + 1) + seg_size * ii) : 0;
      uint8_t cy = dir_mx == 0 ? 0 : (seg_offset * (ii + 1) + seg_size * ii);
      uint8_t color_shift = ii * 50;
      if (cnt <= 1) { idx = XY(i+cx, j+cy); leds[idx] += CHSV( color_shift + d1, 200, actualBrightness); }
      if (cnt <= 2) { idx = XY(j+cx, k+cy); leds[idx] += CHSV( color_shift + d2, 200, actualBrightness); }
      if (cnt <= 3) { idx = XY(k+cx, m+cy); leds[idx] += CHSV( color_shift + d3, 200, actualBrightness); }
      if (cnt <= 4) { idx = XY(m+cx, i+cy); leds[idx] += CHSV( color_shift + d4, 200, actualBrightness); }
      
      // При соединении матрицы из угла вверх или вниз почему-то слева и справа узора остаются полосы, которые 
      // не гаснут обычным blur - гасим полоски левой и правой стороны дополнительно.
      // При соединении из угла влево или вправо или на неквадратных матрицах такого эффекта не наблюдается
      uint8_t fade_step = map8(effectBrightness, 1, 15);
      for (uint8_t i2 = cy; i2 < cy + seg_size; i2++) { 
        fadePixel(cx + BorderWidth, i2, fade_step);
        fadePixel(cx + seg_size - BorderWidth - 1, i2, fade_step);
      }
    }
  }
  else 
  {
    uint8_t  i = beatsin8(m1, BorderWidth, WIDTH - BorderWidth - 1);
    uint8_t  j = beatsin8(m1, BorderWidth, HEIGHT - BorderWidth - 1);
    uint8_t  k = beatsin8(m3, BorderWidth, WIDTH - BorderWidth - 1);
    uint8_t  m = beatsin8(m4, BorderWidth, HEIGHT - BorderWidth - 1);
    
    if (cnt <= 1) { idx = XY(i, j); leds[idx] += CHSV( ms / 29, 200, actualBrightness); }
    if (cnt <= 2) { idx = XY(k, j); leds[idx] += CHSV( ms / 41, 200, actualBrightness); }
    if (cnt <= 3) { idx = XY(k, m); leds[idx] += CHSV( ms / 73, 200, actualBrightness); }
    if (cnt <= 4) { idx = XY(i, m); leds[idx] += CHSV( ms / 97, 200, actualBrightness); }
  
    if (WIDTH == HEIGHT) {
      // При соединении матрицы из угла вверх или вниз почему-то слева и справа узора остаются полосы, которые 
      // не гаснут обычным blur - гасим полоски левой и правой стороны дополнительно.
      // При соединении из угла влево или вправо или на неквадратных матрицах такого эффекта не наблюдается
      uint8_t fade_step = map8(effectBrightness, 1, 15);
      for (uint8_t i = 0; i < HEIGHT; i++) { 
        fadePixel(0, i, fade_step);
        fadePixel(WIDTH-1, i, fade_step);
      }
    } 
  }
}

// ------------- ВОДОВОРОТ -------------

uint8_t USE_SEGMENTS_SWIRL = 0;

void swirlRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    // modeCode = MC_SWIRL;
    FastLED.clear();  // очистить
    dir_mx = WIDTH > HEIGHT ? 0 : 1;                                   // 0 - квадратные сегменты расположены горизонтально, 1 - вертикально
    seg_num = dir_mx == 0 ? (WIDTH / HEIGHT) : (HEIGHT / WIDTH);     // вычисляем количество сегментов, умещающихся на матрице
    seg_size = dir_mx == 0 ? HEIGHT : WIDTH;                           // Размер квадратного сегмента (высота и ширина равны)
    seg_offset = ((dir_mx == 0 ? WIDTH : HEIGHT) - seg_size * seg_num) / (seg_num + 1); // смещение от края матрицы и между сегментами    
    BorderWidth = seg_num == 1 ? 0 : 1;
    USE_SEGMENTS_SWIRL = effectScaleParam2[MC_SWIRL];
  }

  // Если совсем задержки нет - матрица мерцает от постоянного обновления
  delay(5);

  uint8_t effectBrightness = getBrightnessCalculated(globalBrightness, effectContrast[thisMode]);

  // Apply some blurring to whatever's already on the matrix
  // Note that we never actually clear the matrix, we just constantly
  // blur it repeatedly.  Since the blurring is 'lossy', there's
  // an automatic trend toward black -- by design.

  uint8_t blurAmount = map(effectBrightness, 32,255, 65,91);
  uint8_t actualBrightness = map(effectBrightness, 32,255, 125,250);
  blur2d(leds, WIDTH, HEIGHT, blurAmount);

  uint32_t ms = millis();  
  int16_t idx;

  float spd = (map8(255-getEffectSpeedValue(MC_SWIRL), 50, 100) / 100.0) / (USE_SEGMENTS_PAINTBALL != 0 ? 1 : (float)seg_num);

  // Отрисовка режима происходит на максимальной скорости. Знеачение effectSpeed влияет на параметр BPM функции beatsin8
  // The easiest way to construct this is to multiply a floating point BPM value (e.g. 120.3) by 256, (e.g. resulting in 30796 in this case), and pass that as the 16-bit BPM argument.
  uint8_t m1 = (41.0 * spd) + 0.51;
  uint8_t m2 = (27.0 * spd) + 0.51;

  if (USE_SEGMENTS_SWIRL != 0) {
    // Use two out-of-sync sine waves
    uint8_t  i = beatsin8(m1, 0, seg_size - BorderWidth - 1);
    uint8_t  j = beatsin8(m2, 0, seg_size - BorderWidth - 1);

    // Also calculate some reflections
    uint8_t ni = (seg_size-1)-i;
    uint8_t nj = (seg_size-1)-j;

    uint8_t d1 = ms / 11;
    uint8_t d2 = ms / 13;
    uint8_t d3 = ms / 17;
    uint8_t d4 = ms / 29;
    uint8_t d5 = ms / 37;
    uint8_t d6 = ms / 41;
    
    for (uint8_t ii = 0; ii < seg_num; ii++) {
      delay(0); // Для предотвращения ESP8266 Watchdog Timer      
      uint8_t cx = dir_mx == 0 ? (seg_offset * (ii + 1) + seg_size * ii) : 0;
      uint8_t cy = dir_mx == 0 ? 0 : (seg_offset * (ii + 1) + seg_size * ii);
      uint8_t color_shift = ii * 50;
    
      // The color of each point shifts over time, each at a different speed.
      idx = XY( i+cx, j+cy); leds[idx] += CHSV( color_shift + d1, 200, actualBrightness);
      idx = XY(ni+cx,nj+cy); leds[idx] += CHSV( color_shift + d2, 200, actualBrightness);
      idx = XY( i+cx,nj+cy); leds[idx] += CHSV( color_shift + d3, 200, actualBrightness);
      idx = XY(ni+cx, j+cy); leds[idx] += CHSV( color_shift + d4, 200, actualBrightness);
      idx = XY( j+cx, i+cy); leds[idx] += CHSV( color_shift + d5, 200, actualBrightness);
      idx = XY(nj+cx,ni+cy); leds[idx] += CHSV( color_shift + d6, 200, actualBrightness);
      
      // При соединении матрицы из угла вверх или вниз почему-то слева и справа узора остаются полосы, которые 
      // не гаснут обычным blur - гасим полоски левой и правой стороны дополнительно.
      // При соединении из угла влево или вправо или на неквадратных матрицах такого эффекта не наблюдается
      uint8_t fade_step = map8(effectBrightness, 1, 15);
      for (uint8_t i2 = cy; i2 < cy + seg_size; i2++) { 
        fadePixel(cx, i2, fade_step);
        fadePixel(cx + BorderWidth, i2, fade_step);
        fadePixel(cx + seg_size - 1, i2, fade_step);
        fadePixel(cx + seg_size - BorderWidth - 1, i2, fade_step);
      }
    }
  } 
  else 
  {
    // Use two out-of-sync sine waves
    uint8_t  i = beatsin8(m1, BorderWidth, WIDTH - BorderWidth - 1);
    uint8_t  j = beatsin8(m2, BorderWidth, HEIGHT - BorderWidth - 1);

    // Also calculate some reflections
    uint8_t ni = (WIDTH-1)-i;
    uint8_t nj = (HEIGHT-1)-j;

    // The color of each point shifts over time, each at a different speed.
    idx = XY( i, j); leds[idx] += CHSV( ms / 11, 200, actualBrightness);
    idx = XY(ni,nj); leds[idx] += CHSV( ms / 13, 200, actualBrightness);
    idx = XY( i,nj); leds[idx] += CHSV( ms / 17, 200, actualBrightness);
    idx = XY(ni, j); leds[idx] += CHSV( ms / 29, 200, actualBrightness);
    
    if (HEIGHT == WIDTH) {
      // для квадратных матриц - 6 точек создают более красивую картину
      idx = XY( j, i); leds[idx] += CHSV( ms / 37, 200, actualBrightness);
      idx = XY(nj,ni); leds[idx] += CHSV( ms / 41, 200, actualBrightness);
      
      // При соединении матрицы из угла вверх или вниз почему-то слева и справа узора остаются полосы, которые 
      // не гаснут обычным blur - гасим полоски левой и правой стороны дополнительно.
      // При соединении из угла влево или вправо или на неквадратных матрицах такого эффекта не наблюдается
      uint8_t fade_step = map8(effectBrightness, 1, 15);
      for (uint8_t i = 0; i < HEIGHT; i++) { 
        fadePixel(0, i, fade_step);
        fadePixel(WIDTH-1, i, fade_step);
      }
    }  
  }
}

// ----------------------------- СВЕТЛЯКИ ------------------------------

#define LIGHTERS_AM 100
int16_t lightersPos[2][LIGHTERS_AM];
int8_t  lightersSpeed[2][LIGHTERS_AM];
uint8_t lightersColor[LIGHTERS_AM];
uint8_t loopCounter;

int16_t angle[LIGHTERS_AM];
int16_t speedV[LIGHTERS_AM];
int8_t  angleSpeed[LIGHTERS_AM];

void lightersRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    // modeCode = MC_LIGHTERS;
    randomSeed(millis());
    for (uint8_t i = 0; i < LIGHTERS_AM; i++) {
      lightersPos[0][i] = random(0, WIDTH * 10);
      lightersPos[1][i] = random(0, HEIGHT * 10);
      lightersSpeed[0][i] = random(-10, 10);
      lightersSpeed[1][i] = random(-10, 10);
      lightersColor[i] = random(0, 255);
    }
  }

  uint8_t effectBrightness = getBrightnessCalculated(globalBrightness, effectContrast[thisMode]);
  FastLED.clear();

  if (++loopCounter > 20) loopCounter = 0;

  for (uint8_t i = 0; i < map8(effectScaleParam[MC_LIGHTERS],5,150); i++) {
    if (loopCounter == 0) {     // меняем скорость каждые 20 отрисовок
      lightersSpeed[0][i] += random(-3, 4);
      lightersSpeed[1][i] += random(-3, 4);
      lightersSpeed[0][i] = constrain(lightersSpeed[0][i], -20, 20);
      lightersSpeed[1][i] = constrain(lightersSpeed[1][i], -20, 20);
    }

    lightersPos[0][i] += lightersSpeed[0][i];
    lightersPos[1][i] += lightersSpeed[1][i];

    if (lightersPos[0][i] < 0) lightersPos[0][i] = (WIDTH - 1) * 10;
    if (lightersPos[0][i] >= WIDTH * 10) lightersPos[0][i] = 0;

    if (lightersPos[1][i] < 0) {
      lightersPos[1][i] = 0;
      lightersSpeed[1][i] = -lightersSpeed[1][i];
    }
    if (lightersPos[1][i] >= (HEIGHT - 1) * 10) {
      lightersPos[1][i] = (HEIGHT - 1) * 10;
      lightersSpeed[1][i] = -lightersSpeed[1][i];
    }
    drawPixelXY(lightersPos[0][i] / 10, lightersPos[1][i] / 10, CHSV(lightersColor[i], 255, effectBrightness));
  }
}

// ******************* PACIFICA ********************

//////////////////////////////////////////////////////////////////////////
//
// In this animation, there are four "layers" of waves of light.  
//
// Each layer moves independently, and each is scaled separately.
//
// All four wave layers are added together on top of each other, and then 
// another filter is applied that adds "whitecaps" of brightness where the 
// waves line up with each other more.  Finally, another pass is taken
// over the led array to 'deepen' (dim) the blues and greens.
//
// The speed and scale and motion each layer varies slowly within independent 
// hand-chosen ranges, which is why the code has a lot of low-speed 'beatsin8' functions
// with a lot of oddly specific numeric ranges.
//
// These three custom blue-green color palettes were inspired by the colors found in
// the waters off the southern coast of California, https://goo.gl/maps/QQgd97jjHesHZVxQ7
//
CRGBPalette16 pacifica_palette_1 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50 };
CRGBPalette16 pacifica_palette_2 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F };
CRGBPalette16 pacifica_palette_3 = 
    { 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33, 
      0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF };

void pacificaRoutine()
{
  if (loadingFlag) {
    // modeCode = MC_PACIFICA;
    loadingFlag = false;
  }

  // Если совсем задержки нет - матрица мерцает от постоянного обновления
  delay(5);

  // uint8_t effectBrightness = getBrightnessCalculated(globalBrightness, getEffectContrastValue(thisMode));

  // Increment the four "color index start" counters, one for each wave layer.
  // Each is incremented at a different speed, and the speeds vary over time.
  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
  static uint32_t sLastms = 0;
  
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastms;
  
  sLastms = ms;
  
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (deltams * speedfactor1) / 256;
  uint32_t deltams2 = (deltams * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  
  sCIStart1 += (deltams1 * beatsin88(1011,10,13));
  sCIStart2 -= (deltams21 * beatsin88(777,8,11));
  sCIStart3 -= (deltams1 * beatsin88(501,5,7));
  sCIStart4 -= (deltams2 * beatsin88(257,4,6));

  // Clear out the LED array to a dim background blue-green
  fill_solid( leds, NUM_LEDS, CRGB( 2, 6, 10));

  // Render each of four layers, with different scales and speeds, that vary over time
  pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301) );
  pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401) );
  pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503));
  pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601));

  // Add brighter 'whitecaps' where the waves lines up more
  pacifica_add_whitecaps();

  // Deepen the blues and greens a bit
  pacifica_deepen_colors();
}

// Add one layer of waves into the led array
void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;

  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    waveangle += 250;
    uint16_t s16 = sin16( waveangle ) + 32768;
    uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16( ci) + 32768;
    uint8_t sindex8 = scale16( sindex16, 240);
    CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
    leds[i] += c;
  }
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void pacifica_add_whitecaps()
{
  uint8_t basethreshold = beatsin8( 9, 55, 65);
  uint8_t wave = beat8( 7 );
  
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = leds[i].getAverageLight();
    if( l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8( overage, overage);
      leds[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
    }
  }
}

// Deepen the blues and greens
void pacifica_deepen_colors()
{
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i].blue = scale8( leds[i].blue,  145); 
    leds[i].green= scale8( leds[i].green, 200); 
    leds[i] |= CRGB( 2, 5, 7);
  }
}

// ********************** SHADOWS ***********************

void shadowsRoutine() {
  if (loadingFlag) {
    // modeCode = MC_SHADOWS;
    loadingFlag = false;
  }

  // Если совсем задержки нет - матрица мерцает от постоянного обновления
  delay(5);

  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t  sat8 = beatsin88( 87, 220, 250);
  uint8_t  brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t  msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  
  uint8_t  effectBrightness = getBrightnessCalculated(globalBrightness, effectContrast[thisMode]);

  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;

  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, map8(bri8, map(effectBrightness, 32, 255, 32,125), map(effectBrightness, 32,255, 125,250))); 
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}

// ***************************** ПАЛИТРА *****************************

#define BLOCK_SIZE 4       // Размер квадратика палитры
#define FADE_IN_STEPS 16   // За сколько шагов плашка появляется на экране    
#define FADE_OUT_STEPS 32  // За сколько шагов плашка убирается с экрана    
#define BLOCK_ON_START 5   // Сколько блоков сразу появлять в начале эффекта

uint8_t num_x, num_y, off_x, off_y;

uint8_t **palette_h; // Н in CHSV
uint8_t **palette_s; // S in CHSV
uint8_t **block_sta; // Block state: // 0 - появление; 1 - исчезновение; 2 - пауза перед появлением 3 - пауза перед удалением
uint8_t **block_dur; // время паузы блока

void paletteRoutine() {

  if (loadingFlag) {
    // modeCode = MC_PALETTE;
    loadingFlag = false;
    
    num_x = WIDTH / BLOCK_SIZE;
    num_y = HEIGHT / BLOCK_SIZE;
    off_x = (WIDTH - BLOCK_SIZE * num_x) / 2;
    off_y = (HEIGHT - BLOCK_SIZE * num_y) / 2;

    dir_mx = WIDTH > HEIGHT ? 0 : 1;                                   // 0 - квадратные сегменты расположены горизонтально, 1 - вертикально
    seg_num = dir_mx == 0 ? (WIDTH / HEIGHT) : (HEIGHT / WIDTH);     // вычисляем количество сегментов, умещающихся на матрице
    
    if (palette_h == NULL) { palette_h = new uint8_t*[num_x]; for (uint8_t i = 0; i < num_x; i++) { palette_h[i] = new uint8_t [num_y]; }}
    if (palette_s == NULL) { palette_s = new uint8_t*[num_x]; for (uint8_t i = 0; i < num_x; i++) { palette_s[i] = new uint8_t [num_y]; }}
    if (block_sta == NULL) { block_sta = new uint8_t*[num_x]; for (uint8_t i = 0; i < num_x; i++) { block_sta[i] = new uint8_t [num_y]; }}
    if (block_dur == NULL) { block_dur = new uint8_t*[num_x]; for (uint8_t i = 0; i < num_x; i++) { block_dur[i] = new uint8_t [num_y]; }}

    // Для всех блоков определить состояние - "ожидание появления
    for (uint8_t c = 0; c < num_x; c++) {
      for (uint8_t r = 0; r < num_y; r++) {
        block_sta[c][r] = 2;                // Состояние - пауза перед появлением
        block_dur[c][r] = random8(25,125);  // Длительность паузы
      }
    }

    // Для некоторого количества начальных - установить "За шаг до появления"
    // При первом же проходе состояние переключится на "появление"
    for (uint8_t i = 0; i < BLOCK_ON_START * seg_num; i++) {
      uint8_t c = random8(0, num_x - 1);
      uint8_t r = random8(0, num_y - 1);
      block_dur[c][r] = 1;                  // Счетчик до начала появления
    }
    FastLED.clear();
  }
  
  uint8_t effectBrightness = getBrightnessCalculated(globalBrightness, effectContrast[thisMode]);

  for (uint8_t c = 0; c < num_x; c++) {
    uint8_t block_x = off_x + c * BLOCK_SIZE;
    for (uint8_t r = 0; r < num_y; r++) {    
      
      uint8_t block_y = off_y + r * BLOCK_SIZE;
      uint8_t h = palette_h[c][r];      
      uint8_t s = palette_s[c][r];

      // Проверить состояние блока
      if (block_sta[c][r] > 1) {
        
        // Одна из пауз (2 или 3) - пауза перед появлением или перед исчезновением
        // Уменьшить время паузы. Если стало 0 - переключить с паузы на появление / исчезновение
         block_dur[c][r] -= 1;
         if (block_dur[c][r] == 0) {
           block_sta[c][r] -= 2;     // 3->1 - исчезать; 2->0 появлять за указанное количество шагов
           if (block_sta[c][r] == 0) {
             block_dur[c][r] = FADE_IN_STEPS;    // Количество шагов появления блока
             palette_h[c][r] = random8(0,255);   // Цвет нового блока
             palette_s[c][r] = random8(112,254); // Насыщенность цвета нового блока
           } else { 
             block_dur[c][r] = FADE_OUT_STEPS;  // Кол-во шагов убирания блока
           }  
         }

      }
      
      if (block_sta[c][r] < 2) {

        // В процессе появления или исчезновения (0 или 1)
        // Выполнить один шаг появления / исчезновения блока
        uint8_t fade_dir = block_sta[c][r]; // 0 - появляться, 1 - исчезать
        uint8_t fade_step = block_dur[c][r];

        // Яркость блока
        uint8_t bri = fade_dir == 0
           ? map(fade_step, 0,FADE_IN_STEPS,  0,effectBrightness)
           : map(fade_step, 0,FADE_OUT_STEPS, effectBrightness,0);

        // Нарисовать блок   
        for (uint8_t i=0; i<BLOCK_SIZE; i++) {        
          for (uint8_t j=0; j<BLOCK_SIZE; j++) {
            
            //uint8_t k = fade_dir == 0 ? (2 * i*j) : (2 * (BLOCK_SIZE * BLOCK_SIZE - i*j));
            //uint8_t bri2 = (bri > k ? bri - k : 0);
            CHSV color = CHSV(h, s, bri); // bri2

            uint8_t xx = block_x + j;
            uint8_t yy = block_y + BLOCK_SIZE - i - 1;
            if (xx < WIDTH && yy < HEIGHT) {
              uint16_t idx = getPixelNumber(xx, yy);
              leds[idx] = color;
            }
          }
        }

        // Шаг появления - обработан
        block_dur[c][r] -= 1;

        // Весь процесс появления / исчезновения выполнен?
        // Сменить статус блока
        if (block_dur[c][r] == 0) {
           // Появление / исчезновение закончено
           block_sta[c][r] = block_sta[c][r] == 0 ? 3 : 2; // вкл паузу перед исчезновением после появления или паузу перед появлением после исчезновения
           block_dur[c][r] = random8(25,125);              // Длительность паузы (циклов обращения палитры)
        }        
      }      
    }
  }
}

void paletteRoutineRelease() {
  if (block_dur != NULL) { for( uint8_t i = 0; i < num_x; i++ ) { delete [] block_dur[i];} delete [] block_dur; block_dur = NULL; }
  if (block_sta != NULL) { for( uint8_t i = 0; i < num_x; i++ ) { delete [] block_sta[i];} delete [] block_sta; block_sta = NULL; }
  if (palette_s != NULL) { for( uint8_t i = 0; i < num_x; i++ ) { delete [] palette_s[i];} delete [] palette_s; palette_s = NULL; }
  if (palette_h != NULL) { for( uint8_t i = 0; i < num_x; i++ ) { delete [] palette_h[i];} delete [] palette_h; palette_h = NULL; }
}

// ****************************** СИНУСЫ *****************************

void prizmataRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    dir_mx = WIDTH >= HEIGHT ? 0 : 1;                                 // 0 - квадратные сегменты расположены горизонтально, 1 - вертикально
    // modeCode = MC_PRIZMATA;
  }

  // Если совсем задержки нет - матрица мерцает от постоянного обновления
  delay(5);
  
  EVERY_N_MILLIS(33) {
     hue++;
  }
  
  FastLED.clear();

  // Отрисовка режима происходит на максимальной скорости. Знеачение effectSpeed влияет на параметр BPM функции beatsin8
  uint8_t spd = map8(255-getEffectSpeedValue(MC_PRIZMATA), 12, 64);   
  uint8_t effectBrightness = getBrightnessCalculated(globalBrightness, effectContrast[thisMode]);

  if (dir_mx == 0) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      uint8_t y = beatsin8(spd + x, 0,HEIGHT-1);
      drawPixelXY(x, y, ColorFromPalette(RainbowColors_p, x * 7 + hue, effectBrightness));
    }
  } else {
    for (uint8_t y = 0; y < HEIGHT; y++) {
      uint8_t x = beatsin8(spd + y, 0, WIDTH-1);
      drawPixelXY(x, y, ColorFromPalette(RainbowColors_p, x * 7 + hue, effectBrightness));
    }
  }
}

// *************************** ДОЖДЬ **************************

CRGB rainColor = CRGB(60,80,90);
CRGB lightningColor = CRGB(72,72,80);
CRGBPalette16 rain_p( CRGB::Black, rainColor);
CRGBPalette16 rainClouds_p( CRGB::Black, CRGB(15,24,24), CRGB(9,15,15), CRGB::Black );

uint8_t cloudHeight = HEIGHT * 0.2 + 1;
uint8_t **noise3d;
uint8_t *cloud;

void rain(uint8_t backgroundDepth, uint8_t spawnFreq, uint8_t tailLength, bool splashes, bool clouds, bool storm) {
  
  static uint16_t noiseX = random16();
  static uint16_t noiseY = random16();
  static uint16_t noiseZ = random16();

  uint8_t effectBrightness = getBrightnessCalculated(globalBrightness, effectContrast[thisMode]);
  
  fadeToBlackBy( leds, NUM_LEDS, 255-tailLength);

  // Loop for each column individually
  for (uint8_t x = 0; x < WIDTH; x++) {
    // Step 1.  Move each dot down one cell
    for (uint8_t i = 0; i < HEIGHT; i++) {
      if (noise3d[x][i] >= backgroundDepth) {  // Don't move empty cells
        if (i > 0) noise3d[x][wrapY(i-1)] = noise3d[x][i];
        noise3d[x][i] = 0;
      }
    }

    // Step 2.  Randomly spawn new dots at top
    if (random8() < spawnFreq) {
      noise3d[x][HEIGHT-1] = random(backgroundDepth, effectBrightness);
    }

    // Step 3. Map from tempMatrix cells to LED colors
    for (uint8_t y = 0; y < HEIGHT; y++) {
      if (noise3d[x][y] >= backgroundDepth) {  // Don't write out empty cells
        leds[XY(x,y)] = ColorFromPalette(rain_p, noise3d[x][y], effectBrightness);
      }
    }

    // Step 4. Add splash if called for
    if (splashes) {
      // FIXME, this is broken
      uint8_t j = line[x];
      uint8_t v = noise3d[x][0];

      if (j >= backgroundDepth) {
        leds[XY(wrapX(x-2),0)] = ColorFromPalette(rain_p, j/3, effectBrightness);
        leds[XY(wrapX(x+2),0)] = ColorFromPalette(rain_p, j/3, effectBrightness);
        line[x] = 0;   // Reset splash
      }

      if (v >= backgroundDepth) {
        leds[XY(wrapX(x-1),1)] = ColorFromPalette(rain_p, v/2, effectBrightness);
        leds[XY(wrapX(x+1),1)] = ColorFromPalette(rain_p, v/2, effectBrightness);
        line[x] = v; // Prep splash for next frame
      }
    }

    // Step 5. Add lightning if called for
    if (storm && random16() < 72) {
      
      uint8_t *lightning = (uint8_t *) malloc(WIDTH * HEIGHT);
      
      if (lightning != NULL) { 
        lightning[scale8(random8(), WIDTH-1) + (HEIGHT-1) * WIDTH] = 255;  // Random starting location
        for(uint8_t ly = HEIGHT-1; ly > 1; ly--) {
          for (uint8_t lx = 1; lx < WIDTH-1; lx++) {
            if (lightning[lx + ly * WIDTH] == 255) {
              lightning[lx + ly * WIDTH] = 0;
              uint8_t dir = random8(4);
              switch (dir) {
                case 0:
                  leds[XY(lx+1,ly-1)] = lightningColor;
                  lightning[(lx+1) + (ly-1) * WIDTH] = 255; // move down and right
                break;
                case 1:
                  leds[XY(lx,ly-1)] = CRGB(128,128,128);   // я без понятия, почему у верхней молнии один оттенок, а у остальных - другой
                  lightning[lx + (ly-1) * WIDTH] = 255;    // move down
                break;
                case 2:
                  leds[XY(lx-1,ly-1)] = CRGB(128,128,128);
                  lightning[(lx-1) + (ly-1) * WIDTH] = 255; // move down and left
                break;
                case 3:
                  leds[XY(lx-1,ly-1)] = CRGB(128,128,128);
                  lightning[(lx-1) + (ly-1) * WIDTH] = 255; // fork down and left
                  leds[XY(lx-1,ly-1)] = CRGB(128,128,128);
                  lightning[(lx+1) + (ly-1) * WIDTH] = 255; // fork down and right
                break;
              }
            }
          }
        }
        free(lightning);
      }
    }

    // Step 6. Add clouds if called for
    if (clouds) {
      uint16_t noiseScale = 250;  // A value of 1 will be so zoomed in, you'll mostly see solid colors. A value of 4011 will be very zoomed out and shimmery
      
      if (cloud != NULL) {      
        int16_t xoffset = noiseScale * x + hue;  
        for(uint8_t z = 0; z < cloudHeight; z++) {
          int16_t yoffset = noiseScale * z - hue;
          uint8_t dataSmoothing = 192;
          uint8_t noiseData = qsub8(inoise8(noiseX + xoffset,noiseY + yoffset,noiseZ),16);
          noiseData = qadd8(noiseData,scale8(noiseData,39));
          cloud[x * cloudHeight + z] = scale8( cloud[x * cloudHeight + z], dataSmoothing) + scale8( noiseData, 256 - dataSmoothing);
          nblend(leds[XY(x,HEIGHT-z-1)], ColorFromPalette(rainClouds_p, cloud[x * cloudHeight + z], effectBrightness), (cloudHeight-z)*(250/cloudHeight));
        }
      }
      noiseZ++;
    }
  }
}

void rainRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    //modeCode = MC_RAIN;
    cloudHeight = HEIGHT * 0.2 + 1; // это уже 20% c лишеним, но на высоких матрицах будет чуть меньше
    
    if (noise3d == NULL) { noise3d = new uint8_t*[WIDTH]; for (uint8_t i = 0; i < WIDTH; i++) { noise3d[i] = new uint8_t [HEIGHT]; }}
    if (line == NULL)    { line = new uint8_t[WIDTH]; }
    if (cloud == NULL)   { cloud = new uint8_t[WIDTH * cloudHeight]; }
  }

  uint8_t intensity = beatsin8(map8(effectScaleParam[MC_RAIN],2,6), 4, 60);
  
  // ( Depth of dots, frequency of new dots, length of tails, splashes, clouds, ligthening )
  if (intensity <= 35) 
    // Lightweight
    rain(60, intensity, 10, true, true, false);
  else
    // Stormy
    rain(0, intensity, 10, true, true, true);
}

void rainRoutineRelease() {
  if (cloud != NULL)   { delete[] cloud; cloud = NULL; }
  if (line != NULL)    { delete[] line; line = NULL; }
  if (noise3d != NULL) { for (uint8_t i = 0; i < WIDTH; i++) delete[] noise3d[i]; delete[] noise3d; noise3d = NULL; }
}

// ********************** ОГОНЬ-2 (КАМИН) *********************

void fire2Routine() {
  if (loadingFlag) {
    loadingFlag = false;
    //modeCode = MC_FIRE2;
    if (noise3d == NULL) { noise3d = new uint8_t*[WIDTH]; for (uint8_t i = 0; i < WIDTH; i++) { noise3d[i] = new uint8_t [HEIGHT]; } }
  }

  // Если совсем задержки нет - матрица мерцает от постоянного обновления
  delay(5);

  static uint8_t FIRE_BASE = HEIGHT/6 > 6 ? 6 : HEIGHT/6+1;
  
  // COOLING: How much does the air cool as it rises?
  // Less cooling = taller flames.  More cooling = shorter flames.  
  uint8_t cooling = map8(effectScaleParam[MC_FIRE2], 70, 100);     
  
  // SPARKING: What chance (out of 255) is there that a new spark will be lit?
  // Higher chance = more roaring fire.  Lower chance = more flickery fire.
  uint8_t sparking = map8(effectScaleParam[MC_FIRE2], 90, 150);
  
  // SMOOTHING; How much blending should be done between frames
  // Lower = more blending and smoother flames. Higher = less blending and flickery flames
  const uint8_t fireSmoothing = 80;
  
  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy(random(256));

  uint8_t effectBrightness = map8(getBrightnessCalculated(globalBrightness, effectContrast[thisMode]), 32,128);

  // Loop for each column individually
  for (uint8_t x = 0; x < WIDTH; x++) {
    
    // Step 1.  Cool down every cell a little
    for (uint8_t i = 0; i < HEIGHT; i++) {
      noise3d[x][i] = qsub8(noise3d[x][i], random(0, ((cooling * 10) / HEIGHT) + 2));
    }

    // Step 2.  Heat from  cell drifts 'up' and diffuses a little
    for (uint8_t k = HEIGHT; k > 1; k--) {
      noise3d[x][wrapY(k)] = (noise3d[x][k - 1] + noise3d[x][wrapY(k - 2)] + noise3d[x][wrapY(k - 2)]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < sparking) {
      uint8_t j = random8(FIRE_BASE);
      noise3d[x][j] = qadd8(noise3d[x][j], random(160, 255));
    }

    // Step 4.  Map from heat cells to LED colors
    // Blend new data with previous frame. Average data between neighbouring pixels
    for (uint8_t y = 0; y < HEIGHT; y++)
      nblend(leds[XY(x,y)], ColorFromPalette(HeatColors_p, ((noise3d[x][y]*0.7) + (noise3d[wrapX(x+1)][y]*0.3)), effectBrightness), fireSmoothing);
  }
}

void fire2RoutineRelease() {
  if (noise3d != NULL) { for (uint8_t i = 0; i < WIDTH; i++) delete[] noise3d[i]; delete[] noise3d; noise3d = NULL; }
}

// ************************** СТРЕЛКИ *************************
int8_t   arrow_x[4], arrow_y[4], stop_x[4], stop_y[4];
uint8_t  arrow_direction;            // 0x01 - слева направо; 0x02 - снизу вверх; 0х04 - справа налево; 0х08 - сверху вниз
uint8_t  arrow_mode, arrow_mode_orig;// 0 - по очереди все варианты
                                     // 1 - по очереди от края до края экрана; 
                                     // 2 - одновременно по горизонтали навстречу к ентру, затем одновременно по вертикали навстречу к центру
                                     // 3 - одновременно все к центру
                                     // 4 - по два (горизонталь / вертикаль) все от своего края к противоположному, стрелки смещены от центра на 1/3
                                     // 5 - одновременно все от своего края к противоположному, стрелки смещены от центра на 1/3
bool     arrow_complete, arrow_change_mode;
uint8_t  arrow_hue[4];
uint8_t  arrow_play_mode_count[6];        // Сколько раз проигрывать полностью каждый режим если вариант 0 - текущий счетчик
uint8_t  arrow_play_mode_count_orig[6];   // Сколько раз проигрывать полностью каждый режим если вариант 0 - исходные настройки

void arrowsRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    //modeCode = MC_ARROWS;
    FastLED.clear();
    arrow_complete = false;
    arrow_mode_orig = (specialTextEffectParam >= 0) ? specialTextEffectParam : effectScaleParam2[MC_ARROWS];
    
    arrow_mode = (arrow_mode_orig == 0 || arrow_mode_orig > 5) ? random8(1,5) : arrow_mode_orig;
    arrow_play_mode_count_orig[0] = 0;
    arrow_play_mode_count_orig[1] = 4;  // 4 фазы - все стрелки показаны по кругу один раз - переходить к следующему ->
    arrow_play_mode_count_orig[2] = 4;  // 2 фазы - гориз к центру (1), затем верт к центру (2) - обе фазы повторить по 2 раза -> 4
    arrow_play_mode_count_orig[3] = 4;  // 1 фаза - все к центру (1) повторить по 4 раза -> 4
    arrow_play_mode_count_orig[4] = 4;  // 2 фазы - гориз к центру (1), затем верт к центру (2) - обе фазы повторить по 2 раза -> 4
    arrow_play_mode_count_orig[5] = 4;  // 1 фаза - все сразу (1) повторить по 4 раза -> 4
    for (uint8_t i=0; i<6; i++) {
      arrow_play_mode_count[i] = arrow_play_mode_count_orig[i];
    }
    arrowSetupForMode(arrow_mode, true);
  }
  
  uint8_t effectBrightness = map8(getBrightnessCalculated(globalBrightness, effectContrast[thisMode]), 32,255);  

  fader(65);
  CHSV color;
  
  // движение стрелки - cлева направо
  if ((arrow_direction & 0x01) > 0) {
    color = CHSV(arrow_hue[0], 255, effectBrightness);
    for (int8_t x = 0; x <= 4; x++) {
      for (int8_t y = 0; y <= x; y++) {    
        if (arrow_x[0] - x >= 0 && arrow_x[0] - x <= stop_x[0]) { 
          CHSV clr = (x < 4 || (x == 4 && y < 2)) ? color : CHSV(0,0,0);
          drawPixelXY(arrow_x[0] - x, arrow_y[0] - y, clr);
          drawPixelXY(arrow_x[0] - x, arrow_y[0] + y, clr);
        }
      }    
    }
    arrow_x[0]++;
  }

  // движение стрелки - cнизу вверх
  if ((arrow_direction & 0x02) > 0) {
    color = CHSV(arrow_hue[1], 255, effectBrightness);
    for (int8_t y = 0; y <= 4; y++) {
      for (int8_t x = 0; x <= y; x++) {    
        if (arrow_y[1] - y >= 0 && arrow_y[1] - y <= stop_y[1]) { 
          CHSV clr = (y < 4 || (y == 4 && x < 2)) ? color : CHSV(0,0,0);
          drawPixelXY(arrow_x[1] - x, arrow_y[1] - y, clr);
          drawPixelXY(arrow_x[1] + x, arrow_y[1] - y, clr);
        }
      }    
    }
    arrow_y[1]++;
  }

  // движение стрелки - cправа налево
  if ((arrow_direction & 0x04) > 0) {
    color = CHSV(arrow_hue[2], 255, effectBrightness);
    for (int8_t x = 0; x <= 4; x++) {
      for (int8_t y = 0; y <= x; y++) {    
        if (arrow_x[2] + x >= stop_x[2] && arrow_x[2] + x < WIDTH) { 
          CHSV clr = (x < 4 || (x == 4 && y < 2)) ? color : CHSV(0,0,0);
          drawPixelXY(arrow_x[2] + x, arrow_y[2] - y, clr);
          drawPixelXY(arrow_x[2] + x, arrow_y[2] + y, clr);
        }
      }    
    }
    arrow_x[2]--;
  }

  // движение стрелки - cверху вниз
  if ((arrow_direction & 0x08) > 0) {
    color = CHSV(arrow_hue[3], 255, effectBrightness);
    for (int8_t y = 0; y <= 4; y++) {
      for (int8_t x = 0; x <= y; x++) {    
        if (arrow_y[3] + y >= stop_y[3] && arrow_y[3] + y < HEIGHT) { 
          CHSV clr = (y < 4 || (y == 4 && x < 2)) ? color : CHSV(0,0,0);
          drawPixelXY(arrow_x[3] - x, arrow_y[3] + y, clr);
          drawPixelXY(arrow_x[3] + x, arrow_y[3] + y, clr);
        }
      }    
    }
    arrow_y[3]--;
  }

  // Проверка завершения движения стрелки, переход к следующей фазе или режиму
  
  switch (arrow_mode) {

    case 1:
      // Последовательно - слева-направо -> снизу вверх -> справа налево -> сверху вниз и далее по циклу
      // В каждый сомент времени сктивна только одна стрелка, если она дошла до края - переключиться на следующую и задать ее начальные координаты
      arrow_complete = false;
      switch (arrow_direction) {
        case 1: arrow_complete = arrow_x[0] > stop_x[0]; break;
        case 2: arrow_complete = arrow_y[1] > stop_y[1]; break;
        case 4: arrow_complete = arrow_x[2] < stop_x[2]; break;
        case 8: arrow_complete = arrow_y[3] < stop_y[3]; break;
      }

      arrow_change_mode = false;
      if (arrow_complete) {
        arrow_direction = (arrow_direction << 1) & 0x0F;
        if (arrow_direction == 0) arrow_direction = 1;
        if (arrow_mode_orig == 0) {
          arrow_play_mode_count[1]--;
          if (arrow_play_mode_count[1] == 0) {
            arrow_play_mode_count[1] = arrow_play_mode_count_orig[1];
            arrow_mode = random8(1,5);
            arrow_change_mode = true;
          }
        }

        arrowSetupForMode(arrow_mode, arrow_change_mode);
      }
      break;

    case 2:
      // Одновременно горизонтальные навстречу до половины экрана
      // Затем одновременно вертикальные до половины экрана. Далее - повторять
      arrow_complete = false;
      switch (arrow_direction) {
        case  5: arrow_complete = arrow_x[0] > stop_x[0]; break;   // Стрелка слева и справа встречаются в центре одновременно - проверять только стрелку слева
        case 10: arrow_complete = arrow_y[1] > stop_y[1]; break;   // Стрелка снизу и сверху встречаются в центре одновременно - проверять только стрелку снизу
      }

      arrow_change_mode = false;
      if (arrow_complete) {
        arrow_direction = arrow_direction == 5 ? 10 : 5;
        if (arrow_mode_orig == 0) {
          arrow_play_mode_count[2]--;
          if (arrow_play_mode_count[2] == 0) {
            arrow_play_mode_count[2] = arrow_play_mode_count_orig[2];
            arrow_mode = random8(1,5);
            arrow_change_mode = true;
          }
        }
        
        arrowSetupForMode(arrow_mode, arrow_change_mode);
      }
      break;

    case 3:
      // Одновременно со всех сторон к центру
      // Завершение кадра режима - когда все стрелки собрались в центре.
      // Проверять стрелки по самой длинной стороне
      if (WIDTH >= HEIGHT)
        arrow_complete = arrow_x[0] > stop_x[0];
      else 
        arrow_complete = arrow_y[1] > stop_y[1];
        
      arrow_change_mode = false;
      if (arrow_complete) {
        if (arrow_mode_orig == 0) {
          arrow_play_mode_count[3]--;
          if (arrow_play_mode_count[3] == 0) {
            arrow_play_mode_count[3] = arrow_play_mode_count_orig[3];
            arrow_mode = random8(1,5);
            arrow_change_mode = true;
          }
        }
        
        arrowSetupForMode(arrow_mode, arrow_change_mode);
      }
      break;

    case 4:
      // Одновременно слева/справа от края до края со смещением горизонтальной оси на 1/3 высоты, далее
      // одновременно снизу/сверху от края до края со смещением вертикальной оси на 1/3 ширины
      // Завершение кадра режима - когда все стрелки собрались в центре.
      // Проверять стрелки по самой длинной стороне
      switch (arrow_direction) {
        case  5: arrow_complete = arrow_x[0] > stop_x[0]; break;   // Стрелка слева и справа движутся и достигают края одновременно - проверять только стрелку слева
        case 10: arrow_complete = arrow_y[1] > stop_y[1]; break;   // Стрелка снизу и сверху движутся и достигают края одновременно - проверять только стрелку снизу
      }

      arrow_change_mode = false;
      if (arrow_complete) {
        arrow_direction = arrow_direction == 5 ? 10 : 5;
        if (arrow_mode_orig == 0) {
          arrow_play_mode_count[4]--;
          if (arrow_play_mode_count[4] == 0) {
            arrow_play_mode_count[4] = arrow_play_mode_count_orig[4];
            arrow_mode = random8(1,5);
            arrow_change_mode = true;
          }
        }
        
        arrowSetupForMode(arrow_mode, arrow_change_mode);
      }
      break;

    case 5:
      // Одновременно со всех сторон от края до края со смещением горизонтальной оси на 1/3 высоты, далее
      // Проверять стрелки по самой длинной стороне
      if (WIDTH >= HEIGHT)
        arrow_complete = arrow_x[0] > stop_x[0];
      else 
        arrow_complete = arrow_y[1] > stop_y[1];

      arrow_change_mode = false;
      if (arrow_complete) {
        if (arrow_mode_orig == 0) {
          arrow_play_mode_count[5]--;
          if (arrow_play_mode_count[5] == 0) {
            arrow_play_mode_count[5] = arrow_play_mode_count_orig[5];
            arrow_mode = random8(1,5);
            arrow_change_mode = true;
          }
        }
        
        arrowSetupForMode(arrow_mode, arrow_change_mode);
      }
      break;
  }

}

void arrowSetupForMode(uint8_t mode, bool change) {
    switch (mode) {
      case 1:
        if (change) arrow_direction = 1;
        arrowSetup_mode1();    // От края матрицы к краю, по центру гориз и верт
        break;
      case 2:
        if (change) arrow_direction = 5;
        arrowSetup_mode2();    // По центру матрицы (гориз / верт) - ограничение - центр матрицы
        break;
      case 3:
        if (change) arrow_direction = 15;
        arrowSetup_mode2();    // как и в режиме 2 - по центру матрицы (гориз / верт) - ограничение - центр матрицы
        break;
      case 4:
        if (change) arrow_direction = 5;
        arrowSetup_mode4();    // От края матрицы к краю, верт / гориз
        break;
      case 5:
        if (change) arrow_direction = 15;
        arrowSetup_mode4();    // как и в режиме 4 от края матрицы к краю, на 1/3
        break;
    }
}
void arrowSetup_mode1() {
  // Слева направо
  if ((arrow_direction & 0x01) > 0) {
    arrow_hue[0] = random8();
    arrow_x[0] = 0;
    arrow_y[0] = HEIGHT / 2;
    stop_x [0] = WIDTH + 7;      // скрывается за экраном на 7 пикселей
    stop_y [0] = 0;              // неприменимо 
  }    
  // снизу вверх
  if ((arrow_direction & 0x02) > 0) {
    arrow_hue[1] = random8();
    arrow_y[1] = 0;
    arrow_x[1] = WIDTH / 2;
    stop_y [1] = HEIGHT + 7;     // скрывается за экраном на 7 пикселей
    stop_x [1] = 0;              // неприменимо 
  }    
  // справа налево
  if ((arrow_direction & 0x04) > 0) {
    arrow_hue[2] = random8();
    arrow_x[2] = WIDTH - 1;
    arrow_y[2] = HEIGHT / 2;
    stop_x [2] = -7;             // скрывается за экраном на 7 пикселей
    stop_y [2] = 0;              // неприменимо 
  }
  // сверху вниз
  if ((arrow_direction & 0x08) > 0) {
    arrow_hue[3] = random8();
    arrow_y[3] = HEIGHT - 1;
    arrow_x[3] = WIDTH / 2;
    stop_y [3] = -7;             // скрывается за экраном на 7 пикселей
    stop_x [3] = 0;              // неприменимо 
  }
}

void arrowSetup_mode2() {
  // Слева направо до половины экрана
  if ((arrow_direction & 0x01) > 0) {
    arrow_hue[0] = random8();
    arrow_x[0] = 0;
    arrow_y[0] = HEIGHT / 2;
    stop_x [0] = WIDTH / 2 - 1;  // до центра экрана
    stop_y [0] = 0;              // неприменимо 
  }    
  // снизу вверх до половины экрана
  if ((arrow_direction & 0x02) > 0) {
    arrow_hue[1] = random8();
    arrow_y[1] = 0;
    arrow_x[1] = WIDTH / 2;
    stop_y [1] = HEIGHT / 2 - 1; // до центра экрана
    stop_x [1] = 0;              // неприменимо 
  }    
  // справа налево до половины экрана
  if ((arrow_direction & 0x04) > 0) {
    arrow_hue[2] = random8();
    arrow_x[2] = WIDTH - 1;
    arrow_y[2] = HEIGHT / 2;
    stop_x [2] = WIDTH / 2;      // до центра экрана
    stop_y [2] = 0;              // неприменимо 
  }
  // сверху вниз до половины экрана
  if ((arrow_direction & 0x08) > 0) {
    arrow_hue[3] = random8();
    arrow_y[3] = HEIGHT - 1;
    arrow_x[3] = WIDTH / 2;
    stop_y [3] = HEIGHT / 2;     // до центра экрана
    stop_x [3] = 0;              // неприменимо 
  }
}

void arrowSetup_mode4() {
  // Слева направо
  if ((arrow_direction & 0x01) > 0) {
    arrow_hue[0] = random8();
    arrow_x[0] = 0;
    arrow_y[0] = (HEIGHT / 3) * 2;
    stop_x [0] = WIDTH + 7;      // скрывается за экраном на 7 пикселей
    stop_y [0] = 0;              // неприменимо 
  }    
  // снизу вверх
  if ((arrow_direction & 0x02) > 0) {
    arrow_hue[1] = random8();
    arrow_y[1] = 0;
    arrow_x[1] = (WIDTH / 3) * 2;
    stop_y [1] = HEIGHT + 7;     // скрывается за экраном на 7 пикселей
    stop_x [1] = 0;              // неприменимо 
  }    
  // справа налево
  if ((arrow_direction & 0x04) > 0) {
    arrow_hue[2] = random8();
    arrow_x[2] = WIDTH - 1;
    arrow_y[2] = HEIGHT / 3;
    stop_x [2] = -7;             // скрывается за экраном на 7 пикселей
    stop_y [2] = 0;              // неприменимо 
  }
  // сверху вниз
  if ((arrow_direction & 0x08) > 0) {
    arrow_hue[3] = random8();
    arrow_y[3] = HEIGHT - 1;
    arrow_x[3] = WIDTH / 3;
    stop_y [3] = -7;             // скрывается за экраном на 7 пикселей
    stop_x [3] = 0;              // неприменимо 
  }
}

// ---------------------------------------- ЦИКЛОН ------------------------------------------

int16_t cycle_x, cycle_y; // могут уходить в минус при смене направления
uint8_t move_dir, fade_divider, inc_cnt, USE_SEGMENTS_CYCLON;

void cyclonRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    // modeCode = MC_CYCLON;
    USE_SEGMENTS_CYCLON = effectScaleParam2[MC_CYCLON];
    dir_mx = WIDTH > HEIGHT ? 0 : 1;                                                                      // 0 - сегменты расположены горизонтально, 1 - вертикально
    seg_num = dir_mx == 0 ? (WIDTH / HEIGHT) : (HEIGHT / WIDTH);                                        // вычисляем количество сегментов, умещающихся на матрице, в режиме без сигментов ширина одной полоски будет равна кол-ву сегментов
    seg_size = dir_mx == 0 ? HEIGHT : WIDTH;                                                              // Размер квадратного сегмента (высота и ширина равны)
    seg_offset_y = USE_SEGMENTS_CYCLON == 1 ? (dir_mx == 1 ? HEIGHT - seg_size * seg_num : 0) / 2 : 0;     // смещение от низа/верха матрицы
    seg_offset_x = USE_SEGMENTS_CYCLON == 1 ? (dir_mx == 0 ? WIDTH - seg_size * seg_num : 0) / 2 : 0;      // смещение от левого/правого края матрицы
    hue = 0;
    cycle_x = USE_SEGMENTS_CYCLON == 1 ? (seg_offset_x + seg_size - 1) : WIDTH - 1; // начало - от правого края к левому
    cycle_y = USE_SEGMENTS_CYCLON == 1 ?  seg_offset_y : 0;
    move_dir = 1;
    fade_divider = 0;
    inc_cnt = NUM_LEDS / 312;
    if (inc_cnt == 0) inc_cnt = 1;
    FastLED.clear();  // очистить
  }

  uint8_t effectBrightness = getBrightnessCalculated(globalBrightness, effectContrast[thisMode]);
  uint8_t actualBrightness = map(effectBrightness, 32,255, 125,250);
  
  // Использовать отрисовку по сегментам
  // Если сегменты не используется - ширина одной полоски - кол-во сегментов
  for (uint8_t i=0; i < seg_num; i++) {
    for (uint8_t k=0; k < inc_cnt; k++) { 
      if (USE_SEGMENTS_CYCLON == 1) {
        if (cycle_y + k - seg_offset_y >= seg_size) continue;
        idx = dir_mx == 0
           ? getPixelNumber(cycle_x + i * seg_size, cycle_y + k)
           : getPixelNumber(cycle_x, cycle_y + i * seg_size + k);
      } else {
        if (cycle_y + k  >= HEIGHT) continue;
        idx = getPixelNumber(cycle_x + i, cycle_y + k);
      }
      if (idx >= 0 && idx < NUM_LEDS) 
          leds[idx] = CHSV(hue + k + (USE_SEGMENTS_CYCLON == 1 ? i * 85 : 0), 255, actualBrightness);              
    }
  }  

  hue += inc_cnt;
  
  // Затухание - не на каждый цикл, а регулируется параметром эффекта
  uint8_t fader_param = map8(255 - getEffectScaleParamValue(MC_CYCLON),0,5);
  fade_divider++;
  if (fade_divider > fader_param) {
    fade_divider = 0;
    fader(5);
  }

  cycle_y += inc_cnt;

  if (USE_SEGMENTS_CYCLON) {
    
    if (cycle_y - seg_offset_y >= seg_size) {
      cycle_y = seg_offset_y;
  
      if (move_dir == 0) {
        // Слева направо
        cycle_x++;     
        if (cycle_x - seg_offset_x >= seg_size) {
            move_dir = 1;
            cycle_x = seg_size - 1 + seg_offset_x;
        }
      } else {
        // Справа налево
        cycle_x--;     
        if (cycle_x < seg_offset_x) {
            move_dir = 0;
            cycle_x = seg_offset_x;
        }
      }    
    }
    
  } else {
    
    if (cycle_y >= HEIGHT) {
      cycle_y = 0;
  
      if (move_dir == 0) {
        // Слева направо
        cycle_x += seg_num;     
        if (cycle_x >= WIDTH) {
            move_dir = 1;
            cycle_x = WIDTH - 1;
        }
      } else {
        // Справа налева
        cycle_x -= seg_num;     
        if (cycle_x < 0) {
            move_dir = 0;
            cycle_x = 0;
        }
      }    
    }
  }
}

// *************************** ВЫШИВАНКА **************************

// ------ Эффект "Вышиванка" 
// (с) проект Aurora "Munch"
// adopted/updated by kostyamat

int8_t  count = 0;
uint8_t flip = 0;
uint8_t generation = 0;
uint8_t rnd = 4; //1-8
uint8_t mic[2];
uint8_t minDimLocal = maxDim > 32 ? 32 : 16;

const uint8_t width_adj = (WIDTH < HEIGHT ? (HEIGHT - WIDTH) / 2 : 0);
const uint8_t height_adj = (HEIGHT < WIDTH ? (WIDTH - HEIGHT) / 2 : 0);
const uint8_t maxDim_steps = 256 / maxDim;
int8_t   dir;
void munchRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    //modeCode = MC_MUNCH;
    generation = 0;
    dir = 1;
    count = 0;
    flip = 0;
    FastLED.clear();
  }

  uint8_t effectBrightness = getBrightnessCalculated(globalBrightness, effectContrast[thisMode]);

  for (uint8_t x = 0; x < minDimLocal; x++) {
    for (uint8_t y = 0; y < minDimLocal; y++) {
      CRGB color = (x ^ y ^ flip) < count ? ColorFromPalette(RainbowColors_p, ((x ^ y) << rnd) + generation, effectBrightness) : CRGB::Black;
      if (x < WIDTH and y < HEIGHT) leds[XY(x, y)] = color;
      if (x + minDimLocal < WIDTH and y < HEIGHT) leds[XY(x + minDimLocal, y)] = color;
      if (y + minDimLocal < HEIGHT and x < WIDTH) leds[XY(x, y + minDimLocal)] = color;
      if (x + minDimLocal < WIDTH and y + minDimLocal < HEIGHT) leds[XY(x + minDimLocal, y + minDimLocal)] = color;
    }
  }

  count += dir;

  if (count <= 0 || count >= mic[0]) {
    dir = -dir;
    if (count <= 0) {
      mic[0] = mic[1];
      if (flip == 0)
        flip = mic[1] - 1;
      else
        flip = 0;
    }
  }
  
  generation++;
  mic[1] = minDimLocal;
}
