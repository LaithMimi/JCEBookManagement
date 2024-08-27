#include "GUIManager.h"

GUIManager::GUIManager(BookManager& bookManager) : bookManager(bookManager) {    // Set up the color scheme for ImGui
    ImGui::StyleColorsDark(); // Start with a dark theme
    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.2f, 1.0f);        // Dark blue background
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.25f, 1.0f);      // Slightly lighter blue background for child windows
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.9f, 0.7f, 0.7f, 1.0f);         // Darker frame background
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

GUIManager::~GUIManager() {
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

        title_search_results.clear();
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

void GUIManager::displaySearchResults() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this] { return title_search_completed.load() || author_search_completed.load(); });

    ImGui::Text("Searched Books:");

    // Create the Searched Books child window on the left
    ImGui::BeginChild("SearchResults", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 400), true);

    if (!author_search_results.empty()) {
        for (const auto& book : author_search_results) {
            if (ImGui::Selectable(book.getTitle().c_str(), false, 0, ImVec2(0, 0))) {
                auto result = selected_books_map.emplace(book.getTitle(), book);
                selected_book = &result.first->second;
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

    ImGui::EndChild();

    ImGui::SameLine();  // Position the "Books History" section to the right of "SearchResults"

    // Create the Books History child window on the right
    ImGui::BeginChild("BooksHistory", ImVec2(ImGui::GetContentRegionAvail().x, 400), true);
    ImGui::Text("Books History:");

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
            selected_books_map.erase(selected_book->getTitle()); // Remove the selected book from the map
            selected_book = nullptr; // Clear the currently selected book
        }
    }

    // Add a button to clear all books from history
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 210); // Adjust value for proper alignment
    if (ImGui::Button("Clear All History", ImVec2(200, 0))) { // Adjust the width of the button
        selected_books_map.clear(); // Clear the entire history
        selected_book = nullptr;    // Clear the currently selected book
    }

    ImGui::Separator();

    if (selected_book) {
        ImGui::Text("Selected Book Details:");
        displayBookDetails(*selected_book);
    }

}
void GUIManager::displayBookDetails(const Book& book) {
    ImGui::Text("Title: %s", book.getTitle().c_str());
    ImGui::Text("Author: %s", book.getAuthor().c_str());
    ImGui::Text("ISBN: %s", book.getISBN().c_str());
    ImGui::Text("Publisher: %s", book.getPublisher().c_str());
    ImGui::Text("Year: %d", book.getYear());


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
    static float minRatingFilter = 0.0f;

    if (!title_search_in_progress && !author_search_in_progress) {

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
        ImGui::Separator();
        ImGui::Spacing();

        // Set the color of the text box
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

        // Adjust width of input fields based on window size
        ImGui::Text("Title:");
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // Black color for text
        ImGui::SetNextItemWidth(windowSize.x - 150); // Adjust width to fit the window
        ImGui::InputText("##search_query", search_query, IM_ARRAYSIZE(search_query));
        ImGui::PopStyleColor(); // Revert to previous color

        // Revert to the previous style
        ImGui::PopStyleColor(3);

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
            // Set the color of the text box
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));


            ImGui::Text("Author:");
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // Black color for text
            ImGui::SetNextItemWidth(windowSize.x - 150); // Adjust width to fit the window
            ImGui::InputText("##author_query", author_query, IM_ARRAYSIZE(author_query));
            ImGui::PopStyleColor(); // Revert to previous color

            // Revert to the previous style
            ImGui::PopStyleColor(3);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();


        // Set the color of the button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));

        // Center the submit button
        ImGui::SetCursorPosX((windowSize.x - 120) / 2);
        if (ImGui::Button("Submit", ImVec2(120, 0))) {
            if (title_search_thread.joinable()) {
                title_search_thread.join();
            }
            if (author_search_thread.joinable()) {
                author_search_thread.join();
            }

            title_search_completed = false;
            author_search_completed = false;

            if (search_by_author && strlen(author_query) > 0) {
                author_search_thread = std::thread(&GUIManager::searchBooksByAuthor, this, author_query);
            }
            else if (!search_by_author) {
                title_search_thread = std::thread(&GUIManager::searchBooksByTitle, this, search_query);
            }

        }
        // Revert to the previous style
        ImGui::PopStyleColor(3);

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

        // Calculate the centered position
        float textPosX = (windowSize.x - textSize.x) / 2.0f;
        float textPosY = (windowSize.y - textSize.y) / 2.0f;

        // Set the cursor position to the calculated centered position
        ImGui::SetCursorPos(ImVec2(textPosX, textPosY));

        // Draw the text
        ImGui::Text("%s", text);

    }


    ImGui::Spacing();
    //ImGui::Separator();
    ImGui::Spacing();

    // If no details were found, show a popup
    if (no_details_found) {
        ImGui::OpenPopup("No Details Found");
        no_details_found = false; // Reset the flag
    }

    if (ImGui::BeginPopupModal("No Details Found", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("No details found for one or more books.");
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }


    if (author_search_completed || title_search_completed) {
        displaySearchResults();
    }

    ImGui::End();
}
