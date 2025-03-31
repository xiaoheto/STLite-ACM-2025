#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {
    /**
     * @brief a container like std::priority_queue which is a heap internal.
     * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
     * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its
     * original state before the operation began.
     */
    template<typename T, class Compare = std::less<T>>
    class priority_queue {
    private:
        struct Node {
            Node *left;
            Node *right;
            T data;
            Node(const T &value, Node *l = nullptr, Node *r = nullptr) : data(value), left(l), right(r) {}
        };
        Node *root_;
        size_t size_;

        Node *Copy(Node *other) {
            if (!other) {
                return nullptr;
            }
            Node *newNode = new Node(other->data);
            newNode->left = Copy(other->left);
            newNode->right = Copy(other->right);
            return newNode;
        }

        void Clear(Node *node) {
            if (!node) {
                return;
            }
            Clear(node->left);
            Clear(node->right);
            delete node;
        }

        Node *Merge(Node *node1, Node *node2) {
            if (!node1) {
                return node2;
            }
            if (!node2) {
                return node1;
            }
            try {
                if (Compare()(node1->data, node2->data)) {
                    Node *temp = node2->left;
                    node2->left = Merge(node1, node2->right);
                    node2->right = temp;
                    return node2;
                } else {
                    Node *temp = node1->left;
                    node1->left = Merge(node2, node1->right);
                    node1->right = temp;
                    return node1;
                }
            } catch (...) {
                throw;
            }
        }

    public:
        /**
         * @brief default constructor
         */
        priority_queue() : root_(nullptr), size_(0) {}

        /**
         * @brief copy constructor
         * @param other the priority_queue to be copied
         */
        priority_queue(const priority_queue &other) : root_(Copy(other.root_)), size_(other.size_) {}

        /**
         * @brief deconstructor
         */
        ~priority_queue() {
            Clear(root_);
        }

        /**
         * @brief Assignment operator
         * @param other the priority_queue to be assigned from
         * @return a reference to this priority_queue after assignment
         */
        priority_queue &operator=(const priority_queue &other) {
            if (this == &other) {
                return *this;
            }
            Clear(root_);
            root_ = Copy(other.root_);
            size_ = other.size_;
            return *this;
        }

        /**
         * @brief get the top element of the priority queue.
         * @return a reference of the top element.
         * @throws container_is_empty if empty() returns true
         */
        const T &top() const {
            if (empty()) {
                throw container_is_empty();
            }
            return root_->data;
        }

        /**
         * @brief push new element to the priority queue.
         * @param e the element to be pushed
         */
        void push(const T &e) {
            Node *node = new Node(e);
            try {
                root_ = Merge(root_, node);
                size_++;
            } catch (...) {
                delete node;
                throw;
            }
        }

        /**
         * @brief delete the top element from the priority queue.
         * @throws container_is_empty if empty() returns true
         */
        void pop() {
            if (empty()) {
                throw container_is_empty();
            }
            Node *temp = root_;
            try {
                root_ = Merge(root_->left, root_->right);
                size_--;
                delete temp;
            } catch (...) {
                root_ = temp;
                throw;
            }
        }

        /**
         * @brief return the number of elements in the priority queue.
         * @return the number of elements.
         */
        size_t size() const {
            return size_;
        }

        /**
         * @brief check if the container is empty.
         * @return true if it is empty, false otherwise.
         */
        bool empty() const {
            return size_ == 0;
        }

        /**
         * @brief merge another priority_queue into this one.
         * The other priority_queue will be cleared after merging.
         * The complexity is at most O(logn).
         * @param other the priority_queue to be merged.
         */
        void merge(priority_queue &other) {
            try {
                root_ = Merge(root_, other.root_);
                size_ += other.size_;
                other.root_ = nullptr;
                other.size_ = 0;
            } catch (...) {
                throw;
            }
        }
    };
} // namespace sjtu

#endif