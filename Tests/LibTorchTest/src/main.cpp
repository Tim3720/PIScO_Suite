#include <iostream>
#include <torch/cuda.h>
#include <torch/torch.h>

int main()
{
    torch::Tensor tensor = torch::rand({ 2, 3 });
    std::cout << torch::cuda::is_available() << std::endl;
    std::cout << tensor << std::endl;
}
