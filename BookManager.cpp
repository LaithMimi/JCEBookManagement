#include "BookManager.h"
#include <iostream>
void BookManager::addBook(const Book& book) {
    books.push_back(book);
}

Book* BookManager::getBook(const std::string& isbn) {
    for (auto& book : books) {
        if (book.getISBN() == isbn) {
            return &book;
        }
    }
    return nullptr;
}

std::vector<Book>& BookManager::getBooks() {
    return books;
}

std::vector<Book> BookManager::loadBooksFromAPI(const std::string& searchQuery) {
    // Fetch books from API using HTTPClient
    books = httpClient.searchBook(searchQuery);
    return books;
}

std::vector<Book> BookManager::searchBooksByTitle(const std::string& title) {
    std::vector<Book> result;
    for (const auto& book : books) {
        if (book.getTitle().find(title) != std::string::npos) {
            result.push_back(book);
        }
    }
    return result;
}

std::vector<Book> BookManager::searchBooksByAuthor(const std::string& author) {
    std::vector<Book> result;

    // Check if the books vector is empty
    if (books.empty()) {
        // Optionally, log an error, display a message, or handle the empty case in some way
        std::cout << "No books available to search." << std::endl;
        return result;  // Return an empty result if the books vector is empty
    }

    for (const auto& book : books) {
        if (book.getAuthor().find(author) != std::string::npos) {
            result.push_back(book);
        }
    }

    return result;
}

