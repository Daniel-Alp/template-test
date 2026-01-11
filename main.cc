#include <stdio.h>

template<typename T, typename U>
struct is_same {
    static constexpr bool value = false;
};

template<typename T>
struct is_same<T, T> {
    static constexpr bool value = true;
};

template<typename>
struct FunctionArity {};

template<typename R, typename... Args>
struct FunctionArity<R(*)(Args...)> {
    static_assert(is_same<R, int>::value, "return type must be int");
    static_assert((is_same<Args, int>::value && ...), "parameter types must be ints");
    static constexpr int value = sizeof...(Args);
};

typedef int(*ForeignFn)(int*);

template<auto F>
int foreign_fn_wrapper(int *vals)
{
    constexpr int N = FunctionArity<decltype(F)>::value;
    static_assert(N <= 3, "foreign function takes at most 3 arguments");
    if constexpr (N == 0) {
        return F();
    } else if constexpr (N == 1) {
        return F(vals[0]);
    } else if constexpr (N == 2) {
        return F(vals[0], vals[1]);
    } else if constexpr (N == 3) {
        return F(vals[0], vals[1], vals[2]);
    }
}

template<auto F>
ForeignFn make_foreign_fn()
{
    return foreign_fn_wrapper<F>;
}

int fn(int a, int b)
{
    return a + b;
}

float fn2(int a, int b)
{
    return 1.0;
}

int fn3(int a, float b)
{
    return 1;
}

int main(void)
{
    ForeignFn f_fn = make_foreign_fn<fn>();
    int vals[2];
    vals[0] = 20;
    vals[1] = 42;
    printf("%d\n", f_fn(vals));
}
