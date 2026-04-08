#ifndef IMAGEDISPLAYPATTERN_H
#define IMAGEDISPLAYPATTERN_H

#include <Arduino.h>
#include <vector>
#include "DisplayPattern.h"
#include "IO\FileReader.h"
#include "Utils\BmpUtils.h"
#include "Utils\DebugUtils.h"

enum class ImageDisplayShiftType : byte
{
    None = 0,
    Right = 1,
    Left = 2,
    Up = 3,
    Down = 4
};

class ImageDisplayPattern : public DisplayPattern
{
    public:
        ImageDisplayPattern();

        static std::vector<String> getParameterNames();
        void setShiftType(int shiftType);
        void setImageNumber(int imageNumber);

    protected:
        virtual void updateInternal(PixelMap* pixelMap) override;
        virtual void resetInternal(PixelMap* pixelMap) override;

    private:
        std::vector<std::vector<unsigned long>> imageData;
        ImageDisplayShiftType shiftType{ImageDisplayShiftType::None};
        int imageNumber{0};

        void loadImageData(String filename);
        int edgeRowOrCol{0};

    };

#endif