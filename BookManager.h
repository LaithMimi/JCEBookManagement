#pragma once
#include "Book.h"
#include "HTTPClient.h"
#include <vector>
#include <string>

class BookManager {
public:
    void addBook(const Book& book);
    Book* getBook(const std::string& isbn);
    std::vector<Book>& getBooks();
    std::vector<Book> searchBooksByTitle(const std::string& title);
    std::vector<Book> searchBooksByAuthor(const std::string& author);

private:
    std::vector<Book> books;
    HTTPClient httpClient;
};
