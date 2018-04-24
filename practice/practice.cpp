#include <iostream>


class Node;
bool find (Node * node, int element);
bool addElement(Node * node, int element);


class Node {
public:
    int value;
    Node * next = NULL;
};


bool find (Node * node, int element) {
    while (node != NULL) {
        if (node -> value == element) return true;
        else node = node -> next;
    }

    return false;
}

bool addElement(Node * node, int element) {
    while (node -> next != NULL) {
        node = node -> next;
    }

    node -> next = new Node();
    node -> next -> value = element;

    return true;

}



int main() {
    //create head
    Node * head = new Node();
    head->value = 0;
    
    for (int i = 1; i < 6; ++i) {
        addElement(head, i);
    }
    
    //scan every node and cout the value
    for (Node * p = head; p != NULL; p = p -> next) {
        std::cout<< "current node value: "<< p->value << std::endl;
    }


}
