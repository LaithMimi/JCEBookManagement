#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <vector>

class Book {
public:
    Book(std::string title, std::string author, std::string isbn,
        std::string publisher, int year, std::string coverImage,
        std::vector<std::string> subjects, std::vector<std::string> languages, std::string publishDate);

    std::string getTitle() const;

    std::string getAuthor() const;

    std::string getISBN() const;

    std::string getPublisher() const;

    int getYear() const;

    std::string getCoverImage() const;

    std::vector<std::string> getSubjects() const;

    std::vector<std::string> getLanguages() const;

    std::string getPublishDate() const;

private:
    std::string title;
    std::string author;
    std::string isbn;
    std::string publisher;
    int publish_year;

    std::string coverImage;
    std::vector<std::string> subjects;
    std::vector<std::string> languages;
    std::string publishDate;
};

#endif // BOOK_H