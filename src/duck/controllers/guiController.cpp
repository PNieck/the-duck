#include <duck/controllers/guiController.hpp>

#include <stdexcept>


GuiController::GuiController(Model & model, MainController & controller):
    SubController(model, controller)
{
}
