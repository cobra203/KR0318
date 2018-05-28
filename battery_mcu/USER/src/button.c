#include <button.h>
#include <sys_common.h>

/**********************************************************************************/
/* Button Interface Functions                                                     */
/**********************************************************************************/

int button_check_active(BUTTON_S *button)
{
    int interval = 0;
    
    /* shack process */
    if(button->state.shake) {
        if((button->state.shake == SHAKE_LAST_PRESS  && button->state.press) ||
           (button->state.shake == SHAKE_LAST_LOOSEN && !button->state.press)) {
            button->state.duration++;
        }
        else {
            button->state.shake = button->state.press ? SHAKE_LAST_PRESS : SHAKE_LAST_LOOSEN;
            button->state.duration = 0;
        }

        if(button->state.duration >= button->interval.shake) {
            button->state.shake     = 0;
            button->state.duration  = 0;
            if(!button->state.effective && button->state.press) {
                button->state.effective = ECT_PRESSED;
                button->state.avtice = 1;
            }
            else if(button->state.effective && !button->state.press) {
                button->state.effective = ECT_LOOSE;
				button->state.avtice = 1;
            }
        }
        return button->state.avtice;
    }

    switch(button->state.effective) {
    case ECT_LOOSE:
        if(button->state.press) {
            button->state.shake = SHAKE_LAST_PRESS;
        }
        break;
    case ECT_PRESSED:
    case ECT_FOCUSED:
        if(!button->state.press) {
            button->state.shake = SHAKE_LAST_LOOSEN;
            button->state.duration  = 0;
        }
        else {
            button->state.duration++;
            interval = button->state.effective == ECT_PRESSED ? button->interval.focused : button->interval.pressed;
            if(button->state.duration >= interval) {
                button->state.duration  = 0;
                button->state.effective = ECT_FOCUSED;
                button->state.avtice    = 1;
            }
        }
        break;
    }
    return button->state.avtice;
}




