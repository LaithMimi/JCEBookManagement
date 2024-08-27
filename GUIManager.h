#pragma once
#include "imgui.h"
#include "BookManager.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>
#include <vector>

class GUIManager {
public:
    GUIManager(BookManager& bookManager);
    ~GUIManager();

    void renderMainWindow();

private:
    BookManager& bookManager;
    bool no_details_found = false;  // Flag to indicate if no details were found
    const Book* selected_book = nullptr;  // Pointer to the currently selected book

    // Store search results
    std::vector<Book> title_search_results;
    std::vector<Book> author_search_results;

    // Map to store selected books with the title as the key
    std::map<std::string, Book> selected_books_map;

    // Synchronization variables
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> title_search_in_progress{ false };
    std::atomic<bool> author_search_in_progress{ false };
    std::atomic<bool> title_search_completed{ false };
    std::atomic<bool> author_search_completed{ false };

    // Threads for searching
    std::thread title_search_thread;
    std::thread author_search_thread;

    // Functions to handle the search operations in threads
    void searchBooksByTitle(const std::string& title);
    void searchBooksByAuthor(const std::string& author);
    void displayBookDetails(const Book& book);

    // Function to display search results and book details
    void displaySearchResults();
};
