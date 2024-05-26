#include <duck/controllers/utils/mouseState.hpp>


MouseState::MouseState():
    oldMousePos(0), actMousePos(0)
{
    for (int i = 0; i < BUTTON_CNT; i++) {
        buttonClicked[i] = false;
    }
}


void MouseState::Moved(int x, int y)
{
    oldMousePos = actMousePos;

    actMousePos.X() = x;
    actMousePos.Y() = y;
}
