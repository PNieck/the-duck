#pragma once

#include "../model.hpp"


class MainController;


class SubController {
public:
    SubController(Model& model, MainController& controller):
        model(model),  mainController(controller) {}

protected:
    Model& model;

private:
    MainController& mainController;
};
