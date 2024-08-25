#ifndef BOOK_MANAGER_H
#define BOOK_MANAGER_H

#include <vector>
#include <string>
#include "Book.h"
#include "JSONParser.h"
#include "HTTPClient.h"

class BookManager {
public:
    // Adds a new book to the collection
    void addBook(const Book& book);

    // Returns a pointer to a book by its ISBN
    Book* getBook(const std::string& isbn);

    // Returns a reference to the list of all books
    std::vector<Book>& getBooks();

    // Method to load books from an API using HTTPClient
    std::vector<Book> loadBooksFromAPI(const std::string& searchQuery);

    // Method to search books by title
    std::vector<Book> searchBooksByTitle(const std::string& title);

    // Method to search books by author
    std::vector<Book> searchBooksByAuthor(const std::string& author);

private:
    std::vector<Book> books;
    JSONParser jsonParser;
    HTTPClient httpClient;  // Add HTTPClient as a member
};

#endif // BOOK_MANAGER_H
