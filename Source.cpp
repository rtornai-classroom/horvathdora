enum eVertexArrayObject {
    VAOCube,
    VAOSphere,
    VAOCount
};
enum eBufferObject {
    VBOCube,
    VBOSphere,
    EBOSphere,
    BOCount
};
enum eProgram {
    MainProgram,
    ProgramCount
};
enum eTexture {
    SunTexture,
    TextureCount
};

#include "common.cpp"

GLchar  windowTitle[] = "Szamitogepes Grafika Beadando";

// --- Kamera Paraméterek (Hengerkoordináta rendszer) ---
const float r = 9.0f;               // r sugár (8 <= r <= 10)
float camAngle = 0.0f;              // Forgás a Z-tengely körül
float camZ = 0.0f;                  // Kamera magassága (Z-tengely mentén)
const float camSpeed = 1.5f;

vec3    cameraPosition;
vec3    cameraTarget = vec3(0.0f, 0.0f, 0.0f); // Mindig az origóba néz
vec3    cameraUpVector = vec3(0.0f, 0.0f, 1.0f); // UP vektor (0, 0, 1)

// --- Fényforrás Paraméterek ---
GLuint  lightPositionLoc;
GLuint  lightColorLoc;
GLuint  isLightOnLoc;
GLuint  isSunLoc;
GLuint  inverseTransposeMatrixLoc;

vec3    lightPosition;
float   lightAngle = 0.0f;
bool    isLightOn = true;

// --- Adatszerkezetek ---
vector<GLfloat> sphere_vertices;
vector<GLuint>  sphere_indices;

// Kocka adatok: Pozíció (3), Normálvektor (3), Textúra (2) - Bár a kockán nem használunk textúrát, a shader miatt kell
GLfloat cubeVertices[] = {
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
    // Left face
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    // Right face
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     // Bottom face
     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     // Top face
     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

// Gömb (Nap) generálása
void generateSpherePoints(int numHorizontalFaces, int numVerticalFaces) {
    float horizontalStep = radians(360.0f) / numHorizontalFaces;
    float verticalStep = radians(180.0f) / numVerticalFaces;
    float radius = 0.25f; // d = 0.5

    for (int i = 0; i <= numVerticalFaces; ++i) {
        for (int j = 0; j <= numHorizontalFaces; ++j) {
            float u = i * verticalStep;
            float v = j * horizontalStep;

            float x = radius * sin(u) * cos(v);
            float y = radius * sin(u) * sin(v);
            float z = radius * cos(u);

            vec3 normal = normalize(vec3(x, y, z));
            float s = (float)j / numHorizontalFaces;
            float t = 1.0f - (float)i / numVerticalFaces;

            // Pos (3), Normal (3), TexCoord (2)
            sphere_vertices.push_back(x); sphere_vertices.push_back(y); sphere_vertices.push_back(z);
            sphere_vertices.push_back(normal.x); sphere_vertices.push_back(normal.y); sphere_vertices.push_back(normal.z);
            sphere_vertices.push_back(s); sphere_vertices.push_back(t);
        }
    }

    for (int i = 0; i < numVerticalFaces; ++i) {
        for (int j = 0; j < numHorizontalFaces; ++j) {
            int p1 = i * (numHorizontalFaces + 1) + j;
            int p2 = p1 + numHorizontalFaces + 1;

            sphere_indices.push_back(p1);
            sphere_indices.push_back(p2);
            sphere_indices.push_back(p1 + 1);

            sphere_indices.push_back(p1 + 1);
            sphere_indices.push_back(p2);
            sphere_indices.push_back(p2 + 1);
        }
    }
}

GLuint loadTexture(const GLchar* texturePath) {
    GLuint textureID = SOIL_load_OGL_texture(texturePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    if (textureID == 0) {
        cout << "Hiba: Textúra nem található (" << texturePath << ")" << endl;
        // cleanUpScene(EXIT_FAILURE); -> Ezt kivettem, hogy ne omoljon össze, ha nincs meg a textúra
    }
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    return textureID;
}

void initShaderProgram() {
    ShaderInfo shader_info[] = {
        { GL_VERTEX_SHADER,     "./vertexShader.glsl" },
        { GL_FRAGMENT_SHADER,   "./fragmentShader.glsl" },
        { GL_NONE, nullptr }
    };

    program[MainProgram] = LoadShaders(shader_info);

    locationMatModel = glGetUniformLocation(program[MainProgram], "matModel");
    locationMatView = glGetUniformLocation(program[MainProgram], "matView");
    locationMatProjection = glGetUniformLocation(program[MainProgram], "matProjection");

    inverseTransposeMatrixLoc = glGetUniformLocation(program[MainProgram], "inverseTransposeMatrix");
    lightPositionLoc = glGetUniformLocation(program[MainProgram], "lightPosition");
    lightColorLoc = glGetUniformLocation(program[MainProgram], "lightColor");
    isLightOnLoc = glGetUniformLocation(program[MainProgram], "isLightOn");
    isSunLoc = glGetUniformLocation(program[MainProgram], "isSun");

    // --- KOCKA VAO ---
    glBindVertexArray(VAO[VAOCube]);
    glBindBuffer(GL_ARRAY_BUFFER, BO[VBOCube]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // Position (0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Normal (1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // TexCoord (2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // --- GÖMB VAO ---
    generateSpherePoints(32, 16);
    glBindVertexArray(VAO[VAOSphere]);

    glBindBuffer(GL_ARRAY_BUFFER, BO[VBOSphere]);
    glBufferData(GL_ARRAY_BUFFER, sphere_vertices.size() * sizeof(GLfloat), sphere_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BO[EBOSphere]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere_indices.size() * sizeof(GLuint), sphere_indices.data(), GL_STATIC_DRAW);

    // Position (0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Normal (1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // TexCoord (2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // --- TEXTÚRA ÉS ÁLLAPOTOK ---
    glUseProgram(program[MainProgram]);
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Textúra betöltése (Nap képe) - Ide egy helyi képet kell betenned, amit letöltöttél!
    texture[SunTexture] = loadTexture("sun.jpg");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[SunTexture]);
}

void computeCameraMatrix() {
    // Kamera pozíció kiszámítása hengerkoordinátákkal
    cameraPosition.x = r * cos(camAngle);
    cameraPosition.y = r * sin(camAngle);
    cameraPosition.z = camZ;

    matView = lookAt(cameraPosition, cameraTarget, cameraUpVector);
    glUniformMatrix4fv(locationMatView, 1, GL_FALSE, value_ptr(matView));
}

void drawCube(vec3 position) {
    matModel = translate(mat4(1.0f), position);
    glUniformMatrix4fv(locationMatModel, 1, GL_FALSE, value_ptr(matModel));

    mat3 invTransp = mat3(inverseTranspose(matModel));
    glUniformMatrix3fv(inverseTransposeMatrixLoc, 1, GL_FALSE, value_ptr(invTransp));

    glUniform1i(isSunLoc, 0); // Kockát rajzolunk (fehér)

    glBindVertexArray(VAO[VAOCube]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawSun() {
    matModel = translate(mat4(1.0f), lightPosition);
    glUniformMatrix4fv(locationMatModel, 1, GL_FALSE, value_ptr(matModel));

    glUniform1i(isSunLoc, 1); // Napot rajzolunk (textúrás, világító)

    glBindVertexArray(VAO[VAOSphere]);
    glDrawElements(GL_TRIANGLES, (GLsizei)sphere_indices.size(), GL_UNSIGNED_INT, nullptr);
}

void display(GLFWwindow* window, double currentTime) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static GLdouble lastFrame = 0.0f;
    GLdouble deltaTime = currentTime - lastFrame;
    lastFrame = currentTime;

    // --- Irányítás ---
    if ((keyboard[GLFW_KEY_LEFT]))  camAngle -= camSpeed * deltaTime;
    if ((keyboard[GLFW_KEY_RIGHT])) camAngle += camSpeed * deltaTime;
    if ((keyboard[GLFW_KEY_UP]))    camZ += camSpeed * 2.0f * deltaTime;
    if ((keyboard[GLFW_KEY_DOWN]))  camZ -= camSpeed * 2.0f * deltaTime;

    computeCameraMatrix();

    // --- Fényforrás frissítése ---
    lightAngle += 1.0f * deltaTime;
    float lightRadius = 2.0f * r;
    lightPosition = vec3(lightRadius * cos(lightAngle), lightRadius * sin(lightAngle), 0.0f);

    vec3 lightColor = vec3(1.0f, 0.9f, 0.4f); // Meleg sárgás/napfény

    glUniform3fv(lightPositionLoc, 1, value_ptr(lightPosition));
    glUniform3fv(lightColorLoc, 1, value_ptr(lightColor));
    glUniform1i(isLightOnLoc, isLightOn ? 1 : 0);

    // --- Kirajzolás ---
    // 1. Kockák kirajzolása (Z-tengelyen helyezkednek el)
    drawCube(vec3(0.0f, 0.0f, 0.0f)); // Origó
    drawCube(vec3(0.0f, 0.0f, 2.0f)); // Felső kocka (1 egység hézag)
    drawCube(vec3(0.0f, 0.0f, -2.0f)); // Alsó kocka (1 egység hézag)

    // 2. Nap kirajzolása (csak ha ég a villany)
    if (isLightOn) {
        drawSun();
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    windowWidth = glm::max(width, 1);
    windowHeight = glm::max(height, 1);
    glViewport(0, 0, windowWidth, windowHeight);

    // Perspective vetítés 55 fokkal, ahogy a feladat kéri
    GLfloat aspectRatio = (GLfloat)windowWidth / (GLfloat)windowHeight;
    matProjection = perspective(radians(55.0f), aspectRatio, 0.1f, 100.0f);

    glUseProgram(program[MainProgram]);
    glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, value_ptr(matProjection));
}

// Üres callback függvény az egér mozgásának lekezeléséhez (a common.cpp init() függvénye keresi)
void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
    // A feladathoz most nincs szükségünk az egér pozíciójára
}

// Üres callback függvény az egérkattintások lekezeléséhez (a common.cpp init() függvénye keresi)
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // A feladathoz most nincs szükségünk az egérkattintásokra
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (action == GLFW_PRESS) keyboard[key] = GL_TRUE;
    else if (action == GLFW_RELEASE) keyboard[key] = GL_FALSE;

    // Világítás ki/be kapcsolása L betűvel
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        isLightOn = !isLightOn;
    }
}

int main(void) {
    init(3, 3, GLFW_OPENGL_CORE_PROFILE);
    initShaderProgram();
    framebufferSizeCallback(window, windowWidth, windowHeight);

    cout << "Kamera forgatas: Bal / Jobb nyil" << endl;
    cout << "Kamera fel/le: Fel / Le nyil" << endl;
    cout << "Vilagitas kapcsolasa: L" << endl;

    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanUpScene(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}