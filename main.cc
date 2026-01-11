template<typename>
struct FunctionArity {};

template<typename R, typename... Args>
struct FunctionArity <R(Args...)>
{
    static constexpr int value = sizeof...(Args);
};

int add(int a, int b)
{
    return a + b;
}

int main(void)
{
    static_assert(FunctionArity<decltype(add)>::value == 2, "");
}
