#include "correction.hpp"
#include "error.hpp"
#include "settings.hpp"

int main(int argc, char** argv)
{
#if (DEBUG)
    std::cout << makeRed("Program is compiled in DEBUG mode") << std::endl;
#endif
    if (argc > 1) {
        loadSettings(argv[1]);
    } else {
        std::cout << makeBlue("No input file given, using default settings.")
                  << std::endl;
        defaultSettings();
    }

    runCorrection().check();
}
