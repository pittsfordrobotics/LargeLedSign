#include "Digit8PixelBuffer.h"

Digit8PixelBuffer::Digit8PixelBuffer(int gpioPin) : PixelBuffer(gpioPin)
{
    // 356 actual pixels
    // 23 columns, 39 rows
    m_numPixels = 356;
    m_pixelBufferSize = std::max((uint) PB_MINIMUM_PIXELS, m_numPixels);
    m_pixelColors = new uint32_t[m_pixelBufferSize];

    m_columns.push_back(new std::vector<int>{ 0, 9, 18, 27, 36, 240, 249, 258, 267, 276, 308, 351});
    m_columns.push_back(new std::vector<int>{ 5, 14, 23, 32, 42, 245, 254, 263, 272, 282, 303, 313, 346, 355});
    m_columns.push_back(new std::vector<int>{ 1, 10, 19, 28, 37, 47, 241, 250, 259, 268, 277, 287, 307, 318, 341, 350});
    m_columns.push_back(new std::vector<int>{ 6, 15, 24, 33, 43, 52, 246, 255, 264, 273, 283, 292, 302, 312, 322, 336, 345, 354});
    m_columns.push_back(new std::vector<int>{ 2, 11, 20, 29, 38, 48, 56, 242, 251, 260, 269, 278, 288, 296, 306, 317, 326, 332, 340, 349});
    m_columns.push_back(new std::vector<int>{ 7, 16, 25, 34, 44, 53, 247, 256, 265, 274, 284, 293, 301, 311, 321, 335, 344, 353});
    m_columns.push_back(new std::vector<int>{ 3, 12, 21, 30, 39, 49, 57, 243, 252, 261, 270, 279, 289, 297, 305, 316, 325, 331, 339, 348});
    m_columns.push_back(new std::vector<int>{ 8, 17, 26, 35, 45, 54, 248, 257, 266, 275, 285, 294, 300, 310, 320, 328, 334, 343, 352});
    m_columns.push_back(new std::vector<int>{ 4, 13, 22, 31, 40, 50, 58, 244, 253, 262, 271, 280, 290, 298, 304, 315, 324, 330, 338, 347});
    m_columns.push_back(new std::vector<int>{ 46, 55, 286, 295, 309, 319, 327, 333, 342});
    m_columns.push_back(new std::vector<int>{ 41, 51, 59, 281, 291, 299, 314, 323, 329, 337});
    m_columns.push_back(new std::vector<int>{ 60, 61, 238, 239});
    m_columns.push_back(new std::vector<int>{ 103, 113, 121, 136, 145, 151, 159, 219, 229, 237});
    m_columns.push_back(new std::vector<int>{ 108, 117, 131, 141, 149, 155, 164, 224, 233});
    m_columns.push_back(new std::vector<int>{ 66, 75, 84, 93, 102, 112, 120, 126, 137, 146, 152, 160, 169, 182, 191, 200, 209, 218, 228, 236});
    m_columns.push_back(new std::vector<int>{ 70, 79, 88, 97, 107, 116, 122, 132, 142, 150, 156, 165, 174, 186, 195, 204, 213, 223, 232});
    m_columns.push_back(new std::vector<int>{ 65, 74, 83, 92, 101, 111, 119, 127, 138, 147, 153, 161, 170, 181, 190, 199, 208, 217, 227, 235});
    m_columns.push_back(new std::vector<int>{ 69, 78, 87, 96, 106, 115, 123, 133, 143, 157, 166, 175, 185, 194, 203, 212, 222, 231});
    m_columns.push_back(new std::vector<int>{ 64, 73, 82, 91, 100, 110, 118, 128, 139, 148, 154, 162, 171, 180, 189, 198, 207, 216, 226, 234});
    m_columns.push_back(new std::vector<int>{ 68, 77, 86, 95, 105, 114, 124, 134, 144, 158, 167, 176, 184, 193, 202, 211, 221, 230});
    m_columns.push_back(new std::vector<int>{ 63, 72, 81, 90, 99, 109, 129, 140, 163, 172, 179, 188, 197, 206, 215, 225});
    m_columns.push_back(new std::vector<int>{ 67, 76, 85, 94, 104, 125, 135, 168, 177, 183, 192, 201, 210, 220});
    m_columns.push_back(new std::vector<int>{ 62, 71, 80, 89, 98, 130, 173, 178, 187, 196, 205, 214});

    m_rows.push_back(new std::vector<int>{ 234, 235, 236, 237, 296, 297, 298, 299});
    m_rows.push_back(new std::vector<int>{ 230, 231, 232, 233, 238, 292, 293, 294, 295});
    m_rows.push_back(new std::vector<int>{ 225, 226, 227, 228, 229, 287, 288, 289, 290, 291});
    m_rows.push_back(new std::vector<int>{ 220, 221, 222, 223, 224, 239, 282, 283, 284, 285, 286});
    m_rows.push_back(new std::vector<int>{ 214, 215, 216, 217, 218, 219, 276, 277, 278, 279, 280, 281});
    m_rows.push_back(new std::vector<int>{ 210, 211, 212, 213, 272, 273, 274, 275});
    m_rows.push_back(new std::vector<int>{ 205, 206, 207, 208, 209, 267, 268, 269, 270, 271});
    m_rows.push_back(new std::vector<int>{ 201, 202, 203, 204, 263, 264, 265, 266});
    m_rows.push_back(new std::vector<int>{ 196, 197, 198, 199, 200, 258, 259, 260, 261, 262});
    m_rows.push_back(new std::vector<int>{ 192, 193, 194, 195, 254, 255, 256, 257});
    m_rows.push_back(new std::vector<int>{ 187, 188, 189, 190, 191, 249, 250, 251, 252, 253});
    m_rows.push_back(new std::vector<int>{ 183, 184, 185, 186, 245, 246, 247, 248});
    m_rows.push_back(new std::vector<int>{ 178, 179, 180, 181, 182, 240, 241, 242, 243, 244});
    m_rows.push_back(new std::vector<int>{ 122, 123, 124, 125, 352, 353, 354, 355});
    m_rows.push_back(new std::vector<int>{ 126, 127, 128, 129, 130, 347, 348, 349, 350, 351});
    m_rows.push_back(new std::vector<int>{ 131, 132, 133, 134, 135, 342, 343, 344, 345, 346});
    m_rows.push_back(new std::vector<int>{ 136, 137, 138, 139, 140, 337, 338, 339, 340, 341});
    m_rows.push_back(new std::vector<int>{ 141, 142, 143, 144, 333, 334, 335, 336});
    m_rows.push_back(new std::vector<int>{ 145, 146, 147, 148, 329, 330, 331, 332});
    m_rows.push_back(new std::vector<int>{ 149, 150, 327, 328});
    m_rows.push_back(new std::vector<int>{ 151, 152, 153, 154, 323, 324, 325, 326});
    m_rows.push_back(new std::vector<int>{ 155, 156, 157, 158, 319, 320, 321, 322});
    m_rows.push_back(new std::vector<int>{ 159, 160, 161, 162, 163, 314, 315, 316, 317, 318});
    m_rows.push_back(new std::vector<int>{ 164, 165, 166, 167, 168, 309, 310, 311, 312, 313});
    m_rows.push_back(new std::vector<int>{ 169, 170, 171, 172, 173, 304, 305, 306, 307, 308});
    m_rows.push_back(new std::vector<int>{ 174, 175, 176, 177, 300, 301, 302, 303});
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