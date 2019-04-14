//
// Created by netdex on 2019-04-14.
//

#ifndef RENDEN_BYTEBUFFER_HPP
#define RENDEN_BYTEBUFFER_HPP

#include <vector>
#include <glad/glad.h>

template<typename B = GLbyte>
class bytebuf {
    std::vector<B> buf;
public:
    bytebuf() {}

    template<typename T>
    void put(const T &var) {
        auto ptr = reinterpret_cast<const char *>(&var);
        buf.insert(buf.end(), ptr, ptr + sizeof(T));
    }

    template<typename T>
    void put(const T *var, size_t count) {
        auto ptr = reinterpret_cast<const char *>(var);
        buf.insert(buf.end(), ptr, ptr + count * sizeof(T));
    }

    const B *data() { return buf.data(); }

    size_t size() { return buf.size(); }

};

#endif //RENDEN_BYTEBUFFER_HPP
