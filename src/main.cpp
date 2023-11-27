#include "engine/window.hpp"
int main() {
    rose::window *gamewindow = new rose::window("Sample", "rose-small", false);

    gamewindow->addFont(rose::font("../src/data/interface/crimson-pro.ttf", 32, 32, 128, gamewindow->dim().x, gamewindow->dim().y));
    gamewindow->addFont(rose::font("../src/data/interface/impact.ttf", 32, 32, 128, gamewindow->dim().x, gamewindow->dim().y));

    gamewindow->start();
    return 0;
}