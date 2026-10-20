
std::is_integral<> and similar type traits in <type_traits> perform compile time introspection. They allow compiler to 
inspect, query or transform properties of types at compile time with the zero runtime cost.
Under the hood, these traits are implemented using template specialization and a helper wrapper called 
integral_constant

<code>
    template <typename T, T v>
    struct integral_constant {
        static constexpr T value = v;
        using value_type = T;
        using type = integral_constant;
        constexpr operator value_type() const noexcept { return value; }
    };

    // Aliases for true and false boolean constants
    using true_type = integral_constant<bool, true>
    using false_type = integral_constant<bool, false>

<code>


is_integral defines a primary template that inherits from false_type by default. Then it explicitly specializes for
every single built-in integral type to inherit from true_type.


<code>
    template <typename T>
    struct my_is_integral : false_type {};

    // Specialization for all fundamental integral types
    template <> struct my_is_integral<bool>             : true_type {}
    template <> struct my_is_integral<char>             : true_type {}
    template <> struct my_is_integral<signed char>      : true_type {}
    template <> struct my_is_integral<unsigned char>    : true_type {}
    template <> struct my_is_integral<int>              : true_type {}
    template <> struct my_is_integral<unsigned int>     : true_type {}
    template <> struct my_is_integral<long>             : true_type {}
    template <> struct my_is_integral<long long>        : true_type {}
    template <> struct my_is_integral<unsigned long long> : true_type {}

    // Handle const/volatile modifiers using partial specialization
    template <typename T> struct my_is_integral<const T>        : my_is_integral<T> {};
    template <typename T> struct my_is_integral<volatile T>        : my_is_integral<T> {};
    template <typename T> struct my_is_integral<const volatile T> : my_is_integral<T> {};

<code>

When you select my_is_integral<int>::value, the compiler selects the int specialization, which inherits value=true. 
If you pass my_is_integral<double>::value, it falls back to the primary template where value = false.



The 4 Major Categories of <type_traits>

1. Primary Type Categories (Boolean Queries)
    These asks "What fundamental kind of type is 'T'?"
    -> is_integral<T>   / is_floating_point<T>
    -> is_pointer<T>    / is_reference<T>   / is_array<T>
    -> is_class<T>      / is_enum<T>        / is_union<T>

2. Composite Type Properties
    These combine primary queries to check broader categories
    -> is_arithmetic<T> == is_integral<T>::value || is_floating_point<T>::value
    -> is_scalar<T> = Numbers, pointers, enums, and pointers-to-members
    -> is_signed<T>     / is_unsigned<T>

3. Type Relationships
    These inspect how two types interact.
    -> is_same<T, U> = checks if T and U are identical
    -> is_base_of<Base, Derived> = checks class inheritance
    -> is_convertible<From, To> = checks if 'From' implicitly converts to 'To'

4. Type Transformations (Modifying Types)
    Instead of returning a boolean ::value, these yield a modified type via a nested ::type alias
    std::remove_reference<T>::type = Turns T& or T&& into T.
    std::remove_const<T>::type = Strips const.
    std::add_pointer<T>::type = Turns T into T*
    std::decay<T>::type = Applies implicit conversions (arrays to pointers, stripping reference/const) like function
        argument passing.