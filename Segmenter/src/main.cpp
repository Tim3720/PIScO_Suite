#include "error.hpp"
#include <iostream>

int main()
{
    Error error = Error::InvalidInput;
    error.addMessage("Test");
    error.check();

    std::cout << "Hello world\n";
}
