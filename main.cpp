#include <iostream>
#include <string>
#include "rust_part.h"
#include <chrono>
#include <tuple>

const std::string &get_name(const Person &person)
{
    return person.name;
}

std::unique_ptr<Person> make_person()
{
    return std::make_unique<Person>();
}

bool shared::Color::is_black() const noexcept
{
    return this->r == 0 && this->g == 0 && this->b == 0;
}

int cpp_echo(int val)
{
    return val;
}

int test_fun()
{
    int sum = 0;
    for (int i = 0; i < 1000000; i += 1)
    {
        sum += rust_part::rust_echo(i);
    }
    return sum;
}

int test_inline()
{
    int sum = 0;
    for (int i = 0; i < 1000000; i += 1)
    {
        sum += cpp_echo(i);
    }
    return sum;
}

auto test_lto()
{
    auto t1 = std::chrono::high_resolution_clock::now();
    auto sum = test_fun();
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration_r = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

    std::cout << "Calling rust function"
              << ", time elapsed: " << duration_r << " ns." << std::endl;

    t1 = std::chrono::high_resolution_clock::now();
    sum = test_inline();
    t2 = std::chrono::high_resolution_clock::now();
    auto duration_c = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

    std::cout << "Calling c++ function"
              << ", time elapsed: " << duration_c << " ns." << std::endl;

    return std::make_tuple(duration_r, duration_c);
}

int main(int argc, char** argv)
{
    if (argc != 2) {
      std::cerr
        << "One argument expected: ON | OFF, indicating whether LTO is enabled."
        << std::endl;
      return EXIT_FAILURE;
    }
    bool lto_on = argv[1] == std::string("ON");

    auto thing = rust_part::make_shared_thing();
    rust_part::print_shared_thing(thing);

    auto [d_rust, d_cpp] = test_lto();

    bool rust_much_slower_than_cpp = d_rust / d_cpp > 10;

    if (lto_on && rust_much_slower_than_cpp) {
      std::cerr
        << "Cross-language LTO appears not to be working as Rust is too slow:\n  "
        << d_rust << " ns in Rust vs " << d_cpp << " ns in C++."
        << std::endl;
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;

}
