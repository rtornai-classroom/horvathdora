enum eVertexArrayObject {
	VAOCurveData,
	VAOCount
};
enum eVertexBufferObject {
	VBOHermiteData,
	VBOBezierData,
	BOCount
};
enum eProgram {
	CurveTesselationProgram,
	QuadScreenProgram,
	ProgramCount
};
enum eTexture {
	NoTexture,		// fixes 0 sized array problem
	TextureCount
};

#include "common.cpp"
#include <vector> // A dinamikus tömbhöz

#define	HERMITE_GMT			1
#define	BEZIER_GMT			2
#define	BEZIER_BERNSTEIN	3

GLchar	windowTitle[] = "Hermite and Bezier Curves with Tesselation Shader (Bonus Tasks included)";

// Hermite adatok maradhatnak statikusak, mert az mindig 2 pont + 2 tangens
GLfloat	hermite_data[][3] = {
	{ -0.2f, -0.3f, 0.0f }, {  0.3f,  0.2f, 0.0f },
	{ -5.0f,  5.0f, 0.0f }, { -5.0f,  5.0f, 0.0f }
};

// BÓNUSZ 3: A Bezier pontokat std::vector-ba tesszük, hogy dinamikusan tudjunk hozzáadni/törölni
std::vector<glm::vec3> bezier_control_points = {
	glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(-0.5f,  0.5f, 0.0f),
	glm::vec3(0.5f,  0.5f, 0.0f), glm::vec3(0.5f, -0.5f, 0.0f)
};

GLuint locationTessMatProjection, locationTessMatModelView, locationCurveType, locationControlPointsNumber;
GLuint curveType = BEZIER_BERNSTEIN, controlPointsNumber = 4; // Kezdjünk a Bernstein-nel, hogy rögtön menjen a hozzáadás

GLint dragged = -1;

GLfloat distanceSquare(vec2 p1, vec2 p2) {
	vec2 delta = p1 - p2;
	return dot(delta, delta);
}

GLint getActivePoint(GLfloat sensitivity, vec2 mousePosition) {
	GLfloat sensitivitySquare = sensitivity * sensitivity;
	if (curveType == HERMITE_GMT) {
		for (int i = 0; i < 4; i++) {
			vec2 p;
			if (i < 2) p = vec2(hermite_data[i][0], hermite_data[i][1]);
			else p = vec2(hermite_data[i - 2][0] + hermite_data[i][0], hermite_data[i - 2][1] + hermite_data[i][1]);
			if (distanceSquare(p, mousePosition) < sensitivitySquare) return i;
		}
	}
	else {
		// Vector méretét használjuk
		for (int i = 0; i < bezier_control_points.size(); i++) {
			vec2 p = vec2(bezier_control_points[i].x, bezier_control_points[i].y);
			if (distanceSquare(p, mousePosition) < sensitivitySquare) return i;
		}
	}
	return -1;
}

void initTesselationShader() {
	ShaderInfo shader_info[] = {
		{ GL_FRAGMENT_SHADER,			"./CurveFragShader.glsl" },
		{ GL_TESS_CONTROL_SHADER,		"./CurveTessContShader.glsl" },
		{ GL_TESS_EVALUATION_SHADER,	"./CurveTessEvalShader.glsl" },
		{ GL_VERTEX_SHADER,				"./CurveVertShader.glsl" },
		{ GL_NONE,						nullptr }
	};
	program[CurveTesselationProgram] = LoadShaders(shader_info);

	glBindVertexArray(VAO[VAOCurveData]);
	glBindBuffer(GL_ARRAY_BUFFER, BO[VBOHermiteData]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hermite_data), hermite_data, GL_DYNAMIC_DRAW); // GL_DYNAMIC_DRAW kell a változtatáshoz

	glBindBuffer(GL_ARRAY_BUFFER, BO[VBOBezierData]);
	// Vector adatainak feltöltése
	glBufferData(GL_ARRAY_BUFFER, bezier_control_points.size() * sizeof(glm::vec3), bezier_control_points.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, BO[VBOBezierData]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	locationCurveType = glGetUniformLocation(program[CurveTesselationProgram], "curveType");
	locationControlPointsNumber = glGetUniformLocation(program[CurveTesselationProgram], "controlPointsNumber");
	locationTessMatProjection = glGetUniformLocation(program[CurveTesselationProgram], "matProjection");
	locationTessMatModelView = glGetUniformLocation(program[CurveTesselationProgram], "matModelView");

	glUseProgram(program[CurveTesselationProgram]);
	glUniform1i(locationCurveType, curveType);
	controlPointsNumber = bezier_control_points.size();
	glUniform1i(locationControlPointsNumber, controlPointsNumber);
}

void initShaderProgram() {
	ShaderInfo shader_info[] = {
		{ GL_FRAGMENT_SHADER,			"./QuadScreenFragShader.glsl" },
		{ GL_VERTEX_SHADER,				"./QuadScreenVertShader.glsl" },
		{ GL_NONE,						nullptr }
	};
	program[QuadScreenProgram] = LoadShaders(shader_info);
	locationMatProjection = glGetUniformLocation(program[QuadScreenProgram], "matProjection");
	locationMatModelView = glGetUniformLocation(program[QuadScreenProgram], "matModelView");
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_COLOR_BUFFER_BIT);

	// Uniform location lekérdezése a színekhez
	GLuint colorLocCurve = glGetUniformLocation(program[CurveTesselationProgram], "uColor");
	GLuint colorLocQuad = glGetUniformLocation(program[QuadScreenProgram], "uColor");

	// ==========================================
	// 1. GÖRBE KIRAJZOLÁSA (Piros) - BÓNUSZ 2
	// ==========================================
	glUseProgram(program[CurveTesselationProgram]);
	glUniform3f(colorLocCurve, 1.0f, 0.0f, 0.0f); // Piros szín a görbének

	switch (curveType) {
	case HERMITE_GMT:
	case BEZIER_GMT:
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glDrawArrays(GL_PATCHES, 0, 4);
		break;
	case BEZIER_BERNSTEIN:
		glPatchParameteri(GL_PATCH_VERTICES, bezier_control_points.size());
		glDrawArrays(GL_PATCHES, 0, bezier_control_points.size());
		break;
	}

	// ==========================================
	// 2. KONTROLLPOLIGON ÉS PONTOK KIRAJZOLÁSA 
	// ==========================================
	glUseProgram(program[QuadScreenProgram]);
	switch (curveType) {
	case HERMITE_GMT:
	{
		// Hermite kontrollpontok (Kék) - BÓNUSZ 2
		glUniform3f(colorLocQuad, 0.0f, 0.0f, 1.0f);
		glDrawArrays(GL_POINTS, 0, 2);

		// Tangensek vonalai ("Kontrollpoligon" funkció Hermite-nél) (Zöld) - BÓNUSZ 1 & 2
		GLfloat tangent_lines[4][3] = {
			{ hermite_data[0][0], hermite_data[0][1], 0.0f },
			{ hermite_data[0][0] + hermite_data[2][0], hermite_data[0][1] + hermite_data[2][1], 0.0f },
			{ hermite_data[1][0], hermite_data[1][1], 0.0f },
			{ hermite_data[1][0] + hermite_data[3][0], hermite_data[1][1] + hermite_data[3][1], 0.0f }
		};

		GLuint tempVBO;
		glGenBuffers(1, &tempVBO);
		glBindBuffer(GL_ARRAY_BUFFER, tempVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tangent_lines), tangent_lines, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glUniform3f(colorLocQuad, 0.0f, 1.0f, 0.0f); // Zöld vonalak
		glDrawArrays(GL_LINES, 0, 4);

		glUniform3f(colorLocQuad, 0.0f, 0.0f, 1.0f); // Kék pontok a tangensek végén
		glDrawArrays(GL_POINTS, 1, 1);
		glDrawArrays(GL_POINTS, 3, 1);

		glDeleteBuffers(1, &tempVBO);
		glBindBuffer(GL_ARRAY_BUFFER, BO[VBOHermiteData]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		break;
	}
	case BEZIER_GMT:
	case BEZIER_BERNSTEIN:
		// Kontrollpoligon (nem záródik vissza: GL_LINE_STRIP) (Zöld) - BÓNUSZ 1 & 2
		glUniform3f(colorLocQuad, 0.0f, 1.0f, 0.0f);
		glDrawArrays(GL_LINE_STRIP, 0, bezier_control_points.size());

		// Kontrollpontok (Kék) - BÓNUSZ 2
		glUniform3f(colorLocQuad, 0.0f, 0.0f, 1.0f);
		glDrawArrays(GL_POINTS, 0, bezier_control_points.size());
		break;
	}
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	windowWidth = glm::max(width, 1);
	windowHeight = glm::max(height, 1);
	float aspectRatio = (float)windowWidth / (float)windowHeight;
	glViewport(0, 0, windowWidth, windowHeight);

	if (projectionType == Orthographic)
		if (windowWidth < windowHeight)
			matProjection = ortho(-worldSize, worldSize, -worldSize / aspectRatio, worldSize / aspectRatio, -100.0, 100.0);
		else
			matProjection = ortho(-worldSize * aspectRatio, worldSize * aspectRatio, -worldSize, worldSize, -100.0, 100.0);
	else
		matProjection = perspective(radians(45.0f), aspectRatio, 0.1f, 100.0f);

	matModel = mat4(1.0);
	matView = lookAt(vec3(0.0f, 0.0f, 9.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	matModelView = matView * matModel;

	glUseProgram(program[QuadScreenProgram]);
	glUniformMatrix4fv(locationMatModelView, 1, GL_FALSE, glm::value_ptr(matModelView));
	glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, glm::value_ptr(matProjection));

	glUseProgram(program[CurveTesselationProgram]);
	glUniformMatrix4fv(locationTessMatModelView, 1, GL_FALSE, glm::value_ptr(matModelView));
	glUniformMatrix4fv(locationTessMatProjection, 1, GL_FALSE, glm::value_ptr(matProjection));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	glUseProgram(program[CurveTesselationProgram]);
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (action == GLFW_PRESS) keyboard[key] = GL_TRUE;
	else if (action == GLFW_RELEASE) keyboard[key] = GL_FALSE;

	if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		projectionType = Orthographic;
		framebufferSizeCallback(window, windowWidth, windowHeight);
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		projectionType = Perspective;
		framebufferSizeCallback(window, windowWidth, windowHeight);
	}

	// A + és - gombokat meghagytam arra az esetre, ha kézzel akarnád állítani
	if ((action == GLFW_PRESS) && ((key == GLFW_KEY_KP_ADD) || (key == GLFW_KEY_EQUAL))) {
		controlPointsNumber++;
		glUniform1i(locationControlPointsNumber, controlPointsNumber);
	}
	if ((action == GLFW_PRESS) && ((key == GLFW_KEY_KP_SUBTRACT) || (key == GLFW_KEY_MINUS)) && (controlPointsNumber > 1)) {
		controlPointsNumber--;
		glUniform1i(locationControlPointsNumber, controlPointsNumber);
	}

	if (key == GLFW_KEY_H && action == GLFW_PRESS) {
		curveType = HERMITE_GMT;
		glBindBuffer(GL_ARRAY_BUFFER, BO[VBOHermiteData]);
	}
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		curveType = BEZIER_GMT;
		glBindBuffer(GL_ARRAY_BUFFER, BO[VBOBezierData]);
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		curveType = BEZIER_BERNSTEIN;
		glBindBuffer(GL_ARRAY_BUFFER, BO[VBOBezierData]);
	}
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glUniform1i(locationCurveType, curveType);
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	if (dragged >= 0) {
		vec2 mousePosition;
		mousePosition.x = xPos * 2.0f / (GLdouble)windowWidth - 1.0f;
		mousePosition.y = ((GLdouble)windowHeight - yPos) * 2.0f / (GLdouble)windowHeight - 1.0f;

		float aspectRatio = (float)windowWidth / (float)windowHeight;
		if (windowWidth < windowHeight) mousePosition.y /= aspectRatio;
		else mousePosition.x *= aspectRatio;

		if (curveType == HERMITE_GMT) {
			if (dragged < 2) {
				hermite_data[dragged][0] = mousePosition.x;
				hermite_data[dragged][1] = mousePosition.y;
			}
			else {
				hermite_data[dragged][0] = mousePosition.x - hermite_data[dragged - 2][0];
				hermite_data[dragged][1] = mousePosition.y - hermite_data[dragged - 2][1];
			}
			glBindBuffer(GL_ARRAY_BUFFER, BO[VBOHermiteData]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(hermite_data), hermite_data, GL_DYNAMIC_DRAW);
		}
		else {
			bezier_control_points[dragged].x = mousePosition.x;
			bezier_control_points[dragged].y = mousePosition.y;
			glBindBuffer(GL_ARRAY_BUFFER, BO[VBOBezierData]);
			glBufferData(GL_ARRAY_BUFFER, bezier_control_points.size() * sizeof(glm::vec3), bezier_control_points.data(), GL_DYNAMIC_DRAW);
		}
	}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);

	vec2 mousePosition;
	mousePosition.x = xPos * 2.0f / (GLdouble)windowWidth - 1.0f;
	mousePosition.y = ((GLdouble)windowHeight - yPos) * 2.0f / (GLdouble)windowHeight - 1.0f;

	float aspectRatio = (float)windowWidth / (float)windowHeight;
	if (windowWidth < windowHeight) mousePosition.y /= aspectRatio;
	else mousePosition.x *= aspectRatio;

	// BÓNUSZ 3: Pont hozzáadása (Bal klikk üres helyre)
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		dragged = getActivePoint(0.1f, mousePosition);

		// Ha nem kattintottunk meglevõ pontra, és Bezier módban vagyunk, hozzáadunk egy újat
		if (dragged == -1 && curveType == BEZIER_BERNSTEIN) {
			bezier_control_points.push_back(glm::vec3(mousePosition.x, mousePosition.y, 0.0f));

			// VBO frissítése az új mérettel
			glBindBuffer(GL_ARRAY_BUFFER, BO[VBOBezierData]);
			glBufferData(GL_ARRAY_BUFFER, bezier_control_points.size() * sizeof(glm::vec3), bezier_control_points.data(), GL_DYNAMIC_DRAW);

			// Uniform frissítése
			controlPointsNumber = bezier_control_points.size();
			glUseProgram(program[CurveTesselationProgram]);
			glUniform1i(locationControlPointsNumber, controlPointsNumber);

			// Rögtön meg is fogjuk az új pontot, ha mozgatni akarjuk
			dragged = bezier_control_points.size() - 1;
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		dragged = -1;
	}

	// BÓNUSZ 3: Pont törlése (Jobb klikk meglévõ pontra)
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		int clicked = getActivePoint(0.1f, mousePosition);

		// Ha meglévõ pontra kattintottunk, és van még legalább 3 pontunk (hogy legyen értelme a görbének)
		if (clicked >= 0 && curveType == BEZIER_BERNSTEIN && bezier_control_points.size() > 2) {
			bezier_control_points.erase(bezier_control_points.begin() + clicked);

			// VBO frissítése
			glBindBuffer(GL_ARRAY_BUFFER, BO[VBOBezierData]);
			glBufferData(GL_ARRAY_BUFFER, bezier_control_points.size() * sizeof(glm::vec3), bezier_control_points.data(), GL_DYNAMIC_DRAW);

			// Uniform frissítése
			controlPointsNumber = bezier_control_points.size();
			glUseProgram(program[CurveTesselationProgram]);
			glUniform1i(locationControlPointsNumber, controlPointsNumber);

			dragged = -1;
		}
	}
}

int main(void) {
	init(4, 0, GLFW_OPENGL_COMPAT_PROFILE);
	initTesselationShader();
	initShaderProgram();
	setlocale(LC_ALL, "");

	cout << "Hermite and Bezier Curves with Tesselation Shader (Bonus Tasks included)" << endl;
	cout << "Keyboard control" << endl;
	cout << "ESC\texit" << endl;
	cout << "O\tinduces orthographic projection" << endl;
	cout << "P\tinduces perspective projection" << endl;
	cout << "H\tHermite curve with GMT" << endl;
	cout << "B\tBezier curve with GMT" << endl;
	cout << "A\tArrayed Bezier curve with Bernstein polynoms" << endl;
	cout << "Bal egergomb\tAdd new point / Drag point" << endl;
	cout << "Jobb egergomb\tRemove existing point" << endl << endl;

	framebufferSizeCallback(window, windowWidth, windowHeight);

	// A kerek pontokhoz (opcionális, de szebbé teszi a pontokat, ha a shadered támogatja)
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(10.0f);

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanUpScene(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}