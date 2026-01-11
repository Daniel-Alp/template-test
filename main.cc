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
struct FunctionArity<R(Args...)> {
    static_assert(is_same<R, int>::value, "return type must be int");
    static_assert((is_same<Args, int>::value && ...), "parameter types must be ints");
    static constexpr int value = sizeof...(Args);
};

int fn(int a, int b)
{
    return 1;
}

float fn2(int a, int b)
{
    return 1.0;
}

float fn3(int a, float b)
{
    return 2.0;
}

int main(void)
{
    static_assert(FunctionArity<decltype(fn)>::value == 2, "");
    // static_assert(FunctionArity<decltype(fn2)>::value == 2, "");
}
