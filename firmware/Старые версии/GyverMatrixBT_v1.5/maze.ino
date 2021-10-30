// игра лабиринт!

// ***************** НАСТРОЙКИ ГЕНЕРАЦИИ ЛАБИРИНТА *****************
#define GAMEMODE 0        // режим игры: 0 - видим весь лабиринт, 1 - видим вокруг себя часть
#define FOV 3             // область видимости в режиме игры 1

#define MAZE_WIDTH 17     // ширина лабиринта (ДОЛЖНА БЫТЬ НЕЧЁТНАЯ!!!!111)
#define MAZE_HEIGHT 17    // высота лабиринта (ДОЛЖНА БЫТЬ НЕЧЁТНАЯ!!!!111)
#define SHIFT 1           // (1 да / 0 нет) смещение лабиринта (чтобы не видеть нижнюю и левую стену)
// при SHIFT 1 размер лабиринта можно ставить на 1 длиннее матрицы

#define MAX_STRAIGHT 7    // максимальная длина прямых отрезков пути по краям
#define HOLES 2           // количество дырок в стенах
#define MIN_PATH 20       // минимальная длина пути к выходу
// не рекомендуется ставить больше, чем треть количества светодиодов на матрице!

// --------------------- ДЛЯ РАЗРАБОТЧИКОВ ----------------------
const uint16_t maxSolves = MAZE_WIDTH * MAZE_WIDTH * 5;
char *maze = (char*)malloc(MAZE_WIDTH * MAZE_HEIGHT * sizeof(char));;
byte playerPos[2];
uint32_t labTimer;

void newGameMaze() {
  FastLED.clear();
  FastLED.show();

  playerPos[0] = !SHIFT;
  playerPos[1] = !SHIFT;

  smartMaze();   // "умная" генерация лабиринта
  makeHoles();   // дырявим несколько стен
  // GenerateMaze(maze, MAZE_WIDTH, MAZE_HEIGHT);    // генерировать лабиринт

  if (!GAMEMODE) {
    for (byte y = 0; y < HEIGHT; y++) {
      for (byte x = 0; x < WIDTH; x++) {
        switch (maze[(y + SHIFT) * MAZE_WIDTH + (x + SHIFT)]) {
          case 1:  drawPixelXY(x, y, GLOBAL_COLOR_1);  break;
          case 2:
            // drawPixelXY(x, y, GLOBAL_COLOR_1);    // рисовать путь к выходу
            break;
          default: drawPixelXY(x, y, 0x000000);  break;
        }
      }
      FastLED.show();
    }
  } else {
    for (byte y = 0; y < FOV; y++) {
      for (byte x = 0; x < FOV; x++) {
        switch (maze[(y + SHIFT) * MAZE_WIDTH + (x + SHIFT)]) {
          case 1:  drawPixelXY(x, y, GLOBAL_COLOR_1);  break;
          case 2:
            // drawPixelXY(x, y, GLOBAL_COLOR_1);    // рисовать путь к выходу
            break;
          default: drawPixelXY(x, y, 0x000000);  break;
        }
      }
    }
  }
  drawPixelXY(playerPos[0], playerPos[1], GLOBAL_COLOR_2);
  FastLED.show();

  labTimer = millis();
}

void mazeRoutine() {
  buttonsTickMaze();
}

void buttonsTickMaze() {
  if (checkButtons()) {
    if (buttons[3]) {   // кнопка нажата
      int8_t newPos = playerPos[0] - 1;
      if (newPos >= 0 && newPos <= WIDTH - 1)
        if (getPixColorXY(newPos, playerPos[1]) == 0) {
          movePlayer(newPos, playerPos[1], playerPos[0], playerPos[1]);
          playerPos[0] = newPos;
        }
      buttons[3] = 0;
    }
    if (buttons[1]) {   // кнопка нажата
      int8_t newPos = playerPos[0] + 1;
      if (newPos >= 0 && newPos <= WIDTH - 1)
        if (getPixColorXY(newPos, playerPos[1]) == 0) {
          movePlayer(newPos, playerPos[1], playerPos[0], playerPos[1]);
          playerPos[0] = newPos;
        }
      buttons[1] = 0;
    }
    if (buttons[0]) {   // кнопка нажата
      int8_t newPos = playerPos[1] + 1;
      if (newPos >= 0 && newPos <= HEIGHT - 1)
        if (getPixColorXY(playerPos[0], newPos) == 0) {
          movePlayer(playerPos[0], newPos, playerPos[0], playerPos[1]);
          playerPos[1] = newPos;
        }
      buttons[0] = 0;
    }
    if (buttons[2]) {   // кнопка нажата
      int8_t newPos = playerPos[1] - 1;
      if (newPos >= 0 && newPos <= HEIGHT - 1)
        if (getPixColorXY(playerPos[0], newPos) == 0) {
          movePlayer(playerPos[0], newPos, playerPos[0], playerPos[1]);
          playerPos[1] = newPos;
        }
      buttons[2] = 0;
    }
  }
}

void movePlayer(int8_t nowX, int8_t nowY, int8_t prevX, int8_t prevY) {
  drawPixelXY(prevX, prevY, 0x000000);
  drawPixelXY(nowX, nowY, GLOBAL_COLOR_2);
  FastLED.show();
  if ((nowX == (MAZE_WIDTH - 2) - SHIFT) && (nowY == (MAZE_HEIGHT - 1) - SHIFT)) {
    loadingFlag = true;
    FastLED.clear();
    FastLED.show();
    displayScore((millis() - labTimer) / 1000);
    delay(1000);
    return;
  }
  if (GAMEMODE) {
    for (int8_t y = nowY - FOV; y < nowY + FOV; y++) {
      for (int8_t x = nowX - FOV; x < nowX + FOV; x++) {
        if (x < 0 || x > WIDTH - 1 || y < 0 || y > WIDTH - 1) continue;
        if (maze[(y + SHIFT) * MAZE_WIDTH + (x + SHIFT)] == 1) {
          drawPixelXY(x, y, GLOBAL_COLOR_1);
        }
      }
      FastLED.show();
    }
  }
}
// функция, перегенерирующая лабиринт до тех пор,
// пока он не будет соответствовать требованиям "интересности"
void smartMaze() {
  byte sum, line;
  int attempt;
  while (sum < MIN_PATH) {                  // пока длина пути меньше заданной
    attempt++;
    //randomSeed(millis());                 // зерно для генератора псевдослучайных чисел
    GenerateMaze(maze, MAZE_WIDTH, MAZE_HEIGHT);    // генерировать лабиринт
    SolveMaze(maze, MAZE_WIDTH, MAZE_HEIGHT);       // найти путь

    // подсчёт длины пути
    sum = 0;
    for (byte y = 0; y < MAZE_HEIGHT; y++) {        // перебор всех пикселей (элементов массива maze)
      for (byte x = 0; x < MAZE_WIDTH; x++) {
        if (maze[y * MAZE_WIDTH + x] == 2) sum++;   // в массиве maze пиксель "правильного пути" имеет значение 2
      }
    }
    if (MAX_STRAIGHT > 0) {
      // подсчёт длины путей у краёв
      // если сумма превышает настройку, обнулить sum, таким образом цикл начнётся по новой
      line = 0;
      for (byte y = 0; y < MAZE_HEIGHT; y++)
        if (maze[y * MAZE_WIDTH + MAZE_WIDTH - 2] == 2) line++;
      if (line > MAX_STRAIGHT) sum = 0;

      line = 0;
      for (byte x = 0; x < MAZE_WIDTH; x++)
        if (maze[MAZE_WIDTH + x] == 2) line++;
      if (line > MAX_STRAIGHT) sum = 0;
    }
  }
}

// функция, делающая отверстия в стенках для "неидеальности" лабиринта
void makeHoles() {
  byte holes = 0;
  byte attempt;
  while (holes < HOLES) {                           // пока текущее число дыр меньше заданного
    attempt++;                                      // прибавляем число попыток
    if (attempt > 200) break;                       // если совершено более 200 попыток, завершить алгоритм (если зависнет)

    // берём случайные координаты внутри лабиринта
    byte x = random(1, MAZE_WIDTH - 1);
    byte y = random(1, MAZE_HEIGHT - 1);

    // итак, элемент массива maze, имеющий значение 1, является "стенкой" лабиринта. Погнали:
    if (maze[y * MAZE_WIDTH + x] == 1                   // если текущий пиксель является стенкой
        && maze[y * MAZE_WIDTH + (x - 1)] == 1          // и если рядом с ним по оси Х есть сосед
        && maze[y * MAZE_WIDTH + (x + 1)] == 1          // и с другой стороны тоже есть
        && maze[(y - 1) * MAZE_WIDTH + x] != 1          // а по оси У нет ни одного соседа
        && maze[(y + 1) * MAZE_WIDTH + x] != 1) {       // (это чтобы не дырявить угловые стенки)
      maze[y * MAZE_WIDTH + x] = 0;                     // сделать стенку проходом (пробить дыру)
      holes++;                                      // увеличить счётчик число дыр в стенах
    } else if (maze[y * MAZE_WIDTH + x] == 1            // если нет, проверим ещё раз
               && maze[(y - 1) * MAZE_WIDTH + x] == 1   // и если рядом с ним по оси У есть сосед
               && maze[(y + 1) * MAZE_WIDTH + x] == 1   // и с другой стороны тоже есть
               && maze[y * MAZE_WIDTH + (x - 1)] != 1   // а по оси X нет ни одного соседа
               && maze[y * MAZE_WIDTH + (x + 1)] != 1) {
      maze[y * MAZE_WIDTH + x] = 0;                     // сделать стенку проходом (пробить дыру)
      holes++;                                      // увеличить счётчик число дыр в стенах
    }
  }
}

// копаем лабиринт
void CarveMaze(char *maze, int width, int height, int x, int y) {
  int x1, y1;
  int x2, y2;
  int dx, dy;
  int dir, count;

  dir = random() % 4;
  count = 0;
  while (count < 4) {
    dx = 0; dy = 0;
    switch (dir) {
      case 0:  dx = 1;  break;
      case 1:  dy = 1;  break;
      case 2:  dx = -1; break;
      default: dy = -1; break;
    }
    x1 = x + dx;
    y1 = y + dy;
    x2 = x1 + dx;
    y2 = y1 + dy;
    if (   x2 > 0 && x2 < width && y2 > 0 && y2 < height
           && maze[y1 * width + x1] == 1 && maze[y2 * width + x2] == 1) {
      maze[y1 * width + x1] = 0;
      maze[y2 * width + x2] = 0;
      x = x2; y = y2;
      dir = random() % 4;
      count = 0;
    } else {
      dir = (dir + 1) % 4;
      count += 1;
    }
  }
}

// генерацтор лабиринта
void GenerateMaze(char *maze, int width, int height) {
  int x, y;
  for (x = 0; x < width * height; x++) {
    maze[x] = 1;
  }
  maze[1 * width + 1] = 0;
  for (y = 1; y < height; y += 2) {
    for (x = 1; x < width; x += 2) {
      CarveMaze(maze, width, height, x, y);
    }
  }
  // вход и выход
  maze[0 * width + 1] = 0;
  maze[(height - 1) * width + (width - 2)] = 0;
}

// решатель (ищет путь)
void SolveMaze(char *maze, int width, int height) {
  int dir, count;
  int x, y;
  int dx, dy;
  int forward;
  /* Remove the entry and exit. */
  maze[0 * width + 1] = 1;
  maze[(height - 1) * width + (width - 2)] = 1;

  forward = 1;
  dir = 0;
  count = 0;
  x = 1;
  y = 1;
  unsigned int attempts;
  while (x != width - 2 || y != height - 2) {
    if (attempts++ > maxSolves) {   // если решатель не может найти решение (maxSolves в 5 раз больше числа клеток лабиринта)
      //Serial.println("pizdetc");    // ну пиздец, что сказать
      break;                        // прервать решение
    }
    dx = 0; dy = 0;
    switch (dir) {
      case 0:  dx = 1;  break;
      case 1:  dy = 1;  break;
      case 2:  dx = -1; break;
      default: dy = -1; break;
    }
    if (   (forward  && maze[(y + dy) * width + (x + dx)] == 0)
           || (!forward && maze[(y + dy) * width + (x + dx)] == 2)) {
      maze[y * width + x] = forward ? 2 : 3;
      x += dx;
      y += dy;
      forward = 1;
      count = 0;
      dir = 0;
    } else {
      dir = (dir + 1) % 4;
      count += 1;
      if (count > 3) {
        forward = 0;
        count = 0;
      }
    }
  }
  /* Replace the entry and exit. */
  maze[(height - 2) * width + (width - 2)] = 2;
  maze[(height - 1) * width + (width - 2)] = 2;
}
