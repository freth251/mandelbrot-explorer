#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <iostream>

float zoom = 1.0f;           // Start with no zoom
float centerX = 0.0f;        // Start zooming into center
float centerY = 0.0f;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    float zoomFactor = 1.0f + yoffset * 0.1f; 
    zoom *= zoomFactor;

    zoom = fmax(0.1f, zoom);
    
}

std::string read_file(const char* path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
bool isDragging = false;  

double lastMouseX, lastMouseY;  

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isDragging = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
            printf("Started dragging at %.1f, %.1f\n", lastMouseX, lastMouseY);
        } else if (action == GLFW_RELEASE) {
            isDragging = false;
            printf("Stopped dragging\n");
        }
    }
    
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    if (isDragging) {
        // Calculate how much mouse moved
        double deltaX = xpos - lastMouseX;
        double deltaY = ypos - lastMouseY;
        
        // Get window size
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        
        centerX -= 1000 * deltaX / width / zoom; 
        centerY += 1000 * deltaY / height / zoom ; 
                
        lastMouseX = xpos;
        lastMouseY = ypos;
    }
}

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

    GLFWwindow* window = glfwCreateWindow(800, 600, "Mandelbrot Explorer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetCursorPosCallback(window, cursor_pos_callback);

    float vertices[] = {
        // First triangle
        -1.0f, 1.0f, 0.0f,   // top left
        1.0f, 1.0f, 0.0f,    // top right
        -1.0f,  -1.0f, 0.0f, // bottom left

        // Second triangle
        1.0f, 1.0f, 0.0f,    // top right
        -1.0f,  -1.0f, 0.0f, // bottom left
        1.0f, -1.0f, 0.0f,   // bottom right
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    std::string vertexCode = read_file("shaders/vertex.glsl");
    std::string fragmentCode = read_file("shaders/fragment.glsl");

    if (vertexCode.empty()) {
        std::cerr << "Vertex shader file not found or empty.\n";
    }
    if (fragmentCode.empty()) {
        std::cerr << "Fragment shader file not found or empty.\n";
    }


    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    int window_size = glGetUniformLocation(shaderProgram, "u_window_size");


    while (!glfwWindowShouldClose(window)) {
        int width, height;
        
        glfwGetFramebufferSize(window, &width, &height);

        // Send values to shader
        glUniform2f(window_size, (float)width, (float)height );
        glUniform1f(glGetUniformLocation(shaderProgram, "u_zoom"), zoom);
        glUniform2f(glGetUniformLocation(shaderProgram, "u_center"), centerX, centerY);
      
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6); // 6 vertices = 2 triangles


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
