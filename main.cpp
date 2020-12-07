#include <cstddef>
#include <iterator>
#include <stdexcept>

template <typename T>
class List {
public:
    using value_type = T;

    List() noexcept;
    explicit List(size_t count, const T& value = T()) noexcept;
    List(const List<T>& other) noexcept;
    List(List<T>&& other) noexcept;

    ~List();

    List<T>& operator=(const List<T>& other) noexcept;
    List<T>& operator=(List<T>&& other) noexcept;

    [[nodiscard]] size_t size() const;
    [[nodiscard]] bool empty() const;

    [[nodiscard]] T& front();
    [[nodiscard]] const T& front() const;
    [[nodiscard]] T& back();
    [[nodiscard]] const T& back() const;

    void push_front(const T& value);
    void push_front(T&& value);
    void push_back(const T& value);
    void push_back(T&& value);

    void pop_front();
    void pop_back();

    void clear();

    template <bool is_const = false>
    class Iterator;

    using iterator = Iterator<>;
    using const_iterator = Iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    [[nodiscard]] iterator begin();
    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator cbegin() const;

    [[nodiscard]] iterator end();
    [[nodiscard]] const_iterator end() const;
    [[nodiscard]] const_iterator cend() const;

    [[nodiscard]] reverse_iterator rbegin();
    [[nodiscard]] const_reverse_iterator rbegin() const;
    [[nodiscard]] const_reverse_iterator crbegin() const;

    [[nodiscard]] reverse_iterator rend();
    [[nodiscard]] const_reverse_iterator rend() const;
    [[nodiscard]] const_reverse_iterator crend() const;

    iterator insert(const_iterator it, const T& value);
    iterator insert(const_iterator it, T&& value);
    template <class InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last);

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    void emplace_front(T&& value);
    void emplace_back(T&& value);
    iterator emplace(const_iterator pos, T&& value);

    void reverse();
    void unique();
private:
    struct Node;
    Node* _front;
    Node* _back;
    size_t _size;
};

template <typename T>
struct List<T>::Node {
    explicit Node(Node* next = nullptr, Node* prev = nullptr) noexcept:
            data(static_cast<T*>(::operator new(sizeof(T)))), next(next), prev(prev) {}

    Node(const Node& other) noexcept = default;
    Node(Node&& other) noexcept = default;

    Node& operator=(const Node& other) noexcept = default;
    Node& operator=(Node&& other) noexcept = default;

    T* data;
    Node* next = nullptr;
    Node* prev = nullptr;
};

template <typename T>
List<T>::List() noexcept: _front(new Node), _back(new Node), _size(0) {
    _front->next = _back;
    _back->prev = _front;
}

template <typename T>
List<T>::List(size_t count, const T& value) noexcept: List() {
    _size = count;
    if (count > 0) {
        Node* tmp = _front;
        for (size_t i = 0; i < count; ++i) {
            tmp->next = new Node(nullptr, tmp);
            tmp->next->data = new (tmp->next->data) T(value);
            tmp = tmp->next;
        }
        tmp->next = _back;
        _back->prev = tmp;
    }
}

template <typename T>
List<T>::List(const List<T>& other) noexcept: _front(new Node), _back(new Node), _size(other.size()) {
    Node* tmp = _front;
    for (auto other_tmp = other._front->next; other_tmp != other._back; other_tmp = other_tmp->next) {
        tmp->next = new Node(nullptr, tmp);
        tmp->next->data = new (tmp->next->data) T(*(other_tmp->data));
        tmp = tmp->next;
    }
    tmp->next = _back;
    _back->prev = tmp;
}

template <typename T>
List<T>::List(List<T>&& other) noexcept: _front(new Node), _back(new Node), _size(other._size) {
    _front->next = other._front->next;
    _front->next->prev = _front;
    _back->prev = other._back->prev;
    _back->prev->next = _back;

    other._front->next = other._back;
    other._back->prev = other._front;
    other._size = 0;
}

template <typename T>
List<T>::~List() {
    clear();
    ::operator delete(_front->data);
    ::operator delete(_back->data);
    delete _front;
    delete _back;
}

template <typename T>
List<T>& List<T>::operator=(const List<T>& other) noexcept {
    if (this != &other) {
        clear();

        _size = other.size();
        Node* tmp = _front;
        for (auto other_tmp = other._front->next; other_tmp != other._back; other_tmp = other_tmp->next) {
            tmp->next = new Node(nullptr, tmp);
            tmp->next->data = new (tmp->next->data) T(*(other_tmp->data));
            tmp = tmp->next;
        }
        tmp->next = _back;
        _back->prev = tmp;
    }
    return *this;
}

template <typename T>
List<T>& List<T>::operator=(List<T>&& other) noexcept {
    if (this != &other) {
        clear();

        _size = other._size;
        _front->next = other._front->next;
        _front->next->prev = _front;
        _back->prev = other._back->prev;
        _back->prev->next = _back;

        other._front->next = other._back;
        other._back->prev = other._front;
        other._size = 0;
    }
    return *this;
}

template <typename T>
size_t List<T>::size() const {
    return _size;
}

template <typename T>
bool List<T>::empty() const {
    return !size();
}

template <typename T>
T& List<T>::front() {
    if (empty()) {
        throw std::out_of_range("The list is empty!");
    }
    return *(_front->next->data);
}

template <typename T>
const T& List<T>::front() const {
    return front();
}

template <typename T>
T& List<T>::back() {
    if (empty()) {
        throw std::out_of_range("The list is empty!");
    }
    return *(_back->prev->data);
}

template <typename T>
const T& List<T>::back() const {
    return back();
}

template <typename T>
void List<T>::push_front(const T& value) {
    insert(begin(), value);
}

template <typename T>
void List<T>::push_front(T&& value) {
    insert(begin(), std::move(value));
}

template <typename T>
void List<T>::push_back(const T& value) {
    insert(end(), value);
}

template <typename T>
void List<T>::push_back(T&& value) {
    insert(end(), std::move(value));
}

template <typename T>
void List<T>::pop_front() {
    erase(begin());
}

template <typename T>
void List<T>::pop_back() {
    if (begin() != end()) {
        erase(--end());
    }
}

template <typename T>
void List<T>::clear() {
    erase(begin(), end());
}


template <typename T>
typename List<T>::iterator List<T>::begin() {
    return iterator(_front->next);
}

template <typename T>
typename List<T>::const_iterator List<T>::begin() const {
    return List::const_iterator(_front->next);
}

template <typename T>
typename List<T>::const_iterator List<T>::cbegin() const {
    return List::const_iterator(_front->next);
}

template <typename T>
typename List<T>::iterator List<T>::end() {
    return iterator(_back);
}

template <typename T>
typename List<T>::const_iterator List<T>::end() const {
    return List::const_iterator(_back);
}

template <typename T>
typename List<T>::const_iterator List<T>::cend() const {
    return List::const_iterator(_back);
}

template <typename T>
typename List<T>::reverse_iterator List<T>::rbegin() {
    return List::reverse_iterator(end());
}

template <typename T>
typename List<T>::const_reverse_iterator List<T>::rbegin() const {
    return List::const_reverse_iterator(end());
}

template <typename T>
typename List<T>::const_reverse_iterator List<T>::crbegin() const {
    return List::const_reverse_iterator(end());
}

template <typename T>
typename List<T>::reverse_iterator List<T>::rend() {
    return List::reverse_iterator(begin());
}

template <typename T>
typename List<T>::const_reverse_iterator List<T>::rend() const {
    return List::const_reverse_iterator(begin());
}

template <typename T>
typename List<T>::const_reverse_iterator List<T>::crend() const {
    return List::const_reverse_iterator(begin());
}

template <typename T>
typename List<T>::iterator List<T>::insert(const_iterator it, const T& value) {
    T copyValue = value;
    return insert(it, std::move(copyValue));
}

template <typename T>
typename List<T>::iterator List<T>::insert(const_iterator it, T&& value) {
    Node* prev = it._current_node->prev;
    Node* next = it._current_node;
    Node* cur = new Node(next, prev);
    cur->data = new (cur->data) T(std::move(value));

    prev->next = cur;
    next->prev = cur;

    ++_size;

    return iterator(cur);
}

template <typename T>
template <typename InputIt>
typename List<T>::iterator List<T>::insert(const_iterator pos, InputIt first, InputIt last) {
    if (first == last) {
        return iterator(pos._current_node);
    }

    auto returnedIter = insert(pos, *first);
    for (++first; first != last; ++first) {
        insert(pos, *first);
    }
    return returnedIter;
}


template <typename T>
typename List<T>::iterator List<T>::erase(const_iterator pos) {
    if (pos == end()) {
        return end();
    }

    auto prev = pos._current_node->prev;
    auto next = pos._current_node->next;
    prev->next = next;
    next->prev = prev;
    pos._current_node->data->~T();
    ::operator delete(pos._current_node->data);
    delete pos._current_node;
    pos._current_node = nullptr;
    --_size;
    return iterator(next);
}

template <typename T>
typename List<T>::iterator List<T>::erase(const_iterator first, const_iterator last) {
    for (; first != last; first = erase(first)) ;
    return iterator(last._current_node);
}

template <typename T>
void List<T>::emplace_front(T&& value) {
    push_front(std::move(value));
}

template <typename T>
void List<T>::emplace_back(T&& value) {
    push_back(std::move(value));
}

template <typename T>
typename List<T>::iterator List<T>::emplace(List<T>::const_iterator pos, T&& value) {
    return insert(pos, std::move(value));
}

template <typename T>
void List<T>::reverse() {
    Node* current = _front;
    while (current) {
        Node* temp = current->prev;
        current->prev = current->next;
        current->next = temp;
        current = current->prev;
    }
    std::swap(_front, _back);
}

template <typename T>
void List<T>::unique() {
    if (size() >= 2) {
        auto j = begin();
        auto i = j;
        for (++i; i != end(); ++i) {
            if (*j != *i) {
                ++j;
                *j = *i;
            }
        }

        ++j;
        erase(j, end());
    }
}


template <typename T>
template <bool is_const>
class List<T>::Iterator {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using reference = typename std::conditional<is_const, const T&, T&>::type;
    using pointer = typename std::conditional<is_const, const T*, T*>::type;
    using iterator_category = std::bidirectional_iterator_tag;

    Iterator() noexcept = default;

    explicit Iterator(Node* node) noexcept: _current_node(node) {}

    Iterator(const Iterator<false>& other) noexcept: _current_node(other._current_node) {}

    // Нельзя конструировть неконстанты из констант
    Iterator(const Iterator<true>& other, typename std::enable_if<is_const>) noexcept:
            _current_node(other._current_node) {}

    Iterator& operator++();
    const Iterator operator++(int);

    Iterator& operator--();
    const Iterator operator--(int);

    [[nodiscard]] bool operator==(const Iterator& other) const;
    [[nodiscard]] bool operator!=(const Iterator& other) const;

    [[nodiscard]] reference operator*() const;
    [[nodiscard]] pointer operator->() const;
private:
    friend Iterator<true>;
    friend Iterator<false>;
    friend List<T>;
    Node* _current_node = nullptr;
};

template <typename T>
template <bool is_const>
typename List<T>::template Iterator<is_const>& List<T>::Iterator<is_const>::operator++() {
    _current_node = _current_node->next;
    return *this;
}

template <typename T>
template <bool is_const>
const typename List<T>::template Iterator<is_const> List<T>::Iterator<is_const>::operator++(int) {
    Iterator this_copy = *this;
    ++*this;
    return this_copy;
}

template <typename T>
template <bool is_const>
typename List<T>::template Iterator<is_const>& List<T>::Iterator<is_const>::operator--() {
    _current_node = _current_node->prev;
    return *this;
}

template <typename T>
template <bool is_const>
const typename List<T>::template Iterator<is_const> List<T>::Iterator<is_const>::operator--(int) {
    Iterator this_copy = *this;
    --*this;
    return this_copy;
}

template <typename T>
template <bool is_const>
bool List<T>::Iterator<is_const>::operator==(const List<T>::Iterator<is_const>& other) const {
    return _current_node == other._current_node;
}

template <typename T>
template <bool is_const>
bool List<T>::Iterator<is_const>::operator!=(const List<T>::Iterator<is_const>& other) const {
    return !(*this == other);
}

template <typename T>
template <bool is_const>
typename List<T>::template Iterator<is_const>::reference List<T>::Iterator<is_const>::operator*() const {
    return *(_current_node->data);
}

template <typename T>
template <bool is_const>
typename List<T>::template Iterator<is_const>::pointer List<T>::Iterator<is_const>::operator->() const {
    return _current_node->data;
}
