#include "BleProxyService.h"

BleProxyService::BleProxyService()
{
    nullStatusDisplay = new NullStatusDisplay();
    statusDisplay = nullStatusDisplay;
    nextSecondaryConnectionCheck = millis() + SECONDARY_PING_INTERVAL;
}

BleProxyService::~BleProxyService()
{
    delete nullStatusDisplay;
}

void BleProxyService::setStatusDisplay(StatusDisplay* display)
{
    if (display == nullptr)
    {
        statusDisplay = nullStatusDisplay;
    }
    else
    {
        statusDisplay = display;
    }
}

void BleProxyService::update()
{
    if (millis() < nextSecondaryConnectionCheck)
    {
        // Not time to check
        return;
    }

    nextSecondaryConnectionCheck = millis() + SECONDARY_PING_INTERVAL;
    checkSecondaryConnections();
}

void BleProxyService::disconnectSecondaries()
{
    DebugUtils::println("Disconnecting all secondary devices.");

    for (uint i = 0; i < allSecondaries.size(); i++)
    {
        if (allSecondaries.at(i)->isConnected())
        {
            allSecondaries.at(i)->disconnect();
        }

        delete allSecondaries.at(i);
    }

    allSecondaries.clear();
}

void BleProxyService::connectToSecondaries()
{
    // Disconnect any secondaries that we may still be connected to.
    disconnectSecondaries();

    // Repopulate the secondaries.
    populateSecondaryClients();
    updateOffsetDataForSecondaryClients();
}

void BleProxyService::setConfiguration(byte brightness, byte speed, PatternData pattern)
{
    if (allSecondaries.size() == 0)
    {
        return;
    }

    if (currentBrightness == brightness &&
        currentSpeed == speed &&
        currentPatternData == pattern)
    {
        // No changes to propagate.
        return;
    }
    
    for (uint i = 0; i < allSecondaries.size(); i++)
    {
        allSecondaries[i]->setBrightness(brightness);
        allSecondaries[i]->setSpeed(speed);
        allSecondaries[i]->setPatternData(pattern);
    }

    ulong timestamp = millis();
    for (uint i = 0; i < allSecondaries.size(); i++)
    {
        allSecondaries[i]->updateSyncData(timestamp);
    }

    currentBrightness = brightness;
    currentSpeed = speed;
    currentPatternData = pattern;
}

void BleProxyService::displaySecondaryBatteryVoltages()
{
    if (allSecondaries.size() == 0)
    {
        statusDisplay->displayTemporary("-0-", 1000);
        return;
    }

    std::vector<String> stringsToDisplay;
    stringsToDisplay.reserve(allSecondaries.size());
    for (uint i = 0; i < allSecondaries.size(); i++)
    {
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d=%.2f", i + 1, allSecondaries[i]->getBatteryVoltage());
        stringsToDisplay.push_back(String(buffer));
    }

    statusDisplay->displaySequence(stringsToDisplay, 1500);
}

void BleProxyService::updateOffsetDataForSecondaryClients()
{
    statusDisplay->setDisplay("C---");
    uint numSecondaries = allSecondaries.size();
    if (numSecondaries == 0)
    {
        // Not sure how we got here with no secondary clients.
        statusDisplay->clear();
        return;
    }

    // Stash the sign config data for each secondary so we don't retrieve it every time.
    std::vector<SignConfigurationData> signConfigurations;
    for (uint i = 0; i < numSecondaries; i++)
    {
        SignStatus status = allSecondaries[i]->getSignStatus();
        signConfigurations.push_back(status.signConfigurationData);
    }

    int numCols = 0;
    for (uint i = 0; i < numSecondaries; i++)
    {
        numCols += signConfigurations[i].columnCount;
    }

    int colsSoFar = 0;
    for (uint i = 0; i < numSecondaries; i++)
    {
        statusDisplay->setDisplay("C--" + String(i + 1));
        SignOffsetData offsetData;
        offsetData.digitsToLeft = i;
        offsetData.digitsToRight = numSecondaries - i - 1;
        offsetData.columnsToLeft = colsSoFar;
        offsetData.columnsToRight = numCols - colsSoFar - signConfigurations[i].columnCount;
        colsSoFar += signConfigurations[i].columnCount;

        // Write the data back to the secondary
        allSecondaries[i]->setSignOffsetData(offsetData);
    }

    statusDisplay->clear();
}

void BleProxyService::populateSecondaryClients()
{
    ulong scanTimeout = millis() + MAX_TOTAL_SCAN_TIME;

    statusDisplay->setDisplay("C-0");
    // Continue to look for secondaries until we pass the timeout after finding at least one.
    while ((allSecondaries.size() < MAX_SECONDARY_COUNT) && (millis() < scanTimeout || allSecondaries.size() == 0))
    {
        SecondaryClient *secondary = scanForSecondaryClient();
        if (secondary->isValidClient())
        {
            allSecondaries.push_back(secondary);
            statusDisplay->setDisplay("C-" + String(allSecondaries.size()));

            // Reset timeout and continue looking for secondaries.
            scanTimeout = millis() + MAX_TOTAL_SCAN_TIME;
        }
        else
        {
            // Clean up the memory from the invalid peripheral.
            delete secondary;
        }
    }

    // We found all we could in the time interval. Order them by position, lowest position first.
    std::sort(
            allSecondaries.begin(),
            allSecondaries.end(),
            [](SecondaryClient *&a, SecondaryClient *&b)
            { return a->getSignOrder() < b->getSignOrder(); });

    statusDisplay->clear();
}

SecondaryClient *BleProxyService::scanForSecondaryClient()
{
    ulong scanTimeout = millis() + MAX_SECONDARY_SCAN_TIME;

    DebugUtils::print("Scanning for peripherals with uuid = ");
    DebugUtils::println(BTCOMMON_SECONDARYCONTROLLER_UUID);
    bool allowDuplicateAdvertisements = true;
    BLE.scanForUuid(BTCOMMON_SECONDARYCONTROLLER_UUID, allowDuplicateAdvertisements);

    BLEDevice peripheral = BLE.available();
    while (!peripheral)
    {
        peripheral = BLE.available();

        if (millis() > scanTimeout)
        {
            DebugUtils::println("Scan timed out.");
            BLE.stopScan();
            // Return a dummy client - it'll be ignored and cleaned up.
            return new SecondaryClient(peripheral);
        }
    }

    String localName = "unknown";
    if (peripheral)
    {
        DebugUtils::println("Found peripheral.");
        if (peripheral.hasLocalName())
        {
            localName = peripheral.localName();
            DebugUtils::print("Peripheral name: ");
            DebugUtils::println(localName);
        }
    }

    BLE.stopScan();

    if (peripheral.connect())
    {
        DebugUtils::print("Connected to ");
        DebugUtils::println(localName);
    }
    else
    {
        DebugUtils::println("Failed to connect!");
    }

    return new SecondaryClient(peripheral);
}

void BleProxyService::checkSecondaryConnections()
{
    DebugUtils::print("Number of connected secondaries: ");
    DebugUtils::println(String(allSecondaries.size()));

    if (allSecondaries.size() == 0)
    {
        // No secondaries defined - if we got here it's because we lost connection with the secondaries.
        statusDisplay->setDisplay("-NC-");
        return;
    }

    String status = " . . ." + String(allSecondaries.size());
    statusDisplay->displayTemporary(status, 200);
    bool atLeastOneDisconnected = false;
    for (uint i = 0; i < allSecondaries.size(); i++)
    {
        DebugUtils::print("Secondary [");
        DebugUtils::print(allSecondaries.at(i)->getLocalName());
        DebugUtils::print("]");
        if (allSecondaries.at(i)->isConnected())
        {
            DebugUtils::println(" is connected.");
        }
        else
        {
            DebugUtils::println(" has been disconnected.");
            atLeastOneDisconnected = true;
        }
    }

    if (atLeastOneDisconnected)
    {
        // Disconnect from all the secondaries.
        // This will also set the display to show we've disconnected.
        // Only attempt to reconnect when manually triggered.
        disconnectSecondaries();
    }
}
