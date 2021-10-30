
#if (USE_BUTTONS == 1)
#include "buttonMinim.h"
uint32_t commonBtnTimer;

buttonMinim bt_left(BUTT_RIGHT);
buttonMinim bt_right(BUTT_LEFT);
buttonMinim bt_up(BUTT_UP);
buttonMinim bt_down(BUTT_DOWN);
buttonMinim bt_set(BUTT_SET);
#endif

boolean checkButtons() {
#if (USE_BUTTONS == 1)
  if (!gameDemo) {
    if (bt_left.pressed(&commonBtnTimer)) {
      buttons = 3;
      controlFlag = true;
    }
    if (bt_right.pressed(&commonBtnTimer)) {
      buttons = 1;
      controlFlag = true;
    }
    if (bt_up.pressed(&commonBtnTimer)) {
      buttons = 0;
      controlFlag = true;
    }
    if (bt_down.pressed(&commonBtnTimer)) {
      buttons = 2;
      controlFlag = true;
    }
  }
#endif

  // если нажали кнопку, возврат к обычному режиму
  if (controlFlag) {    
    idleTimer.reset();
    controlFlag = false;
    idleState = false;
    gameDemo = false;
    return true;
  }

  if (buttons != 4) return true;
  return false;
}
