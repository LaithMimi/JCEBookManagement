#ifndef BOOK_H
#define BOOK_H

#include <string>

class Book {
public:
    //Book() = default;  // Default constructor
    Book(std::string title, std::string author, std::string isbn,
        std::string publisher, int year, std::string coverImage);

    std::string getTitle() const;
    std::string getAuthor() const;
    std::string getISBN() const;
    std::string getPublisher() const;
    int getYear() const;
    std::string getCoverImage() const;

private:
    std::string title;
    std::string author;
    std::string isbn;
    std::string publisher;
    int publish_year;
    std::string coverImage;
};

#endif // BOOK_H