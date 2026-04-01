#ifndef IMAGEDISPLAYPATTERN_H
#define IMAGEDISPLAYPATTERN_H

#include <Arduino.h>
#include <vector>
#include "DisplayPattern.h"
#include "IO\BmpUtils.h"
#include "IO\FileReader.h"
#include "Utils\DebugUtils.h"

class ImageDisplayPattern : public DisplayPattern
{
    public:
        ImageDisplayPattern();

        static std::vector<String> getParameterNames();

    protected:
        virtual void updateInternal(PixelMap* pixelMap) override;
        virtual void resetInternal(PixelMap* pixelMap) override;

    };

#endif