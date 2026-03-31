#include "FileReader.h"

FileReader::FileReader()
{
}

const char* FileReader::getFileContents(String filename) 
{
    if (isBuiltInFile(filename))
    {
        Serial.println("Reading built-in file: " + filename);
        return readBuiltInFile(filename);
    }

    initialize();

    if (!isSdCardDetected) {
        Serial.println("SD card not detected. Cannot read file: " + filename);
        return nullptr;
    }

    if (!SD.begin(chipSelectPin))
    {
        Serial.println("File reader was initialized but SD.begin failed. Cannot read file: " + filename);
        return nullptr;
    }

    if (!SD.exists(filename))
    {
        Serial.println("File does not exist on SD card: " + filename);
        SD.end();
        return nullptr;
    }

    Serial.println("Reading file from SD card: " + filename);
    File file = SD.open(filename);

    if (!file) 
    {
        Serial.println("Failed to open file: " + filename);
        SD.end();
        return nullptr;
    }

    size_t fileSize = file.size();
    char* fileContents = new char[fileSize + 1];
    file.readBytes(fileContents, fileSize);
    fileContents[fileSize] = '\0';  // Add null terminator
    Serial.println("File read successfully.");

    file.close();
    SD.end();
    
    return fileContents;
}

std::vector<uint8_t> FileReader::getFileBytes(String filename)
{
    initialize();

    if (!isSdCardDetected) {
        Serial.println("SD card not detected. Cannot read file: " + filename);
        return std::vector<uint8_t>();
    }

    if (!SD.begin(chipSelectPin))
    {
        Serial.println("File reader was initialized but SD.begin failed. Cannot read file: " + filename);
        return std::vector<uint8_t>();
    }

    if (!SD.exists(filename))
    {
        Serial.println("File does not exist on SD card: " + filename);
        SD.end();
        return std::vector<uint8_t>();
    }

    Serial.println("Reading file from SD card as bytes: " + filename);
    File file = SD.open(filename);

    if (!file) 
    {
        Serial.println("Failed to open file: " + filename);
        SD.end();
        return std::vector<uint8_t>();
    }

    size_t fileSize = file.size();
    std::vector<uint8_t> fileBytes(fileSize);
    file.read(fileBytes.data(), fileSize);
    Serial.println("File read successfully.");

    file.close();
    SD.end();
    
    return fileBytes;
}

void FileReader::initialize() 
{
    if (isInitialized) 
    {
        return;
    }

    // Initialize the SD Card reader
    SPI.setSCK(SDCARD_SPI_CLOCK);
    SPI.setMOSI(SDCARD_SPI_COPI);
    SPI.setMISO(SDCARD_SPI_CIPO);
    chipSelectPin = SDCARD_CHIPSELECT;

    // Try to initialize SD card
    if (SD.begin(chipSelectPin))
    {
        Serial.println("SD card detected on primary pins.");
        SD.end();
        isInitialized = true;
        isSdCardDetected = true;
        return;
    }

    Serial.println("SD card not detected on primary pins. Attempting alternate pins.");

    // Try the alternate pins if the first attempt failed.
    SPI.setSCK(SDCARD_ALT_SPI_CLOCK);
    SPI.setMOSI(SDCARD_ALT_SPI_COPI);
    SPI.setMISO(SDCARD_ALT_SPI_CIPO);
    chipSelectPin = SDCARD_ALT_CHIPSELECT;

    // Try to initialize SD card again
    if (SD.begin(chipSelectPin))
    {
        Serial.println("SD card detected on alternate pins.");
        SD.end();
        isInitialized = true;
        isSdCardDetected = true;
        return;
    }

    Serial.println("SD card initialization failed on alternate pins.");

    isInitialized = true;
    isSdCardDetected = false;
}

bool FileReader::isBuiltInFile(String filename) 
{
    return filename.startsWith(BUILT_IN_FILE_INDICATOR) && filename.endsWith(BUILT_IN_FILE_INDICATOR);
}

const char* FileReader::readBuiltInFile(String filename) 
{
    Serial.println("'Reading' built-in file: " + filename);

    if (filename == "::Display1::")
    {
        return StringUtils::copyString(BuiltInFiles::Digit1Json(), strlen(BuiltInFiles::Digit1Json()));
    }

    if (filename == "::Display3::")
    {
        return StringUtils::copyString(BuiltInFiles::Digit3Json(), strlen(BuiltInFiles::Digit3Json()));
    }

    if (filename == "::Display8::")
    {
        return StringUtils::copyString(BuiltInFiles::Digit8Json(), strlen(BuiltInFiles::Digit8Json()));
    }

    if (filename == "::Display15::")
    {
        return StringUtils::copyString(BuiltInFiles::LogoJson(), strlen(BuiltInFiles::LogoJson()));
    }

    if (filename == "::Display255::")
    {
        return StringUtils::copyString(BuiltInFiles::TestMatrixJson(), strlen(BuiltInFiles::TestMatrixJson()));
    }

    Serial.println("Built-in file not found. Returning null.");
    return nullptr;
}
