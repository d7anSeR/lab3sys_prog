#include <pthread.h>
#include <iostream>
#include <queue>
#include <optional>
#include<unistd.h>

template <typename T>
class threadsafe_queue {
public:
    threadsafe_queue(size_t max_size) : max_size(max_size) {
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&not_full, NULL);
        pthread_cond_init(&not_empty, NULL);
    }

    threadsafe_queue(const threadsafe_queue&) = delete;
    threadsafe_queue(threadsafe_queue&&) = delete;

    ~threadsafe_queue() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&not_full);
        pthread_cond_destroy(&not_empty);
    }

    void enqueue(const T& v) {
        pthread_mutex_lock(&mutex);
        while (queue.size() >= max_size) {
            pthread_cond_wait(&not_full, &mutex);
        }
        queue.push(v);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&not_empty);
    }

    T dequeue() {
        pthread_mutex_lock(&mutex);
        while (queue.empty()) {
            pthread_cond_wait(&not_empty, &mutex);
        }
        T v = queue.front();
        queue.pop();
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&not_full);
        return v;
    }

    std::optional<T> try_dequeue() {
        pthread_mutex_lock(&mutex);
        if (queue.empty()) {
            pthread_mutex_unlock(&mutex);
            return std::nullopt;
        }
        T v = queue.front();
        queue.pop();
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&not_full);
        return v;
    }

    bool try_enqueue(const T& v) {
        pthread_mutex_lock(&mutex);
        if (queue.size() >= max_size) {
            pthread_mutex_unlock(&mutex);
            return false;
        }
        queue.push(v);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&not_empty);
        return true;
    }

    bool full() const {
        pthread_mutex_lock(&mutex);
        bool is_full = queue.size() >= max_size;
        pthread_mutex_unlock(&mutex);
        return is_full;
    }

    bool empty() const {
        pthread_mutex_lock(&mutex);
        bool is_empty = queue.empty();
        pthread_mutex_unlock(&mutex);
        return is_empty;
    }

private:
    std::queue<T> queue;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
    size_t max_size;
};

void* writer_thread(void* arg) {
    threadsafe_queue<int>* q = static_cast<threadsafe_queue<int>*>(arg);
    for (int i = 0; i < 5; ++i) {
        q->enqueue(i);
        usleep(100 * 1000);
    }
    pthread_exit(NULL);
}

void* reader_thread(void* arg) {
    threadsafe_queue<int>* q = static_cast<threadsafe_queue<int>*>(arg);
    while (true) {
        auto value = q->try_dequeue();
        if (!value.has_value()) {
            break;
        }
        std::cout << "Got value: " << value.value() << std::endl;
        usleep(300 * 1000);
    }
    pthread_exit(NULL);
}
int usleep(useconds_t usec);
int main() {
    const size_t max_queue_size = 10;
    threadsafe_queue<int> q(max_queue_size);

    pthread_t writer_threads[3];
    for (int i = 0; i < 3; ++i) {
        pthread_create(&writer_threads[i], NULL, writer_thread, &q);
    }

    pthread_t reader_threads[2];
    for (int i = 0; i < 2; ++i) {
        pthread_create(&reader_threads[i], NULL, reader_thread, &q);
    }

    for (int i = 0; i < 3; ++i) {
        pthread_join(writer_threads[i], NULL);
    }

    for (int i = 0; i < 2; ++i) {
        q.enqueue(-1); 
    }

    for (int i = 0; i < 2; ++i) {
        pthread_join(reader_threads[i], NULL);
    }

    return 0;
}