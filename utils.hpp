#pragma once
#include <memory>
namespace DSA
{
    namespace Utils
    {

        template <typename Pair>
        struct Select1stKeyOfValue
        {
            const Pair::first_type &operator()(const Pair &v) const { return v.first; }
        };
        template <typename T>
        struct IdentityKeyOfValue
        {
            const T &operator()(const T &k) const { return k; }
        };
        struct ICloneable
        {
            virtual ~ICloneable() = default;
            std::unique_ptr<ICloneable> clone() const { return std::unique_ptr<ICloneable>(clone_unsafe()); }

        protected:
            [[nodiscard]] virtual ICloneable *clone_unsafe() const = 0;
        };

        /**
         * @brief 模板函数，用于获取表示“无穷大”的值。
         * @tparam T 数值类型（如 int, float, double）。
         * @return T 类型的无穷大值。
         *
         * @details
         * 这个函数根据模板参数 T 的类型，返回一个合适的值来代表无穷远。
         * 这在最短路径算法中非常重要，用于初始化距离数组。
         */
        template <typename T>
        constexpr T Infinity()
        {
            // 这里确保了 Infinity 函数只用于算术类型（整数、浮点数等）。
            // static_assert to ensure T is an arithmetic type
            static_assert(std::is_arithmetic_v<T>,
                          "Infinity identifier is only for arithmetic types (int, float, etc).");

            // if constexpr 是一个编译时 if 语句。
            // 这意味着编译器会根据 T 的类型只保留一个分支的代码，提高效率。
            if constexpr (std::is_integral_v<T>)
            {
                // 如果 T 是整数类型（int, long, short 等），
                // 我们使用该类型的最大值来近似表示无穷大。
                // If T is an integer type (int, long, short, etc.)
                return std::numeric_limits<T>::max();
            }
            else
            {
                // 如果 T 不是整数（由 static_assert 可知，它必然是浮点数），
                // 我们使用标准的无穷大表示。
                // Assumes floating point, already checked by static_assert
                // If T is a floating point type (float, double, etc.)
                return std::numeric_limits<T>::infinity();
            }
        }
    }
}