#include "Book.h"

Book::Book(std::string title, std::string author, std::string isbn,
    std::string publisher, int year, std::string coverImage)
    : title(std::move(title)), author(std::move(author)),
    isbn(std::move(isbn)),publisher(std::move(publisher)), publish_year(year), coverImage(std::move(coverImage)) {}

std::string Book::getTitle() const { 
    return title;
}
std::string Book::getAuthor() const {
    return author;
}
std::string Book::getISBN() const {
    return isbn;
}
std::string Book::getPublisher() const {
    return publisher;
}
int Book::getYear() const {
    return publish_year;
}
std::string Book::getCoverImage() const {
    return coverImage;
}