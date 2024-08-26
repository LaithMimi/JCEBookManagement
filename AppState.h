#pragma once
#include "Book.h"
#include <vector>

struct AppState {
    std::vector<Book> books;
    std::string lastSearchQuery;
};
