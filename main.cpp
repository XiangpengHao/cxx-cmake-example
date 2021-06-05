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

#define N 1000000

int test_rust_speed()
{
    int sum = 0;
    for (int i = 0; i < N; i += 1)
    {
        sum += rust_part::rust_echo(i);
    }
    return sum;
}

int test_cpp_speed()
{
    int sum = 0;
    for (int i = 0; i < N; i += 1)
    {
        sum += cpp_echo(i);
    }
    return sum;
}

auto test_lto()
{
    using std::chrono::high_resolution_clock;

    auto r_t1 = high_resolution_clock::now();
    auto r_sum = test_rust_speed();
    auto r_t2 = high_resolution_clock::now();

    auto c_t1 = high_resolution_clock::now();
    auto c_sum = test_cpp_speed();
    auto c_t2 = high_resolution_clock::now();

    auto c_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(c_t2 - c_t1).count();
    auto r_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(r_t2 - r_t1).count();

    return std::make_tuple(r_duration, c_duration, r_sum, c_sum);
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

    auto [d_rust, d_cpp, r_sum, c_sum] = test_lto();
    std::cout << "Results produced by Rust / C++: " << r_sum << " / " << c_sum << std::endl;
    if (r_sum != c_sum) {
      std::cerr << "Rust and C++ versions returned mismatching results." << std::endl;
      return EXIT_FAILURE;
    }

    auto ratio = (float)d_rust / d_cpp;

    std::cout << "Rust execution took " << d_rust << " ns, C++ execution took " << d_cpp << " ns. Ratio: " << ratio << std::endl;

    bool rust_much_slower_than_cpp = d_rust / d_cpp > 10;

    if (lto_on && rust_much_slower_than_cpp) {
      std::cerr
        << "Cross-language LTO appears not to be working as Rust is too slow."
        << std::endl;
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;

}
