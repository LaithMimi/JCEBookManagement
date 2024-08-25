#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include "imgui.h"
#include "BookManager.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class GUIManager {
public:
    GUIManager(BookManager& bookManager);
    ~GUIManager();

    void renderMainWindow();

private:
    BookManager& bookManager;

    // Store search results
    std::vector<Book> title_search_results;
    std::vector<Book> author_search_results;

    // Map to store selected books with the title as the key
    std::map<std::string, Book> selected_books_map;

    // Pointer to the selected book
    const Book* selected_book = nullptr;

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

    // Function to display search results and book details
    void displaySearchResults();
};

#endif // GUI_MANAGER_H
