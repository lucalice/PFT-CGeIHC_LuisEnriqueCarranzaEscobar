/*---------------------------------------------------------*/
/*---------------  Proyecto Final CGeIHC ------------------*/
/*-----------------    2021-1   ---------------------------*/
/*------------- Alumno:Carranza Escobar Luis Enrique ------*/
//#include <Windows.h>

#include "glad.h"
#include <GLFW/glfw3.h>    //main
//#include <GL/glew.h>
#include <stdlib.h>
#include <glm/glm.hpp>    //camera y model
#include <glm/gtc/matrix_transform.hpp>    //camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>    //Texture

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>

/*
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collection;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Media;
*/
//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void my_input(GLFWwindow *window);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor *monitors;

void getResolution(void);

// camera
//Camera camera(glm::vec3(0.0f, 10.0f, 90.0f));
Camera camera(glm::vec3(-360.0f, 106.0f, -380.0f));
float MovementSpeed = 0.01f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double    deltaTime = 0.0f,
        lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

// posiciones
//float x = 0.0f;
//float y = 0.0f;
//Variables para el manejo de las animaciones
float   movAuto_x = 0.0f,
        movAuto_z = 0.0f,
        movAuto_y = 0.0f,
        orienta = 0.0f,
        giroPuerta = 0.0f,
        giroLlantas = 0.0f,
        movPersonaje = 0.0f,
        movPuerta = 0.0f;
bool    animacion = false,
        animacionDos = false,
        puertaFlag = false,
        flag = false;

#define MAX_FRAMES 9

//Función encargada del manejo de las animacinones
void animate(void)
{

    //Vehículo
    if (animacion)
    {
        if(movAuto_z >= 0  and movAuto_z < 2080){
            movAuto_z += 6.0f;
            giroLlantas += 6.0f;
            movPersonaje += 0.8f;
        }else if (movAuto_z >= 2080 and movAuto_z <= 2250){
            movAuto_z += 6.0f;
            giroLlantas += 6.0f;
            orienta += 3.0f;
            movPersonaje += 0.8f;
        }else{
            movAuto_x += 6.0f;
            giroLlantas += 6.0f;
        }
        //movPersonaje += 0.8f;
        if (movPuerta <= 90.0f && puertaFlag == false) {
            movPuerta += 1.0f;
        }else {
            if (puertaFlag == false)
                puertaFlag = true;
            if (movPuerta != 0.0f)
                movPuerta -= 1.0f;
        }
    }
    //Puertas
    if(animacionDos){
        if (giroPuerta < 90 && flag != true) {
            giroPuerta++;
        }
    }else{
        if(giroPuerta >= 0 && giroPuerta <= 90){
            giroPuerta--;
        }
    }
}
//Función encargada de obtener la resolución
void getResolution()
{
    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    SCR_WIDTH = mode->width;
    SCR_HEIGHT = (mode->height) - 80;
}


//Función principal
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    // --------------------
    monitors = glfwGetPrimaryMonitor();
    getResolution();

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetWindowPos(window, 0, 30);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, my_input);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader staticShader("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/Shaders/shader_Lights.vs", "/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/Shaders/shader_Lights.fs");
    Shader skyboxShader("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/Shaders/skybox.vs", "/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/Shaders/skybox.fs");
    Shader animShader("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/Shaders/anim.vs", "/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/Shaders/anim.fs");

    vector<std::string> faces
    {
        "/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/skybox/right.jpg",
        "/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/skybox/left.jpg",
        "/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/skybox/top.jpg",
        "/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/skybox/bottom.jpg",
        "/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/skybox/front.jpg",
        "/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/skybox/back.jpg"
    };

    Skybox skybox = Skybox(faces);

    // Shader configuration
    // --------------------
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // load models
    // -----------
    Model piso("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/piso/Piso.obj");
    Model carro("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Lambo/Carroceria.obj");
    Model llanta("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Lambo/wheel.obj");
    
    Model autoVerde("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/LamboRojo/Carroceria.obj");
    
    Model carroP("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Lambo/LamboPuerta/CarrosinP.obj");
    
    Model puerta("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Lambo/LamboPuerta/puerta.obj");
    
    Model arbol("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/arbol/Gledista_Triacanthos_OBJ/Gledista_Triacanthos_3.obj");
    
    Model mesa("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/mesa/3d-model.obj");
    
    Model silla("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/sillas/Chair.obj");
    
    Model puertaEdificio6("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Edificios/edificio6/Departamento6/puertaMM6.obj");
    
    Model puertaAnchaEdificio6("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Edificios/edificio6/Departamento6/puertaAncha.obj");
    
    Model puertaCorredizaEdificio6("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Edificios/edificio6/Departamento6/corredisa.obj");
    
    Model ventanaEdificio6("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Edificios/edificio6/Departamento6/puertaMM6.obj");
    
    Model edificioCuatro("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Edificios/edificio4/edificioCuatro.obj");
    Model consulado("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Edificios/CasaB/Consulado.obj");
    Model edificioSeis("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Edificios/edificio6/Departamento6/edificioSeis.obj");
    
    Model kiosco("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Edificios/kiosko/Gazebo.obj");
    
    ModelAnim personajeHombre("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Unarmed Walk Forward/Unarmed Walk Forward.dae");
    personajeHombre.initShaders(animShader.ID);
    
    ModelAnim megan("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/megan/megan.dae");
    megan.initShaders(animShader.ID);

    ModelAnim kate("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/kate/kate.dae");
    kate.initShaders(animShader.ID);
    
    ModelAnim meganSillyDancing("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Megan Silly Dancing/Silly Dancing.dae");
    meganSillyDancing.initShaders(animShader.ID);
    
    ModelAnim jamesSillyDancing("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/James Silly Dancing/Silly Dancing.dae");
    jamesSillyDancing.initShaders(animShader.ID);
    
    
    ModelAnim sentado("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Sitting/Sitting.dae");
    sentado.initShaders(animShader.ID);

    ModelAnim aplauso("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Sitting Clap/Sitting Clap.dae");
    aplauso.initShaders(animShader.ID);
    
    /*Modelos Teoría*/
    
    Model sofa("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/sofa2/furniture.obj");
    
    Model tele("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/tele/Smart TV - .obj .mtl/SmartTV.obj");
    
    Model comedor("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/mesaCuarto/3d-model.obj");
    
    Model cocina("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/cocina/KICHEN4.obj");
    
    Model refri("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/Refrigerator/Refri/OBJ/Fridge.obj");
    
    Model mueble("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/mueble/Long_Table.obj");
    
    Model muebleTV("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/muebleTV/muebleTV.obj");
    
    Model alfombra("/Users/luiscarranza/Documents/CompuGrafica/proyectoFinal/ProyectoFinal/extraFiles/resources/objects/alfombra/alfombra.obj");
    
    
    //Users/luiscarranza/Desktop/TEXTURES/
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        skyboxShader.setInt("skybox", 0);
        
        // per-frame time logic
        // --------------------
        lastFrame = SDL_GetTicks();

        // input
        // -----
        //my_input(window);
        animate();

        // render
        // ------
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        staticShader.use();
        //Setup Advanced Lights
        //Este se trata de una fuente de luz direccional
        staticShader.setVec3("viewPos", camera.Position);
        staticShader.setVec3("dirLight.direction", lightDirection);
        staticShader.setVec3("dirLight.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
        staticShader.setVec3("dirLight.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
        staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));

        staticShader.setVec3("pointLight[0].position", lightPosition);
        staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
        staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
        staticShader.setFloat("pointLight[0].constant", 0.08f);
        staticShader.setFloat("pointLight[0].linear", 0.009f);
        staticShader.setFloat("pointLight[0].quadratic", 0.032f);

        staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
        staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
        staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
        staticShader.setFloat("pointLight[1].constant", 1.0f);
        staticShader.setFloat("pointLight[1].linear", 0.009f);
        staticShader.setFloat("pointLight[1].quadratic", 0.032f);

        staticShader.setFloat("material_shininess", 32.0f);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 tmp = glm::mat4(1.0f);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        staticShader.setMat4("projection", projection);
        staticShader.setMat4("view", view);

        // Light
        glm::vec3 lightColor = glm::vec3(0.6f);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);
        

        // -------------------------------------------------------------------------------------------------------------------------
        // Personaje Animacion
        // -------------------------------------------------------------------------------------------------------------------------
        //Remember to activate the shader with the animation
        animShader.use(); //Se activa el shader de animación
        animShader.setMat4("projection", projection);
        animShader.setMat4("view", view);
    
        animShader.setVec3("material.specular", glm::vec3(0.5f));
        animShader.setFloat("material.shininess", 32.0f);
        animShader.setVec3("light.ambient", ambientColor);
        animShader.setVec3("light.diffuse", diffuseColor);
        animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        animShader.setVec3("light.direction", lightDirection);
        animShader.setVec3("viewPos", camera.Position);
        
        /*
         *Personajes caminando
         */
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f,0.0f,50.0f+movPersonaje));
        model = glm::scale(model, glm::vec3(0.03f));
        animShader.setMat4("model", model);
        personajeHombre.Draw(animShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-46.0f,0.0f,50.0f+movPersonaje));
        model = glm::scale(model, glm::vec3(0.069f));
        animShader.setMat4("model", model);
        megan.Draw(animShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-360.0f,147.0f,-390.0f));
        model = glm::rotate(model, glm::radians(160.0f),glm::vec3(0.0f,1.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.078f));
        animShader.setMat4("model", model);
        meganSillyDancing.Draw(animShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-350.0f,147.0f,-390.0f));
        model = glm::rotate(model, glm::radians(160.0f),glm::vec3(0.0f,1.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.078f));
        animShader.setMat4("model", model);
        jamesSillyDancing.Draw(animShader);
        
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-46.0f,0.0f,-80.0f+movPersonaje));
        model = glm::scale(model, glm::vec3(0.069f));
        animShader.setMat4("model", model);
        kate.Draw(animShader);
        
        //model = glm::translate(glm::mat4(1.0f), glm::vec3(-318.0f, 145.0f, -388.0f));
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-320.0f,148.0f,-388));
        model = glm::rotate(model, glm::radians(240.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.09f));
        animShader.setMat4("model", model);
        sentado.Draw(animShader);
        
        //model = glm::translate(glm::mat4(1.0f), glm::vec3(-318.0f, 145.0f, -388.0f));
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-345.0f,148.0f,-410));
        model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.09f));
        animShader.setMat4("model", model);
        aplauso.Draw(animShader);
        
        // -------------------------------------------------------------------------------------------------------------------------
        // Escenario
        // -------------------------------------------------------------------------------------------------------------------------
        staticShader.use();
        staticShader.setMat4("projection", projection);
        staticShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        staticShader.setMat4("model", model);
        piso.Draw(staticShader);
        
        // -----------------------------------------------------------------------------------------------------------------
        // Carro Amarillo
        // -----------------------------------------------------------------------------------------------------------------
        
        model = glm::translate(model, glm::vec3(15.0f + movAuto_x, -1.0f+movAuto_y, movAuto_z));
        tmp = model = glm::rotate(model, glm::radians(orienta), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        staticShader.setMat4("model", model);
        carro.Draw(staticShader);

        model = glm::translate(tmp, glm::vec3(20.5f, 10.5f, 37.9f));
        model = glm::rotate(model, glm::radians(giroLlantas),glm::vec3(1.0f,0.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        staticShader.setMat4("model", model);
        llanta.Draw(staticShader);    //Izq delantera

        model = glm::translate(tmp, glm::vec3(-20.5f, 10.5f, 37.9f));
        model = glm::rotate(model, glm::radians(giroLlantas),glm::vec3(1.0f,0.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        staticShader.setMat4("model", model);
        llanta.Draw(staticShader);    //Der delantera

        model = glm::translate(tmp, glm::vec3(-20.5f,10.5f, -43.5f));
        model = glm::rotate(model, glm::radians(giroLlantas),glm::vec3(1.0f,0.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        staticShader.setMat4("model", model);
        llanta.Draw(staticShader);    //Der trasera

        model = glm::translate(tmp, glm::vec3(20.5f, 10.5f, -43.5f));
        model = glm::rotate(model, glm::radians(giroLlantas),glm::vec3(1.0f,0.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        staticShader.setMat4("model", model);
        llanta.Draw(staticShader);    //Izq trase
         
        
        //-------------------------------------------------------------
        // Carro sin puerta
        //-------------------------------------------------------------
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-20.0f, -2.1f, -60.0f));
        tmp = model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.06f, 0.065f, 0.065f));
        staticShader.setMat4("model", model);
        carroP.Draw(staticShader);

        model = glm::translate(tmp, glm::vec3(-6.2f, 4.0f, 5.9f));
        model = glm::rotate(model, glm::radians(movPuerta), glm::vec3(1.0f, 0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(0.065f, 0.065f, 0.065f));
        staticShader.setMat4("model", model);
        puerta.Draw(staticShader);
        
        model = glm::translate(tmp, glm::vec3(4.7f, 2.6f, 8.2f));
        model = glm::rotate(model, glm::radians(0.0f),glm::vec3(1.0f,0.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.065f, 0.065f, 0.065f));
        staticShader.setMat4("model", model);
        llanta.Draw(staticShader);    //Izq delantera

        model = glm::translate(tmp, glm::vec3(-5.0f, 2.6f, 8.2f));
        model = glm::rotate(model, glm::radians(0.0f),glm::vec3(1.0f,0.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.065f, 0.065f, 0.065f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        staticShader.setMat4("model", model);
        llanta.Draw(staticShader);    //Der delantera

        model = glm::translate(tmp, glm::vec3(-5.0f, 2.6f, -9.5f));
        model = glm::rotate(model, glm::radians(0.0f),glm::vec3(1.0f,0.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.065f, 0.065f, 0.065f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        staticShader.setMat4("model", model);
        llanta.Draw(staticShader);    //Der trasera

        model = glm::translate(tmp, glm::vec3(4.7f, 2.6f, -9.5f));
        model = glm::rotate(model, glm::radians(0.0f),glm::vec3(1.0f,0.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.065f, 0.065f, 0.065f));
        staticShader.setMat4("model", model);
        llanta.Draw(staticShader);    //Izq trase
        
        // -----------------------------------------------------------------------------------------------------------------
        // Carro Verde
        // -----------------------------------------------------------------------------------------------------------------
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-480.0f, -2.1f, -60.0f));
        tmp = model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.06f, 0.065f, 0.065f));
        staticShader.setMat4("model", model);
        autoVerde.Draw(staticShader);

        model = glm::translate(tmp, glm::vec3(4.7f, 2.6f, 8.2f));
        model = glm::rotate(model, glm::radians(0.0f),glm::vec3(1.0f,0.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.065f, 0.065f, 0.065f));
        staticShader.setMat4("model", model);
        llanta.Draw(staticShader);    //Izq delantera

        model = glm::translate(tmp, glm::vec3(-5.0f, 2.6f, 8.2f));
        model = glm::rotate(model, glm::radians(0.0f),glm::vec3(1.0f,0.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.065f, 0.065f, 0.065f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        staticShader.setMat4("model", model);
        llanta.Draw(staticShader);    //Der delantera

        model = glm::translate(tmp, glm::vec3(-5.0f, 2.6f, -9.5f));
        model = glm::rotate(model, glm::radians(0.0f),glm::vec3(1.0f,0.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.065f, 0.065f, 0.065f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        staticShader.setMat4("model", model);
        llanta.Draw(staticShader);    //Der trasera

        model = glm::translate(tmp, glm::vec3(4.7f, 2.6f, -9.5f));
        model = glm::rotate(model, glm::radians(0.0f),glm::vec3(1.0f,0.0f,0.0f));
        model = glm::scale(model, glm::vec3(0.065f, 0.065f, 0.065f));
        staticShader.setMat4("model", model);
        llanta.Draw(staticShader);    //Izq trase
         
        //------------------------------------------------------------------------------------------------------------------
        // Arboles
        //------------------------------------------------------------------------------------------------------------------
        /*
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-275.0f, 0.0f, -45.0f));
        model = glm::scale(model, glm::vec3(1.0f));
        staticShader.setMat4("model", model);
        arbol.Draw(staticShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-175.0f, 0.0f, -15.0f));
        model = glm::scale(model, glm::vec3(1.0f));
        staticShader.setMat4("model", model);
        arbol.Draw(staticShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-75.0f, 0.0f, 45.0f));
        model = glm::scale(model, glm::vec3(1.0f));
        staticShader.setMat4("model", model);
        arbol.Draw(staticShader);
        */
        // -----------------------------------------------------------------------------------------------------------------
        // Edificios
        // -----------------------------------------------------------------------------------------------------------------
        
        //Aquí es donde nosotros le daremos las coordenadas a cada edificio para que pueda s///<esplegado de manera correcta en nuestro espacio de trabajo
        
        //Edificios frontales a la vista
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(170.0f, -20.0f, 50.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(6.0f));
        staticShader.setMat4("model", model);
        consulado.Draw(staticShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(172.0f, -5.0f, -480.0f));
        model = glm::scale(model, glm::vec3(3.0f));
        staticShader.setMat4("model", model);
        edificioCuatro.Draw(staticShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(244.0f, -5.0f, -480.0f));
        model = glm::scale(model, glm::vec3(3.0f));
        staticShader.setMat4("model", model);
        edificioCuatro.Draw(staticShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(316.0f, -5.0f, -480.0f));
        model = glm::scale(model, glm::vec3(3.0f));
        staticShader.setMat4("model", model);
        edificioCuatro.Draw(staticShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(388.0f, -5.0f, -480.0f));
        model = glm::scale(model, glm::vec3(3.0f));
        staticShader.setMat4("model", model);
        edificioCuatro.Draw(staticShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(460.0f, -5.0f, -480.0f));
        model = glm::scale(model, glm::vec3(3.0f));
        staticShader.setMat4("model", model);
        edificioCuatro.Draw(staticShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-290.0f, 0.0f, -400.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        staticShader.setMat4("model", model);
        edificioSeis.Draw(staticShader);
        
        
        //Puerta 1
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-274.0f, 106.4f, -366.0f));
        model = glm::rotate(model, glm::radians(90.0f + giroPuerta), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        staticShader.setMat4("model", model);
        puertaEdificio6.Draw(staticShader);
        
        
        //Puerta 2
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-274.0f, 106.4f, -434.0f));
        model = glm::rotate(model, glm::radians(90.0f + giroPuerta), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        staticShader.setMat4("model", model);
        puertaEdificio6.Draw(staticShader);
        
        //Puerta 3
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-297.0f, 106.4f, -416.5f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        staticShader.setMat4("model", model);
        puertaCorredizaEdificio6.Draw(staticShader);
        
        //Ventana1
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-297.0f, 106.5f, -372.0f));
        model = glm::rotate(model, glm::radians(0.0f + giroPuerta), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        staticShader.setMat4("model", model);
        ventanaEdificio6.Draw(staticShader);
        
        //Ventana2
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-296.5f, 106.5f, -384.6f));
        model = glm::rotate(model, glm::radians(180.0f + giroPuerta), glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        staticShader.setMat4("model", model);
        ventanaEdificio6.Draw(staticShader);
        
        
        //Ventana3
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-297.0f, 106.5f, -386.5f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        staticShader.setMat4("model", model);
        puertaAnchaEdificio6.Draw(staticShader);
        
        //Mesa
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-330.0f,147.0f,-400));
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        staticShader.setMat4("model", model);
        mesa.Draw(animShader);
        
        /*Cuarto 1*/
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-297.0f, 106.5f, -386.5f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        staticShader.setMat4("model", model);
        puertaAnchaEdificio6.Draw(staticShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-430.0f, 104.5f, -395.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f));
        staticShader.setMat4("model", model);
        tele.Draw(staticShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-389.0f, 99.5f, -395.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.18f));
        staticShader.setMat4("model", model);
        comedor.Draw(staticShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-374.0f, 99.0f, -335.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.7f));
        staticShader.setMat4("model", model);
        cocina.Draw(staticShader);

        model = glm::translate(glm::mat4(1.0f), glm::vec3(-356.0f, 97.0f, -340.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(9.0f));
        staticShader.setMat4("model", model);
        refri.Draw(staticShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-358.0f, 97.0f, -455.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(12.5f));
        staticShader.setMat4("model", model);
        mueble.Draw(staticShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-430.0f, 99.0f, -400.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(2.2f));
        staticShader.setMat4("model", model);
        muebleTV.Draw(staticShader);
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-400.0f, 99.0f, -380.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.005f));
        staticShader.setMat4("model", model);
        alfombra.Draw(staticShader);

        model = glm::translate(glm::mat4(1.0f), glm::vec3(-388.0f, 100.5f, -455.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(6.0f));
        staticShader.setMat4("model", model);
        sofa.Draw(staticShader);
        
        
        /*Parte del techo*/
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-345.0f, 145.0f, -410.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.4f));
        staticShader.setMat4("model", model);
        silla.Draw(staticShader);
        
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-318.0f, 145.0f, -388.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(130.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.4f));
        staticShader.setMat4("model", model);
        silla.Draw(staticShader);
        
        
        /*
         *Kiosco que va a la mitad del parque
         */
        
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-170.0f, 0.0f, 170.0f));
        model = glm::scale(model, glm::vec3(0.10f));
        staticShader.setMat4("model", model);
        kiosco.Draw(staticShader);
        
        // -------------------------------------------------------------------------------------------------------------------------
        // Caja Transparente --- Siguiente Práctica
        // -------------------------------------------------------------------------------------------------------------------------
        /*glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -70.0f));
        model = glm::scale(model, glm::vec3(5.0f));
        staticShader.setMat4("model", model);
        cubo.Draw(staticShader);
        glEnable(GL_BLEND);*/
        // -------------------------------------------------------------------------------------------------------------------------
        // Termina Escenario
        // -------------------------------------------------------------------------------------------------------------------------
        
        //saveFrame();

        //-------------------------------------------------------------------------------------
        // draw skybox as last
        // -------------------
        skyboxShader.use();
        skybox.Draw(skyboxShader, view, projection, camera);

        // Limitar el framerate a 60
        deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
        //std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
        if (deltaTime < LOOP_TIME)
        {
            SDL_Delay((int)(LOOP_TIME - deltaTime));
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    skybox.Terminate();

    glfwTerminate();
    return 0;
}

//Función que se encarga del manejo de las entradas del teclado. Dependiendo de la tecla, se va a crear ejecutar una acción distinta.
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    //Teclas para movernos en el escenario
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, (float)deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, (float)deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, (float)deltaTime);
    //To Configure Model
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        lightPosition.x++;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        lightPosition.x--;

    //Car animation
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
        animacion ^= true;
        flag = false;
    }
    
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
        animacionDos ^= true;
    
    if (key == GLFW_KEY_R && action == GLFW_PRESS){ //Con la tecla R reiniciamos la animación
        movAuto_z = 0.0f;
        movAuto_y = 0.0f;
        movAuto_x = 0.0f;
        movPersonaje = 0.0f;
        giroPuerta = 0.0f;
        orienta = 0.0f;
        animacion = false;
        flag = false;
        puertaFlag = false;
        animacionDos = false;
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS){
        animacionDos = false;
    }
    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
