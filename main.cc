#include <stdio.h>

template <typename T, typename U>
struct IsSame {
    static constexpr bool value = false;
};

template <typename T>
struct IsSame<T, T> {
    static constexpr bool value = true;
};

template <typename>
struct IsValidForeignFn {
    static constexpr bool value = false;
};

template <typename R, typename... Args>
struct IsValidForeignFn<R (*)(Args...)> {
    static constexpr bool value = IsSame<R, int>::value && (IsSame<Args, int>::value && ...);
};

template <typename>
struct FunctionArity {};

template <typename R, typename... Args>
struct FunctionArity<R (*)(Args...)> {
    static constexpr int value = sizeof...(Args);
};

typedef int (*ForeignFnWrapper)(int *);

template <auto F>
int foreign_fn_wrapper(int *vals)
{
    if constexpr (!IsValidForeignFn<decltype(F)>::value) {
        static_assert(false, "expected foreign function");
    } else {
        constexpr int N = FunctionArity<decltype(F)>::value;
        static_assert(N <= 3, "foreign function must take at most 3 arguments");
        if constexpr (N == 0) return F();
        if constexpr (N == 1) return F(vals[0]);
        if constexpr (N == 2) return F(vals[0], vals[1]);
        if constexpr (N == 3) return F(vals[0], vals[1], vals[2]);
    }
}

template <auto F>
ForeignFnWrapper make_foreign_fn()
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
    ForeignFnWrapper f_fn = make_foreign_fn<fn>();
    int vals[2];
    vals[0] = 20;
    vals[1] = 42;
    printf("%d\n", f_fn(vals));

    // comment out to compile
    make_foreign_fn<fn2>();
    make_foreign_fn<fn3>();
    make_foreign_fn<3>();
}
