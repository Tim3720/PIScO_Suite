#include <iostream>

#include "reader.hpp"

int main()
{
    std::string filename = "/home/tim/Documents/ArbeitTestData/TestResults/objects_contours.dat";

    FileReader reader(filename, OneFile);

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<Object> objects;
    for (size_t i = 0; i < 9184; i++)
        reader.getNextObjectStack(objects, 10);

    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Duration: " << duration << "ms" << std::endl;

    // for (Object& o : objects)
    //     std::cout << o << std::endl;
}
