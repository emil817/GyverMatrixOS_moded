# Bluetooth адресная матрица на Arduino
* [Описание проекта](#chapter-0)
* [Папки проекта](#chapter-1)
* [Схемы подключения](#chapter-2)

### Этот проект является дополнением проекта AlexGyver GyverMatrixBT с режимами из vvip-68 GyverPanelWiFi.
### Это прошивка для гирлянд на ws2812b с управлением по bluetooth, основа - GyverMatrixBT, добавлены режимы из прошивки vvip-68.

[Ссылка сразу на прошивку](https://github.com/emil817/GyverMatrixOS_moded/tree/main/firmware/GyverMatrixOS_v1.12)

<a id="chapter-0"></a>
## Описание проекта
Матрица из адресных светодиодов с управлением по bluetooth.

https://youtu.be/m3FgDEwhsDs

https://github.com/AlexGyver/GyverMatrixBT
https://alexgyver.ru/gyvermatrixbt/

https://github.com/vvip-68/GyverPanelWiFi


Гайд по матрицам от AlexGyver: https://alexgyver.ru/matrix_guide/

#### Особенности:
 - Управление со смартфона по Bluetooth
#### Режимы:
 - Рисование
 - Загрузка картинок
 - Бегущая строка  
#### Эффекты:
 - "Дыхание" яркости
 - Смена цвета
 - Снегопад
 - Блуждающий кубик
 - Радуга
 - Огонь
 - The Matrix
 - Летающие частицы
 - Линии  
 - Псевдоанализатор спектра
 - Пейнтбол
 - Светлячки
 - Палитра
 - Тени
 - Синусы
 - Вышиванка

#### Игры:
 - Змейка
 - Tетриc
 - Бегалка-прыгалка
 - Арканоид 
#### Возможности:
- Автоподключение к матрице при запуске
- Настройки яркости и скорости отображения
- Использование акселерометра в играх

<a id="chapter-1"></a>
## Папки
**ВНИМАНИЕ! Если это твой первый опыт работы с Arduino, читай [инструкцию](#chapter-4)**
- **libraries** - библиотеки проекта. Заменить имеющиеся версии (в этом проекте внешних библиотек нет)
- **firmware** - прошивки для Arduino
- **schemes** - схемы подключения компонентов
- **Android** - файлы с приложениями, примерами для Android и Thunkable

<a id="chapter-2"></a>
## Схемы
![SCHEME](https://github.com/AlexGyver/GyverMatrixBT/blob/master/schemes/scheme.jpg)

Подробную информацию по подключению матрицы и загрузке программы можно найти:
https://alexgyver.ru/gyvermatrixbt/
