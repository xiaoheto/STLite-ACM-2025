#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <cstddef>
#include <functional>
#include "exceptions.hpp"
#include "utility.hpp"

namespace sjtu {

    template<class Key, class T, class Compare = std::less<Key>>
    class map {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;
        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
        class const_iterator;

    private:
        struct Node {
            value_type *data;
            Node *left;
            Node *right;
            int height;
            Node *parent;
            Node(const value_type &d, Node *l = nullptr, Node *r = nullptr, Node *p = nullptr, int h = 1) :
                left(l), right(r), parent(p), height(h) {
                data = new value_type(d);
            }
            Node(const Key &k, const T &t, Node *l = nullptr, Node *r = nullptr, Node *p = nullptr, int h = 1) :
                left(l), right(r), parent(p), height(h) {
                data = new value_type(k, t);
            }
            ~Node() {
                if (data) {
                    delete data;
                }
            }
        };
        Node *root_;
        int size_;

        void Clear(Node *node) {
            if (!node) {
                return;
            }
            Clear(node->left);
            Clear(node->right);
            delete node;
        }

        void Copy(Node *other, Node *&node, Node *par) {
            if (!other) {
                node = nullptr;
                return;
            }
            node = new Node(*(other->data), nullptr, nullptr, par, other->height);
            Copy(other->left, node->left, node);
            Copy(other->right, node->right, node);
            return;
        }

        int Height(Node *node) const { return node == nullptr ? 0 : node->height; }

        int calHeight(Node *t) const {
            if (!t)
                return 0;
            int lh = Height(t->left);
            int rh = Height(t->right);
            return (lh > rh) ? (lh + 1) : (rh + 1);
        }

        void LL(Node *&nodeA) {
            Node *nodeB = nodeA->left;
            nodeB->parent = nodeA->parent;
            nodeA->left = nodeB->right;
            if (nodeA->left) {
                nodeA->left->parent = nodeA;
            }
            nodeA->parent = nodeB;
            nodeB->right = nodeA;
            nodeA = nodeB;

            nodeA->right->height = calHeight(nodeA->right);
            nodeA->height = calHeight(nodeA);
        }

        void RR(Node *&nodeA) {
            Node *nodeB = nodeA->right;
            nodeB->parent = nodeA->parent;
            nodeA->right = nodeB->left;
            if (nodeA->right) {
                nodeA->right->parent = nodeA;
            }
            nodeA->parent = nodeB;
            nodeB->left = nodeA;
            nodeA = nodeB;

            nodeA->left->height = calHeight(nodeA->left);
            nodeA->height = calHeight(nodeA);
        }

        void LR(Node *&node) {
            RR(node->left);
            LL(node);
        }

        void RL(Node *&node) {
            LL(node->right);
            RR(node);
        }

        Node *Last(Node *node) const {
            if (!node) {
                return nullptr;
            }
            while (node->right) {
                node = node->right;
            }
            return node;
        }

        Node *Find(const Key &key, Node *node) const {
            if (!node) {
                return nullptr;
            }
            if (Compare()(node->data->first, key)) {
                return Find(key, node->right);
            } else if (Compare()(key, node->data->first)) {
                return Find(key, node->left);
            } else {
                return node;
            }
        }

        pair<Node *, bool> Insert(const value_type &value, Node *&node, Node *par) {
            if (!node) {
                node = new Node(value, nullptr, nullptr, par, 1); // insert in an empty tree
                return {node, true};
            }
            pair<Node *, bool> ans;
            if (Compare()(value.first, node->data->first)) {
                pair<Node *, bool> temp = Insert(value, node->left, node);
                ans.first = temp.first;
                ans.second = temp.second;
                if (Height(node->left) - Height(node->right) >= 2) {
                    if (Compare()(value.first, node->left->data->first)) {
                        LL(node);
                    } else {
                        LR(node);
                    }
                }
            } else if (Compare()(node->data->first, value.first)) {
                pair<Node *, bool> temp = Insert(value, node->right, node);
                ans.first = temp.first;
                ans.second = temp.second;
                if (Height(node->right) - Height(node->left) >= 2) {
                    if (Compare()(node->right->data->first, value.first)) {
                        RR(node);
                    } else {
                        RL(node);
                    }
                }
            } else {
                return {node, false};
            }
            node->height = std::max(Height(node->left), Height(node->right)) + 1;
            return ans;
        }

        bool Adjust(Node *&node, int subTree) { // subtree shows that we process nodes on the left
            if (subTree) {
                if (Height(node->right) - Height(node->left) == 1) {
                    return true;
                }
                if (Height(node->right) == Height(node->left)) {
                    --node->height;
                    return false;
                }
                if (Height(node->right->left) > Height(node->right->right)) {
                    RL(node);
                    return false;
                }
                RR(node);
                if (Height(node->right) == Height(node->left)) {
                    return false;
                } else {
                    return true;
                }
            } else {
                if (Height(node->left) - Height(node->right) == 1) {
                    return true;
                }
                if (Height(node->right) == Height(node->left)) {
                    --node->height;
                    return false;
                }
                if (Height(node->left->right) > Height(node->left->left)) {
                    LR(node);
                    return false;
                }
                LL(node);
                if (Height(node->right) == Height(node->left)) {
                    return false;
                } else {
                    return true;
                }
            }
        }

        bool Remove(const Key &key, Node *&node) {
            if (!node) {
                return true;
            }
            if (Compare()(key, node->data->first)) {
                if (Remove(key, node->left)) {
                    return true;
                } else {
                    return Adjust(node, 1);
                }
            } else if (Compare()(node->data->first, key)) {
                if (Remove(key, node->right)) {
                    return true;
                } else {
                    return Adjust(node, 0);
                }
            } else { // 当前节点是需要移除的节点
                if (!node->left || !node->right) {
                    Node *oldNode = node;
                    if (!node->left && !node->right) {
                        node = nullptr;
                    } else if (node->right) {
                        node->right->parent = node->parent;
                        node = node->right;
                    } else {
                        node->left->parent = node->parent;
                        node = node->left;
                    }
                    delete oldNode;
                    return false;
                } else {
                    Node *temp = node->left;
                    while (temp->right) {
                        temp = temp->right;
                    }
                    Node *node_r = node->right;
                    Node *node_l = node->left;
                    if (temp == node->left) {
                        temp->parent = node->parent;
                        node->parent = temp;
                        node->right = temp->right;
                        if (temp->right)
                            temp->right->parent = node;
                        node->left = temp->left;
                        if (temp->left)
                            temp->left->parent = node;
                        temp->left = node;
                        temp->right = node_r;
                        if (node_r)
                            node_r->parent = temp;
                    } else {
                        Node *node_p = node->parent;
                        node->parent = temp->parent;
                        temp->parent = node_p;
                        node->parent->right = node;

                        node->right = temp->right;
                        if (temp->right)
                            temp->right->parent = node;
                        temp->right = node_r;
                        if (node_r)
                            node_r->parent = temp;

                        node->left = temp->left;
                        if (temp->left)
                            temp->left->parent = node;
                        temp->left = node_l;
                        if (node_l)
                            node_l->parent = temp;
                    }
                    node->data->first.~Key();
                    node->data->second.~T();
                    new (node->data) value_type(*(temp->data));
                    int h = node->height;
                    node->height = temp->height;
                    temp->height = h;
                    node = temp;
                    if (Remove(temp->data->first, node->left)) {
                        return true;
                    } else {
                        return Adjust(node, true);
                    }
                }
            }
        }

    public:
        class iterator {
        private:
            map *container;
            Node *current;
            friend class map;

        public:
            iterator() : container(nullptr), current(nullptr) {}

            iterator(map *container, Node *current) : container(container), current(current) {}

            iterator(const iterator &other) : container(other.container), current(other.current) {}

            iterator operator++(int) {
                if (!current || !container) {
                    throw invalid_iterator();
                }
                iterator temp(*this);
                ++(*this);
                return temp;
            }

            iterator &operator++() {
                if (!current || !container) {
                    throw invalid_iterator();
                }
                if (current->right) {
                    current = current->right;
                    while (current->left) {
                        current = current->left;
                    }
                    return *this;
                }
                while (current->parent && current == current->parent->right) {
                    current = current->parent;
                }
                current = current->parent;
                return *this;
            }

            iterator operator--(int) {
                if (!container) {
                    throw invalid_iterator();
                }
                iterator temp(*this);
                --(*this);
                return temp;
            }

            iterator &operator--() {
                if (!container) {
                    throw invalid_iterator();
                }
                if (!current) {
                    current = container->Last(container->root_);
                    if (!current) {
                        throw invalid_iterator();
                    }
                    return *this;
                }
                if (current->left) {
                    current = current->left;
                    while (current->right) {
                        current = current->right;
                    }
                    return *this;
                }
                while (current->parent && current == current->parent->left) {
                    current = current->parent;
                }
                if (current->parent) {
                    current = current->parent;
                    return *this;
                } else {
                    throw invalid_iterator();
                }
            }

            value_type &operator*() const {
                if (!current) {
                    throw invalid_iterator();
                }
                return *(current->data);
            }

            bool operator==(const iterator &rhs) const { return container == rhs.container && current == rhs.current; }

            bool operator==(const const_iterator &rhs) const {
                return container == rhs.container && current == rhs.current;
            }

            bool operator!=(const iterator &rhs) const { return container != rhs.container || current != rhs.current; }

            bool operator!=(const const_iterator &rhs) const {
                return container != rhs.container || current != rhs.current;
            }

            value_type *operator->() const {
                if (!current) {
                    throw invalid_iterator();
                }
                return current->data;
            }
        };

        class const_iterator {
        private:
            const map *container;
            Node *current;
            friend class map;

        public:
            const_iterator() : container(nullptr), current(nullptr) {}

            const_iterator(const map *container, Node *current) : container(container), current(current) {}

            const_iterator(const const_iterator &other) : container(other.container), current(other.current) {}

            const_iterator(const iterator &other) : container(other.container), current(other.current) {}

            const_iterator operator++(int) {
                if (!current || !container) {
                    throw invalid_iterator();
                }
                const_iterator temp(*this);
                ++(*this);
                return temp;
            }

            const_iterator &operator++() {
                if (!current || !container) {
                    throw invalid_iterator();
                }
                if (current->right) {
                    current = current->right;
                    while (current->left) {
                        current = current->left;
                    }
                    return *this;
                }
                while (current->parent && current == current->parent->right) {
                    current = current->parent;
                }
                current = current->parent;
                return *this;
            }

            const_iterator operator--(int) {
                if (!container) {
                    throw invalid_iterator();
                }
                const_iterator temp(*this);
                --(*this);
                return temp;
            }

            const_iterator &operator--() {
                if (!container) {
                    throw invalid_iterator();
                }
                if (!current) {
                    current = container->Last(container->root_);
                    if (!current) {
                        throw invalid_iterator();
                    }
                    return *this;
                }
                if (current->left) {
                    current = current->left;
                    while (current->right) {
                        current = current->right;
                    }
                    return *this;
                }
                while (current->parent && current == current->parent->left) {
                    current = current->parent;
                }
                if (current->parent) {
                    current = current->parent;
                    return *this;
                } else {
                    throw invalid_iterator();
                }
            }

            value_type &operator*() const {
                if (!current) {
                    throw invalid_iterator();
                }
                return *(current->data);
            }

            bool operator==(const iterator &rhs) const { return container == rhs.container && current == rhs.current; }

            bool operator==(const const_iterator &rhs) const {
                return container == rhs.container && current == rhs.current;
            }

            bool operator!=(const iterator &rhs) const { return container != rhs.container || current != rhs.current; }

            bool operator!=(const const_iterator &rhs) const {
                return container != rhs.container || current != rhs.current;
            }

            value_type *operator->() const {
                if (!current) {
                    throw invalid_iterator();
                }
                return current->data;
            }
        };
        /**
         * TODO two constructors
         */
        map() : root_(nullptr), size_(0) {}
        map(const map &other) : size_(other.size_) { Copy(other.root_, root_, nullptr); }
        /**
         * TODO assignment operator
         */
        map &operator=(const map &other) {
            if (this != &other) {
                Clear(root_);
                Copy(other.root_, root_, nullptr);
                size_ = other.size_;
                return *this;
            }
            return *this;
        }
        /**
         * TODO Destructors
         */
        ~map() { clear(); }
        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type index_out_of_bound'
         */
        T &at(const Key &key) {
            Node *node = Find(key, root_);
            if (!node) {
                throw index_out_of_bound();
            }
            return node->data->second;
        }
        const T &at(const Key &key) const {
            Node *node = Find(key, root_);
            if (!node) {
                throw index_out_of_bound();
            }
            return node->data->second;
        }
        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T &operator[](const Key &key) {
            Node *node = Find(key, root_);
            if (node) {
                return node->data->second;
            } else {
                T d;
                value_type temp(key, d);
                pair<Node *, bool> ans = Insert(temp, root_, nullptr);
                ++size_;
                return ans.first->data->second;
            }
        }
        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T &operator[](const Key &key) const {
            Node *node = Find(key, root_);
            if (!node) {
                throw index_out_of_bound();
            }
            return node->data->second;
        }
        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            if (!root_) {
                return end();
            }
            Node *node = root_;
            while (node->left) {
                node = node->left;
            }
            return iterator(this, node);
        }
        const_iterator cbegin() const {
            if (!root_) {
                return const_iterator(this, nullptr);
            }
            Node *node = root_;
            while (node->left) {
                node = node->left;
            }
            return const_iterator(static_cast<const map *>(this), node);
        }

        const_iterator cend() const { return const_iterator(static_cast<const map *>(this), nullptr); }
        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() { return iterator(this, nullptr); }
        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const { return size_ == 0; }
        /**
         * returns the number of elements.
         */
        int size() const { return size_; }
        /**
         * clears the contents
         */
        void clear() {
            Clear(root_);
            root_ = nullptr;
            size_ = 0;
        }
        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that getPrevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            pair<Node *, bool> result = Insert(value, root_, nullptr);
            iterator temp(this, result.first);
            if (result.second) {
                ++size_;
                return {temp, true};
            } else {
                return {temp, false};
            }
        }
        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            if (pos.container != this || !pos.current) {
                throw invalid_iterator();
            }
            Remove(pos->first, root_);
            --size_;
        }
        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        int count(const Key &key) const {
            Node *node = Find(key, root_);
            return node == nullptr ? 0 : 1;
        }
        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            Node *node = Find(key, root_);
            return iterator(this, node);
        }
        const_iterator find(const Key &key) const {
            Node *node = Find(key, root_);
            return const_iterator(this, node);
        }
    };

} // namespace sjtu

#endif