#ifndef LIST_LINKER_H
#define LIST_LINKER_H

#include <string>
#include <iostream>
using namespace std;
template<typename T>

class List {
private:
    struct Elem {
        T* data;
        Elem* next;

        Elem(T* data, Elem* next) {
            this->data = data;
            this->next = next;
        }
    };

    Elem* head, * tail;
    int size;


public:
    List() {
        head = 0;
        tail = 0;
        size = 0;
    }

    void addLast(T* data) {
        Elem* elem = new Elem(data, 0);
        if (tail) {
            tail->next = elem;
            tail = elem;
        }
        else {
            head = tail = elem;
        }
        size++;
    }

    T* getWithoutRemovingFromList(int ind) {
        int i = 0;
        Elem* curr = head;
        while (i < ind) {
            curr = curr->next;
            i++;
        }

        return curr->data;
    }

    T* removeFirst() {
        if (!head) { //prazna lista
            return 0;
        }
        Elem* elem = head;
        head = head->next;
        if (!head) { //izbrisali smo poslednji element u listi, i zato moramo tail da stavimo na 0
            tail = 0;
        }
        T* ret = elem->data;
        size--;
        delete elem;
        return ret; //vracamo vrednost izbrisanog elementa
    }

    void removeFirstForever() {
        if (!head) { //prazna lista
            return;
        }
        Elem* elem = head;
        head = head->next;
        if (!head) { //izbrisali smo poslednji element u listi, i zato moramo tail da stavimo na 0
            tail = 0;
        }
        T* ret = elem->data;
        size--;
        delete elem;
    }

    int getSize() {
        return size;
    }

    void setSize(int sizee) {
         size = sizee;
    }
};
#endif
