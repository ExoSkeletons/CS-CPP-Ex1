//
// Created by Aviad Levine on 26/03/2025.
//

#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H
#include <stdexcept>

namespace ds {
    template<class T>
    class LinkedList {
        int len = 0;

    public:
        template<class V>
        class Link {
        public:
            V val = 0;
            Link *next = nullptr;
        };

        Link<T> *head = nullptr;

        LinkedList() = default;

        ~LinkedList() {
            while (head) {
                const auto tmp = head;
                head = head->next;
                delete tmp;
            }
        }

        void addFirst(const T val) {
            const auto newNode = new Link<T>();
            newNode->val = val;
            if (head) newNode->next = head;
            head = newNode;
            len++;
        }

        void addLast(const T val) {
            const auto newNode = new Link<T>();
            newNode->val = val;
            if (head == nullptr) {
                head = newNode;
            } else {
                auto tail = head;
                while (tail->next) tail = tail->next;
                tail->next = newNode;
            }
            len++;
        }

        void addLast(const LinkedList *copy) {
            auto node = copy->head;
            while (node) {
                addLast(node->val);
                node = node->next;
            }
        }

        bool deleteFirst() {
            if (head) {
                const auto tmp = head;
                head = head->next;
                delete tmp;
                len--;
                return true;
            }
            return false;
        }

        void clear() {
            while (head) {
                const auto tmp = head;
                head = head->next;
                delete tmp;
            }
            len = 0;
        }

        bool deleteLast() {
            if (head) {
                if (head->next) {
                    auto prev = head, tail = head->next;
                    while (tail->next) {
                        prev = tail;
                        tail = tail->next;
                    }
                    delete tail;
                    prev->next = nullptr;
                } else {
                    delete head;
                    head = nullptr;
                }
                len--;
                return true;
            }
            return false;
        }

        bool removeValue(const T val) {
            if (head && head->val == val) {
                deleteFirst();
                return true;
            }
            auto node = head->next, prev = head;
            while (node) {
                if (node->val == val) {
                    prev->next = node->next;
                    delete node;
                    len--;
                    return true;
                }
                prev = node;
                node = node->next;
            }
            return false;
        }

        bool contains(const T val) {
            auto node = head;
            while (node) {
                if (node->val == val)
                    return true;
                node = node->next;
            }
            return false;
        }

        T *find(const T val) {
            auto node = head;
            while (node) {
                if (node->val == val)
                    return &(node->val);
                node = node->next;
            }
            return nullptr;
        }

        bool isEmpty() {
            return head == nullptr;
        }

        int length() const {
            return this->len;
        }
    };

    template<class T>
    class Queue {
        LinkedList<T> *list = new LinkedList<T>();

    public:
        ~Queue() {
            clear();
            delete list;
        }

        void insert(const T val) {
            // FIFO
            list->addLast(val);
        }

        void insert(const LinkedList<T> *list) {
            // FIFO
            this->list->addLast(list);
        }

        T peek() {
            // FIFO
            if (!list->head) throw std::out_of_range("Queue is empty");
            return list->head->val;
        }

        T pop() {
            // FIFO
            const auto first = peek();
            list->deleteFirst();
            return first;
        }

        T peekMin() {
            T min = peek();
            auto node = list->head;
            while (node) {
                if (node->val < min)
                    min = node->val;
                node = node->next;
            }
            return min;
        }

        T popMin() {
            const auto min = peekMin();
            list->removeValue(min);
            return min;
        }

        bool isEmpty() {
            return list->isEmpty();
        }

        bool contains(const T val) {
            return list->contains(val);
        }

        T *find(const T val) { return list->find(val); }

        int size() { return list->length(); }

        void clear() { list->clear(); }
    };

    class UnionSet {
        int *parent, *rank;

    public:
        UnionSet(const int n) {
            parent = new int[n];
            rank = new int[n];
            for (int i = 0; i < n; i++) {
                parent[i] = i;
                rank[i] = 1;
            }
        }

        ~UnionSet() {
            delete[] parent;
            delete[] rank;
        }

        int find(const int i) {
            return (parent[i] == i) ? i : (parent[i] = find(parent[i]));
        }

        void unite(const int x, const int y) {
            const int s1 = find(x), s2 = find(y);
            if (s1 != s2) {
                if (rank[s1] < rank[s2]) parent[s1] = s2;
                else if (rank[s1] > rank[s2]) parent[s2] = s1;
                else parent[s2] = s1, rank[s1]++;
            }
        }
    };

    template class Queue<int>;
}


#endif //DATASTRUCTURES_H
