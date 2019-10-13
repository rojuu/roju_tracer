template <typename T>
struct Array {
    T* members;
    size_t count;
};

template <typename T>
class SafeQueue {
private:
    std::queue<T> m_Queue;
    SDL_mutex *m_Mutex;

public:
    SafeQueue() : m_Queue(), m_Mutex(SDL_CreateMutex()) {}

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
    SDL_LockMutex(m_Mutex);
    auto result = m_Queue.front();
    SDL_UnlockMutex(m_Mutex);
    return result;
}

template <typename T>
T
SafeQueue<T>::back() {
    SDL_LockMutex(m_Mutex);
    auto result = m_Queue.back();
    SDL_UnlockMutex(m_Mutex);
    return result;
}

template <typename T>
bool
SafeQueue<T>::empty() {
    SDL_LockMutex(m_Mutex);
    auto result = m_Queue.empty();
    SDL_UnlockMutex(m_Mutex);
    return result;
}

template <typename T>
size_t
SafeQueue<T>::size() {
    SDL_LockMutex(m_Mutex);
    auto result = m_Queue.size();
    SDL_UnlockMutex(m_Mutex);
    return result;
}

template <typename T>
void
SafeQueue<T>::unsafePush(T e) {
    m_Queue.push(e);
}

template <typename T>
void
SafeQueue<T>::push(T e) {
    SDL_LockMutex(m_Mutex);
    m_Queue.push(e);
    SDL_UnlockMutex(m_Mutex);
}

template <typename T>
void
SafeQueue<T>::clear() {
    SDL_LockMutex(m_Mutex);
    m_Queue = std::queue<T>();
    SDL_UnlockMutex(m_Mutex);
}

template <typename T>
bool
SafeQueue<T>::pop(T* out) {
    SDL_LockMutex(m_Mutex);
    if (m_Queue.empty()) {
        SDL_UnlockMutex(m_Mutex);
        return false;
    }
    if (out != nullptr) {
        *out = m_Queue.front();
    }
    m_Queue.pop();
    SDL_UnlockMutex(m_Mutex);
    return true;
}