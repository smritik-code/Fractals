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

    const int MAX_ITER = 300;

    // Color palette from Color Hunt: #FEF9E1, #E5D0AC, #A31D1D, #6D2323
    vec3 palette(float t) {
        vec3 color1 = vec3(0.996, 0.976, 0.882); // #FEF9E1
        vec3 color2 = vec3(0.898, 0.816, 0.675); // #E5D0AC
        vec3 color3 = vec3(0.639, 0.114, 0.114); // #A31D1D
        vec3 color4 = vec3(0.427, 0.137, 0.137); // #6D2323

        // Smooth transitions between colors
        if (t < 0.25) return mix(color1, color2, t * 4.0);
        else if (t < 0.5) return mix(color2, color3, (t - 0.25) * 4.0);
        else if (t < 0.75) return mix(color3, color4, (t - 0.5) * 4.0);
        else return mix(color4, color1, (t - 0.75) * 4.0);
    }

    // Mandelbrot calculation
    float mandelbrot(vec2 c) {
        vec2 z = c;
        float iter;
        for (iter = 0.0; iter < MAX_ITER; iter++) {
            if (length(z) > 2.0) break;
            z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
        }
        return iter / MAX_ITER;
    }

    // Recursive fractal patterns
    float recursiveFractal(vec2 c, float scale, float time) {
        float fractal = 0.0;



        fractal += mandelbrot(c * scale) * 0.95;//Here 0.5
        fractal += mandelbrot(c * scale * 2.0) * 0.8; //0.3
        fractal += mandelbrot(c * scale * 4.0) * 0.6; //0.2
        fractal += mandelbrot(c * scale * 6.0) * 0.4; //thena
        fractal += mandelbrot(c * scale * 8.0) * 0.3; //thena
        fractal += mandelbrot(c * scale * 10.0) * 0.1; //thena
        return fractal;
    }

    void main() {
        float zoom = exp(iTime * 0.13); // Smooth zoom in
        vec2 center = vec2(-0.745428, 0.131825); // Visually interesting point
        center += 0.01 * vec2(sin(iTime * 0.15), cos(iTime * 0.1)); // Subtle movement

        vec2 c = (fragCoord * iResolution - 0.5 * iResolution.xy) / (iResolution.y * 0.2) / zoom + center;

        
        float t = mandelbrot(c);
        vec3 color = palette(t);

        // Add recursive fractal patterns inside the boundary
        if (t > 0.98) {
            float innerFractal = recursiveFractal(c, 2.0, iTime);
            float outlineFactor = mod(innerFractal * 15.0, 1.0);
            vec3 innerColor = mix(vec3(0.427, 0.137, 0.137), vec3(0.996, 0.976, 0.882), outlineFactor);
            color = mix(color, innerColor, 0.95);
        }

        FragColor = vec4(color, 1.0);
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

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Mandelbrot Renderer", glfwGetPrimaryMonitor(), NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = true;
    glewInit();
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);
    
    glfwSetKeyCallback(window, keyCallback);
    
    GLuint VAO, VBO;
    float vertices[] = {
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  
        -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");
    
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkCompileErrors(shaderProgram, "PROGRAM");
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glUseProgram(shaderProgram);
        glUniform2f(glGetUniformLocation(shaderProgram, "iResolution"), (float)width, (float)height);
        glUniform1f(glGetUniformLocation(shaderProgram, "iTime"), (float)glfwGetTime());
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}