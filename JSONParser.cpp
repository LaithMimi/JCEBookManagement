#pragma once
#include "JSONParser.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::vector<Book> JSONParser::parseBooks(const std::string& jsonData) {
    std::vector<Book> books;
    json j = json::parse(jsonData);

    for (const auto& item : j) {
        std::vector<std::string> subjects;
        if (item.contains("subject") && item["subject"].is_array()) {
            for (const auto& subject : item["subject"]) {
                subjects.push_back(subject.get<std::string>());
            }
        }

        std::vector<std::string> languages;
        if (item.contains("language") && item["language"].is_array()) {
            for (const auto& language : item["language"]) {
                languages.push_back(language.get<std::string>());
            }
        }

        books.emplace_back(
            item["title"].get<std::string>(),
            item["author"].get<std::string>(),
            item["isbn"].get<std::string>(),
            item["publisher"].get<std::string>(),
            item["year"].get<int>(),
            item["coverImage"].get<std::string>(),
            subjects,
            languages,
            item.value("publish_date", "")
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

    // Add subjects
    j["subjects"] = book.getSubjects();

    // Add languages
    j["languages"] = book.getLanguages();

    // Add publish date
    j["publishDate"] = book.getPublishDate();

    return j.dump();
}
