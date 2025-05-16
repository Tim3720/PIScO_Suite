#include "writer.hpp"
#include "settings.hpp"

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
