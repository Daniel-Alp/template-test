#include "value.h"
#include <stdio.h>

template <typename T, typename U>
constexpr bool is_same = false;

template <typename T>
constexpr bool is_same<T, T> = true;

template <typename>
constexpr bool is_foreign_fn = false;

template <typename R, typename... Args>
constexpr bool is_foreign_fn<R (*)(Args...)> =
    is_same<R, InterpResult> && ((is_same<Args, double> || is_same<Args, bool>) && ...);

template <typename>
struct FunctionArity;

template <typename R, typename... Args>
struct FunctionArity<R (*)(Args...)> {
    static constexpr int value = sizeof...(Args);
};

template <int... I>
struct IdxSeq {};

template <int N, int... I>
struct MakeIdxSeq : MakeIdxSeq<N - 1, N - 1, I...> {};

template <int... I>
struct MakeIdxSeq<0, I...> {
    using type = IdxSeq<I...>;
};

typedef InterpResult (*ForeignFnWrapper)(Value *);

template <typename T>
T convert(Value val);

template <>
double convert(Value val)
{
    if (val.tag == VAL_NUM)
        return AS_NUM(val);
    throw "error";
}

template <>
bool convert(Value val)
{
    if (val.tag == VAL_BOOL)
        return AS_BOOL(val);
    throw "error";
}

template <auto F, typename... Args, int... I>
InterpResult foreign_fn_call(Value *vals, InterpResult (*f)(Args...), IdxSeq<I...>)
{
    return F(convert<Args>(vals[I])...);
}

template <auto F>
    requires is_foreign_fn<decltype(F)>
InterpResult wrap_foreign_fn(Value *vals)
{
    constexpr int N = FunctionArity<decltype(F)>::value;
    using Idxs = typename MakeIdxSeq<N>::type;
    try {
        return foreign_fn_call<F>(vals, F, Idxs{});
    } catch (const char *message) {
        return InterpResult{.tag = INTERP_ERR, .message = message};
    }
}

InterpResult fn1(double a, double b)
{
    double c = a + b;
    return InterpResult{.tag = INTERP_OK, .val = MK_NUM(c)};
}

InterpResult fn2(double a, double b, bool c)
{
    double d = c ? a : b;
    return InterpResult{.tag = INTERP_OK, .val = MK_NUM(d)};
}

// cannot wrap into a foreign function, see below
InterpResult fn3_fail(int a, int b)
{
    return InterpResult{.tag = INTERP_OK, .val = MK_NUM(double(a + b))};
}

int main(void)
{
    ForeignFnWrapper f_fn = wrap_foreign_fn<fn1>;
    Value vals[2] = {MK_NUM(20), MK_NUM(42)}; // change arguments to MK_BOOL or MK_NULL and you get runtime error
    InterpResult res = f_fn(vals);
    if (res.tag == INTERP_ERR) {
        printf("%s\n", res.message);
    } else {
        printf("%f\n", AS_NUM(res.val));
    }

    // ForeignFnWrapper f_fn2 = wrap_foreign_fn<fn2>;
    // Value vals2[3] = {MK_NUM(100), MK_NUM(200), MK_BOOL(false)};
    // InterpResult res2 = f_fn2(vals2);
    // if (res2.tag == INTERP_ERR) {
    //     printf("%s\n", res2.message);
    // } else {
    //     printf("%f\n", AS_NUM(res2.val));
    // }

    // uncomment to get compile error
    // ForeignFnWrapper _ = wrap_foreign_fn<fn3_fail>;
    // ForeignFnWrapper _ = wrap_foreign_fn<3>;
    // ForeignFnWrapper _ = wrap_foreign_fn;
}
