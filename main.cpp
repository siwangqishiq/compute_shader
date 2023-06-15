#include <iostream>

#include <glad/glad.h>
#include <KHR/khrplatform.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <vector>

// #define GLFW_INCLUDE_NONE


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

//读取文本文件
std::string ReadFileAsText(std::string path){
    std::string content;
    std::ifstream infile(path);
    if(infile.is_open()){
        std::string line;
        while(std::getline(infile , line)){
            line += "\n";
            content += line;
        }//end while
    }else{
        std::cerr<< "open file : "<< path.c_str() <<" error" << std::endl;
    }
    infile.close();
    return content;
}

GLuint BuildComputeShader(){
    std::string computeSrc = ReadFileAsText("shader/compute.glsl");
    // std::cout << computeSrc << std::endl;
    
    const char *src = computeSrc.c_str();
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    if(shader == 0){
        std::cerr << "create shader error" << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    glShaderSource(shader, 1 , &src , nullptr);
    glCompileShader(shader);

    GLuint program = glCreateProgram();
    glAttachShader(program , shader);
    glLinkProgram(program);

    std::cout << "error "<<glGetError() << std::endl;

    GLint linkResult = GL_TRUE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkResult);
    if(linkResult == GL_FALSE){
        char szLog[1024] = { 0 };
        GLsizei logLen = 0;
        glGetProgramInfoLog(shader, 1024, &logLen, szLog);
        std::cerr << "Compute Shader linked failed"<< std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return program;
}



class App {
public:
    void init(){
        //prepare compute shader
        std::cout << "app init" << std::endl;
        shader = BuildComputeShader();
        std::cout << "comput shader : " << shader << std::endl;

        

        //init texture
        texInput = buildTexture(texWidth , texHeight);
        texOutput = buildTexture(texWidth , texHeight);


        //set data
        int size = texWidth * texHeight * 4;
        float *data = new float[texWidth * texHeight * 4];
        int index = 0;
        for(int i = 0 ; i < texHeight ; i++){
            for(int j = 0 ; j < texWidth ;j++){
                data[index + 0] = 0.1f;
                data[index + 1] = 0.2f;
                data[index + 2] = 0.3f;
                data[index + 3] = 0.4f;

                index += 4;
            }//end for j
        }//end for i

        setDataToGPU(texInput , texWidth , texHeight , data);
        delete []data;

        // test();
    }

    void test(){
        auto data = readDataFromGPU(texInput , texWidth , texHeight);
        std::cout << "result: " << std::endl;
        for(int i = 0 ; i < texWidth * texHeight * 4 ;i++){
            std::cout << data[i] << ",";
        }//end for i
        std::cout << std::endl;
    }

    void tick(){
        // glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);
        float v[4] = {0.5f , 0.5f ,0.5f , 0.5f};
        glUniform1fv(glGetUniformLocation(shader ,"v"), 4 , v);
        
        //create image object
        glBindImageTexture(0 , texInput , 0 , GL_FALSE , 0 , GL_READ_WRITE , GL_RGBA32F);
        glBindImageTexture(1 , texOutput , 0 , GL_FALSE , 0 , GL_READ_WRITE , GL_RGBA32F);
        glDispatchCompute(1, 1, 1);

        std::cout << "error4 "<<glGetError() << std::endl;

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glFinish();

        auto data = readDataFromGPU(texOutput , texWidth , texHeight);
        std::cout << "result: " << std::endl;
        for(int i = 0 ; i < texWidth * texHeight * 4 ;i++){
            std::cout << data[i] << ",";
        }//end for i
        std::cout << std::endl;

        delete[] data;
    }

    void dispose(){
        std::cout << "app dispose" << std::endl;
    }
private:
    unsigned int shader;
    unsigned int texInput;
    unsigned int texOutput;

    const int texWidth = 16;
    const int texHeight = 16;

    unsigned int buildTexture(unsigned int width , unsigned int height){
        unsigned int texture = 0;
        glGenTextures(1 , &texture);
        glBindTexture(GL_TEXTURE_2D , texture);
        glTexStorage2D(GL_TEXTURE_2D , 1 , GL_RGBA32F, width , height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glBindTexture(GL_TEXTURE_2D , 0);
        return texture;
    }

    void setDataToGPU(unsigned int texture , int width, int height , float *data){
        glBindTexture(GL_TEXTURE_2D , texture);
        glTexSubImage2D(GL_TEXTURE_2D , 0,0,0, width, height, 
            GL_RGBA, GL_FLOAT, data);
        glBindTexture(GL_TEXTURE_2D , 0);
    }

    float* readDataFromGPU(unsigned int texture ,int width , int height){
        float* data = new float[width * height * 4];
        glBindTexture(GL_TEXTURE_2D, texture);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, data);
        glBindTexture(GL_TEXTURE_2D, 0);
        return data;
    }
};



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

    App app;

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

    glfwSetKeyCallback(window , 
        [](GLFWwindow* windows_,int key,int scancode,int action,int mods){
    });

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    displayHardwareInfo();

    app.init();
    
    while (!glfwWindowShouldClose(window)) {
        // std::cout << "tick" << std::endl;

        app.tick();

        glfwSwapBuffers(window);
        glfwPollEvents();
        
        glfwSwapInterval(1);//锁定固定帧率
    }//end while

    // app.tick();
    
    app.dispose();
	glfwTerminate();
    return 0;
}
