// HTTPClient.h
#pragma once
#include <string>
#include <vector>
#include <mutex>
#include "Book.h"
#include "JSONParser.h"

class HTTPClient {
public:
    std::vector<Book> searchBook(const std::string& bookName);

private:
    std::string fetchBookData(const std::string& bookName);
    std::vector<Book> parseBookData(const std::string& jsonData);
    std::mutex mtx;
    JSONParser jsonParser;
};