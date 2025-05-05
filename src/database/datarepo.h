//
// Created by christou on 5/5/25.
//
#include <string>
#ifndef DATAREPO_H
#define DATAREPO_H
typedef struct user {
    int id;
    std::string name ;
    std::string email ;
    std::string password ;
}UserData;
typedef struct book {
    std::string title ;
    std::string isbn ;
    std::string author ;
    float price;
    bool inStock;
}BookData;
#endif //DATAREPO_H
