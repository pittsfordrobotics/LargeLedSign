#include "PitSignPixelBuffer.h"

PitSignPixelBuffer::PitSignPixelBuffer(int gpioPin) : PixelBuffer(gpioPin)
{
    // 272 actual pixels
    // 54 total columns, 17 rows
    m_numPixels = 272;
    m_pixelBufferSize = m_numPixels;
    m_pixelColors = new uint32_t[m_pixelBufferSize];

    m_columns.push_back(new std::vector<int>{3, 75});
    m_columns.push_back(new std::vector<int>{0, 6, 72, 78});
    m_columns.push_back(new std::vector<int>{4, 9, 69, 76});
    m_columns.push_back(new std::vector<int>{1, 7, 12, 66, 73, 79});
    m_columns.push_back(new std::vector<int>{5, 10, 70, 77});
    m_columns.push_back(new std::vector<int>{2, 8, 13, 67, 74, 80});
    m_columns.push_back(new std::vector<int>{11, 39, 71});
    m_columns.push_back(new std::vector<int>{14, 68});
    m_columns.push_back(new std::vector<int>{40});
    m_columns.push_back(new std::vector<int>{15, 63});
    m_columns.push_back(new std::vector<int>{18, 36, 41, 42, 60});
    m_columns.push_back(new std::vector<int>{16, 21, 27, 33, 45, 51, 57, 64});
    m_columns.push_back(new std::vector<int>{19, 24, 30, 37, 43, 48, 54, 61});
    m_columns.push_back(new std::vector<int>{17, 22, 28, 34, 46, 52, 58, 65});
    m_columns.push_back(new std::vector<int>{20, 25, 31, 38, 44, 49, 55, 62});
    m_columns.push_back(new std::vector<int>{23, 29, 35, 47, 53, 59});
    m_columns.push_back(new std::vector<int>{26, 32, 50, 56});
    m_columns.push_back(new std::vector<int>{114});
    m_columns.push_back(new std::vector<int>{115});
    m_columns.push_back(new std::vector<int>{120});
    m_columns.push_back(new std::vector<int>{116});
    m_columns.push_back(new std::vector<int>{121});
    m_columns.push_back(new std::vector<int>{117, 125});
    m_columns.push_back(new std::vector<int>{81, 86, 91, 96, 101, 106, 111, 122, 128});
    m_columns.push_back(new std::vector<int>{84, 89, 94, 99, 104, 109, 118, 126});
    m_columns.push_back(new std::vector<int>{82, 87, 92, 97, 102, 107, 112, 123, 129});
    m_columns.push_back(new std::vector<int>{85, 90, 95, 100, 105, 110, 119, 127});
    m_columns.push_back(new std::vector<int>{83, 88, 93, 98, 103, 108, 113, 124, 130});
    m_columns.push_back(new std::vector<int>{133, 139, 144, 161, 166, 171});
    m_columns.push_back(new std::vector<int>{137, 142, 147, 158, 164, 169});
    m_columns.push_back(new std::vector<int>{134, 140, 145, 150, 155, 162, 167, 172});
    m_columns.push_back(new std::vector<int>{131, 138, 143, 148, 159, 165, 170, 175});
    m_columns.push_back(new std::vector<int>{135, 141, 146, 151, 156, 163, 168, 173});
    m_columns.push_back(new std::vector<int>{149, 153, 160});
    m_columns.push_back(new std::vector<int>{132, 136, 152, 157, 174, 176});
    m_columns.push_back(new std::vector<int>{154});
    m_columns.push_back(new std::vector<int>{177, 179, 196, 200, 216, 221});
    m_columns.push_back(new std::vector<int>{193, 199, 203});
    m_columns.push_back(new std::vector<int>{180, 185, 190, 197, 201, 206, 211, 217});
    m_columns.push_back(new std::vector<int>{178, 183, 188, 194, 204, 209, 214, 220});
    m_columns.push_back(new std::vector<int>{181, 186, 191, 198, 202, 207, 212, 218});
    m_columns.push_back(new std::vector<int>{184, 189, 195, 205, 210, 215});
    m_columns.push_back(new std::vector<int>{182, 187, 192, 208, 213, 219});
    m_columns.push_back(new std::vector<int>{255});
    m_columns.push_back(new std::vector<int>{256});
    m_columns.push_back(new std::vector<int>{261});
    m_columns.push_back(new std::vector<int>{257});
    m_columns.push_back(new std::vector<int>{262});
    m_columns.push_back(new std::vector<int>{258, 266});
    m_columns.push_back(new std::vector<int>{222, 227, 232, 237, 242, 247, 252, 263, 269});
    m_columns.push_back(new std::vector<int>{225, 230, 235, 240, 245, 250, 259, 267});
    m_columns.push_back(new std::vector<int>{223, 228, 233, 238, 243, 248, 253, 264, 270});
    m_columns.push_back(new std::vector<int>{226, 231, 236, 241, 246, 251, 260, 268});
    m_columns.push_back(new std::vector<int>{224, 229, 234, 239, 244, 249, 254, 265, 271});

    m_rows.push_back(new std::vector<int>{63, 64, 65, 66, 67, 68, 128, 129, 130, 131, 132, 220, 221, 269, 270, 271});
    m_rows.push_back(new std::vector<int>{60, 61, 62, 69, 70, 71, 125, 126, 127, 133, 134, 135, 136, 216, 217, 218, 219, 266, 267, 268});
    m_rows.push_back(new std::vector<int>{57, 58, 59, 72, 73, 74, 120, 121, 122, 123, 124, 137, 138, 214, 215, 261, 262, 263, 264, 265});
    m_rows.push_back(new std::vector<int>{54, 55, 56, 75, 76, 77, 114, 115, 116, 117, 118, 119, 139, 140, 141, 211, 212, 213, 255, 256, 257, 258, 259, 260});
    m_rows.push_back(new std::vector<int>{51, 52, 53, 78, 79, 80, 111, 112, 113, 142, 143, 209, 210, 252, 253, 254});
    m_rows.push_back(new std::vector<int>{48, 49, 50, 109, 110, 144, 145, 146, 206, 207, 208, 250, 251});
    m_rows.push_back(new std::vector<int>{45, 46, 47, 106, 107, 108, 147, 148, 149, 203, 204, 205, 247, 248, 249});
    m_rows.push_back(new std::vector<int>{42, 43, 44, 104, 105, 150, 151, 152, 200, 201, 202, 245, 246});
    m_rows.push_back(new std::vector<int>{39, 40, 41, 101, 102, 103, 153, 154, 199, 242, 243, 244});
    m_rows.push_back(new std::vector<int>{36, 37, 38, 99, 100, 155, 156, 157, 196, 197, 198, 240, 241});
    m_rows.push_back(new std::vector<int>{33, 34, 35, 96, 97, 98, 158, 159, 160, 193, 194, 195, 237, 238, 239});
    m_rows.push_back(new std::vector<int>{30, 31, 32, 94, 95, 161, 162, 163, 190, 191, 192, 235, 236});
    m_rows.push_back(new std::vector<int>{0, 1, 2, 27, 28, 29, 91, 92, 93, 164, 165, 188, 189, 232, 233, 234});
    m_rows.push_back(new std::vector<int>{3, 4, 5, 24, 25, 26, 89, 90, 166, 167, 168, 185, 186, 187, 230, 231});
    m_rows.push_back(new std::vector<int>{6, 7, 8, 21, 22, 23, 86, 87, 88, 169, 170, 183, 184, 227, 228, 229});
    m_rows.push_back(new std::vector<int>{9, 10, 11, 18, 19, 20, 84, 85, 171, 172, 173, 174, 179, 180, 181, 182, 225, 226});
    m_rows.push_back(new std::vector<int>{12, 13, 14, 15, 16, 17, 81, 82, 83, 175, 176, 177, 178, 222, 223, 224});

    // Pixels for "3"
    std::vector<int> *digit0 = new std::vector<int>();
    for (int i = 0; i < 81; i++)
    {
        digit0->push_back(i);
    }

    // Pixels for first "1"
    std::vector<int> *digit1 = new std::vector<int>();
    for (int i = 81; i < 131; i++)
    {
        digit1->push_back(i);
    }

    // Pixels for "8"
    std::vector<int> *digit2 = new std::vector<int>();
    for (int i = 131; i < 222; i++)
    {
        digit2->push_back(i);
    }

    // Pixels for second "1"
    std::vector<int> *digit3 = new std::vector<int>();
    for (int i = 222; i < 272; i++)
    {
        digit3->push_back(i);
    }

    m_digits.push_back(digit0);
    m_digits.push_back(digit1);
    m_digits.push_back(digit2);
    m_digits.push_back(digit3);
}