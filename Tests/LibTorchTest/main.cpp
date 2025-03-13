#include <iostream>
#include <opencv2/opencv.hpp>
#include <torch/torch.h>

int main()
{
    // Load an image using OpenCV
    cv::Mat image = cv::imread("/home/tim/Documents/ArbeitTestData/selection/SO308_1-5-1_PISCO2_0012.81dbar-20.10S-036.10E-26.08C_20241106-15340569.png", cv::IMREAD_GRAYSCALE); // Load as grayscale for simplicity
    if (image.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return -1;
    }

    // Convert OpenCV image to Torch tensor (H, W) -> (1, 1, H, W) and normalize
    torch::Tensor img_tensor = torch::from_blob(image.data, { 1, 1, image.rows, image.cols }, torch::kByte);
    img_tensor = img_tensor.to(torch::kFloat) / 255.0; // Normalize to [0,1]

    // Move to GPU
    if (torch::cuda::is_available()) {
        img_tensor = img_tensor.to(torch::kCUDA);
        std::cout << "Using GPU for processing.\n";
    } else {
        std::cout << "CUDA not available, running on CPU.\n";
    }

    // Apply a simple operation: invert the image (1 - pixel_value)
    torch::Tensor result_tensor = 1.0 - img_tensor;

    // Move back to CPU and convert to OpenCV format
    result_tensor = result_tensor.to(torch::kCPU);
    result_tensor = (result_tensor * 255).to(torch::kByte); // Convert back to byte range

    // Convert torch tensor to OpenCV Mat
    cv::Mat result(image.rows, image.cols, CV_8UC1, result_tensor.data_ptr());

    // Display original and processed images
    cv::imshow("Original", image);
    cv::imshow("Processed", result);
    cv::waitKey(0);

    return 0;
}
