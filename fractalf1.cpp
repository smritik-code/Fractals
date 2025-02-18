#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

const char* vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec2 aPos;
    out vec2 fragCoord;
    void main() {
        fragCoord = aPos * 0.5 + 0.5;
        gl_Position = vec4(aPos, 0.0, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 fragCoord;

    uniform vec2 iResolution;
    uniform float iTime;
    uniform vec2 iCenter;

    const int MAX_ITER = 256;

    vec3 palette(float t) {
        vec3 color1 = vec3(0.996, 0.976, 0.882); // #FEF9E1
        vec3 color2 = vec3(0.898, 0.816, 0.675); // #E5D0AC
        vec3 color3 = vec3(0.639, 0.114, 0.114); // #A31D1D
        vec3 color4 = vec3(0.427, 0.137, 0.137); // #6D2323

        if (t < 0.25) return mix(color1, color2, t * 4.0);
        else if (t < 0.5) return mix(color2, color3, (t - 0.25) * 4.0);
        else if (t < 0.75) return mix(color3, color4, (t - 0.5) * 4.0);
        else return mix(color4, color1, (t - 0.75) * 4.0);
    }

    float mandelbrot(vec2 c) {
        vec2 z = c;
        float iter;
        for (iter = 0.0; iter < MAX_ITER; iter++) {
            if (length(z) > 2.0) break;
            z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
        }
        return iter / MAX_ITER;
    }

    float julia(vec2 c, vec2 z) {
        float iter;
        for (iter = 0.0; iter < MAX_ITER; iter++) {
            if (length(z) > 2.0) break;
            z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
        }
        return iter / MAX_ITER;
    }

    // Smooth easing function for smooth camera transition
    float smoothEaseInOut(float t) {
        return t * t * (3.0 - 2.0 * t);
    }

void main() {
    float totalTime = 125.0;  // Total animation time for one zoom cycle
    float phase = mod(iTime, totalTime) / totalTime;  // Normalize iTime to the [0, 1] range

    // Zoom timing parameters
    float zoomInDuration = 80.0;  // Zoom-in duration in seconds
    float zoomOutDuration = 45.0; // Zoom-out duration in seconds
    float maxZoom = 10;          // Maximum zoom factor
    float baseZoom = 0.5;         // Base zoom factor

    // Smooth transition of zoom factor over time
    float zoom;
    if (phase < (zoomInDuration / totalTime)) {
        // Zoom-in phase
        float zoomPhase = smoothEaseInOut(phase / (zoomInDuration / totalTime));
        zoom = baseZoom + zoomPhase * (maxZoom - baseZoom);
    } else {
        // Zoom-out phase
        float zoomPhase = smoothEaseInOut((phase - zoomInDuration / totalTime) / (zoomOutDuration / totalTime));
        zoom = maxZoom - zoomPhase * (maxZoom - baseZoom);
    }

    vec2 center = iCenter; // Use uniform for camera position
    vec2 c = vec2(-0.8, 0.156);  // Julia constant

    // Calculate fractal coordinates based on zoom
    vec2 z = (fragCoord * iResolution - 0.5 * iResolution.xy) / iResolution.y / zoom + center;

    // Compute Mandelbrot and Julia set values
    float mandelbrotVal = mandelbrot(z);
    float juliaVal = julia(c, z);

    // Blend Mandelbrot and Julia fractals
    float blendFactor = sin(iTime * 0.2);  // Blending factor
    float blendedVal = mix(mandelbrotVal, juliaVal, (blendFactor + 1.0) * 0.4);  // Smooth blending

    // Apply a color palette based on blended fractal value
    vec3 color = palette(blendedVal);

    FragColor = vec4(color, 1.0);  // Set the final output color
}
)";

void checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "Shader Compilation Error (" << type << "):\n" << infoLog << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "Program Linking Error:\n" << infoLog << std::endl;
        }
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return -1;
    }

    // Set GLFW window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_TRUE);

    // Create GLFW window
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Fractal Renderer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed!" << std::endl;
        return -1;
    }

    GLuint VAO, VBO;
    float vertices[] = {
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f
    };

    // Set up vertex array and buffer
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    // Create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkCompileErrors(shaderProgram, "PROGRAM");

    // Cleanup shaders as they are no longer needed after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);

    GLuint iTimeLocation = glGetUniformLocation(shaderProgram, "iTime");
    GLuint iResolutionLocation = glGetUniformLocation(shaderProgram, "iResolution");
    GLuint iCenterLocation = glGetUniformLocation(shaderProgram, "iCenter");

    float posX = 0.15f; // Initial camera position
    float posY = 0.0f;
    float moveSpeed = 0.01f;
    float startTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        // Auto-close after 50 seconds
        if (glfwGetTime() - startTime > 125.0) {
            glfwSetWindowShouldClose(window, true);
        }

        // Handle arrow key camera movement
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            posX -= moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            posX += moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            posY += moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            posY -= moveSpeed;

        float currentTime = static_cast<float>(glfwGetTime());

        glClear(GL_COLOR_BUFFER_BIT);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glUniform2f(iResolutionLocation, width, height);
        glUniform1f(iTimeLocation, currentTime);
        glUniform2f(iCenterLocation, posX, posY);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}