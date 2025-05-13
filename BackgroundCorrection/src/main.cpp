#include "correction.hpp"
#include "error.hpp"
#include "settings.hpp"

int main(int argc, char** argv)
{
    if (argc > 1) {
        loadSettings(argv[1]);
    } else {
        defaultSettings();
    }

    runCorrection().check();
}
