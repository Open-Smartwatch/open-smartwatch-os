#ifndef OSW_UTIL_H
#define OSW_UTIL_H

#include <cstddef> // size_t

namespace OswUtil {

/**
 * This allows _compile-time_ deduction of the length of a c-array.
 * This is a workaround to get the std::size() function of c++17.
 * 
 * Reference: https://stackoverflow.com/questions/4108313/how-do-i-find-the-length-of-an-array
 */
template<typename T, size_t N>
constexpr size_t size(T (&)[N]) { 
    return N;
}

}

#endif