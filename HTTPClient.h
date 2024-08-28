#pragma once
#include <string>
#include <vector>
#include <mutex>
#include "Book.h"
#include "JSONParser.h"

class HTTPClient {
public:
    std::vector<Book> searchBooksByTitle(const std::string& title);  // Search by book title
    std::vector<Book> searchBookByAuthor(const std::string& authorName);  // Search by author name

private:
    std::string fetchBookData(const std::string& query);
    std::vector<Book> parseBookData(const std::string& jsonData);
    JSONParser jsonParser;
};
