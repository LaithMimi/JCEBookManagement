#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <vector>

class Book {
public:
    Book(std::string title, std::string author, std::string isbn,
        std::string publisher, int year);

    std::string getTitle() const;

    std::string getAuthor() const;

    std::string getISBN() const;

    std::string getPublisher() const;

    int getYear() const;


private:
    std::string title;
    std::string author;
    std::string isbn;
    std::string publisher;
    int publish_year;

};

#endif // BOOK_H