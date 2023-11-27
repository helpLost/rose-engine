#include "engine/window.hpp"
int main() {
    rose::window *gamewindow = new rose::window("Sample", "simple-textbox", false);

    gamewindow->addShader(rose::shader("font")); gamewindow->addShader(rose::shader("texture"));
    gamewindow->addFont(rose::font(gamewindow->shaders[0], "../src/data/interface/crimson-pro.ttf", 32, 32, 128, gamewindow->dim().x, gamewindow->dim().y));
    gamewindow->addFont(rose::font(gamewindow->shaders[0], "../src/data/interface/impact.ttf", 32, 32, 128, gamewindow->dim().x, gamewindow->dim().y));

    gamewindow->start();
    return 0;
}