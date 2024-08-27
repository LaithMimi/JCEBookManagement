
// HTTPClient.cpp
#include "HTTPClient.h"
#include <iostream>
#include "httplib.h"
#include <nlohmann/json.hpp>

std::string HTTPClient::fetchBookData(const std::string& bookName) {
    httplib::Client cli("openlibrary.org");
    auto res = cli.Get(("/search.json?title=" + httplib::detail::encode_url(bookName)).c_str());
    if (res && res->status == 200) {
        return res->body;
    }
    else {
        std::cerr << "HTTP request failed: " << res.error() << std::endl;
        return "";
    }
}

// HTTPClient.cpp
std::vector<Book> HTTPClient::parseBookData(const std::string& jsonData) {
    nlohmann::json result = nlohmann::json::parse(jsonData);
    nlohmann::json books = nlohmann::json::array();

    if (result.contains("docs") && result["docs"].is_array()) {
        for (const auto& book : result["docs"]) {
            nlohmann::json bookJson;
            bookJson["title"] = book.value("title", "");

            if (book.contains("author_name") && book["author_name"].is_array() && !book["author_name"].empty()) {
                bookJson["author"] = book["author_name"][0];
            }
            else {
                bookJson["author"] = "";
            }

            if (book.contains("isbn") && book["isbn"].is_array() && !book["isbn"].empty()) {
                bookJson["isbn"] = book["isbn"][0];
            }
            else {
                bookJson["isbn"] = "";
            }

            if (book.contains("publisher") && book["publisher"].is_array() && !book["publisher"].empty()) {
                bookJson["publisher"] = book["publisher"][0];
            }
            else {
                bookJson["publisher"] = "";
            }

            bookJson["year"] = book.value("first_publish_year", 0);
            if (book.contains("cover_i")) {
                bookJson["coverImage"] = "https://covers.openlibrary.org/b/id/" + std::to_string(book["cover_i"].get<int>()) + "-M.jpg";
            }
            else {
                bookJson["coverImage"] = "";
            }
            books.push_back(bookJson);
        }
    }

    return jsonParser.parseBooks(books.dump());
}

std::vector<Book> HTTPClient::searchBook(const std::string& bookName) {
    std::string data = fetchBookData(bookName);
    std::lock_guard<std::mutex> guard(mtx);
    return parseBookData(data);
}
