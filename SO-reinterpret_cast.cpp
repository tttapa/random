#include <cmath>        // sqrt
#include <cstddef>      // size_t
#include <iostream>     // ostream, cout, endl
#include <numeric>      // accumulate
#include <type_traits>  // enable_if

template <class T, size_t N>
struct Array {
    T data[N];

    static constexpr size_t length() { return N; }
    using type = T;

    constexpr T &operator[](size_t index) { return data[index]; }
    constexpr const T &operator[](size_t index) const { return data[index]; }
    constexpr T *begin() { return &data[0]; }
    constexpr const T *begin() const { return &data[0]; }
    constexpr T *end() { return &data[N]; }
    constexpr const T *end() const { return &data[N]; }

    /// Subtract
    constexpr Array<T, N> operator-(const Array<T, N> &rhs) const {
        Array<T, N> result = *this;
        result -= rhs;
        return result;
    }

    /// Subtract
    constexpr Array<T, N> &operator-=(const Array<T, N> &rhs) {
        for (size_t i = 0; i < N; ++i)
            (*this)[i] -= rhs[i];
        return *this;
    }

    /// Implicit conversion from Array<T, 1> to T&.
    template <bool EnableBool = true>
    constexpr operator typename std::add_lvalue_reference<
        typename std::enable_if<N == 1 && EnableBool, T>::type>::type() {
        return data[0];
    }

    /// Implicit conversion from const Array<T, 1> to const T&.
    template <bool EnableBool = true>
    constexpr
    operator typename std::add_lvalue_reference<typename std::add_const<
        typename std::enable_if<N == 1 && EnableBool, T>::type>::type>::type()
        const {
        return data[0];
    }

    /// Assignment operator for Array<T, 1>.
    template <bool EnableBool = true>
    constexpr typename std::enable_if<N == 1 && EnableBool, Array<T, 1> &>::type
    operator=(const T &t) {
        data[0] = t;
        return *this;
    }
};

/// Multiplication of single-element arrays.
template <class T>
constexpr Array<T, 1> operator*(Array<T, 1> lhs, Array<T, 1> rhs) {
    return {T{lhs} * T{rhs}};
}

//============================================================================//

/// Matrix type
template <class T, size_t R, size_t C>
using TMatrix = Array<Array<T, C>, R>;

/// Column vector type
template <class T, size_t R>
using TColVector = TMatrix<T, R, 1>;

/// Inner product
template <class T, class U, size_t R>
constexpr auto operator*(const TColVector<T, R> &lhs,
                         const TColVector<U, R> &rhs) {
    using sum_t = std::remove_reference_t<decltype(lhs[0][0] * rhs[0][0])>;
    sum_t sum   = {};
    for (size_t i = 0; i < R; ++i)
        sum += lhs[i] * rhs[i];
    return sum;
}

/// Norm squared
template <class T, size_t C>
constexpr auto normsq(const TColVector<T, C> &colvector)
    -> decltype(colvector[0] * colvector[0]) {
    decltype(colvector[0] * colvector[0]) sumsq = {};
    for (size_t c = 0; c < C; ++c)
        sumsq += colvector[c] * colvector[c];
    return sumsq;
}

/// Norm
template <class T, size_t C>
constexpr auto norm(const TColVector<T, C> &colvector)
    -> decltype(sqrt(normsq(colvector))) {
    return sqrt(normsq(colvector));
}

//============================================================================//

#define REFERENCE

struct Point;
/// Printing a "Point"
inline std::ostream &operator<<(std::ostream &os, Point p);

struct Point {
    float x;
    float y;

    constexpr bool operator==(Point rhs) const {
        return this->x == rhs.x && this->y == rhs.y;
    }

#ifdef REFERENCE
    TColVector<float, 2> &vec() {
        static_assert(sizeof(*this) == sizeof(TColVector<float, 2>));
        return *reinterpret_cast<TColVector<float, 2> *>(this);
    }
    operator TColVector<float, 2> &() { return vec(); }

#else
    TColVector<float, 2> vec() const { return {x, y}; }
    operator TColVector<float, 2>() const { return vec(); }
#endif

    constexpr Point() : x{nan("")}, y{nan("")} {}
    constexpr Point(float x, float y) : x{x}, y{y} {}
    constexpr Point(const TColVector<float, 2> &vec) : x{vec[0]}, y{vec[1]} {}

    static float distance(Point a, Point b) {
        using namespace std;
        auto result = sqrtf(distanceSquared(a, b));
        // cout << "sqrtf(distanceSquared(a, b)) = " << result << endl;
        return result;
    }
    static float distanceSquared(Point a, Point b) {
        using namespace std;
        // cout << "a = " << a << ", b = " << b << endl;
        auto diff = a.vec() - b.vec();
        // cout << "diff = " << Point(diff) << endl;
        auto result = normsq(diff);
        // cout << "normsq(diff) = " << result << endl;
        return result;
    }
};

inline std::ostream &operator<<(std::ostream &os, Point p) {
    return os << '(' << p.x << ", " << p.y << ')';
}

using namespace std;
#include <iomanip>

int main() {
    Point p0       = {185.04774475097656, 254.54849243164062};
    Point p1       = {453.23568725585938, 237.98394775390625};
    float result   = Point::distance(p0, p1);
    float expected = 268.69900512695312;
    cout << setprecision(7) << result << endl;
    bool success = result == expected;
    return success ? 0 : 1;
}

#ifdef STACK_OVERFLOW

    template <class T, size_t N>
    struct Array {
        T data[N];
    };

    template <class T, size_t R, size_t C>
    using TMatrix = Array<Array<T, C>, R>;

    template <class T, size_t R>
    using TColVector = TMatrix<T, R, 1>;

    struct Point {
        float x;
        float y;

        constexpr Point(float x, float y) : x{x}, y{y} {}
        constexpr Point(const TColVector<float, 2> &vec) : x{vec[0]}, y{vec[1]} {}

        TColVector<float, 2> &vec() {
            static_assert(sizeof(*this) == sizeof(TColVector<float, 2>));
            return *reinterpret_cast<TColVector<float, 2> *>(this);
        }
        operator TColVector<float, 2> &() { return vec(); }
    };

#endif
