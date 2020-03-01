#ifndef RENDEN_BYTEBUFFER_HPP
#define RENDEN_BYTEBUFFER_HPP

#include <vector>

#include <glad/glad.h>

template<typename B = GLbyte>
class ByteBuffer {
    std::vector<B> buffer_;
public:
    ByteBuffer() {}

    template<typename T>
    void put(const T &var) {
        auto ptr = reinterpret_cast<const char *>(&var);
        buffer_.insert(buffer_.end(), ptr, ptr + sizeof(T));
    }

    template<typename T>
    void put(const T *var, size_t count) {
        auto ptr = reinterpret_cast<const char *>(var);
        buffer_.insert(buffer_.end(), ptr, ptr + count * sizeof(T));
    }

    const B *data() { return buffer_.data(); }

    size_t size() { return buffer_.size(); }

};

#endif //RENDEN_BYTEBUFFER_HPP
