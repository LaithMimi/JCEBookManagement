#include "BookManager.h"
#include <iostream>
#include <fstream>

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
    if (books.empty()) {
        std::cout << "No books available to search." << std::endl;
        return result;
    }
    for (const auto& book : books) {
        if (book.getAuthor().find(author) != std::string::npos) {
            result.push_back(book);
        }
    }
    return result;
}


void BookManager::sortBooksByTitle() {
    std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
        return a.getTitle() < b.getTitle();
        });
}

void BookManager::sortBooksByAuthor() {
    std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
        return a.getAuthor() < b.getAuthor();
        });
}

void BookManager::sortBooksByYear() {
    std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
        return a.getYear() < b.getYear();
        });
}


void BookManager::markAsFavorite(const std::string& isbn) {
    auto it = std::find(favoriteBooks.begin(), favoriteBooks.end(), isbn);
    if (it == favoriteBooks.end()) {
        favoriteBooks.push_back(isbn);
    }
}

std::vector<Book> BookManager::getFavoriteBooks() const {
    std::vector<Book> favorites;
    for (const auto& isbn : favoriteBooks) {
        for (const auto& book : books) {
            if (book.getISBN() == isbn) {
                favorites.push_back(book);
            }
        }
    }
    return favorites;
}
