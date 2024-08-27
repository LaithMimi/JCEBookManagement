#include "HTTPClient.h"
#include <iostream>
#include "httplib.h"
#include <nlohmann/json.hpp>

std::string HTTPClient::fetchBookData(const std::string& query) {
    httplib::Client cli("openlibrary.org");
    auto res = cli.Get(query.c_str());
    if (res && res->status == 200) {
        return res->body;
    }
    else {
        std::cerr << "HTTP request failed: " << res.error() << std::endl;
        return "";
    }
}

std::vector<Book> HTTPClient::parseBookData(const std::string& jsonData) {
    nlohmann::json result = nlohmann::json::parse(jsonData);
    std::vector<Book> books;

    if (result.contains("docs") && result["docs"].is_array()) {
        for (const auto& book : result["docs"]) {
            Book bookObj(
                book.value("title", ""),
                (book.contains("author_name") && !book["author_name"].empty()) ? book["author_name"][0] : "",
                (book.contains("isbn") && !book["isbn"].empty()) ? book["isbn"][0] : "",
                (book.contains("publisher") && !book["publisher"].empty()) ? book["publisher"][0] : "",
                book.value("first_publish_year", 0),
                book.contains("cover_i") ? "https://covers.openlibrary.org/b/id/" + std::to_string(book["cover_i"].get<int>()) + "-M.jpg" : "",
                std::vector<std::string>(),
                std::vector<std::string>(),
                ""
            );
            books.push_back(bookObj);
        }
    }

    return books;
}

std::vector<Book> HTTPClient::searchBooksByTitle(const std::string& title) {
    std::string url = "/search.json?title=" + httplib::detail::encode_url(title);
    return parseBookData(fetchBookData(url));
}

std::vector<Book> HTTPClient::searchBookByAuthor(const std::string& authorName) {
    std::string url = "/search.json?author=" + httplib::detail::encode_url(authorName);
    return parseBookData(fetchBookData(url));
}
