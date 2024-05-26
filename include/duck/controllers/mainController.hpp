#pragma once

#include "glController.hpp"
#include "guiController.hpp"
#include "../views/guiView.hpp"
#include "../model.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


class MainController {
public:
    MainController(GLFWwindow* window, int window_width, int window_height);
    
    void Render();

    inline void SizeChanged(int width, int height) { glController.WindowSizeChanged(width, height); }
    void MouseClicked(MouseButton button);
    void MouseReleased(MouseButton button);
    void MouseMoved(int x, int y);
    void ScrollMoved(int offset);

private:
    Model model;

    GlController glController;
    GuiController guiController;

    GuiView guiView;
};
