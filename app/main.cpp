#include <duck/window.hpp>
#include <duck/controllers/mainController.hpp>


int main()
{
    Window window(600, 400, "Modeler");

    window.RunMessageLoop();

    return 0;
}
