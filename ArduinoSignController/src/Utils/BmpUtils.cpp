#include "BmpUtils.h"
#include <cstring>

namespace
{
#pragma pack(push, 1)
    struct BitmapFileHeader
    {
        uint16_t type;
        uint32_t size;
        uint16_t reserved1;
        uint16_t reserved2;
        uint32_t offBits;
    };

    struct BitmapInfoHeader
    {
        uint32_t size;
        int32_t width;
        int32_t height;
        uint16_t planes;
        uint16_t bitCount;
        uint32_t compression;
        uint32_t sizeImage;
        int32_t xPelsPerMeter;
        int32_t yPelsPerMeter;
        uint32_t clrUsed;
        uint32_t clrImportant;
    };
#pragma pack(pop)

    constexpr uint16_t BmpSignature = 0x4D42;
    constexpr uint32_t BmpCompressionRgb = 0;
}

std::vector<std::vector<unsigned long>> BmpUtils::ParseBmp(const std::vector<uint8_t>& bytes)
{
    if (bytes.empty())
    {
        return {};
    }

    return ParseBmp(bytes.data(), bytes.size());
}

std::vector<std::vector<unsigned long>> BmpUtils::ParseBmp(const uint8_t* bytes, size_t length)
{
    std::vector<std::vector<unsigned long>> result;

    if (bytes == nullptr)
    {
        return result;
    }

    const size_t minimumHeaderSize = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
    if (length < minimumHeaderSize)
    {
        return result;
    }

    BitmapFileHeader fileHeader{};
    BitmapInfoHeader infoHeader{};
    std::memcpy(&fileHeader, bytes, sizeof(fileHeader));
    std::memcpy(&infoHeader, bytes + sizeof(fileHeader), sizeof(infoHeader));

    if (fileHeader.type != BmpSignature)
    {
        return result;
    }

    // Support only Windows BITMAPINFOHEADER + uncompressed 24-bit RGB.
    if (infoHeader.size != sizeof(BitmapInfoHeader) ||
        infoHeader.planes != 1 ||
        infoHeader.bitCount != 24 ||
        infoHeader.compression != BmpCompressionRgb ||
        infoHeader.width <= 0 ||
        infoHeader.height == 0)
    {
        return result;
    }

    const uint32_t width = static_cast<uint32_t>(infoHeader.width);
    const bool isTopDown = infoHeader.height < 0;
    const uint32_t height = static_cast<uint32_t>(isTopDown ? -infoHeader.height : infoHeader.height);

    const uint32_t bytesPerPixel = 3;
    const uint32_t rowStride = width * bytesPerPixel;
    const uint32_t paddedRowStride = (rowStride + 3U) & ~3U;
    const uint32_t rowPadding = paddedRowStride - rowStride;
    const uint64_t pixelBytesRequired = static_cast<uint64_t>(paddedRowStride) * static_cast<uint64_t>(height);

    if (fileHeader.offBits > length)
    {
        return result;
    }

    if (static_cast<uint64_t>(fileHeader.offBits) + pixelBytesRequired > static_cast<uint64_t>(length))
    {
        return result;
    }

    result.assign(height, std::vector<unsigned long>(width, 0UL));
    size_t readIndex = static_cast<size_t>(fileHeader.offBits);

    for (uint32_t sourceRow = 0; sourceRow < height; ++sourceRow)
    {
        const uint32_t targetRow = isTopDown ? sourceRow : (height - 1U - sourceRow);

        for (uint32_t column = 0; column < width; ++column)
        {
            const uint8_t blue = bytes[readIndex++];
            const uint8_t green = bytes[readIndex++];
            const uint8_t red = bytes[readIndex++];

            const unsigned long rgb =
                (static_cast<unsigned long>(red) << 16) |
                (static_cast<unsigned long>(green) << 8) |
                static_cast<unsigned long>(blue);

            result[targetRow][column] = rgb;
        }

        readIndex += rowPadding;
    }

    return result;
}
