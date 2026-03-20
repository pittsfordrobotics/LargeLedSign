#ifndef FILE_READER_H
#define FILE_READER_H

#include <Arduino.h>
#include <SD.h>
#include "BuiltInFiles.h"
#include "Utils\StringUtils.h"

/*
Valid GPIO pins for SPI0:
CLK: 2, 6, 18, 22
CIPO: 0, 4, 16, 20
COPI: 3, 7, 19, 23
 */
#define SDCARD_CHIPSELECT 18
#define SDCARD_SPI_CLOCK   6
#define SDCARD_SPI_COPI    7
#define SDCARD_SPI_CIPO    4

// The legacy sign uses the default COPI pin for the Status Display CLK signal.
#define SDCARD_ALT_CHIPSELECT 20
#define SDCARD_ALT_SPI_CLOCK  18
#define SDCARD_ALT_SPI_COPI   19
#define SDCARD_ALT_SPI_CIPO    4

#define SYSTEM_CONFIG_FILE "systemconfiguration.json"
#define BUILT_IN_FILE_INDICATOR "::"

class FileReader
{
    public:
        FileReader();
        const char* getSystemConfig() { return getFileContents(SYSTEM_CONFIG_FILE); };
        const char* getFileContents(String filename);

    private:
        bool isInitialized{false};
        bool isSdCardDetected{true};
        int chipSelectPin{SDCARD_CHIPSELECT};

        void initialize();
        bool isBuiltInFile(String filename);
        const char* readBuiltInFile(String filename);
};

#endif // FILE_READER_H