#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>   
/* Tried to add loading gif
#include <GL/glew.h>      // Include GLEW first to manage OpenGL extensions
#include "stb_image.h"    // Include stb_image for loading images
#include <iostream>       // Standard I/O library
*/
#include "GUIManager.h"   
#include "BookManager.h"


int main(int, char**) {
    // Setup GLFW and handle errors
    glfwSetErrorCallback([](int error, const char* description) {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
        });

    if (!glfwInit()) return -1;//Initialize GLFW and check if it succeeds

    const char* glsl_version = "#version 130";// Set GLSL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);// Request OpenGL version 3.x
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    // Create a window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Book Management System", NULL, NULL);
    if (window == NULL) return -1; // Check if the window was created successfully
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Setup ImGui context
    IMGUI_CHECKVERSION(); //Check ImGui version
    ImGui::CreateContext(); // Create ImGui context
    ImGuiIO& io = ImGui::GetIO(); // Access ImGui IO configuration
    io.FontGlobalScale = 1.5f;  // Increase global font size for readability

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load custom fonts
    ImFont* defaultFont = io.Fonts->AddFontFromFileTTF("C:\\Users\\user\\OneDrive\\Desktop\\Preperatiion for the test\\FINALPRJ\\FINALPRJ\\include\\ImGui\\misc\\fonts\\Cousine-Regular.ttf", 15.0f); // Load default font

    /*Tried to add loading gif
    GLuint loadingTexture = LoadTextureFromFile("C:\\Users\\user\\OneDrive\\Desktop\\Preperatiion for the test\\FINALPRJ\\FINALPRJ\\loading.gif");
    */

    // Ensure fonts are ready for use
    ImGui_ImplOpenGL3_CreateFontsTexture();

    BookManager bookManager;
    GUIManager guiManager(bookManager);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        guiManager.renderMainWindow();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}