#ifndef KERNEL_STD_LIST_H_
#define KERNEL_STD_LIST_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "copyable.h"

namespace kernel {
template <class T>
class list : protected NonCopyable<list<T>> {
    class node : protected NonCopyable<node> {
     public:
        T data;
        node *prev, *next;
        node() {
            prev = next = nullptr;
        }
        explicit node(const T& d) : data(d) {
            prev = next = nullptr;
        }
        explicit node(T&& d) : data(d) {
            prev = next = nullptr;
        }
        explicit operator T&() { return data; }
        explicit operator T() const { return data; }
    };
    class iterator{
     friend class list;
     public:
        iterator() = delete;
        iterator(list<T>* cls, node* n) : _cls(cls), _node(n) {}
        T& operator *() { return _node->data; }
        iterator next() const { return iterator(_cls, _node->next); }
        void operator ++() { _node = _node->next; }
        bool operator ==(const iterator& rhs) const { return _cls == rhs._cls && _node == rhs._node; }
        bool operator !=(const iterator& rhs) const { return _cls != rhs._cls || _node != rhs._node; }
     private:
        list<T>* _cls;
        node* _node;
    };
    class const_iterator {
     friend class list;
     public:
        const_iterator() = delete;
        const_iterator(const list<T>* cls, const node* n) : _cls(cls), _node(n) {}
        const T& operator *() const { return _node->data; }
        const_iterator next() const { return const_iterator(_cls, _node->next); }
        void operator ++() { _node = _node->next; }
        bool operator ==(const const_iterator& rhs) const { return _cls == rhs._cls && _node == rhs._node; }
        bool operator !=(const const_iterator& rhs) const { return _cls != rhs._cls || _node != rhs._node; }
     private:
        const list<T>* _cls;
        const node* _node;
    };

 public:
    list() : _size(0) {}
    list(list&& o);
    // capacity
    bool empty() const;
    size_t size() const;
    size_t max_size() const;
    // elemental access
    const T& front() const;
    T& front();
    const T& back() const;
    T& back();
    const T& operator[](size_t index) const;
    T& operator[](size_t index);
    // iterators
    typename list<T>::const_iterator begin() const;
    typename list<T>::iterator begin();
    typename list<T>::const_iterator end() const;
    typename list<T>::iterator end();
    // modifiers
    void clear();
    typename list<T>::iterator erase(typename list<T>::iterator pos);
    void push_front(const T& value);
    void pop_front();
    typename list<T>::iterator insert(typename list<T>::iterator pos, const T& value);

 private:
    node _head;
    size_t _size;
};

// -------------------------------------------------
// move
// -------------------------------------------------
template <class T>
list<T>::list(list<T>&& o) {
    _head = o._head;
    _size = o._size;
    o._head.next = nullptr;
    o._size = 0;
}
// -------------------------------------------------
// capacity
// -------------------------------------------------
template <class T>
bool list<T>::empty() const {
    return _head.next == nullptr;
}
template <class T>
size_t list<T>::size() const {
    return _size;
}
template <class T>
size_t list<T>::max_size() const {
    return SIZE_MAX;
}
// -------------------------------------------------
// elemental access
// -------------------------------------------------
template <class T>
const T& list<T>::front() const {
    return _head.next->data;
}
template <class T>
T& list<T>::front() {
    return _head.next->data;
}
template <class T>
const T& list<T>::back() const {
    for (node *p = _head.next->data; ; p = p->next) {
        if (!p->next) return p->data;
    } return _head.data;
}
template <class T>
T& list<T>::back() {
    for (node *p = _head.next; ; p = p->next) {
        if (!p->next) return p->data;
    } return _head.data;
}
template <class T>
const T& list<T>::operator[](size_t index) const {
    for (auto it = begin(); it != end(); ++it) {
        if (index == 0) return *it;
        index--;
    }
    return _head.data;
}
template <class T>
T& list<T>::operator[](size_t index) {
    for (auto it = begin(); it != end(); ++it) {
        if (index == 0) return *it;
        index--;
    }
    return _head.data;
}
// -------------------------------------------------
// iterators
// -------------------------------------------------
template <class T>
typename list<T>::const_iterator list<T>::begin() const {
    if (empty()) {
        return const_iterator(this, nullptr);
    } else {
        return const_iterator(this, _head.next);
    }
}
template <class T>
typename list<T>::iterator list<T>::begin() {
    if (empty()) {
        return iterator(this, nullptr);
    } else {
        return iterator(this, _head.next);
    }
}
template <class T>
typename list<T>::const_iterator list<T>::end() const {
    return const_iterator(this, nullptr);
}
template <class T>
typename list<T>::iterator list<T>::end() {
    return iterator(this, nullptr);
}
// -------------------------------------------------
// modifiers
// -------------------------------------------------
template <class T>
void list<T>::clear() {
    for (auto it = begin(); it != end(); ) {
        it = erase(it);
    }
}
template <class T>
typename list<T>::iterator list<T>::erase(typename list<T>::iterator pos) {
    auto next = pos.next();
    // rewire prev and next
    pos._node->prev->next = pos._node->next;
    if (pos._node->next) {
        pos._node->next->prev = pos._node->prev;
    }
    // delete node
    delete pos._node;
    _size--;
    return next;
}
template <class T>
void list<T>::push_front(const T& value) {
    node* newnode = new node(value);
    newnode->prev = &_head;
    newnode->next = _head.next;
    if (_head.next) {
        _head.next->prev = newnode;
    }
    _head.next = newnode;
    _size++;
}
template <class T>
void list<T>::pop_front() {
    erase(begin());
}
template <class T>
typename list<T>::iterator list<T>::insert(typename list<T>::iterator pos, const T& value) {
    node* newnode = new node(value);
    newnode->prev = pos._node->prev;
    newnode->next = pos._node;
    pos._node->prev->next = newnode;
    pos._node->prev = newnode;
    _size++;
    return iterator(this, newnode);
}

}  // namespace kernel

#endif  // KERNEL_STD_LIST_H_
