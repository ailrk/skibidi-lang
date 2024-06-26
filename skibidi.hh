#ifndef SKIBIDI_LANG
#define SKIBIDI_LANG 


/* utils */
template <typename T> struct identity { using type = T; };

template <typename T> struct quote : identity<quote<T>> {};

template <typename T> struct unquote;
template <typename T> struct unquote<quote<T>> : T {};

template <typename T> struct eval : T::type {};

template <typename F, typename... Ts> struct apply {
    using type = typename F::template apply<Ts...>::type;
};


/* integer */
template <int N> struct int_ : identity<int_<N>> { static const int value = N; };

struct plus : identity<plus> {
    template <typename A, typename B>
    using apply = int_<A::type::value + B::type::value>;
};

struct minus : identity<minus> {
    template <typename A, typename B>
    using apply = int_<A::type::value - B::type::value>;
};

struct times : identity<times> {
    template <typename A, typename B>
    using apply = int_<A::type::value * B::type::value>;
};


/* boolean */
template <bool N> struct bool_ : identity<bool_<N>> { static const bool value = N; };

template <typename T> struct not_;
template <> struct not_<bool_<true>> : bool_<false> {};
template <> struct not_<bool_<false>> : bool_<true> {};

template <typename... Ts> struct and_ : bool_<(Ts::type::value && ... && true)> {};
template <typename... Ts> struct or_ : bool_<(Ts::type::value || ... || false)> {};

/* ordering */

struct less_equal : identity<less_equal> {
    template <typename A, typename B>
    using apply = bool_<(A::type::value <= B::type::value)>;
};

struct less : identity<less> {
    template <typename A, typename B>
    using apply = bool_<(A::type::value < B::type::value)>;
};

struct greater : identity<greater> {
    template <typename A, typename B>
    using apply =
        not_<typename ::apply<less_equal, A, B>::type>;
};

struct greater_equal : identity<greater_equal> {
    template <typename A, typename B>
    using apply = not_<typename ::apply<less, A, B>::type>;
};


/* list */
namespace {

template <typename H, typename T> struct cons : identity<cons<H, T>> {};
struct nil : identity<nil> {};

template <typename L> struct head;
template <typename H, typename T> struct head<cons<H, T>> : H {};

template <typename L> struct tail;
template <typename H, typename T> struct tail<cons<H, T>> : T {};

template <typename L, typename R> struct append;
template <typename R> struct append<nil, R> : R {};
template <typename H, typename T, typename R> struct append<cons<H, T>, R> : cons<H, append<T, R>> {};
 
template <typename T> struct last;
template <> struct last<nil> : nil {};
template <typename N> struct last<cons<N, nil>> : N {};
template <typename T, typename N> struct last<cons<T, N>> : last<N> {};

template <typename T> struct length;
template <> struct length<nil> : int_<0> {};
template <typename N, typename T> struct length<cons<N, T>> : apply<plus, int_<1>, length<T>> {};

template <typename T> struct null : bool_<false> {};
template <> struct null<nil> : bool_<true> {};

template <typename F, typename L> struct map;
template <typename F> struct map<F, nil> : nil {};
template <typename F, typename H, typename T> struct map<F, cons<H, T>> : cons<apply<F, H>, map<F, T>>{ };

template <typename T> struct reverse;
template <typename S1, typename S2> struct __reverse; 
template <typename S2> struct __reverse<nil, S2> : S2 {}; 
template <typename H, typename T, typename S2> struct __reverse<cons<H, T>, S2> : __reverse<T, cons<H, S2>> {};
template <typename L> struct reverse : __reverse<L, nil> {};

}

/* conditionals */
template <typename C, typename T, typename F> struct if_ {
  private:
    template <bool C_, typename T_, typename F_> struct if_impl;
    template <typename T_, typename F_> struct if_impl<true, T_, F_> : T {};
    template <typename T_, typename F_> struct if_impl<false, T_, F_> : F {};

  public:
    using type = typename if_impl<C::type::value, T, F>::type;
};


/* variables */
template <typename Id> struct var : identity<var<Id>> {};

#define declare(n)                                                             \
    struct n##__tml__internal_defined_var_ {};                                 \
    using n = var<n##__tml__internal_defined_var_>

// internal variables
declare(_v0);
declare(_v1);
declare(_v2);


/* let binding */
namespace {

template <typename A, typename E, typename In> struct let_subst_1;
template <typename A, typename E, typename In> struct let_subst_2;

template <typename A, typename E, typename In> struct let_subst_1 : let_subst_2<A, E, In> {};
template <typename A, typename E, typename V> struct let_subst_1<A, E, unquote<quote<V>>> : let_subst_1<A, E, V> {};
template <typename A, typename E> struct let_subst_1<A, E, A> { using type = E; };

template <typename _A, typename _E, typename V> struct let_subst_2<_A, _E, quote<V>> : quote<V> {};
template <typename _A, typename _E, typename In> struct let_subst_2 { using type = In; };
template <typename A, typename E, template <typename...> typename F, typename... Ts>
struct let_subst_2<A, E, F<Ts...>> { using type = F<typename let_subst_1<A, E, Ts>::type...>; };

template <typename A, typename E, typename In> struct strict_let;

template <typename A, typename E, typename In>
struct strict_let<A, quote<E>, quote<In>> : quote<typename let_subst_1<A, E, In>::type> {};

template <typename A, typename E, typename In>
struct let_quoted : strict_let<typename A::type, typename E::type, typename In::type> {};

template <typename In> struct in : In {};

template <typename A, typename E, typename In> struct let;

template <typename A, typename E, typename In>
struct let<A, E, in<In>> : unquote<typename let_quoted<A, quote<E>, quote<In>>::type> {};
}


/* lambda */
namespace {

template <typename Body, typename... Ts> struct lambda_subst_1;
template <typename Body, typename...> struct lambda_subst_2;

template <typename Body, typename... Ts> struct lambda_subst_1 : lambda_subst_2<Body, Ts...> {};
template <typename Body, typename... Ts>
struct lambda_subst_1<unquote<quote<Body>>, Ts...> : lambda_subst_1<Body, Ts...> {};

template <typename Body, typename... Ts>
struct lambda_subst_2<quote<Body>, Ts...> : identity<lambda_subst_2<quote<Body>, Ts...>> { using apply = quote<Body>; };
template <typename Body> struct lambda_subst_2<Body> : Body {};
template <typename Body, typename T, typename... Ts>
struct lambda_subst_2<Body, T, Ts...> : identity<lambda_subst_2<Body, T, Ts...>> {
    template <typename U> using apply = lambda_subst_1<let<T, U, in<Body>>, Ts...>;
};


template <typename Body, typename... Ts> struct strict_lambda;
template <typename Body, typename... Ts>
struct strict_lambda<quote<Body>, Ts...> : quote<typename lambda_subst_1<Body, Ts...>::type> {};

// body of the lambda should be quoted.
template <typename Body, typename... Ts>
struct lambda_quoted : strict_lambda<typename Body::type, typename Ts::type...> {};

template <typename... Ts> struct lambda {
    template <typename Body>
    using begin = unquote<typename lambda_quoted<quote<Body>, Ts...>::type>;
};

}


#endif
