#pragma once
#include <vector>
#include "Book.h"
#include "include/nlohmann/json.hpp"// Include a JSON library like nlohmann/json
#include <string>


class JSONParser {
public:
    std::vector<Book> parseBooks(const std::string& jsonData);
    std::string serializeBook(const Book& book);
};
