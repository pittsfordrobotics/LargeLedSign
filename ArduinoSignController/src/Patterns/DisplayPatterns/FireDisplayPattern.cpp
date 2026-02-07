#include "FireDisplayPattern.h"

FireDisplayPattern::FireDisplayPattern() : DisplayPattern()
{
    generatePallet();
}

void FireDisplayPattern::setSparkingAmount(byte sparkingAmount)
{
    m_sparking = sparkingAmount;
}

void FireDisplayPattern::setCoolingAmount(byte coolingAmount)
{
    m_cooling = coolingAmount;
}

void FireDisplayPattern::resetInternal(PixelMap* pixelMap)
{
    pixelMap->fill(0);
    m_combinedRowGroups.clear();

    populateCombinedGroupsForIndividualColumns(pixelMap);

    // Initialize the row heat groups based on the row groupings
    // and set all the initial heats to 0
    m_combinedRowHeats.clear();
    for (std::vector<std::vector<int>> rowGroup : m_combinedRowGroups)
    {
        std::vector<int> rowHeatsForGroup;
        for (std::vector<int> rows : rowGroup)
        {
            rowHeatsForGroup.push_back(0);
        }

        m_combinedRowHeats.push_back(rowHeatsForGroup);
    }
}

void FireDisplayPattern::updateInternal(PixelMap* pixelMap)
{
    for (int group = 0; group < m_combinedRowGroups.size(); group++)
    {
        updateRowHeats(m_combinedRowHeats[group]);
        int numRows = m_combinedRowHeats[group].size();
        for (int row = 0; row < numRows; row++)
        {
            byte temperature = m_combinedRowHeats[group][row];
            uint32_t color = m_heatColors[temperature];
            for (int pixel : m_combinedRowGroups[group][numRows - row - 1])
            {
                pixelMap->setRawPixelColor(pixel, color);
            }
        }
    }
}

void FireDisplayPattern::populateCombinedGroupsForIndividualColumns(PixelMap* pixelMap)
{
    // Get the full list of columns and rows.
    // Each column will be a grouping.
    // Find what row each column's pixels are in to get the row mappings.
    // If a column doesn't have a pixel in a specific row, add and empty pixel list for that row.
    std::vector<std::vector<int>*> columns = pixelMap->getAllColumns();
    std::vector<std::vector<int>*> rows = pixelMap->getAllRows();
    
    for(std::vector<int>* column : columns)
    {
        std::vector<std::vector<int>> rowGroup;
        for (std::vector<int>* row : rows)
        {
            std::vector<int> pixelsInRow;
            for (int pixel : *column)
            {
                if (std::find(row->begin(), row->end(), pixel) != row->end())
                {
                    pixelsInRow.push_back(pixel);
                }
            }

            rowGroup.push_back(pixelsInRow);
        }

        m_combinedRowGroups.push_back(rowGroup);
    }
}

void FireDisplayPattern::updateRowHeats(std::vector<int> &rowHeats)
{
    // Originally based on https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#LEDStripEffectFire
    // Colors for temperatures changed.
    //
    // From the site:
    // Cooling indicates how fast a flame cools down. More cooling means shorter flames, and the recommended 
    // values are between 20 and 100. 50 seems the nicest.
    //
    // Sparking indicates the chance (out of 255) that a spark will ignite. A higher value makes the fire 
    // more active. Suggested values lay between 50 and 200, with my personal preference being 120.

    int cooldown;
    int numRows = rowHeats.size();

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < numRows; i++)
    {
        cooldown = random(0, ((m_cooling * 10) / numRows) + 2);

        if (cooldown > rowHeats[i])
        {
            rowHeats[i] = 0;
        }
        else
        {
            rowHeats[i] = rowHeats[i] - cooldown;
        }
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = numRows - 1; k >= 2; k--)
    {
        rowHeats[k] = (rowHeats[k - 1] + rowHeats[k - 2] + rowHeats[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' near the bottom
    if (random(255) < m_sparking)
    {
        int y = random(7);
        rowHeats[y] = rowHeats[y] + random(160, 255);
    }    
}

std::vector<String> FireDisplayPattern::getParameterNames()
{
    std::vector<String> parameterNames;
    parameterNames.push_back("Sparking");
    parameterNames.push_back("Cooling");

    return parameterNames;
}

void FireDisplayPattern::generatePallet()
{
    // generate the temperature-to-color pallet.
    for(int temperature = 0; temperature < 256; temperature++)
    {
        uint16_t hue = 0;
        byte saturation = 255;
        byte value = 255;
    
        if (temperature < 128)
        {
            // bottom half of temperature range:
            // hue = 0 (red), value scaled from 0 to 255
            // This should give black for 0, up to full red at 127.
            value = temperature * 2;    
        }
        else
        {
            // top half of temperature range:
            // hue from 0 to 8200 (yellowish), value at 255
            // This should give red at 128, up to yellow-ish at 255.
            hue = (temperature - 128) / 255.0 * 8200;
        }
    
        m_heatColors[temperature] = NeoPixelUtils::ColorHSV(hue, saturation, value);
    }
}