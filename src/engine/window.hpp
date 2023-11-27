#ifndef SRC_ENGINE_WINDOW_HPP
#define SRC_ENGINE_WINDOW_HPP

    #include <vector>
    #include <GLAD/glm.hpp>
    #include "graphics.hpp"
    namespace helltaken {
        enum envtype{DEBUG, PRERELEASE, RELEASE};
        class monitor {
            private: const GLFWvidmode *resolution; GLFWmonitor *primary;
            public:
                glm::vec2 dimensions() { return glm::vec2(resolution->width, resolution->height); }
                monitor();
        };
        class window {
            private:
                int WIDTH, HEIGHT;
                bool VYSNC = true, ANTIALIASING = true;
                std::string TITLE; monitor PRIMARY; envtype ENVIRONMENT = DEBUG;
            public:
                GLFWwindow *instance; float background[3] = {0.0f, 0.0f, 0.0f}; // init to pure black
                std::vector<font> fonts; std::vector<screen> screens;

                window(std::string title, std::string icon, bool autostart); void changebg(glm::vec3 newbg) { background[0] = newbg.x; background[1] = newbg.y; background[2] = newbg.z; } 
                void addFont(font newface) { fonts.push_back(newface); } void addscreen(screen newscreen) { screens.push_back(newscreen); }
                void start(), render(), update();

                glm::ivec2 dim() { return glm::ivec2(WIDTH, HEIGHT); }
        };
    }

#endif