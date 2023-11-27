#include "engine/window.hpp"
int main() {
    helltaken::window *gamewindow = new helltaken::window("Sample", "helltaken-small", false);

    gamewindow->addFont(helltaken::font("../src/data/interface/crimson-pro.ttf", 32, 32, 128, gamewindow->dim().x, gamewindow->dim().y));
    gamewindow->addFont(helltaken::font("../src/data/interface/impact.ttf", 32, 32, 128, gamewindow->dim().x, gamewindow->dim().y));

    gamewindow->start();
}