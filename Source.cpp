#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <fstream>   // Fájlkezeléshez
#include <sstream>   // Szövegfolyamhoz
#include <string>    // Sztringekhez

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Globális változók és konstansok
const float WINDOW_SIZE = 600.0f;
const float RADIUS = 50.0f;

// Kör állapota
float cx = 300.0f;
float cy = 300.0f;
float vx = 10.0f; // vízszintes mozgás sebessége
float vy = 0.0f;

// Szakasz állapota
float lineY = 300.0f;
const float lineWidth = 200.0f; // Ablak harmada (600/3)
const float lineThickness = 3.0f;

// --- LOGIKAI FÜGGVÉNYEK ---

void updateCirclePosition() {
    cx += vx;
    cy += vy;

    // X tengely visszapattanás (arányosítással)
    if (cx + RADIUS > WINDOW_SIZE) {
        float overshoot = (cx + RADIUS) - WINDOW_SIZE;
        cx = WINDOW_SIZE - RADIUS - overshoot;
        vx = -vx;
    }
    else if (cx - RADIUS < 0.0f) {
        float overshoot = 0.0f - (cx - RADIUS);
        cx = 0.0f + RADIUS + overshoot;
        vx = -vx;
    }

    // Y tengely visszapattanás
    if (cy + RADIUS > WINDOW_SIZE) {
        float overshoot = (cy + RADIUS) - WINDOW_SIZE;
        cy = WINDOW_SIZE - RADIUS - overshoot;
        vy = -vy;
    }
    else if (cy - RADIUS < 0.0f) {
        float overshoot = 0.0f - (cy - RADIUS);
        cy = 0.0f + RADIUS + overshoot;
        vy = -vy;
    }
}

bool checkIntersection() {
    float lineStartX = 300.0f - (lineWidth / 2.0f);
    float lineEndX = 300.0f + (lineWidth / 2.0f);
    float closestX = std::max(lineStartX, std::min(cx, lineEndX));
    float closestY = std::max(lineY - (lineThickness / 2.0f), std::min(cy, lineY + (lineThickness / 2.0f)));

    float distanceX = cx - closestX;
    float distanceY = cy - closestY;
    float distance = std::sqrt((distanceX * distanceX) + (distanceY * distanceY));

    return distance <= RADIUS;
}

// Billentyűzet eseménykezelő (Callback)
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_UP) {
            lineY += 5.0f; // Szakasz fel
        }
        if (key == GLFW_KEY_DOWN) {
            lineY -= 5.0f; // Szakasz le
        }
        if (key == GLFW_KEY_S && action == GLFW_PRESS) {
            // 25 fokos szögben indítás
            float angleRad = 25.0f * (M_PI / 180.0f);
            float speed = 10.0f; //sebesség
            vx = speed * std::cos(angleRad);
            vy = speed * std::sin(angleRad);
        }
    }
}

// ÚJ FÜGGVÉNY: Shader beolvasása fájlból és lefordítása
unsigned int loadAndCompileShader(unsigned int type, const char* filePath) {
    std::string shaderCode;
    std::ifstream shaderFile;

    // Biztosítjuk, hogy az ifstream dobjon kivételt hiba esetén
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // Fájl megnyitása
        shaderFile.open(filePath);
        std::stringstream shaderStream;
        // Fájl tartalmának beolvasása a stream-be
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        // Stream konvertálása stringgé
        shaderCode = shaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "HIBA: Nem talalhato vagy nem olvashato a shader fajl: " << filePath << std::endl;
        return 0;
    }

    const char* shaderSource = shaderCode.c_str();

    // Shader fordítása
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &shaderSource, nullptr);
    glCompileShader(id);

    // Fordítási hibák ellenőrzése
    int success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cout << "HIBA: A shader forditas sikertelen (" << filePath << "):\n" << infoLog << std::endl;
    }

    return id;
}

// --- MAIN FÜGGVÉNY ---
int main() {
    // 1. GLFW Inicializálása
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_SIZE, WINDOW_SIZE, "Szamitogepes Grafika Beadando", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    glfwSwapInterval(1);

    // 2. OpenGL betöltő inicializálása (GLEW)
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // 3. Shaderek beolvasása fájlokból és létrehozásuk
    unsigned int circleVS = loadAndCompileShader(GL_VERTEX_SHADER, "circle_vertex.glsl");
    unsigned int circleFS = loadAndCompileShader(GL_FRAGMENT_SHADER, "circle_fragment.glsl");
    unsigned int circleProgram = glCreateProgram();
    glAttachShader(circleProgram, circleVS);
    glAttachShader(circleProgram, circleFS);
    glLinkProgram(circleProgram);

    unsigned int lineVS = loadAndCompileShader(GL_VERTEX_SHADER, "line_vertex.glsl");
    unsigned int lineFS = loadAndCompileShader(GL_FRAGMENT_SHADER, "line_fragment.glsl");
    unsigned int lineProgram = glCreateProgram();
    glAttachShader(lineProgram, lineVS);
    glAttachShader(lineProgram, lineFS);
    glLinkProgram(lineProgram);

    // 4. Geometriák beállítása (VAO, VBO)
    float circleVertices[] = {
        -RADIUS, -RADIUS,
         RADIUS, -RADIUS,
         RADIUS,  RADIUS,
        -RADIUS,  RADIUS
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    unsigned int circleVAO, circleVBO, circleEBO;
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);
    glGenBuffers(1, &circleEBO);

    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, circleEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int lineVAO, lineVBO, lineEBO;
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    glGenBuffers(1, &lineEBO);

    // --- FELHASZNÁLÓI TÁJÉKOZTATÓ KIÍRÁSA A KONZOLRA ---
    std::cout << " IRANYITAS:" << std::endl;
    std::cout << " [Fel nyil] : Szakasz mozgatasa felfele" << std::endl;
    std::cout << " [Le nyil]  : Szakasz mozgatasa lefele" << std::endl;
    std::cout << " [S] gomb   : Kor elinditasa" << std::endl;

    // 5. Render Ciklus (Main Loop)
    while (!glfwWindowShouldClose(window)) {
        updateCirclePosition();
        bool isIntersecting = checkIntersection();

        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float halfW = lineWidth / 2.0f;
        float halfH = lineThickness / 2.0f;
        float lineVertices[] = {
            300.0f - halfW, lineY - halfH,
            300.0f + halfW, lineY - halfH,
            300.0f + halfW, lineY + halfH,
            300.0f - halfW, lineY + halfH
        };

        glBindVertexArray(lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glUseProgram(lineProgram);
        glBindVertexArray(lineVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(circleProgram);
        glUniform2f(glGetUniformLocation(circleProgram, "circleCenter"), cx, cy);
        glUniform1f(glGetUniformLocation(circleProgram, "radius"), RADIUS);
        glUniform1i(glGetUniformLocation(circleProgram, "isIntersecting"), isIntersecting ? 1 : 0);

        glBindVertexArray(circleVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}