#ifndef __LIST_H__
#define __LIST_H__

#include <spdlog/spdlog.h>
#include <assert.h>

#include <exception>

template <typename T>
class List {
 public:
    class Node {
     private:
        Node *_next = this;
        Node *_prev = this;
        T *_entry = nullptr;

     public:
        Node *Next() const { return _next; }
        Node *Prev() const { return _prev; }
        T *Entry() const { return _entry; }

        Node(T *entry = nullptr): _entry(entry) {}
        ~Node() {
            UnlinkFromPeers();
        }

        bool PointsToSelf() const {
            return _next == this && _prev == this;
        }

        void UnlinkFromPeers() {
            if (!PointsToSelf()) {
                Node *before = _prev;
                Node *after = _next;

                before->_next = after;
                after->_prev = before;
            }
        }

        void InsertBetween(Node *before, Node *after) {
            UnlinkFromPeers();

            before->_next = this;
            after->_prev = this;

            _next = after;
            _prev = before;
        }

        friend class List;
    };

 private:
    Node _base;

 public:
    void PushBack(T *entry, Node *node) {
        node->_entry = entry;
        node->InsertBetween(_base._prev, &_base);
    }

    void PushFront(T *entry, Node *node) {
        node->_entry = entry;
        node->InsertBetween(&_base, _base._next);
    }

    bool Empty() { return _base.PointsToSelf(); }

    T *PopFront() {
        if (Empty()) { return nullptr; }

        Node *cursor = _base.Next();
        cursor->UnlinkFromPeers();
        return cursor->Entry();
    }

    T *PopBack() {
        if (Empty()) { return nullptr; }

        Node *cursor = _base.Prev();
        cursor->UnlinkFromPeers();
        return cursor->Entry();
    }

    T *Front() {
        if (Empty()) { return nullptr; }
        return _base.Next()->Entry();
    }

    T *Back() {
        if (Empty()) { return nullptr; }
        return _base.Prev()->Entry();
    }

    Node *Begin() { return _base.Next(); }
    Node *End() { return &_base; }

    Node *ReverseBegin() { return _base.Prev(); }
    Node *ReverseEnd() { return &_base; }
};

#endif  // __LIST_H__
