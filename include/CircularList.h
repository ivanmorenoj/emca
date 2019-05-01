#ifndef CIRCULARLIST_H_
#define CIRCULARLIST_H_

#include <cstddef>

template <class Type>
struct Node {
    Type *data;
    Node *next;
    Node():next(NULL),data(NULL){ /*Nothing*/ }
    Node(Type *d): next(NULL), data(d){/*Nothing*/}
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