#include <iostream>

#include <glad/glad.h>
#include <KHR/khrplatform.h>
#include <GLFW/glfw3.h>

#define GLFW_INCLUDE_NONE


static bool MouseActionDown = false;
static int mouseX = 0;
static int mouseY = 0;

static bool isFullScreen = false;

const int WIDTH = 1020;
const int HEIGHT = 800;

void displayHardwareInfo(){
    std::cout << "vender: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "render: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "version: " << glGetString(GL_VERSION) << std::endl;
}

int main(int argc , char *argv[]){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    
    GLFWmonitor *mointor = isFullScreen?glfwGetPrimaryMonitor():nullptr;
    GLFWwindow* window = glfwCreateWindow(
                            WIDTH, 
                            HEIGHT, 
                            "run", mointor, nullptr);
    
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // glfwSetWindowUserPointer(window , &app);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* windows_,int w,int h){
        
    });

    glfwSetMouseButtonCallback(window , [](GLFWwindow* windows_,int button,int event,int mods){
    });



    glfwSetCursorPosCallback(window , [](GLFWwindow* windows_,double x,double y){
        mouseX = static_cast<int>(x);
        mouseY = static_cast<int>(y);
        // std::cout << "pos: " << mouseX << "  " << mouseY << std::endl;
    });

    glfwSetKeyCallback(window , [](GLFWwindow* windows_,int key,int scancode,int action,int mods){
        std::cout << "key " << key << "  scancode " << scancode << " action " << action << std::endl;
    });

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    displayHardwareInfo();
    
    while (!glfwWindowShouldClose(window)) {
        // std::cout << "tick" << std::endl;

        glfwSwapBuffers(window);
        glfwPollEvents();
        
        glfwSwapInterval(1);//锁定固定帧率
    }//end while
    
	glfwTerminate();
    return 0;
}
