#include "../skibidi.hh"
#include <iostream>


declare(x); declare(y);

template <typename N>
struct factorial : 
    if_ < apply <less, N, int_<1>>
        , int_<1>
        , apply <times, factorial <apply <minus, N, int_<1>>>, N>
        > {};


using plus3 =
        lambda<x>::begin
            < 
               apply <plus, x, int_<3>>
            >;


using program = 
    let <x, factorial<int_<10>>, in < 
    let <y, int_<10>,            in < 
        apply <minus, y, apply <plus3, x>>
    >>>>;


int main(int argc, char *argv[])
{
    std::cout << eval<program>::value << std::endl;
    
    return 0;
}
