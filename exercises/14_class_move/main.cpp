#include "../exercise.h"
#include <cstring>

// READ: 移动构造函数 <https://zh.cppreference.com/w/cpp/language/move_constructor>
// READ: 移动赋值 <https://zh.cppreference.com/w/cpp/language/move_assignment>
// READ: 运算符重载 <https://zh.cppreference.com/w/cpp/language/operators>

class DynFibonacci {
    size_t *cache;
    int cached;
    int capacity;

public:
    // TODO: 实现动态设置容量的构造器

DynFibonacci(int capacity): cache(new size_t[capacity]), cached(1), capacity(capacity) {
    cache[0] = 0;
    if(capacity > 1){
        cache[1] = 1;
    }
}

    // TODO: 实现移动构造器
    DynFibonacci(DynFibonacci &&other) noexcept {
        cache = std::move(other.cache);
        other.cache = nullptr;
        cached = other.cached;
        capacity = other.capacity;
    }

// TODO: 实现移动赋值
    // NOTICE: ⚠ 注意移动到自身问题 ⚠
    DynFibonacci &operator=(DynFibonacci &&other) noexcept {
        if(this != &other){
            cache = std::move(other.cache);
            other.cache = nullptr;
            cached = other.cached;
            capacity = other.capacity;
        }

        return *this; // 确保返回自身引用
    }

    // TODO: 实现析构器，释放缓存空间
    ~DynFibonacci(){
        if (cache != nullptr) {
            delete []cache;
        }
    }

    // TODO: 实现正确的缓存优化斐波那契计算
    size_t operator[](int i) {
        if(i <= cached){
            return cache[i];
        }
        if(i > capacity){
            size_t * newCache = new size_t [capacity * 2];
            memcpy(newCache, cache,cached * sizeof(size_t));
            delete [] cache;
            cache = newCache;
            capacity *= 2;
        }

        for (cached = cached + 1; cached <= i; ++cached) {
            cache[cached] = cache[cached - 1] + cache[cached - 2];
        }
        return cache[i];
    }

    size_t operator[](int i) const{
        if(i <= cached){
            return cache[i];
        }

        ASSERT(false,"i out of range");
        return 0;
    }

    // NOTICE: 不要修改这个方法
    bool is_alive() const {
        return cache;
    }
};

int main(int argc, char **argv) {
    DynFibonacci fib(12);
    ASSERT(fib[10] == 55, "fibonacci(10) should be 55");

    DynFibonacci const fib_ = std::move(fib);
    ASSERT(!fib.is_alive(), "Object moved");
    ASSERT(fib_[10] == 55, "fibonacci(10) should be 55");

    DynFibonacci fib0(6);
    DynFibonacci fib1(12);

    fib0 = std::move(fib1);
    fib0 = std::move(fib0);
    ASSERT(fib0[10] == 55, "fibonacci(10) should be 55");

    return 0;
}
