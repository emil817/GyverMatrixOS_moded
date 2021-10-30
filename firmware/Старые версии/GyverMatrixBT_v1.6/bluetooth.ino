// вкладка работы с bluetooth

#if (BT_MODE == 1)
#define PARSE_AMOUNT 4    // максимальное количество значений в массиве, который хотим получить
#define header '$'        // стартовый символ
#define divider ' '       // разделительный символ
#define ending ';'        // завершающий символ

byte prevY = 0;
byte prevX = 0;
boolean runningFlag;
boolean gameFlag;
boolean effectsFlag;
byte game;
byte effect;
byte intData[PARSE_AMOUNT];     // массив численных значений после парсинга
uint32_t prevColor;
boolean recievedFlag;
byte lastMode = 0;
boolean parseStarted;

void bluetoothRoutine() {
  parsing();                           // принимаем данные

  if (!parseStarted) {                // на время принятия данных матрицу не обновляем!

    if (runningFlag) fillString(runningText);   // бегущая строка
    if (gameFlag) games();                      // игры
    if (effectsFlag) effects();                 // эффекты
  }
}

// блок эффектов, работают по общему таймеру
void effects() {

  switch (effect) {
    case 0: brightnessRoutine();
      break;
    case 1: colorsRoutine();
      break;
    case 2: snowRoutine();
      break;
    case 3: ballRoutine();
      break;
    case 4: rainbowRoutine();
      break;
    case 5: rainbowColorsRoutine();
      break;
    case 6: fireRoutine();
      break;
    case 7: matrixRoutine();
      break;
    case 8: ballsRoutine();
      break;
    case 9: wavesRoutine();
      break;
    case 10: starfallRoutine();
      break;
  }
  FastLED.show();
}

// блок игр
void games() {
  switch (game) {
    case 0:
      if (loadingFlag) {
        FastLED.clear();
        loadingFlag = false;
        newGameSnake();
      }
      snakeRoutine();
      break;
    case 1:
      if (loadingFlag) {
        FastLED.clear();
        loadingFlag = false;
        newGameTetris();
      }
      tetrisRoutine();
      break;
    case 2:
      if (loadingFlag) {
        FastLED.clear();
        loadingFlag = false;
        newGameMaze();
      }
      mazeRoutine();
      break;
  }
}

byte index;
String string_convert = "";
enum modes {NORMAL, COLOR, TEXT} parseMode;

// ********************* ПРИНИМАЕМ ДАННЫЕ **********************
void parsing() {
  // ****************** ОБРАБОТКА *****************
  /*
    Протокол связи, посылка начинается с режима. Режимы:
    0 - отправка цвета $0 colorHEX;
    1 - отправка координат точки $1 X Y;
    2 - заливка - $2;
    3 - очистка - $3;
    4 - яркость - $4 value;
    5 - картинка $5 colorHEX X Y;
    6 - текст $6 some text
    7 - управление текстом: $7 1; пуск, $7 0; стоп
    8 - эффект
      - $8 0 номерЭффекта;
      - $8 1 старт/стоп;
    9 - игра
    10 - кнопка вверх
    11 - кнопка вправо
    12 - кнопка вниз
    13 - кнопка влево
    14 - пауза в игре
    15 - скорость $8 скорость;
  */
  if (recievedFlag) {      // если получены данные
    recievedFlag = false;
    BTcontrol = true;
    idleTimer.reset();

    switch (intData[0]) {
      case 1:
        drawPixelXY(intData[1], intData[2], gammaCorrection(globalColor));
        FastLED.show();
        break;
      case 2:
        fillAll(gammaCorrection(globalColor));
        FastLED.show();
        break;
      case 3:
        FastLED.clear();
        FastLED.show();
        break;
      case 4:
        globalBrightness = intData[1];
        breathBrightness = globalBrightness;
        FastLED.setBrightness(globalBrightness);
        FastLED.show();
        break;
      case 5:
        drawPixelXY(intData[2], intData[3], gammaCorrection(globalColor));
        // делаем обновление матрицы каждую строчку, чтобы реже обновляться
        // и не пропускать пакеты данных (потому что отправка на большую матрицу занимает много времени)
        if (prevY != intData[3] || ( (intData[3] == 0) && (intData[2] == WIDTH - 1) ) ) {
          prevY = intData[3];
          FastLED.show();
        }
        break;
      case 6:
        loadingFlag = true;
        // строка принимается в переменную runningText
        break;
      case 7:
        if (intData[1] == 1) runningFlag = true;
        if (intData[1] == 0) runningFlag = false;
        break;
      case 8:
        if (intData[1] == 0) {
          effect = intData[2];
          gameFlag = false;
          loadingFlag = true;
          breathBrightness = globalBrightness;
          FastLED.setBrightness(globalBrightness);    // возвращаем яркость
          globalSpeed = intData[3];
          gameTimer.setInterval(globalSpeed * 4);
        }
        else if (intData[1] == 1) effectsFlag = !effectsFlag;
        break;
      case 9:
        if (lastMode != 1) loadingFlag = true;    // начать новую игру при переходе со всех режимов кроме рисования
        effectsFlag = false;
        //gameFlag = true;
        game = intData[1];
        globalSpeed = intData[2];
        gameTimer.setInterval(globalSpeed * 4);
        break;
      case 10:
        buttons = 0;
        break;
      case 11:
        buttons = 1;
        break;
      case 12:
        buttons = 2;
        break;
      case 13:
        buttons = 3;
        break;
      case 14:
        gameFlag = !gameFlag;
        break;
      case 15: globalSpeed = intData[1];
        if (gameFlag) gameTimer.setInterval(globalSpeed * 4);   // для игр скорость нужно меньше!
        if (effectsFlag) effectTimer.setInterval(globalSpeed);
        if (runningFlag) scrollTimer.setInterval(globalSpeed);
        break;
    }
    lastMode = intData[0];  // запомнить предыдущий режим
  }

  // ****************** ПАРСИНГ *****************
  if (Serial.available() > 0) {
    char incomingByte;
    if (parseMode == TEXT) {     // если нужно принять строку
      runningText = Serial.readString();  // принимаем всю
      incomingByte = ending;              // сразу завершаем парс
      parseMode = NORMAL;
    } else {
      incomingByte = Serial.read();        // обязательно ЧИТАЕМ входящий символ
    }
    if (parseStarted) {                         // если приняли начальный символ (парсинг разрешён)
      if (incomingByte != divider && incomingByte != ending) {   // если это не пробел И не конец
        string_convert += incomingByte;       // складываем в строку
      } else {                                // если это пробел или ; конец пакета
        if (index == 0) {
          byte thisMode = string_convert.toInt();
          if (thisMode == 0 || thisMode == 5) parseMode = COLOR;    // передача цвета (в отдельную переменную)
          else if (thisMode == 6) parseMode = TEXT;
          else parseMode = NORMAL;
          //if (thisMode != 7 || thisMode != 0) runningFlag = false;
        }

        if (index == 1) {       // для второго (с нуля) символа в посылке
          if (parseMode == NORMAL) intData[index] = string_convert.toInt();             // преобразуем строку в int и кладём в массив}
          if (parseMode == COLOR) globalColor = strtol(&string_convert[0], NULL, 16);     // преобразуем строку HEX в цифру
        } else {
          intData[index] = string_convert.toInt();  // преобразуем строку в int и кладём в массив
        }
        string_convert = "";                  // очищаем строку
        index++;                              // переходим к парсингу следующего элемента массива
      }
    }
    if (incomingByte == header) {             // если это $
      parseStarted = true;                      // поднимаем флаг, что можно парсить
      index = 0;                              // сбрасываем индекс
      string_convert = "";                    // очищаем строку
    }
    if (incomingByte == ending) {             // если таки приняли ; - конец парсинга
      parseMode == NORMAL;
      parseStarted = false;                     // сброс
      recievedFlag = true;                    // флаг на принятие
    }
  }
}
#elif (BT_MODE == 0)
void bluetoothRoutine() {
  return;
}
#endif
