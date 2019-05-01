#include "CircularList.h"

template <class Type>
CircularList<Type>::CircularList() {
    head = NULL;
    current = NULL;
}
template <class Type>
CircularList<Type>::~CircularList() {
    deleteAll();
}
template <class Type>
void CircularList<Type>::addNode(Type *newNode) {
    if (head == NULL ) {
        head = new Node<Type> (newNode);
        head->next = head;
        current = head;
    } else {
        Node<Type> *n_node = new Node<Type> (newNode);
        Node<Type> *tmp = head;
        do {
            tmp = tmp->next;
        }while (tmp->next != head);
        tmp->next = n_node;
        n_node->next = head; 
    }
}
template <class Type>
void CircularList<Type>::next() {
    current = current->next;
}
template <class Type>
void CircularList<Type>::deleteAll() {
    while (head) {
        current = NULL;
        if (head->next == head) {
            head->next = NULL;
            delete head;
            head = NULL;
        } else {
            Node<Type> *tmp = head;
            Node<Type> *aux;
            do {
                aux = tmp;
                tmp = tmp->next;
            } while (tmp->next != head);
            aux->next = head;
            tmp->next =NULL;
            delete tmp;
        }
    }
}
template <class Type>
int CircularList<Type>::getLength() {
    int count = 0;
    if (head) {
        Node<Type> *tmp = head;
		do{
			count++;
			tmp = tmp->next;
		}while(tmp != head);
    }
    return count;
}
template <class Type>
Type *CircularList<Type>::getCurrentData() {
    if(head)
        return current->data;
    else 
        return NULL;
}