#ifndef CIRCULARLIST_H_
#define CIRCULARLIST_H_

#include <cstddef>

template <class Type>
struct Node {
    Type *data;
    Node *next;
    Node():data(NULL),next(NULL){ /*Nothing*/ }
    Node(Type *d): data(d),next(NULL){/*Nothing*/}
};

template <class Type>
class CircularList {
private:
    Node<Type> *head;
    Node<Type> *current;
public:
    CircularList();
    ~CircularList();
    void addNode(Type *newNode);
    void next();
    void deleteAll();
    int getLength();
    Type *getCurrentData();
};

#endif