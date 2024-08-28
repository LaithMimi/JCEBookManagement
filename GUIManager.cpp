#include "GUIManager.h"
/*Tried to add loading gif
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


GLuint LoadTextureFromFile(const char* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data); // Free the image memory
    return texture;
}*/

GUIManager::GUIManager(BookManager& bookManager) : bookManager(bookManager) {    // Set up the color scheme for ImGui
    ImGui::StyleColorsDark(); // Start with a dark theme
    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.2f, 1.0f);        // Dark blue background
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.25f, 1.0f);      // Slightly lighter blue background for child windows
    
    /*Boxes Background*/
    style.Colors[ImGuiCol_FrameBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);         // Darker frame background
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1f, 0.1f, 0.1f, 0.1f);  // Hovered frame background
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.1f, 0.1f, 0.1f, 0.1f);   // Active frame background

    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.5f, 0.2f, 1.0f);          // Green buttons
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.6f, 0.3f, 1.0f);   // Hovered green buttons
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1f, 0.4f, 0.1f, 1.0f);    // Active green buttons
    style.Colors[ImGuiCol_Separator] = ImVec4(0.75f, 0.2f, 0.2f, 1.0f);      // Red separators
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.2f, 0.2f, 0.3f, 0.9f);         // Popup background with slight transparency
    style.Colors[ImGuiCol_Header] = ImVec4(0.1f, 0.1f, 0.1f, 0.1f);          // Header background
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.4f, 0.4f, 0.6f, 1.0f);   // Hovered header background
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2f, 0.2f, 0.4f, 1.0f);    // Active header background
}

GUIManager::~GUIManager() { //ensures that any threads started have completed their execution before the object is destroyed. 
    if (title_search_thread.joinable()) {
        title_search_thread.join();
    }
    if (author_search_thread.joinable()) {
        author_search_thread.join();
    }
}

void GUIManager::searchBooksByTitle(const std::string& title) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        title_search_in_progress = true;
        title_search_completed = false;

        title_search_results.clear();//removes all the old entries

        // Call the function to load books from the API based on the title
        title_search_results = bookManager.searchBooksByTitle(title);

        title_search_in_progress = false;
        title_search_completed = true;
    }
    cv.notify_one();  // Notify that the title search is complete
}
void GUIManager::searchBooksByAuthor(const std::string& author) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        author_search_in_progress = true;
        author_search_completed = false;

        author_search_results.clear();

        // Call the function to load books from the API based on the author's name
        author_search_results = bookManager.searchBooksByAuthor(author);

        author_search_in_progress = false;
        author_search_completed = true;
    }
    cv.notify_one();  // Notify that the author search is complete
}

void GUIManager::displayBookDetails(const Book& book) {
    ImGui::SetWindowFontScale(1.25f);
    ImGui::Text("Title: %s", book.getTitle().c_str());
    ImGui::Text("Author: %s", book.getAuthor().c_str());
    ImGui::Text("ISBN: %s", book.getISBN().c_str());
    ImGui::Text("Publisher: %s", book.getPublisher().c_str());
    ImGui::Text("Year: %d", book.getYear());
    ImGui::SetWindowFontScale(1.0f);
}

void GUIManager::displaySearchResults() {
    std::unique_lock<std::mutex> lock(mtx);    // Lock the mutex to ensure thread-safe access to shared data.

    cv.wait(lock, [this] { return title_search_completed.load() || author_search_completed.load(); });

    ImGui::Text("Searched Books:");
    
    ImGui::SameLine();//Move to the same line to position the next child window beside the previous one
    const char* historyText = "Books History:";
    float historyTextWidth = ImGui::CalcTextSize(historyText).x;
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x*0.5f + historyTextWidth);
    ImGui::Text("%s", historyText);


    //Begin a child window named "SearchResults" on the left side, occupying half the available width and 400 pixels in height.
    ImGui::BeginChild("SearchResults", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 400), true);

    if (!author_search_results.empty()) {// Check if there are any results from the author search
        for (const auto& book : author_search_results) {
            if (ImGui::Selectable(book.getTitle().c_str(), false, 0, ImVec2(0, 0))) {
                // Display each book title as a selectable item in the GUI
                // If a book title is clicked, the following block executes

                auto result = selected_books_map.emplace(book.getTitle(), book); // Add the selected book to the selected_books_map
                selected_book = &result.first->second; // Set "the selected_book" pointer to the newly selected book.
            }
        }
    }
    else if (!title_search_results.empty()) {
        for (const auto& book : title_search_results) {
            if (ImGui::Selectable(book.getTitle().c_str(), false, 0, ImVec2(0, 0))) {
                auto result = selected_books_map.emplace(book.getTitle(), book);
                selected_book = &result.first->second;
            }
        }
    }
    else {
        ImGui::Text("No results found.");
    }
    ImGui::EndChild(); //End the "SearchResults" child window

    ImGui::SameLine();//Move to the same line to position the next child window beside the previous one
    
    // Create the Books History child window on the right
    ImGui::BeginChild("BooksHistory", ImVec2(ImGui::GetContentRegionAvail().x, 400), true);
    // Iterate through the selected_books_map and display the books in the history
    for (auto& pair : selected_books_map) {
        if (ImGui::Selectable(pair.first.c_str(), selected_book == &pair.second, 0, ImVec2(0, 0))) {
            selected_book = &pair.second; // Set the selected book to the one clicked in the history
        }
    }
    ImGui::EndChild();

    // Add a button to delete the selected book from history
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 210); // Adjust value for proper alignment
    if (selected_book) {
        if (ImGui::Button("Delete", ImVec2(200, 0))) {
            //Create a "Delete" button with a width of 200 pixels.
            //If the button is clicked, the following block executes.

            selected_books_map.erase(selected_book->getTitle()); // Remove the selected book from the map
            selected_book = nullptr; // Clear the currently selected book (the ptr)
        }
    }

    //Add a button to clear all books from history
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 210); // Adjust value for proper alignment
    if (ImGui::Button("Clear All History", ImVec2(200, 0))) { // Adjust the width of the button
        selected_books_map.clear(); // Clear the entire history
        selected_book = nullptr;    // Clear the currently selected book
    }

    ImGui::Separator();
    if (selected_book) {
        ImGui::SetWindowFontScale(1.35f);
        ImGui::Text("Selected Book Details:");
        ImGui::SetWindowFontScale(1.0f);
        displayBookDetails(*selected_book);
    }

}
void GUIManager::renderMainWindow() {
    // Get the current size of the application's main window
    ImVec2 windowSize = ImGui::GetIO().DisplaySize;

    // Set the next window size to match the application's main window size
    ImGui::SetNextWindowSize(windowSize);
    ImGui::SetNextWindowPos(ImVec2(0, 0)); // Position the window at the top-left corner

    ImGui::Begin("Book Management System", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    static char search_query[128] = "";
    static char author_query[128] = "";
    static bool search_by_author = false;

    if (!title_search_in_progress && !author_search_in_progress) { // Check if no search is currently in progress
        ImGui::Text("Search for Books");
        ImGui::SameLine();
        // Refresh Button
        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x);
        if (ImGui::Button("Refresh", ImVec2(160, 0))) {
            // Reset states when Refresh is clicked
            search_query[0] = '\0';   // Clear search query
            author_query[0] = '\0';   // Clear author query
            title_search_results.clear();  // Clear search results by title
            author_search_results.clear(); // Clear search results by author
            selected_book = nullptr;  // Deselect any selected book
        }
        ImGui::Spacing();

        // Adjust width of input fields based on window size
        ImGui::Text("Title:");
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // Black color for text in input box
        ImGui::SetNextItemWidth(windowSize.x - 150); // Adjust width to fit the window
        ImGui::InputText("##search_query", search_query, IM_ARRAYSIZE(search_query));
        ImGui::PopStyleColor(); // Revert to default text color

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Sorting Buttons
        if (ImGui::Button("Sort by Title")) {
            if (search_by_author) {
                std::sort(author_search_results.begin(), author_search_results.end(), [](const Book& a, const Book& b) {
                    return a.getTitle() < b.getTitle();
                    });
            }
            else {
                std::sort(title_search_results.begin(), title_search_results.end(), [](const Book& a, const Book& b) {
                    return a.getTitle() < b.getTitle();
                    });
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Sort by Author")) {
            if (search_by_author) {
                std::sort(author_search_results.begin(), author_search_results.end(), [](const Book& a, const Book& b) {
                    return a.getAuthor() < b.getAuthor();
                    });
            }
            else {
                std::sort(title_search_results.begin(), title_search_results.end(), [](const Book& a, const Book& b) {
                    return a.getAuthor() < b.getAuthor();
                    });
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Sort by Year")) {
            if (search_by_author) {
                std::sort(author_search_results.begin(), author_search_results.end(), [](const Book& a, const Book& b) {
                    return a.getYear() < b.getYear();
                    });
            }
            else {
                std::sort(title_search_results.begin(), title_search_results.end(), [](const Book& a, const Book& b) {
                    return a.getYear() < b.getYear();
                    });
            }
        }

        if (ImGui::Checkbox("Search by Author Name", &search_by_author)) {
            if (!search_by_author) {
                memset(author_query, 0, sizeof(author_query));
            }
        }

        if (search_by_author) {
            ImGui::Text("Author:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(windowSize.x - 150); // Adjust width to fit the window
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // Black color for text in input box
            ImGui::InputText("##author_query", author_query, IM_ARRAYSIZE(author_query));
            ImGui::PopStyleColor(); // Revert to default text color

        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Center the submit button
        ImGui::SetCursorPosX((windowSize.x - 120) / 2);
        if (ImGui::Button("Submit", ImVec2(120, 0))) { // If there are active threads, join them before starting new searches
            if (title_search_thread.joinable()) {
                title_search_thread.join();
            }
            if (author_search_thread.joinable()) {
                author_search_thread.join();
            }
            // Reset the completion flags
            title_search_completed = false;
            author_search_completed = false;

            // Start the search threads based on the query and search mode
            if (search_by_author && strlen(author_query) > 0) {
                author_search_thread = std::thread(&GUIManager::searchBooksByAuthor, this, author_query);
            }
            else if (!search_by_author) {
                title_search_thread = std::thread(&GUIManager::searchBooksByTitle, this, search_query);
            }
        }

        // Display the total count of books currently shown in the list
        int displayedBookCount = search_by_author ? author_search_results.size() : title_search_results.size();
        ImGui::Text("Total Books Found: %d", displayedBookCount);

    }
    else {
        const char* text = "Searching... Please wait.";

        // Calculate the size of the text
        ImVec2 textSize = ImGui::CalcTextSize(text);

        // Get the window's size
        ImVec2 windowSize = ImGui::GetWindowSize();

        // Calculate the centered position for the text and image
        float textPosX = (windowSize.x - textSize.x) / 2.2f;
        float textPosY = (windowSize.y - textSize.y) / 2.0f;
        /*
        // Calculate the centered position for the image
        float imagePosX = (windowSize.x - 64) / 2.2f;  // Assuming the image is 64x64 pixels
        float imagePosY = textPosY - 70; // Position the image above the text

       
       // Render the loading image
        ImGui::SetCursorPos(ImVec2(imagePosX, imagePosY));
        if (loadingTexture) {
            // Safe to use loadingTexture
            ImGui::Image((void*)(intptr_t)loadingTexture, ImVec2(64, 64));
        }
        */
        
        // Set the cursor position to the calculated centered position for the text
        ImGui::SetCursorPos(ImVec2(textPosX, textPosY));

        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("%s", text);
        ImGui::SetWindowFontScale(1.0f);
    }

    ImGui::Spacing();

    if (author_search_completed || title_search_completed) {
        displaySearchResults();
    }

    ImGui::End();
}
