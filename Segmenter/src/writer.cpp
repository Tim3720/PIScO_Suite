#include "writer.hpp"
#include "settings.hpp"
#include <format>
#include <iostream>

Writer::Writer()
{
    m_saveFile.open(e_savePath + "segmenter_output.dat");
    m_saveFile << "Image ID\tThreshold\tx\ty\twidth\theight\tArea\tContour\n";
}

Error Writer::writeObjects(const std::vector<SegmenterObject>& objects)
{
    {
        std::unique_lock lock(m_writerLock);
        for (const SegmenterObject& object : objects) {
            m_saveFile << object.m_imageId << "\t" << object.m_threshold << "\t"
                       << object.m_boundingBox.x << "\t" << object.m_boundingBox.y << "\t"
                       << object.m_boundingBox.width << "\t"
                       << object.m_boundingBox.height << "\t" << object.m_area << "\t";
            for (size_t i = 0; i < object.m_contour.size() - 1; i++) {
                m_saveFile << object.m_contour[i].x << "," << object.m_contour[i].y
                           << ";";
            }
            m_saveFile << object.m_contour.back().x << "," << object.m_contour.back().y
                       << "\n";
        }
    }

    return Error::Success;
}

////////////////////////////////////////////////////////////////////////////////
// H5 helpers:
// Utility to write a scalar dataset
template <typename T>
void write_scalar_dataset(H5::Group& group, const std::string& name, const T& value)
{
    H5::DataSpace dataspace(H5S_SCALAR);
    H5::DataSet dataset =
        group.createDataSet(name, H5::PredType::NATIVE_DOUBLE, dataspace);
    dataset.write(&value, H5::PredType::NATIVE_DOUBLE);
}

void writeIntDataset(H5::Group& group, const std::string& name, int value)
{
    H5::DataSpace dataspace(H5S_SCALAR);
    H5::DataSet dataset = group.createDataSet(name, H5::PredType::NATIVE_INT, dataspace);
    dataset.write(&value, H5::PredType::NATIVE_INT);
}

void writeFloatDataset(H5::Group& group, const std::string& name, float value)
{
    H5::DataSpace dataspace(H5S_SCALAR);
    H5::DataSet dataset =
        group.createDataSet(name, H5::PredType::NATIVE_FLOAT, dataspace);
    dataset.write(&value, H5::PredType::NATIVE_FLOAT);
}

void writeImageDataset(H5::Group& group, const std::string& name, const cv::Mat& image,
    bool compression)
{

    if (compression) {
        // H5::DSetCreatPropList plist;
        // hsize_t chunk_dims[2] = {std::min<hsize_t>(1280, dims[0]),
        //     std::min<hsize_t>(1280, dims[1])};
        // plist.setChunk(2, chunk_dims);
        // plist.setDeflate(2);
        // dataset = group.createDataSet(name, H5::PredType::NATIVE_UCHAR, dataspace,
        // plist);

        std::vector<uchar> buf;
        std::vector<int> params = {cv::IMWRITE_PNG_COMPRESSION, 0};
        cv::imencode(".png", image, buf, params);
        //
        // // Store in HDF5
        // hsize_t buffer_dims[1] = {buf.size()};
        // H5::DataSpace dataspace(1, buffer_dims);
        // H5::IntType datatype(H5::PredType::NATIVE_UINT8);
        // H5::DataSet dataset = group.createDataSet(name, datatype, dataspace);
        // dataset.write(buf.data(), H5::PredType::NATIVE_UINT8);
    } else {
        hsize_t dims[2] = {static_cast<hsize_t>(image.rows),
            static_cast<hsize_t>(image.cols)};
        H5::DataSpace dataspace(2, dims);
        H5::DataSet dataset =
            group.createDataSet(name, H5::PredType::NATIVE_UCHAR, dataspace);
        dataset.write(image.data, H5::PredType::NATIVE_UCHAR);
    }
}

// Utility to write a 1D or 2D array
template <typename T>
void write_array_dataset(H5::Group& group, const std::string& name,
    const std::vector<hsize_t>& dims, const std::vector<T>& data,
    const H5::PredType& type)
{
    H5::DataSpace dataspace(dims.size(), dims.data());
    H5::DataSet dataset = group.createDataSet(name, type, dataspace);
    dataset.write(data.data(), type);
}

// Write a string
void write_string_dataset(H5::Group& group, const std::string& name,
    const std::string& str)
{
    H5::StrType str_type(H5::PredType::C_S1, H5T_VARIABLE);
    H5::DataSpace dataspace(H5S_SCALAR);
    H5::DataSet dataset = group.createDataSet(name, str_type, dataspace);
    dataset.write(str, str_type);
}

H5Writer::H5Writer()
{
    std::string filename = e_savePath + "segmenter_output.h5";
    std::cout << filename << std::endl;
    m_saveFile = H5::H5File(filename, H5F_ACC_TRUNC);
}

Error H5Writer::writeObjects(const std::vector<SegmenterObject>& objects, size_t imageId,
    const cv::Mat& image)
{
    {
        std::unique_lock lock(m_writerLock);
        // make new group for image:
        H5::Group imageGroup =
            m_saveFile.createGroup("Image_" + std::format("{:06d}", imageId));
        // writeImageDataset(imageGroup, "image", image, true);
        // writeImageDataset(imageGroup, "background_corrected", image, false);

        for (size_t i = 0; i < objects.size(); i++) {
            SegmenterObject object = objects[i];
            H5::Group objectGroup =
                imageGroup.createGroup("object_" + std::format("{:04d}", i));
            writeIntDataset(objectGroup, "threshold", 1);
            writeIntDataset(objectGroup, "bbox x", object.m_boundingBox.x);
            writeIntDataset(objectGroup, "bbox y", object.m_boundingBox.y);
            writeIntDataset(objectGroup, "bbox w", object.m_boundingBox.width);
            writeIntDataset(objectGroup, "bbox h", object.m_boundingBox.height);
            writeFloatDataset(objectGroup, "area", object.m_area);

            writeImageDataset(objectGroup, "crop", object.m_crop, true);
        }
    }
    // write_array_dataset(objectGroup, "contour", {object.m_contour.size(), 2},
    //     object.m_contour, H5::PredType::NATIVE_INT);

    return Error::Success;
}
