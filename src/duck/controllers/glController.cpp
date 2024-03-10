#include <duck/controllers/glController.hpp>

#include <duck/controllers/mainController.hpp>
#include <duck/utilities/setIntersection.hpp>

#include <stdexcept>


GlController::GlController(Model& model, GLFWwindow* window, MainController& controller):
    window(window), SubController(model, controller)
{
}


void GlController::MouseClick(MouseButton button)
{
    mouseState.ButtonClicked(button);
}


void GlController::MoveCursor() const
{
    auto [x, y] = MouseToViewportCoordinates();
}


void GlController::MouseMove(int x, int y)
{
    mouseState.Moved(x, y);

    if (mouseState.IsButtonClicked(MouseButton::Middle)) {
        auto offset = mouseState.TranslationGet();
        model.RotateCamera(offset.Y() * 0.02f, offset.X() * 0.02f);
    }
}


std::tuple<float,float> GlController::MouseToViewportCoordinates() const
{
    int windowWidth, windowHeight;

    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    int halfWidth = windowWidth / 2;
    int halfHeight = windowHeight / 2;

    auto mousePos = mouseState.PositionGet();
    float x = (float)(mousePos.X() - halfWidth) / (float)halfWidth;
    float y = -(float)(mousePos.Y() - halfHeight) / (float)halfHeight;

    return std::make_tuple(x, y);
}


void GlController::ScrollMoved(int offset)
{
    float val = offset * SCROLL_COEFF;

    if (val < 0.0f) {
        val = (-1.0f) / val;
    }

    model.MultiplyCameraDistanceFromTarget(val);
}
