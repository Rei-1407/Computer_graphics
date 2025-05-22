#define GLEW_STATIC 
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <sstream> 
#include <filesystem> 

// OpenGL Error Checking Macro
#define GL_CHECK(stmt) do { \
    stmt; \
    checkOpenGLError(#stmt, __FILE__, __LINE__); \
} while (0)

void checkOpenGLError(const char* stmt, const char* fname, int line) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error " << err << " (0x" << std::hex << err << std::dec << ") at " << stmt << " in " << fname << ":" << line << std::endl;
        // You might want to set a breakpoint here during debugging
    }
}


struct CharInfo {
    float u1 = 0.0f, v1 = 0.0f, u2 = 0.0f, v2 = 0.0f;
    float width = 0.0f, height = 0.0f;
    float advanceX = 0.0f;
    float offsetX = 0.0f, offsetY = 0.0f; // From BMFont: xoffset, yoffset (distance from top of line to top of char)
};

struct Vertex3D {
    glm::vec3 position;
    glm::vec2 texCoords;
};

SDL_Window* gWindow = nullptr;
SDL_GLContext gContext;
GLuint gShaderProgramID = 0;
GLuint gFontTextureID = 0;

std::map<char, CharInfo> gFontMap;
float gFontAtlasWidth = 0;
float gFontAtlasHeight = 0;
float gFontLineHeight = 0;
float gFontBase = 0;

GLuint gTextVAO = 0, gTextVBO = 0;
std::vector<Vertex3D> gAllTextVertices;

size_t gVertexOffset_Part1 = 0, gVertexCount_Part1 = 0;
size_t gVertexOffset_Part2 = 0, gVertexCount_Part2 = 0;
size_t gVertexOffset_Part3 = 0, gVertexCount_Part3 = 0;

float gTextBlockWidth_Part1 = 0.0f;
float gTextBlockWidth_Part2 = 0.0f;
float gTextBlockWidth_Part3 = 0.0f;

float gMonumentRotationX = 15.0f;
float gCameraDistance = 150.0f;
float gCameraYaw = 0.0f;
float gCameraPitch = 10.0f;


const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

const char* VERTEX_SHADER_SOURCE = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)glsl";

const char* FRAGMENT_SHADER_SOURCE = R"glsl(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D textTexture;
uniform vec3 textColor;

void main() {
    vec4 texColor = texture(textTexture, TexCoord);
    if(texColor.a < 0.1) 
        discard;
    FragColor = vec4(textColor, 1.0) * texColor; 
}
)glsl";

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    GL_CHECK(glShaderSource(shader, 1, &source, NULL));
    GL_CHECK(glCompileShader(shader));
    GLint success;
    GL_CHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
    if (!success) {
        char infoLog[1024];
        GL_CHECK(glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog));
        std::cerr << "ERROR::SHADER::TYPE_" << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        GL_CHECK(glDeleteShader(shader));
        return 0;
    }
    return shader;
}

GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (vertexShader == 0 || fragmentShader == 0) {
        return 0;
    }
    GLuint program = glCreateProgram();
    GL_CHECK(glAttachShader(program, vertexShader));
    GL_CHECK(glAttachShader(program, fragmentShader));
    GL_CHECK(glLinkProgram(program));
    GLint success;
    GL_CHECK(glGetProgramiv(program, GL_LINK_STATUS, &success));
    if (!success) {
        char infoLog[1024];
        GL_CHECK(glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog));
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        GL_CHECK(glDeleteProgram(program));
        program = 0;
    }
    GL_CHECK(glDeleteShader(vertexShader));
    GL_CHECK(glDeleteShader(fragmentShader));
    return program;
}

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return false;
    }
    std::cout << "INFO: SDL_image initialized successfully for PNG." << std::endl;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    gWindow = SDL_CreateWindow("3D Logo Text Intro", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!gWindow) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    gContext = SDL_GL_CreateContext(gWindow);
    if (!gContext) {
        std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(gWindow);
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        std::cerr << "Error initializing GLEW! " << glewGetErrorString(glewError) << std::endl;
        SDL_GL_DeleteContext(gContext);
        SDL_DestroyWindow(gWindow);
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    if (SDL_GL_SetSwapInterval(1) < 0) {
        std::cout << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << std::endl;
    }

    GL_CHECK(glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    gShaderProgramID = createShaderProgram(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE);
    if (gShaderProgramID == 0) {
        std::cerr << "Failed to create shader program." << std::endl;
        return false;
    }
    return true;
}

void parseCommonLine(const std::string& line) {
    std::stringstream ss(line);
    std::string key_value_pair;
    while (ss >> key_value_pair) { // Read space-separated key=value pairs
        size_t eqPos = key_value_pair.find('=');
        if (eqPos != std::string::npos) {
            std::string key = key_value_pair.substr(0, eqPos);
            std::string value_str = key_value_pair.substr(eqPos + 1);
            // Remove quotes from value if any
            if (!value_str.empty() && value_str.front() == '"' && value_str.back() == '"' && value_str.length() >= 2) {
                value_str = value_str.substr(1, value_str.length() - 2);
            }
            try {
                if (key == "lineHeight") {
                    gFontLineHeight = std::stof(value_str);
                }
                else if (key == "base") {
                    gFontBase = std::stof(value_str);
                }
            }
            catch (const std::invalid_argument& ia) {
                std::cerr << "WARNING: Invalid argument for " << key << ": " << value_str << std::endl;
            }
            catch (const std::out_of_range& oor) {
                std::cerr << "WARNING: Out of range for " << key << ": " << value_str << std::endl;
            }
        }
    }
}


bool loadFontAtlas(const std::string& imagePath, const std::string& dataPath) {
    std::cout << "INFO: Attempting to load font image: [" << imagePath << "]" << std::endl;
    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if (!surface) {
        std::cerr << "Failed to load font atlas image: " << imagePath << " SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }
    std::cout << "INFO: Successfully loaded font image: " << imagePath << std::endl;

    gFontAtlasWidth = static_cast<float>(surface->w);
    gFontAtlasHeight = static_cast<float>(surface->h);

    GL_CHECK(glGenTextures(1, &gFontTextureID));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, gFontTextureID));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GLenum format;
    if (surface->format->BytesPerPixel == 4) {
        format = (surface->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA;
    }
    else if (surface->format->BytesPerPixel == 3) {
        format = (surface->format->Rmask == 0x000000ff) ? GL_RGB : GL_BGR;
    }
    else {
        std::cerr << "Unsupported font atlas image format: " << SDL_GetPixelFormatName(surface->format->format) << std::endl;
        SDL_FreeSurface(surface);
        GL_CHECK(glDeleteTextures(1, &gFontTextureID));
        gFontTextureID = 0;
        return false;
    }
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels));
    GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
    SDL_FreeSurface(surface);

    std::cout << "INFO: Attempting to open font data file: [" << dataPath << "]" << std::endl;
    std::ifstream dataFile(dataPath);
    if (!dataFile.is_open()) {
        std::cerr << "Failed to open font data file: " << dataPath << std::endl;
        GL_CHECK(glDeleteTextures(1, &gFontTextureID));
        gFontTextureID = 0;
        return false;
    }
    std::cout << "INFO: Successfully opened font data file: " << dataPath << std::endl;

    std::string line;
    bool commonLineParsed = false;
    bool firstLineIsHeight = false;

    if (std::getline(dataFile, line)) {
        if (line.rfind("common lineHeight=", 0) == 0) {
            parseCommonLine(line);
            commonLineParsed = true;
            std::cout << "INFO: Parsed BMFont common line. LineHeight: " << gFontLineHeight << ", Base: " << gFontBase << std::endl;
        }
        else {
            try {
                gFontLineHeight = std::stof(line);
                std::cout << "INFO: Font line height from simple .dat (first line): " << gFontLineHeight << std::endl;
                if (gFontBase == 0 && gFontLineHeight > 0) gFontBase = gFontLineHeight * 0.8f;
                commonLineParsed = true;
                firstLineIsHeight = true;
            }
            catch (const std::exception& e) {
                dataFile.clear();
                dataFile.seekg(0);
                gFontLineHeight = 0;
            }
        }
    }
    else {
        std::cerr << "ERROR: Font data file is empty or could not be read." << std::endl;
        dataFile.close();
        return false;
    }


    while (std::getline(dataFile, line)) {
        if (line.rfind("info face=", 0) == 0 ||
            (line.rfind("common lineHeight=", 0) == 0 && !firstLineIsHeight) || // Avoid re-parsing if already done
            line.rfind("page id=", 0) == 0 ||
            line.rfind("chars count=", 0) == 0) {
            if (line.rfind("common lineHeight=", 0) == 0 && !firstLineIsHeight) {
                parseCommonLine(line);
                commonLineParsed = true;
                std::cout << "INFO: Parsed BMFont common line (inside loop). LineHeight: " << gFontLineHeight << ", Base: " << gFontBase << std::endl;
            }
            continue;
        }

        std::stringstream ss(line);
        if (line.rfind("char id=", 0) == 0) {
            std::string token;
            int charId = 0;
            CharInfo info;
            while (ss >> token) {
                size_t eqPos = token.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = token.substr(0, eqPos);
                    std::string value_str = token.substr(eqPos + 1);
                    if (!value_str.empty() && value_str.front() == '"' && value_str.back() == '"' && value_str.length() >= 2) {
                        value_str = value_str.substr(1, value_str.length() - 2);
                    }
                    try {
                        if (key == "id") charId = std::stoi(value_str);
                        else if (key == "x") info.u1 = std::stof(value_str);
                        else if (key == "y") info.v1 = std::stof(value_str);
                        else if (key == "width") info.width = std::stof(value_str);
                        else if (key == "height") info.height = std::stof(value_str);
                        else if (key == "xoffset") info.offsetX = std::stof(value_str);
                        else if (key == "yoffset") info.offsetY = std::stof(value_str);
                        else if (key == "xadvance") info.advanceX = std::stof(value_str);
                    }
                    catch (const std::exception& e) {
                        std::cerr << "WARNING: Could not parse BMFont value for " << key << " in line: " << line << " Error: " << e.what() << std::endl;
                    }
                }
            }
            if (charId > 0) {
                info.u2 = info.u1 + info.width;
                info.v2 = info.v1 + info.height;
                info.u1 /= gFontAtlasWidth;
                info.v1 /= gFontAtlasHeight;
                info.u2 /= gFontAtlasWidth;
                info.v2 /= gFontAtlasHeight;
                gFontMap[static_cast<char>(charId)] = info;
            }
        }
        else { // Simple .dat format for char lines
            char c_code_char_simple;
            float x_s, y_s, w_s, h_s, advX_s, offX_s = 0, offY_s = 0;
            // Reset stringstream for the current line
            ss.clear();
            ss.str(line);
            if (ss >> c_code_char_simple >> x_s >> y_s >> w_s >> h_s >> advX_s) {
                if (!(ss >> offX_s)) offX_s = 0;
                if (!(ss >> offY_s)) offY_s = 0;

                CharInfo info;
                info.u1 = x_s / gFontAtlasWidth;
                info.v1 = y_s / gFontAtlasHeight;
                info.u2 = (x_s + w_s) / gFontAtlasWidth;
                info.v2 = (y_s + h_s) / gFontAtlasHeight;
                info.width = w_s;
                info.height = h_s;
                info.advanceX = advX_s;
                info.offsetX = offX_s;
                info.offsetY = offY_s;
                gFontMap[c_code_char_simple] = info;
                if (gFontLineHeight == 0 && h_s > 0 && !firstLineIsHeight) gFontLineHeight = h_s;
                if (gFontBase == 0 && gFontLineHeight > 0 && !commonLineParsed) gFontBase = gFontLineHeight * 0.8f;
            }
            else if (!line.empty() && line.find_first_not_of(" \t\n\v\f\r") != std::string::npos) { // Only warn if line is not just whitespace
                std::cerr << "WARNING: Malformed line in simple .dat: [" << line << "]" << std::endl;
            }
        }
    }
    dataFile.close();

    if (gFontMap.empty()) {
        std::cerr << "No character data loaded from " << dataPath << std::endl;
        GL_CHECK(glDeleteTextures(1, &gFontTextureID));
        gFontTextureID = 0;
        return false;
    }
    if (gFontLineHeight == 0) {
        if (!gFontMap.empty()) gFontLineHeight = gFontMap.begin()->second.height;
        if (gFontLineHeight == 0) gFontLineHeight = 16;
        std::cout << "WARNING: Font line height not determined, defaulting to " << gFontLineHeight << std::endl;
    }
    if (gFontBase == 0 && gFontLineHeight > 0) {
        gFontBase = gFontLineHeight * 0.8f;
        std::cout << "WARNING: Font base not determined, estimated to " << gFontBase << std::endl;
    }

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    std::cout << "INFO: Font loaded successfully." << std::endl;
    return true;
}

void generateTextMeshPart(const std::string& text, float scale, std::vector<Vertex3D>& allVertices, float& outTextBlockWidth) {
    float currentX = 0.0f;
    float currentY_baseline = 0.0f;
    outTextBlockWidth = 0.0f;

    for (char c : text) {
        auto it = gFontMap.find(c);
        if (c == ' ' && it == gFontMap.end()) {
            float spaceAdvance = gFontLineHeight * 0.3f * scale;
            auto questionMarkIt = gFontMap.find('?');
            if (questionMarkIt != gFontMap.end()) {
                spaceAdvance = questionMarkIt->second.advanceX * scale * 0.5f;
            }
            else if (!gFontMap.empty()) {
                spaceAdvance = gFontMap.begin()->second.advanceX * scale * 0.5f;
            }
            currentX += spaceAdvance;
            outTextBlockWidth += spaceAdvance;
            continue;
        }

        if (it == gFontMap.end()) {
            it = gFontMap.find('?');
            if (it == gFontMap.end() && !gFontMap.empty()) it = gFontMap.begin();
            if (it == gFontMap.end()) continue;
        }
        const CharInfo& info = it->second;

        float y_bearing_scaled = (gFontBase - info.offsetY) * scale;
        float x_offset_scaled = info.offsetX * scale;

        float xPos = currentX + x_offset_scaled;
        float yPos_top_of_char = currentY_baseline + y_bearing_scaled;

        float w = info.width * scale;
        float h = info.height * scale;

        glm::vec3 tl = { xPos,     yPos_top_of_char,     0.0f };
        glm::vec3 bl = { xPos,     yPos_top_of_char - h, 0.0f };
        glm::vec3 br = { xPos + w, yPos_top_of_char - h, 0.0f };
        glm::vec3 tr = { xPos + w, yPos_top_of_char,     0.0f };

        glm::vec2 uv_tl = { info.u1, info.v1 };
        glm::vec2 uv_bl = { info.u1, info.v2 };
        glm::vec2 uv_br = { info.u2, info.v2 };
        glm::vec2 uv_tr = { info.u2, info.v1 };

        allVertices.push_back({ tl, uv_tl });
        allVertices.push_back({ bl, uv_bl });
        allVertices.push_back({ br, uv_br });
        allVertices.push_back({ tl, uv_tl });
        allVertices.push_back({ br, uv_br });
        allVertices.push_back({ tr, uv_tr });

        currentX += info.advanceX * scale;
        outTextBlockWidth = currentX;
    }
}

void setupCombinedTextVBO() {
    if (gAllTextVertices.empty()) {
        std::cout << "WARNING: No vertices to setup for VBO." << std::endl;
        return;
    }

    if (gTextVAO == 0) GL_CHECK(glGenVertexArrays(1, &gTextVAO));
    if (gTextVBO == 0) GL_CHECK(glGenBuffers(1, &gTextVBO));

    GL_CHECK(glBindVertexArray(gTextVAO));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, gTextVBO));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, gAllTextVertices.size() * sizeof(Vertex3D), gAllTextVertices.data(), GL_STATIC_DRAW));

    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, position)));
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, texCoords)));
    GL_CHECK(glEnableVertexAttribArray(1));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CHECK(glBindVertexArray(0));
}


void render() {
    GL_CHECK(glClearColor(0.05f, 0.05f, 0.1f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    if (gShaderProgramID == 0 || gFontTextureID == 0 || gTextVAO == 0) return;

    GL_CHECK(glUseProgram(gShaderProgramID));

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, gCameraPitch, gCameraDistance),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    view = glm::rotate(view, glm::radians(gCameraYaw), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);

    GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(gShaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view)));
    GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(gShaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection)));
    GL_CHECK(glUniform3f(glGetUniformLocation(gShaderProgramID, "textColor"), 1.0f, 0.8f, 0.3f));

    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, gFontTextureID));
    GL_CHECK(glUniform1i(glGetUniformLocation(gShaderProgramID, "textTexture"), 0));

    GL_CHECK(glBindVertexArray(gTextVAO));

    if (gVertexCount_Part1 > 0) {
        glm::mat4 model1 = glm::mat4(1.0f);
        model1 = glm::translate(model1, glm::vec3(-gTextBlockWidth_Part1 / 2.0f, 30.0f, 10.0f));
        model1 = glm::rotate(model1, glm::radians(gMonumentRotationX), glm::vec3(1.0f, 0.0f, 0.0f));
        GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(gShaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model1)));
        GL_CHECK(glDrawArrays(GL_TRIANGLES, gVertexOffset_Part1, gVertexCount_Part1));
    }

    if (gVertexCount_Part2 > 0) {
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(-gTextBlockWidth_Part2 / 2.0f, 0.0f, 0.0f));
        model2 = glm::rotate(model2, glm::radians(gMonumentRotationX), glm::vec3(1.0f, 0.0f, 0.0f));
        GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(gShaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model2)));
        GL_CHECK(glDrawArrays(GL_TRIANGLES, gVertexOffset_Part2, gVertexCount_Part2));
    }

    if (gVertexCount_Part3 > 0) {
        glm::mat4 model3 = glm::mat4(1.0f);
        model3 = glm::translate(model3, glm::vec3(-gTextBlockWidth_Part3 / 2.0f, -40.0f, -10.0f));
        model3 = glm::rotate(model3, glm::radians(gMonumentRotationX), glm::vec3(1.0f, 0.0f, 0.0f));
        GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(gShaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model3)));
        GL_CHECK(glDrawArrays(GL_TRIANGLES, gVertexOffset_Part3, gVertexCount_Part3));
    }

    GL_CHECK(glBindVertexArray(0));
    GL_CHECK(glUseProgram(0));
}

void cleanup() {
    if (gTextVAO != 0) GL_CHECK(glDeleteVertexArrays(1, &gTextVAO));
    if (gTextVBO != 0) GL_CHECK(glDeleteBuffers(1, &gTextVBO));
    if (gFontTextureID != 0) GL_CHECK(glDeleteTextures(1, &gFontTextureID));
    if (gShaderProgramID != 0) GL_CHECK(glDeleteProgram(gShaderProgramID));
    SDL_GL_DeleteContext(gContext);
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* args[]) {
    std::cout << "INFO: Program starting." << std::endl;
    if (!init()) {
        std::cerr << "Initialization failed!" << std::endl;
        cleanup();
#ifdef _WIN32
        std::cout << "Press any key to exit..." << std::endl;
        std::cin.get();
#endif
        return 1;
    }

    std::cout << "INFO: Initialization successful." << std::endl;
    try {
        std::cout << "INFO: Current Working Directory: [" << std::filesystem::current_path() << "]" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error getting current_path: " << e.what() << std::endl;
    }

    std::string fontImagePath = "C:/Users/ghlhn/Downloads/big_project_CG/x64/Debug/font_atlas.png";
    std::string fontDataPath = "C:/Users/ghlhn/Downloads/big_project_CG/x64/Debug/font_atlas.dat";

    if (!loadFontAtlas(fontImagePath, fontDataPath)) {
        std::cerr << "Font loading failed!" << std::endl;
        cleanup();
#ifdef _WIN32
        std::cout << "Press any key to exit..." << std::endl;
        std::cin.get();
#endif
        return 1;
    }
    std::cout << "INFO: Main font loaded successfully." << std::endl;

    gAllTextVertices.clear();

    gVertexOffset_Part1 = 0;
    generateTextMeshPart("20th", 20.0f, gAllTextVertices, gTextBlockWidth_Part1); // Increased scale
    gVertexCount_Part1 = gAllTextVertices.size();

    gVertexOffset_Part2 = gAllTextVertices.size();
    generateTextMeshPart("CENTURY", 25.0f, gAllTextVertices, gTextBlockWidth_Part2); // Increased scale
    gVertexCount_Part2 = gAllTextVertices.size() - gVertexOffset_Part2;

    gVertexOffset_Part3 = gAllTextVertices.size();
    generateTextMeshPart("FOX", 35.0f, gAllTextVertices, gTextBlockWidth_Part3);  // Increased scale
    gVertexCount_Part3 = gAllTextVertices.size() - gVertexOffset_Part3;

    setupCombinedTextVBO();
    std::cout << "INFO: Text mesh generated. Total vertices: " << gAllTextVertices.size() << std::endl;
    std::cout << "Part 1: Offset=" << gVertexOffset_Part1 << ", Count=" << gVertexCount_Part1 << ", Width=" << gTextBlockWidth_Part1 << std::endl;
    std::cout << "Part 2: Offset=" << gVertexOffset_Part2 << ", Count=" << gVertexCount_Part2 << ", Width=" << gTextBlockWidth_Part2 << std::endl;
    std::cout << "Part 3: Offset=" << gVertexOffset_Part3 << ", Count=" << gVertexCount_Part3 << ", Width=" << gTextBlockWidth_Part3 << std::endl;


    bool quit = false;
    SDL_Event e;
    Uint32 lastTime = SDL_GetTicks();

    float totalRunTime = 0.0f; // For simple animation timing if needed

    while (!quit) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        totalRunTime += deltaTime;

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
                float camMoveSpeed = 50.0f * deltaTime;
                float camRotSpeed = 60.0f * deltaTime;

                if (e.key.keysym.sym == SDLK_UP) gCameraPitch += camRotSpeed;
                if (e.key.keysym.sym == SDLK_DOWN) gCameraPitch -= camRotSpeed;
                if (e.key.keysym.sym == SDLK_LEFT) gCameraYaw -= camRotSpeed;
                if (e.key.keysym.sym == SDLK_RIGHT) gCameraYaw += camRotSpeed;
                if (e.key.keysym.sym == SDLK_PAGEUP) gCameraDistance -= camMoveSpeed * 5.0f;
                if (e.key.keysym.sym == SDLK_PAGEDOWN) gCameraDistance += camMoveSpeed * 5.0f;
            }
        }

        // Simple intro animation for camera (optional)
        // if (totalRunTime < 5.0f) { // Zoom in for 5 seconds
        //     gCameraDistance = 300.0f - (totalRunTime / 5.0f) * 150.0f;
        // } else {
        //     gCameraDistance = 150.0f;
        // }


        render();
        SDL_GL_SwapWindow(gWindow);
    }

    cleanup();
    std::cout << "INFO: Program ended." << std::endl;
#ifdef _WIN32
    std::cout << "Press any key to exit..." << std::endl;
    std::cin.get();
#endif
    return 0;
}
