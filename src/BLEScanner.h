#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <Arduino.h>
#define RSSI_THRESH -40
#define SCAN_TIME_SEC 2

class BLEProximityScanner{
    public:
        BLEProximityScanner();
        void init();
        void update();
        boolean foundDevice();
    
    private:
        boolean deviceFound;
        BLEScan* bleScan;
};