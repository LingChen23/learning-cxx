#include "../exercise.h"
#include <cstring>

// READ: 类模板 <https://zh.cppreference.com/w/cpp/language/class_template>

template<class T>
struct Tensor4D {
    unsigned int shape[4];
    T *data;

    Tensor4D(unsigned int const shape_[4], T const *data_) {
        unsigned int size = 1;
        // TODO: 填入正确的 shape 并计算 size
        for (unsigned int i = 0; i < 4; ++i) {
            shape[i] = shape_[i];
            size *= shape[i];
        }
        data = new T[size];
        std::memcpy(data, data_, size * sizeof(T));
    }

    ~Tensor4D() {
        delete[] data;
    }

    // 为了保持简单，禁止复制和移动
    Tensor4D(Tensor4D const &) = delete;

    Tensor4D(Tensor4D &&) noexcept = delete;

    // 这个加法需要支持“单向广播”。
    // 具体来说，`others` 可以具有与 `this` 不同的形状，形状不同的维度长度必须为 1。
    // `others` 长度为 1 但 `this` 长度不为 1 的维度将发生广播计算。
    // 例如，`this` 形状为 `[1, 2, 3, 4]`，`others` 形状为 `[1, 2, 1, 4]`，
    // 则 `this` 与 `others` 相加时，3 个形状为 `[1, 2, 1, 4]` 的子张量各自与 `others` 对应项相加。
    Tensor4D &operator+=(Tensor4D const &others) {
        // TODO: 实现单向广播的加法
        // 检查维度是否匹配，或允许广播
        for (unsigned int i = 0; i < 4; ++i) {
            if (shape[i] != others.shape[i] && others.shape[i] != 1 && shape[i] != 1) {
                throw std::invalid_argument("Shapes are not compatible for broadcasting.");
            }
        }
        // 执行加法，使用广播规则
        unsigned int total_size = shape[0] * shape[1] * shape[2] * shape[3];
        for (unsigned int index = 0; index < total_size; ++index) {
            // 计算线性索引
            unsigned int i0 = (index / (shape[1] * shape[2] * shape[3])) % shape[0];
            unsigned int i1 = (index / (shape[2] * shape[3])) % shape[1];
            unsigned int i2 = (index / shape[3]) % shape[2];
            unsigned int i3 = index % shape[3];

            // 计算 others 的线性索引，考虑广播
            unsigned int o0 = (i0 < others.shape[0]) ? i0 : 0; // 如果 this 的第0维大于 others，则用0
            unsigned int o1 = (i1 < others.shape[1]) ? i1 : 0; // 同上
            unsigned int o2 = (i2 < others.shape[2]) ? i2 : 0; // 同上
            unsigned int o3 = (i3 < others.shape[3]) ? i3 : 0; // 同上

            // 如果 others 相应位置是 1，则用 0 代替
            if (others.shape[0] == 1) o0 = 0; // 用数据的第一维
            if (others.shape[1] == 1) o1 = 0; // 用数据的第一维
            if (others.shape[2] == 1) o2 = 0; // 用数据的第一维
            if (others.shape[3] == 1) o3 = 0; // 用数据的第一维

            // 执行加法
            data[index] += others.data[o0 * others.shape[1] * others.shape[2] * others.shape[3]
                                       + o1 * others.shape[2] * others.shape[3]
                                       + o2 * others.shape[3] + o3];
        }
        return *this;
    }
};

// ---- 不要修改以下代码 ----
int main(int argc, char **argv) {
    {
        unsigned int shape[]{1, 2, 3, 4};
        // clang-format off
        int data[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        auto t0 = Tensor4D<int>(shape, data);
        auto t1 = Tensor4D<int>(shape, data);
        t0 += t1;
        for (auto i = 0u; i < sizeof(data) / sizeof(*data); ++i) {
            ASSERT(t0.data[i] == data[i] * 2, "Tensor doubled by plus its self.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        float d0[]{
            1, 1, 1, 1,
            2, 2, 2, 2,
            3, 3, 3, 3,

            4, 4, 4, 4,
            5, 5, 5, 5,
            6, 6, 6, 6};
        // clang-format on
        unsigned int s1[]{1, 2, 3, 1};
        // clang-format off
        float d1[]{
            6,
            5,
            4,

            3,
            2,
            1};
        // clang-format on

        auto t0 = Tensor4D<float>(s0, d0);
        auto t1 = Tensor4D<float>(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == 7.f, "Every element of t0 should be 7 after adding t1 to it.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        double d0[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        unsigned int s1[]{1, 1, 1, 1};
        double d1[]{1};

        auto t0 = Tensor4D<double>(s0, d0);
        auto t1 = Tensor4D<double>(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == d0[i] + 1, "Every element of t0 should be incremented by 1 after adding t1 to it.");
        }
    }
}