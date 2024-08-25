#include "Book.h"

Book::Book(std::string title, std::string author, std::string isbn,
    std::string publisher, int year, std::string coverImage,
    std::vector<std::string> subjects, std::vector<std::string> languages, std::string publishDate)
    : title(title), author(author), isbn(isbn), publisher(publisher), publish_year(year),
    coverImage(coverImage), subjects(subjects), languages(languages), publishDate(publishDate) {}

std::string Book::getTitle() const { return title; }

std::string Book::getAuthor() const { return author; }

std::string Book::getISBN() const { return isbn; }

std::string Book::getPublisher() const { return publisher; }

int Book::getYear() const { return publish_year; }


std::string Book::getCoverImage() const { return coverImage; }
std::vector<std::string> Book::getSubjects() const { return subjects; }
std::vector<std::string> Book::getLanguages() const { return languages; }
std::string Book::getPublishDate() const { return publishDate; }
