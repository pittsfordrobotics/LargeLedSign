#include "LegacySignPixelBuffer.h"

LegacySignPixelBuffer::LegacySignPixelBuffer(int gpioPin) : PixelBuffer(gpioPin)
{
    m_numPixels = 458;
    m_pixelBufferSize = m_numPixels;
    m_pixelColors = new uint32_t[m_numPixels];

    // Map the pixel indices to rows, columns, and digits.
    // ROW 0 is at the TOP of the display.
    // COLUMN 0 is at the LEFT of the display.
    // DIGIT 0 is at the LEFT of the display.
    // 20 rows
    // 73 columns
    // 4 digits

    //
    // Set up row map
    //
    m_rows.push_back(new std::vector<int>{18,19,20,21,22,23,24,25,124,125,126,127,215,216,217,218,329,330,331,332,367,368,369,370});
    m_rows.push_back(new std::vector<int>{14,15,16,17,26,27,28,29,30,128,129,130,131,132,133,219,220,221,222,223,333,334,335,336,337,371,372,373,374,375,376});
    m_rows.push_back(new std::vector<int>{11,12,13,31,32,33,34,134,135,136,137,138,139,140,224,225,226,227,338,339,340,341,377,378,379,380,381,382,383});
    m_rows.push_back(new std::vector<int>{7,8,9,10,35,36,37,38,141,142,143,144,145,146,147,148,228,229,230,231,342,343,344,345,384,385,386,387,388,389,390,391});
    m_rows.push_back(new std::vector<int>{4,5,6,39,40,41,149,150,151,152,153,154,155,156,232,233,234,346,347,348,392,393,394,395,396,397,398,399});
    m_rows.push_back(new std::vector<int>{0,1,2,3,42,43,44,45,157,158,159,160,235,236,237,238,349,350,351,352,400,401,402,403});
    m_rows.push_back(new std::vector<int>{46,47,48,161,162,163,164,239,240,241,353,354,355,404,405,406,407});
    m_rows.push_back(new std::vector<int>{49,50,51,52,165,166,167,168,280,242,243,244,245,246,356,357,358,359,360,408,409,410,411});
    m_rows.push_back(new std::vector<int>{53,54,55,56,57,169,170,171,247,248,249,250,361,362,363,364,412,413,414});
    m_rows.push_back(new std::vector<int>{58,59,60,61,172,173,174,175,251,252,365,366,415,416,417,418});
    m_rows.push_back(new std::vector<int>{98,99,100,101,176,177,178,179,289,290,327,328,419,420,421,422});
    m_rows.push_back(new std::vector<int>{93,94,95,96,97,180,181,182,183,285,286,287,288,323,324,325,326,423,424,425,426});
    m_rows.push_back(new std::vector<int>{89,90,91,92,184,185,186,187,280,281,282,283,284,318,319,320,321,322,427,428,429,430});
    m_rows.push_back(new std::vector<int>{86,87,88,188,189,190,191,277,278,279,315,316,317,431,432,433,434});
    m_rows.push_back(new std::vector<int>{82,83,84,85,102,103,104,105,192,193,194,195,273,274,275,276,311,312,313,314,435,436,437,438});
    m_rows.push_back(new std::vector<int>{79,80,81,106,107,108,196,197,198,199,270,271,272,308,309,310,439,440,441,442});
    m_rows.push_back(new std::vector<int>{75,76,77,78,109,110,111,112,200,201,202,203,266,267,268,269,304,305,306,307,443,444,445,446});
    m_rows.push_back(new std::vector<int>{71,72,73,74,113,114,115,204,205,206,207,262,263,264,265,300,301,302,303,447,448,449,450});
    m_rows.push_back(new std::vector<int>{66,67,68,69,70,116,117,118,119,208,209,210,257,258,259,260,261,295,296,297,298,299,451,452,453});
    m_rows.push_back(new std::vector<int>{62,63,64,65,120,121,122,123,211,212,213,214,253,254,255,256,291,292,293,294,454,455,456,457});

    //
    // Set up column map
    //
    m_columns.push_back(new std::vector<int>{7,0,102,109});
    m_columns.push_back(new std::vector<int>{11,4,106,113});
    m_columns.push_back(new std::vector<int>{14,8,1,103,110,116});
    m_columns.push_back(new std::vector<int>{18,12,5,107,114,120});
    m_columns.push_back(new std::vector<int>{15,9,2,104,111,117});
    m_columns.push_back(new std::vector<int>{19,13,6,108,115,121});
    m_columns.push_back(new std::vector<int>{16,10,3,105,112,118});
    m_columns.push_back(new std::vector<int>{20,122});
    m_columns.push_back(new std::vector<int>{17,58,98,119});
    m_columns.push_back(new std::vector<int>{21,53,93,123});
    m_columns.push_back(new std::vector<int>{59,99});
    m_columns.push_back(new std::vector<int>{22,54,94,62});
    m_columns.push_back(new std::vector<int>{26,60,100,66});
    m_columns.push_back(new std::vector<int>{23,31,55,95,71,63});
    m_columns.push_back(new std::vector<int>{27,35,42,49,61,101,89,82,75,67});
    m_columns.push_back(new std::vector<int>{24,32,39,46,56,96,86,79,72,64});
    m_columns.push_back(new std::vector<int>{28,36,43,50,90,83,76,68});
    m_columns.push_back(new std::vector<int>{25,33,40,47,57,97,87,80,73,65});
    m_columns.push_back(new std::vector<int>{29,37,44,51,91,84,77,69});
    m_columns.push_back(new std::vector<int>{34,41,48,88,81,74});
    m_columns.push_back(new std::vector<int>{30,38,45,52,92,85,78,70});
    m_columns.push_back(new std::vector<int>{149});
    m_columns.push_back(new std::vector<int>{141});
    m_columns.push_back(new std::vector<int>{134,150});
    m_columns.push_back(new std::vector<int>{142});
    m_columns.push_back(new std::vector<int>{135,151});
    m_columns.push_back(new std::vector<int>{128,143});
    m_columns.push_back(new std::vector<int>{136,152});
    m_columns.push_back(new std::vector<int>{129,144});
    m_columns.push_back(new std::vector<int>{124,137,153,161,176,184,192,200});
    m_columns.push_back(new std::vector<int>{130,145,157,165,172,180,188,196,204,211});
    m_columns.push_back(new std::vector<int>{125,138,154,162,169,177,185,193,201,208});
    m_columns.push_back(new std::vector<int>{131,146,158,166,173,181,189,197,205,212});
    m_columns.push_back(new std::vector<int>{126,139,155,163,170,178,186,194,202,209});
    m_columns.push_back(new std::vector<int>{132,147,159,167,174,182,190,198,206,213});
    m_columns.push_back(new std::vector<int>{127,140,156,164,171,179,187,195,203,210});
    m_columns.push_back(new std::vector<int>{133,148,160,168,175,183,191,199,207,214});
    m_columns.push_back(new std::vector<int>{261,269,276,238,231,223});
    m_columns.push_back(new std::vector<int>{265,272,279,284,246,241,234,227});
    m_columns.push_back(new std::vector<int>{260,268,275,283,245,237,230,222});
    m_columns.push_back(new std::vector<int>{256,264,271,278,288,250,240,233,226,218});
    m_columns.push_back(new std::vector<int>{259,267,274,282,244,236,229,221});
    m_columns.push_back(new std::vector<int>{255,263,270,277,287,249,239,232,225,217});
    m_columns.push_back(new std::vector<int>{258,266,273,281,290,252,243,235,228,220});
    m_columns.push_back(new std::vector<int>{254,262,286,248,224,216});
    m_columns.push_back(new std::vector<int>{257,280,289,251,242,219});
    m_columns.push_back(new std::vector<int>{253,285,247,215});
    m_columns.push_back(new std::vector<int>{291,323,361,329});
    m_columns.push_back(new std::vector<int>{295,318,327,365,356,333});
    m_columns.push_back(new std::vector<int>{292,300,324,362,338,330});
    m_columns.push_back(new std::vector<int>{296,304,311,319,328,366,357,349,342,334});
    m_columns.push_back(new std::vector<int>{293,301,308,315,325,363,353,346,339,331});
    m_columns.push_back(new std::vector<int>{297,305,312,320,358,350,343,335});
    m_columns.push_back(new std::vector<int>{294,302,309,316,326,364,354,347,340,332});
    m_columns.push_back(new std::vector<int>{298,306,313,321,359,351,344,336});
    m_columns.push_back(new std::vector<int>{303,310,317,355,348,341});
    m_columns.push_back(new std::vector<int>{299,307,314,322,360,352,345,337});
    m_columns.push_back(new std::vector<int>{392});
    m_columns.push_back(new std::vector<int>{384});
    m_columns.push_back(new std::vector<int>{377,393});
    m_columns.push_back(new std::vector<int>{385});
    m_columns.push_back(new std::vector<int>{378,394});
    m_columns.push_back(new std::vector<int>{371,386});
    m_columns.push_back(new std::vector<int>{379,395});
    m_columns.push_back(new std::vector<int>{372,387});
    m_columns.push_back(new std::vector<int>{367,380,396,404,419,427,435,443});
    m_columns.push_back(new std::vector<int>{373,388,400,408,415,423,431,439,447,454});
    m_columns.push_back(new std::vector<int>{368,381,397,405,412,420,428,436,444,451});
    m_columns.push_back(new std::vector<int>{374,389,401,409,416,424,432,440,448,455});
    m_columns.push_back(new std::vector<int>{369,382,398,406,413,421,429,437,445,452});
    m_columns.push_back(new std::vector<int>{375,390,402,410,417,425,433,441,449,456});
    m_columns.push_back(new std::vector<int>{370,383,399,407,414,422,430,438,446,453});
    m_columns.push_back(new std::vector<int>{376,391,403,411,418,426,434,442,450,457});

    //
    // Set up digit map
    //
    // Pixels for "3"
    std::vector<int>* digit0 = new std::vector<int>();
    for (int i = 0; i < 124; i++) {
        digit0->push_back(i);
    }

    // Pixels for first "1"
    std::vector<int>* digit1 = new std::vector<int>();
    for (int i = 124; i < 215; i++) {
        digit1->push_back(i);
    }

    // Pixels for "8"
    std::vector<int>* digit2 = new std::vector<int>();
    for (int i = 215; i < 367; i++) {
        digit2->push_back(i);
    }

    // Pixels for second "1"
    std::vector<int>* digit3 = new std::vector<int>();
    for (int i = 367; i < 458; i++) {
        digit3->push_back(i);
    }

    m_digits.push_back(digit0);
    m_digits.push_back(digit1);
    m_digits.push_back(digit2);
    m_digits.push_back(digit3);    
}