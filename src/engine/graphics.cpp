#include "graphics.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GLAD/gtc/type_ptr.hpp>
#include <GLAD/gtc/matrix_transform.hpp>
#include <STB/stb_image.h>

void compileShader(unsigned &shader, const char* source, int type), compileProgram(unsigned &programID, unsigned &vertex, unsigned &fragment);
void createVertexObject(unsigned &VAO, unsigned &VBO, float vertices[], float vertSize, int drawType, GLint vertexAttribSize, bool texture, GLsizei stride, const void* pointer);
void createVertexObjectEBO(unsigned &VAO, unsigned &VBO, unsigned &EBO, float vertices[], unsigned int indices[], float vertSize, float indSize, int drawType, GLint vertexAttribSize, bool texture, GLsizei stride, const void* pointer);
namespace rose {
    shader::shader(std::string name)
    {
        unsigned vertex, fragment; std::string vs, fs;
        std::ifstream vfile("../src/data/shaders/" + name + ".vs"), ffile("../src/data/shaders/" + name + ".fs");
        if(vfile && ffile) {
            std::stringstream v, f; v << vfile.rdbuf(); f << ffile.rdbuf();
            vs = v.str(); fs = f.str(); vfile.close(); ffile.close();
        } else {
            std::cout << "A shader file failed to be read! This is probably the result of a missing file, and though this itself is not a fatal error, it will most certainly cause issues with rendering down the line." << std::endl;
        }
        const char* vertexSource = vs.c_str(), *fragmentSource = fs.c_str();
        compileShader(vertex, vertexSource, GL_VERTEX_SHADER); compileShader(fragment, fragmentSource, GL_FRAGMENT_SHADER);
        compileProgram(programID, vertex, fragment);
    }
    font::font(shader fontshader, const char *path, int width, int height, int characterNum, int wwidth, int wheight)
    {
        FT_Library ft; FT_Face face; if(FT_Init_FreeType(&ft)) { endprogram("Freetype failed to initialize on the font with path '" + std::string(path) + "'. Something's very wrong."); }
        if(std::string(path).empty()) { endprogram("There is a font face with a null filename. This will cause too many problems for me to allow continuation."); }
        if (FT_New_Face(ft, path, 0, &face)) { endprogram("The font with path '" + std::string(path) + "' was not able to be found. A file is probably missing."); }
        else {
            FT_Set_Pixel_Sizes(face, height, width); int dlineSpacing = face->height; int slineSpacing = face->size->metrics.height;
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
            for (unsigned char c = 0; c < characterNum; c++)
            {
                if (FT_Load_Char(face, c, FT_LOAD_RENDER)) { std::cout << "Failed to load glyph " << c << " on font with filename '" << path << "'." << std::endl; }

                unsigned texture;
                glGenTextures(1, &texture); glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                character chara = {texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), static_cast<long>(face->glyph->advance.x)};
                Characters.insert(std::pair<char, character>(c, chara));
            }
            glBindTexture(GL_TEXTURE_2D, 0);
            createVertexObject(VAO, VBO, NULL, sizeof(float) * 6 * 4, GL_DYNAMIC_DRAW, 4, false, 4 * sizeof(float), 0);
        }
        fontshader.useShader();
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(wwidth), 0.0f, static_cast<float>(wheight));
        glUniformMatrix4fv(glGetUniformLocation(fontshader.ID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        FT_Done_Face(face); FT_Done_FreeType(ft);
    }
    void font::renderText(shader fontshader, std::string text, float x, float y, float scale, glm::vec3 color)
    {
        glEnable(GL_CULL_FACE); glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        fontshader.useShader();
        glUniform3f(glGetUniformLocation(fontshader.ID(), "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            character ch = Characters[*c];

            float xpos = x + ch.Bearing.x * scale, ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
            float w = ch.Size.x * scale, h = ch.Size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                {xpos, ypos + h, 0.0f, 0.0f},
                {xpos, ypos, 0.0f, 1.0f},
                {xpos + w, ypos, 1.0f, 1.0f},

                {xpos, ypos + h, 0.0f, 0.0f},
                {xpos + w, ypos, 1.0f, 1.0f},
                {xpos + w, ypos + h, 1.0f, 0.0f}};

            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, VBO); glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0); glDrawArrays(GL_TRIANGLES, 0, 6);
            x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0); glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_CULL_FACE); glDisable(GL_BLEND);
    }

    texture::texture(std::string name, float width, float height) {
        float vertices[] = {
            // positions          // texture coords
            width, height, 0.0f,  1.0f, 1.0f,    width,-height, 0.0f,  1.0f, 0.0f, // top right, bottom right
           -width,-height, 0.0f,  0.0f, 0.0f,   -width, height, 0.0f,  0.0f, 1.0f  // bottom left, top left 
        };
        createVertexObjectEBO(VAO, VBO, EBO, vertices, INDICES, sizeof(vertices), sizeof(INDICES), GL_STATIC_DRAW, 3, true, 5 * sizeof(float), (void*)0);

        glGenTextures(1, &TEXTUREID); glBindTexture(GL_TEXTURE_2D, TEXTUREID); // all upcoming texture operations now have effect on this texture object
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);  
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int w, h, channels; std::string path = "../src/data/interface/" + name;
        stbi_set_flip_vertically_on_load(true); unsigned char *data = stbi_load(path.c_str(), &w, &h, &channels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else { std::cout << "Failed to load the texture of name '" << name << "'. There's probably a file missing." << std::endl; }
        stbi_image_free(data);
    }
    void texture::render(shader textureshader) {
        textureshader.useShader(); textureshader.setInt("texture1", 0);
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, TEXTUREID); glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    textbox::textbox(std::string speaker, std::string text, std::string background, int wpr)
        :SPEAKER(speaker), TEXT(text), WPR(wpr)
    {
        // background loading
    }
    void textbox::render() {
        // renders [WPR] words every time the render is called, unless SKIPTEXT is set to true.
    }
}

void compileShader(unsigned &shader, const char* source, int type) {
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success; char errorLog[512]; glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, errorLog);
        std::cout << "'" << type << "' shader compilation failed. Error: " << errorLog << std::endl;
    };
}
void compileProgram(unsigned &programID, unsigned &vertex, unsigned &fragment) {
    programID = glCreateProgram(); glAttachShader(programID, vertex); glAttachShader(programID, fragment); glLinkProgram(programID);
    int success; char errorLog[512]; glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programID, 512, NULL, errorLog); // Log the issue to our printable one.
        std::cout << "Final shader linking failed. This is not a fatal error, but will undoubtably cause issues later. Error: " << errorLog << std::endl;
    }

    glDeleteShader(vertex); glDeleteShader(fragment);
}

void createVertexObject(unsigned &VAO, unsigned &VBO, float vertices[], float vertSize, int drawType, GLint vertexAttribSize, bool texture, GLsizei stride, const void* pointer) {
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO); glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, drawType);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, vertexAttribSize, GL_FLOAT, GL_FALSE, stride, pointer);
    if (texture) {
        glEnableVertexAttribArray(2);  
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }
}
void createVertexObjectEBO(unsigned &VAO, unsigned &VBO, unsigned &EBO, float vertices[], unsigned int indices[], float vertSize, float indSize, int drawType, GLint vertexAttribSize, bool texture, GLsizei stride, const void* pointer) {
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO); glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, drawType);
    glGenBuffers(1, &EBO); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indSize, indices, drawType);
    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, vertexAttribSize, GL_FLOAT, GL_FALSE, stride, pointer);
    if (texture) {
        glEnableVertexAttribArray(2);  
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }
}