#include "Digit8PixelBuffer.h"

Digit8PixelBuffer::Digit8PixelBuffer(int gpioPin) : PixelBuffer(gpioPin)
{
    // 356 actual pixels
    // 23 columns, 39 rows
    m_numPixels = 355;
    m_pixelBufferSize = std::max((uint) PB_MINIMUM_PIXELS, m_numPixels);
    m_pixelColors = new uint32_t[m_pixelBufferSize];

    m_columns.push_back(new std::vector<int>{ 0, 9, 18, 27, 36, 248, 257, 266, 275, 307, 350});
    m_columns.push_back(new std::vector<int>{ 5, 14, 23, 32, 42, 244, 253, 262, 271, 281, 302, 312, 345, 354});
    m_columns.push_back(new std::vector<int>{ 1, 10, 19, 28, 37, 47, 240, 249, 258, 267, 276, 286, 306, 317, 340, 349});
    m_columns.push_back(new std::vector<int>{ 6, 15, 24, 33, 43, 52, 245, 254, 263, 272, 282, 291, 301, 311, 321, 335, 344, 353});
    m_columns.push_back(new std::vector<int>{ 2, 11, 20, 29, 38, 48, 56, 241, 250, 259, 268, 277, 287, 295, 305, 316, 325, 331, 339, 348});
    m_columns.push_back(new std::vector<int>{ 7, 16, 25, 34, 44, 53, 246, 255, 264, 273, 283, 292, 300, 310, 320, 334, 343, 352});
    m_columns.push_back(new std::vector<int>{ 3, 12, 21, 30, 39, 49, 57, 242, 251, 260, 269, 278, 288, 296, 304, 315, 324, 330, 338, 347});
    m_columns.push_back(new std::vector<int>{ 8, 17, 26, 35, 45, 54, 247, 256, 265, 274, 284, 293, 299, 309, 319, 327, 333, 342, 351});
    m_columns.push_back(new std::vector<int>{ 4, 13, 22, 31, 40, 50, 58, 243, 252, 261, 270, 279, 289, 297, 303, 314, 323, 329, 337, 346});
    m_columns.push_back(new std::vector<int>{ 46, 55, 285, 294, 308, 318, 326, 332, 341});
    m_columns.push_back(new std::vector<int>{ 41, 51, 59, 280, 290, 298, 313, 322, 328, 336});
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

    m_rows.push_back(new std::vector<int>{ 234, 235, 236, 237, 240, 241, 242, 243});
    m_rows.push_back(new std::vector<int>{ 230, 231, 232, 233, 238, 244, 245, 246, 247});
    m_rows.push_back(new std::vector<int>{ 225, 226, 227, 228, 229, 248, 249, 250, 251, 252});
    m_rows.push_back(new std::vector<int>{ 220, 221, 222, 223, 224, 239, 253, 254, 255, 256});
    m_rows.push_back(new std::vector<int>{ 214, 215, 216, 217, 218, 219, 257, 258, 259, 260, 261});
    m_rows.push_back(new std::vector<int>{ 210, 211, 212, 213, 262, 263, 264, 265});
    m_rows.push_back(new std::vector<int>{ 205, 206, 207, 208, 209, 266, 267, 268, 269, 270});
    m_rows.push_back(new std::vector<int>{ 201, 202, 203, 204, 271, 272, 273, 274});
    m_rows.push_back(new std::vector<int>{ 196, 197, 198, 199, 200, 275, 276, 277, 278, 279, 280});
    m_rows.push_back(new std::vector<int>{ 192, 193, 194, 195, 281, 282, 283, 284, 285});
    m_rows.push_back(new std::vector<int>{ 187, 188, 189, 190, 191, 286, 287, 288, 289, 290});
    m_rows.push_back(new std::vector<int>{ 183, 184, 185, 186, 291, 292, 293, 294});
    m_rows.push_back(new std::vector<int>{ 178, 179, 180, 181, 182, 295, 296, 297, 298});
    m_rows.push_back(new std::vector<int>{ 122, 123, 124, 125, 299, 300, 301, 302});
    m_rows.push_back(new std::vector<int>{ 126, 127, 128, 129, 130, 303, 304, 305, 306, 307});
    m_rows.push_back(new std::vector<int>{ 131, 132, 133, 134, 135, 308, 309, 310, 311, 312});
    m_rows.push_back(new std::vector<int>{ 136, 137, 138, 139, 140, 313, 314, 315, 316, 317});
    m_rows.push_back(new std::vector<int>{ 141, 142, 143, 144, 318, 319, 320, 321});
    m_rows.push_back(new std::vector<int>{ 145, 146, 147, 148, 322, 323, 324, 325});
    m_rows.push_back(new std::vector<int>{ 149, 150, 326, 327});
    m_rows.push_back(new std::vector<int>{ 151, 152, 153, 154, 328, 329, 330, 331});
    m_rows.push_back(new std::vector<int>{ 155, 156, 157, 158, 332, 333, 334, 335});
    m_rows.push_back(new std::vector<int>{ 159, 160, 161, 162, 163, 336, 337, 338, 339, 340});
    m_rows.push_back(new std::vector<int>{ 164, 165, 166, 167, 168, 341, 342, 343, 344, 345});
    m_rows.push_back(new std::vector<int>{ 169, 170, 171, 172, 173, 346, 347, 348, 349, 350});
    m_rows.push_back(new std::vector<int>{ 174, 175, 176, 177, 351, 352, 353, 354});
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