#include "GUIManager.h"

GUIManager::GUIManager(BookManager& bookManager) : bookManager(bookManager) {}

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
        title_search_results = bookManager.loadBooksFromAPI(title);

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
    ImGui::BeginChild("SearchResults", ImVec2(ImGui::GetContentRegionAvail().x * 0.6f, 400), true);

    if (!author_search_results.empty()) {
        ImGui::Text("Author Search Results:");
        for (const auto& book : author_search_results) {
            if (ImGui::Selectable(book.getTitle().c_str(), false, 0, ImVec2(0, 0))) {
                auto result = selected_books_map.emplace(book.getTitle(), book);
                selected_book = &result.first->second;
            }
        }
    }
    else if (!title_search_results.empty()) {
        ImGui::Text("Title Search Results:");
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

    ImGui::SameLine();

    // Display books history on the right side
    ImGui::BeginChild("BooksHistory", ImVec2(ImGui::GetContentRegionAvail().x, 400), true);
    ImGui::Text("Books History:");
    for (const auto& pair : selected_books_map) {
        if (ImGui::Selectable(pair.first.c_str(), false, 0, ImVec2(0, 0))) {
            selected_book = &pair.second;
        }
    }
    ImGui::EndChild();

    ImGui::Separator();

    if (selected_book) {
        ImGui::Text("Selected Book Details:");
        displayBookDetails(*selected_book);
    }

    ImGui::Separator();
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

        if (ImGui::Checkbox("Search by Author", &search_by_author)) {
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

        // Rating filter
       // ImGui::Text("Minimum Rating:");
       // ImGui::SameLine();
       // ImGui::SetNextItemWidth(windowSize.x / 2 - 150);
        //ImGui::SliderFloat("##minRatingFilter", &minRatingFilter, 0.0f, 5.0f);

        //ImGui::Spacing();
        //ImGui::Separator();
        //ImGui::Spacing();

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
            // Apply rating filter
          // if (minRatingFilter > 0.0f) {
          //     title_search_results = bookManager.filterBooksByRating(minRatingFilter);
          //     author_search_results = bookManager.filterBooksByRating(minRatingFilter);
          // }
        }
        // Revert to the previous style
        ImGui::PopStyleColor(3);
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
    ImGui::Separator();
    ImGui::Spacing();

    if (author_search_completed || title_search_completed) {
        displaySearchResults();
    }

    ImGui::End();
}