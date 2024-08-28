#include "Book.h"

Book::Book(std::string title, std::string author, std::string isbn,
    std::string publisher, int year)
    : title(title), author(author), isbn(isbn), publisher(publisher), publish_year(year)
    {}

std::string Book::getTitle() const { 
    return title;
}

std::string Book::getAuthor() const { return author; }

std::string Book::getISBN() const { return isbn; }

std::string Book::getPublisher() const { return publisher; }

int Book::getYear() const { return publish_year; }


