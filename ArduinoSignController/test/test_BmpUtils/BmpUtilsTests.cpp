#ifdef PIO_UNIT_TESTING
#include <ArduinoFake.h>
#else
#include <Arduino.h>
#endif

#include <unity.h>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "IO\BmpUtils.cpp"

namespace
{
    std::vector<uint8_t> readFixtureBytes(const char* relativePath)
    {
        std::ifstream file(relativePath, std::ios::binary);
        if (!file)
        {
            return {};
        }

        return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

    void appendUint16Le(std::vector<uint8_t>& bytes, uint16_t value)
    {
        bytes.push_back(static_cast<uint8_t>(value & 0xFF));
        bytes.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    }

    void appendUint32Le(std::vector<uint8_t>& bytes, uint32_t value)
    {
        bytes.push_back(static_cast<uint8_t>(value & 0xFF));
        bytes.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
        bytes.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
        bytes.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
    }

    void appendInt32Le(std::vector<uint8_t>& bytes, int32_t value)
    {
        appendUint32Le(bytes, static_cast<uint32_t>(value));
    }

    std::vector<uint8_t> buildBmp24(
        uint32_t width,
        int32_t signedHeight,
        const std::vector<unsigned long>& topLeftToBottomRightRgb)
    {
        const uint32_t height = static_cast<uint32_t>(signedHeight < 0 ? -signedHeight : signedHeight);
        const uint32_t rowStride = width * 3U;
        const uint32_t paddedRowStride = (rowStride + 3U) & ~3U;
        const uint32_t rowPadding = paddedRowStride - rowStride;
        const uint32_t pixelDataSize = paddedRowStride * height;
        const uint32_t pixelOffset = 14U + 40U;
        const uint32_t fileSize = pixelOffset + pixelDataSize;

        std::vector<uint8_t> bytes;
        bytes.reserve(fileSize);

        // BITMAPFILEHEADER
        appendUint16Le(bytes, 0x4D42);
        appendUint32Le(bytes, fileSize);
        appendUint16Le(bytes, 0);
        appendUint16Le(bytes, 0);
        appendUint32Le(bytes, pixelOffset);

        // BITMAPINFOHEADER
        appendUint32Le(bytes, 40);
        appendInt32Le(bytes, static_cast<int32_t>(width));
        appendInt32Le(bytes, signedHeight);
        appendUint16Le(bytes, 1);
        appendUint16Le(bytes, 24);
        appendUint32Le(bytes, 0);
        appendUint32Le(bytes, pixelDataSize);
        appendInt32Le(bytes, 0);
        appendInt32Le(bytes, 0);
        appendUint32Le(bytes, 0);
        appendUint32Le(bytes, 0);

        for (uint32_t sourceRow = 0; sourceRow < height; ++sourceRow)
        {
            const uint32_t topRow = (signedHeight < 0) ? sourceRow : (height - 1U - sourceRow);

            for (uint32_t column = 0; column < width; ++column)
            {
                const unsigned long rgb = topLeftToBottomRightRgb[topRow * width + column];
                const uint8_t red = static_cast<uint8_t>((rgb >> 16) & 0xFF);
                const uint8_t green = static_cast<uint8_t>((rgb >> 8) & 0xFF);
                const uint8_t blue = static_cast<uint8_t>(rgb & 0xFF);

                // BMP pixel order is BGR.
                bytes.push_back(blue);
                bytes.push_back(green);
                bytes.push_back(red);
            }

            for (uint32_t paddingIndex = 0; paddingIndex < rowPadding; ++paddingIndex)
            {
                bytes.push_back(0);
            }
        }

        return bytes;
    }
}

void setUp(void)
{
}

void tearDown(void)
{
}

void parseBmp24BottomUpMapsToTopLeftOrigin()
{
    const std::vector<unsigned long> pixels = {
        0x112233UL, 0x445566UL,
        0x778899UL, 0xAABBCCUL
    };

    const std::vector<uint8_t> bmp = buildBmp24(2, 2, pixels);
    const std::vector<std::vector<unsigned long>> result = BmpUtils::ParseBmp(bmp.data(), bmp.size());

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(2, result.size(), "Expected 2 rows");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(2, result[0].size(), "Expected 2 columns");

    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x112233UL, result[0][0], "Top-left pixel mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x445566UL, result[0][1], "Top-right pixel mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x778899UL, result[1][0], "Bottom-left pixel mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0xAABBCCUL, result[1][1], "Bottom-right pixel mismatch");
}

void parseBmp24TopDownRemainsTopLeftOrigin()
{
    const std::vector<unsigned long> pixels = {
        0x010203UL, 0x0A0B0CUL,
        0x101112UL, 0x1A1B1CUL
    };

    const std::vector<uint8_t> bmp = buildBmp24(2, -2, pixels);
    const std::vector<std::vector<unsigned long>> result = BmpUtils::ParseBmp(bmp);

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(2, result.size(), "Expected 2 rows");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(2, result[0].size(), "Expected 2 columns");

    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x010203UL, result[0][0], "Top-left pixel mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x0A0B0CUL, result[0][1], "Top-right pixel mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x101112UL, result[1][0], "Bottom-left pixel mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x1A1B1CUL, result[1][1], "Bottom-right pixel mismatch");
}

void parseBmpInvalidBitDepthReturnsEmpty()
{
    std::vector<uint8_t> bmp = buildBmp24(1, 1, {0x112233UL});

    // BITMAPINFOHEADER bitCount field offset: 14 + 14 bytes.
    bmp[28] = 32;
    bmp[29] = 0;

    const std::vector<std::vector<unsigned long>> result = BmpUtils::ParseBmp(bmp.data(), bmp.size());

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, result.size(), "Expected parse to fail for non-24-bit BMP");
}

void parseBmpFromByteBufferMapsToTopLeftOrigin()
{
    const std::vector<unsigned long> pixels = {
        0xABCDEFUL, 0x123456UL,
        0x654321UL, 0xFEDCBAUL
    };

    const std::vector<uint8_t> bmp = buildBmp24(2, 2, pixels);
    const std::vector<std::vector<unsigned long>> result = BmpUtils::ParseBmp(bmp.data(), bmp.size());

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(2, result.size(), "Expected 2 rows");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(2, result[0].size(), "Expected 2 columns");

    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0xABCDEFUL, result[0][0], "Top-left pixel mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x123456UL, result[0][1], "Top-right pixel mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x654321UL, result[1][0], "Bottom-left pixel mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0xFEDCBAUL, result[1][1], "Bottom-right pixel mismatch");
}

void parseBmpFromByteBufferNullReturnsEmpty()
{
    const std::vector<std::vector<unsigned long>> result = BmpUtils::ParseBmp(nullptr, 0);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, result.size(), "Expected parse to fail for null buffer");
}

// Copilot generated test file
void parseBmpFromFixtureFile()
{
    std::vector<uint8_t> bmp = readFixtureBytes("test/test_BmpUtils/fixtures/sample_2x2.bmp");
    if (bmp.empty())
    {
        // This fallback helps when running the test binary from a nested build folder.
        bmp = readFixtureBytes("../test/test_BmpUtils/fixtures/sample_2x2.bmp");
    }

    TEST_ASSERT_FALSE_MESSAGE(bmp.empty(), "Failed to read BMP fixture from test/test_BmpUtils/fixtures/sample_2x2.bmp");

    const std::vector<std::vector<unsigned long>> result = BmpUtils::ParseBmp(bmp);

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(2, result.size(), "Expected 2 rows");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(2, result[0].size(), "Expected 2 columns");

    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x112233UL, result[0][0], "Top-left pixel mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x445566UL, result[0][1], "Top-right pixel mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x778899UL, result[1][0], "Bottom-left pixel mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0xAABBCCUL, result[1][1], "Bottom-right pixel mismatch");
}

// 9x16 test bitmap generated by Paint.
void parsePaintGeneratedBitmap()
{
    std::vector<uint8_t> bmp = readFixtureBytes("test/test_BmpUtils/fixtures/Test9x16.bmp");
    if (bmp.empty())
    {
        // This fallback helps when running the test binary from a nested build folder.
        bmp = readFixtureBytes("../test/test_BmpUtils/fixtures/Test9x16.bmp");
    }

    TEST_ASSERT_FALSE_MESSAGE(bmp.empty(), "Failed to read BMP fixture from test/test_BmpUtils/fixtures/sample_2x2.bmp");

    const std::vector<std::vector<unsigned long>> result = BmpUtils::ParseBmp(bmp);

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(9, result.size(), "Expected 9 rows");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(16, result[0].size(), "Expected 16 columns");

    // Verify a few pixels to confirm we parsed the file correctly.
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0xFF0000UL, result[0][0], "Pixel 0,0 mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0xFF0000UL, result[0][2], "Pixel 0,2 mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x000000UL, result[0][3], "Pixel 0,3 mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x000000UL, result[1][0], "Pixel 1,0 mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x00FF00UL, result[4][0], "Pixel 4,0 mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x00FF00UL, result[4][3], "Pixel 4,3 mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x0000FFUL, result[8][0], "Pixel 8,0 mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0x0000FFUL, result[8][5], "Pixel 8,5 mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0xFFFFFFUL, result[8][15], "Pixel 8,15 mismatch");
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(0xFFAEC9UL, result[2][8], "Pixel 2,8 mismatch");
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(parseBmp24BottomUpMapsToTopLeftOrigin);
    RUN_TEST(parseBmp24TopDownRemainsTopLeftOrigin);
    RUN_TEST(parseBmpInvalidBitDepthReturnsEmpty);
    RUN_TEST(parseBmpFromByteBufferMapsToTopLeftOrigin);
    RUN_TEST(parseBmpFromByteBufferNullReturnsEmpty);
    RUN_TEST(parseBmpFromFixtureFile);
    RUN_TEST(parsePaintGeneratedBitmap);

    return UNITY_END();
}
