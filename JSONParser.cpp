#pragma once
#include "JSONParser.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::vector<Book> JSONParser::parseBooks(const std::string& jsonData) {
    std::vector<Book> books;
    json j = json::parse(jsonData);

    for (const auto& item : j) {
        books.emplace_back(
            item["title"].get<std::string>(),
            item["author"].get<std::string>(),
            item["isbn"].get<std::string>(),
            item["publisher"].get<std::string>(),
            item["year"].get<int>(),
            item["coverImage"].get<std::string>()
        );
    }

    return books;
}

std::string JSONParser::serializeBook(const Book& book) {
    json j;
    j["title"] = book.getTitle();
    j["author"] = book.getAuthor();
    j["isbn"] = book.getISBN();
    j["publisher"] = book.getPublisher();
    j["year"] = book.getYear();
    j["coverImage"] = book.getCoverImage();

    return j.dump();
}