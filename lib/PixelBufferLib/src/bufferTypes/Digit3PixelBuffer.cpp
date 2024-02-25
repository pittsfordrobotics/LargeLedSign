#include "Digit3PixelBuffer.h"

Digit3PixelBuffer::Digit3PixelBuffer(int gpioPin) : PixelBuffer(gpioPin)
{
    // 313 actual pixels
    // 23 columns, 39 rows
    m_numPixels = 313;
    m_pixelBufferSize = std::max((uint) PB_MINIMUM_PIXELS, m_numPixels);
    m_pixelColors = new uint32_t[m_pixelBufferSize];

    m_columns.push_back(new std::vector<int>{ 0, 9, 18, 27, 36, 253, 262, 271, 280, 289});
    m_columns.push_back(new std::vector<int>{ 5, 14, 23, 32, 42, 258, 267, 276, 285, 295});
    m_columns.push_back(new std::vector<int>{ 1, 10, 19, 28, 37, 47, 254, 263, 272, 281, 290, 300});
    m_columns.push_back(new std::vector<int>{ 6, 15, 24, 33, 43, 52, 259, 268, 277, 286, 296, 305});
    m_columns.push_back(new std::vector<int>{ 2, 11, 20, 29, 38, 48, 56, 255, 264, 273, 282, 291, 301, 309});
    m_columns.push_back(new std::vector<int>{ 7, 16, 25, 34, 44, 53, 260, 269, 278, 287, 297, 306});
    m_columns.push_back(new std::vector<int>{ 3, 12, 21, 30, 39, 49, 57, 256, 265, 274, 283, 292, 302, 310});
    m_columns.push_back(new std::vector<int>{ 8, 17, 26, 35, 45, 54, 141, 154, 165, 261, 270, 279, 288, 298, 307});
    m_columns.push_back(new std::vector<int>{ 4, 13, 22, 31, 40, 50, 58, 148, 159, 257, 266, 275, 284, 293, 303, 311});
    m_columns.push_back(new std::vector<int>{ 46, 55, 142, 155, 166, 299, 308});
    m_columns.push_back(new std::vector<int>{ 41, 51, 59, 149, 160, 294, 304, 312});
    m_columns.push_back(new std::vector<int>{ 60, 61, 143, 156, 167, 251, 252});
    m_columns.push_back(new std::vector<int>{ 103, 113, 121, 136, 150, 161, 172, 232, 242, 250});
    m_columns.push_back(new std::vector<int>{ 108, 117, 131, 144, 157, 168, 177, 237, 246});
    m_columns.push_back(new std::vector<int>{ 66, 75, 84, 93, 102, 112, 120, 126, 137, 151, 162, 173, 182, 195, 204, 213, 222, 231, 241, 249});
    m_columns.push_back(new std::vector<int>{ 70, 79, 88, 97, 107, 116, 122, 132, 145, 158, 169, 178, 187, 199, 208, 217, 226, 236, 245});
    m_columns.push_back(new std::vector<int>{ 65, 74, 83, 92, 101, 111, 119, 127, 138, 152, 163, 174, 183, 194, 203, 212, 221, 230, 240, 248});
    m_columns.push_back(new std::vector<int>{ 69, 78, 87, 96, 106, 115, 123, 133, 146, 170, 179, 188, 198, 207, 216, 225, 235, 244});
    m_columns.push_back(new std::vector<int>{ 64, 73, 82, 91, 100, 110, 118, 128, 139, 153, 164, 175, 184, 193, 202, 211, 220, 229, 239, 247});
    m_columns.push_back(new std::vector<int>{ 68, 77, 86, 95, 105, 114, 124, 134, 147, 171, 180, 189, 197, 206, 215, 224, 234, 243});
    m_columns.push_back(new std::vector<int>{ 63, 72, 81, 90, 99, 109, 129, 140, 176, 185, 192, 201, 210, 219, 228, 238});
    m_columns.push_back(new std::vector<int>{ 67, 76, 85, 94, 104, 125, 135, 181, 190, 196, 205, 214, 223, 233});
    m_columns.push_back(new std::vector<int>{ 62, 71, 80, 89, 98, 130, 186, 191, 200, 209, 218, 227});

    m_rows.push_back(new std::vector<int>{ 247, 248, 249, 250, 309, 310, 311, 312});
    m_rows.push_back(new std::vector<int>{ 243, 244, 245, 246, 251, 305, 306, 307, 308});
    m_rows.push_back(new std::vector<int>{ 238, 239, 240, 241, 242, 300, 301, 302, 303, 304});
    m_rows.push_back(new std::vector<int>{ 233, 234, 235, 236, 237, 252, 295, 296, 297, 298, 299});
    m_rows.push_back(new std::vector<int>{ 227, 228, 229, 230, 231, 232, 289, 290, 291, 292, 293, 294});
    m_rows.push_back(new std::vector<int>{ 223, 224, 225, 226, 285, 286, 287, 288});
    m_rows.push_back(new std::vector<int>{ 218, 219, 220, 221, 222, 280, 281, 282, 283, 284});
    m_rows.push_back(new std::vector<int>{ 214, 215, 216, 217, 276, 277, 278, 279});
    m_rows.push_back(new std::vector<int>{ 209, 210, 211, 212, 213, 271, 272, 273, 274, 275});
    m_rows.push_back(new std::vector<int>{ 205, 206, 207, 208, 267, 268, 269, 270});
    m_rows.push_back(new std::vector<int>{ 200, 201, 202, 203, 204, 262, 263, 264, 265, 266});
    m_rows.push_back(new std::vector<int>{ 196, 197, 198, 199, 258, 259, 260, 261});
    m_rows.push_back(new std::vector<int>{ 191, 192, 193, 194, 195, 253, 254, 255, 256, 257});
    m_rows.push_back(new std::vector<int>{ 122, 123, 124, 125});
    m_rows.push_back(new std::vector<int>{ 126, 127, 128, 129, 130});
    m_rows.push_back(new std::vector<int>{ 131, 132, 133, 134, 135});
    m_rows.push_back(new std::vector<int>{ 136, 137, 138, 139, 140});
    m_rows.push_back(new std::vector<int>{ 141, 142, 143, 144, 145, 146, 147});
    m_rows.push_back(new std::vector<int>{ 148, 149, 150, 151, 152, 153});
    m_rows.push_back(new std::vector<int>{ 154, 155, 156, 157, 158});
    m_rows.push_back(new std::vector<int>{ 159, 160, 161, 162, 163, 164});
    m_rows.push_back(new std::vector<int>{ 165, 166, 167, 168, 169, 170, 171});
    m_rows.push_back(new std::vector<int>{ 172, 173, 174, 175, 176});
    m_rows.push_back(new std::vector<int>{ 177, 178, 179, 180, 181});
    m_rows.push_back(new std::vector<int>{ 182, 183, 184, 185, 186});
    m_rows.push_back(new std::vector<int>{ 187, 188, 189, 190});
    m_rows.push_back(new std::vector<int>{ 0, 1, 2, 3, 4, 62, 63, 64, 65, 66});
    m_rows.push_back(new std::vector<int>{ 5, 6, 7, 8, 67, 68, 69, 70});
    m_rows.push_back(new std::vector<int>{ 9, 10, 11, 12, 13, 71, 72, 73, 74, 75});
    m_rows.push_back(new std::vector<int>{ 14, 15, 16, 17, 76, 77, 78, 79});
    m_rows.push_back(new std::vector<int>{ 18, 19, 20, 21, 22, 80, 81, 82, 83, 84});
    m_rows.push_back(new std::vector<int>{ 23, 24, 25, 26, 85, 86, 87, 88});
    m_rows.push_back(new std::vector<int>{ 27, 28, 29, 30, 31, 89, 90, 91, 92, 93});
    m_rows.push_back(new std::vector<int>{ 32, 33, 34, 35, 94, 95, 96, 97});
    m_rows.push_back(new std::vector<int>{ 36, 37, 38, 39, 40, 41, 98, 99, 100, 101, 102, 103});
    m_rows.push_back(new std::vector<int>{ 42, 43, 44, 45, 46, 61, 104, 105, 106, 107, 108});
    m_rows.push_back(new std::vector<int>{ 47, 48, 49, 50, 51, 109, 110, 111, 112, 113});
    m_rows.push_back(new std::vector<int>{ 52, 53, 54, 55, 60, 114, 115, 116, 117});
    m_rows.push_back(new std::vector<int>{ 56, 57, 58, 59, 118, 119, 120, 121});

    m_digits.push_back(new std::vector<int>());
    
    for (uint i = 0; i < m_numPixels; i++)
    {
        m_digits[0]->push_back(i);
    }
}