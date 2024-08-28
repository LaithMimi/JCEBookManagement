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

std::vector<Book> BookManager::searchBooksByTitle(const std::string& title) {
    books = httpClient.searchBooksByTitle(title);
    return books;
}

std::vector<Book> BookManager::searchBooksByAuthor(const std::string& author) {
    books = httpClient.searchBookByAuthor(author);
    return books;
}
