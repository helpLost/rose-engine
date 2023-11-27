#ifndef SRC_ENGINE_GRAPHICS_HPP
#define SRC_ENGINE_GRAPHICS_HPP

    #include <string>
    #include <iostream>
    #include <map>
    #include <GLAD/glad.h>
    #include <GLAD/glm.hpp>
    #include <GLFW/glfw3.h>
    #include <ft2build.h>
    #include FT_FREETYPE_H
    namespace helltaken {
        inline void endprogram(std::string error) { glfwTerminate(); throw std::runtime_error(error.c_str()); }; inline void endprogram() { glfwTerminate(); }
        class shader
        {
            private: unsigned programID;
            public:
                shader(std::string name);
                void useShader() { glUseProgram(programID); }; unsigned ID() { return programID; }

                void setBool(const std::string &name, bool value) const { glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value); }
                void setInt(const std::string &name, int value) const { glUniform1i(glGetUniformLocation(programID, name.c_str()), value); }
                void setFloat(const std::string &name, float value) const { glUniform1f(glGetUniformLocation(programID, name.c_str()), value); }
        };
        class font
        {
            private:
                struct character {
                    unsigned TextureID;
                    glm::ivec2 Size;
                    glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
                    long Advance;   // Offset to advance to next glyph
                };
                unsigned VAO, VBO;
                std::map<char, character> Characters;
                shader fontshader = shader("font");
            public:
                font(const char *path, int width, int height, int characterNum, int wwidth, int wheight);
                void renderText(std::string text, float x, float y, float scale, glm::vec3 color);
        };
        class texture {
            private:
            public:
        };
        class textbox {
            private:
                bool SKIPTEXT;
                int WPR; // words per render
                std::string SPEAKER, TEXT;
                int BACKGROUND; // change to image
            public:
                textbox(std::string speaker, std::string text, std::string background, int wpr);
                void render();
        };
        class button {
            private:
            public:
        };
        class screen {
        
        };
    }

#endif