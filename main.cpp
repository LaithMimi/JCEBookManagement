/*#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include "HTTPClient.h"
#include "JSONParser.h"
#include "Book.h"
#include "BookManager.h"


void displayMenu() {
    std::cout << "\n--- Book Manager ---\n";
    std::cout << "1. Search for a book\n";
    std::cout << "2. show history\n";
    std::cout << "3. Exit\n";
    std::cout << "Enter your choice: ";
}

void displayBookDetails(const Book& book) {
    std::cout << "Title: " << book.getTitle() << "\n";
    std::cout << "Author: " << book.getAuthor() << "\n";
    std::cout << "ISBN: " << book.getISBN() << "\n";
    std::cout << "Publisher: " << book.getPublisher() << "\n";
    std::cout << "Year: " << book.getYear() << "\n";
    std::cout << "Cover Image URL: " << book.getCoverImage() << "\n";
    std::cout << "------------------------\n";
}

int main() {
    BookManager bookManager;
    HTTPClient httpClient;
    JSONParser jsonParser;

    bool running = true;
    while (running) {
        displayMenu();

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case 1: {
            std::string bookName;
            std::cout << "Enter book name to search: ";
            std::getline(std::cin, bookName);

            std::vector<Book> searchResults = httpClient.searchBook(bookName);
            if (searchResults.empty()) {
                std::cout << "No books found.\n";
            }
            else {
                std::cout << "Search results:\n";
                nlohmann::json jsonResults = nlohmann::json::array();
                for (const auto& book : searchResults) {
                    displayBookDetails(book);
                    jsonResults.push_back(nlohmann::json::parse(jsonParser.serializeBook(book)));
                }

                // Save the JSON data to a file
                std::ofstream file("search_results.json");
                file << jsonResults.dump(4);  // 4 spaces for indentation
                file.close();
                std::cout << "Search results have been saved to 'search_results.json'\n";
            }
            break;
        }
        case 2: {
            const auto& books = bookManager.getBooks();
            if (books.empty()) {
                std::cout << "No books in the library.\n";
            }
            else {
                std::cout << "Books in the library:\n";
                for (const auto& book : books) {
                    displayBookDetails(book);
                }
            }
            break;
        }
        case 3:
            running = false;
            std::cout << "Exiting program.\n";
            break;
        default:
            std::cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}*/
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "BookManager.h"
#include "GUIManager.h"


int main(int, char**) {
    // Setup GLFW
    glfwSetErrorCallback([](int error, const char* description) {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
        });

    if (!glfwInit()) return -1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Book Management System", NULL, NULL);
    if (window == NULL) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);


    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

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
