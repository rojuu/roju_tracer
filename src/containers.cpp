template <typename T>
struct Array {
    T* members;
    size_t count;
};

template <typename T>
class SafeQueue {
private:
    std::queue<T> m_Queue;
    std::mutex m_Mutex;

public:
    SafeQueue() : m_Queue(), m_Mutex() {}

    T front();
    T back();
    bool empty();
    size_t size();
    void unsafePush(T e);
    void push(T e);
    void clear();
    bool pop(T* out = nullptr);
};

template <typename T>
T
SafeQueue<T>::front() {
    std::lock_guard<std::mutex> lockGuard(m_Mutex);
    return m_Queue.front();
}

template <typename T>
T
SafeQueue<T>::back() {
    std::lock_guard<std::mutex> lockGuard(m_Mutex);
    return m_Queue.back();
}

template <typename T>
bool
SafeQueue<T>::empty() {
    return m_Queue.empty();
}

template <typename T>
size_t
SafeQueue<T>::size() {
    std::lock_guard<std::mutex> lockGuard(m_Mutex);
    return m_Queue.size();
}

template <typename T>
void
SafeQueue<T>::unsafePush(T e) {
    m_Queue.push(e);
}

template <typename T>
void
SafeQueue<T>::push(T e) {
    std::lock_guard<std::mutex> lockGuard(m_Mutex);
    m_Queue.push(e);
}

template <typename T>
void
SafeQueue<T>::clear() {
    std::lock_guard<std::mutex> lockGuard(m_Mutex);
    m_Queue = std::queue<T>();
}

template <typename T>
bool
SafeQueue<T>::pop(T* out) {
    std::lock_guard<std::mutex> lockGuard(m_Mutex);
    if (m_Queue.empty()) {
        return false;
    }
    if (out != nullptr) {
        *out = m_Queue.front();
    }
    m_Queue.pop();
    return true;
}