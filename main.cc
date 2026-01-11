#include <stdio.h>
#include "value.h"

template <typename T, typename U>
constexpr bool is_same = false;

template <typename T>
constexpr bool is_same<T, T> = true;

template <typename>
constexpr bool is_foreign_fn = false;

template <typename R, typename... Args>
constexpr bool is_foreign_fn<R (*)(Args...)> = is_same<R, InterpResult> && (is_same<Args, Value> && ...);

template <typename>
struct FunctionArity;

template <typename R, typename... Args>
struct FunctionArity<R (*)(Args...)> {
    static constexpr int value = sizeof...(Args);
};

template <int ...I>
struct IdxSeq {};

template <int N, int ...I>
struct MakeIdxSeq : MakeIdxSeq<N-1, N-1, I...> {};

template <int ...I>
struct MakeIdxSeq<0, I...> {
    using type = IdxSeq<I...>;
};

typedef InterpResult (*ForeignFnWrapper)(Value *);

template <auto F, int ...I>
InterpResult foreign_fn_call(Value *vals, IdxSeq<I...>)
{
    return F(vals[I]...);
}

template <auto F>
InterpResult foreign_fn_wrapper(Value *vals)
{
    if constexpr (!is_foreign_fn<decltype(F)>) {
        static_assert(false, "expected foreign function");        
    } else {
        constexpr int N = FunctionArity<decltype(F)>::value;
        return foreign_fn_call<F>(vals, typename MakeIdxSeq<N>::type{});
    }
}

template <auto F>
ForeignFnWrapper make_foreign_fn()
{
    return foreign_fn_wrapper<F>;
}

InterpResult fn1(Value a, Value b, Value c, Value d, Value e)
{
    double f = AS_NUM(a) + AS_NUM(b) + AS_NUM(c) + AS_NUM(d);
    return InterpResult{.tag=INTERP_OK, .val = MK_NUM(f)};
}

int main(void)
{
    static_assert(is_foreign_fn<decltype(&fn1)> == true, "");

    ForeignFnWrapper f_fn = make_foreign_fn<fn1>();
    Value vals[2];
    vals[0] = MK_NUM(20);
    vals[1] = MK_NUM(42);
    printf("%f\n", f_fn(vals).val.as.number);
    // comment out to compile
    // make_foreign_fn<fn2>();
    // make_foreign_fn<fn3>();
    // make_foreign_fn<3>();
    // make_foreign_fn();
}
