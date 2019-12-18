#include "BLEScanner.h"

BLEProximityScanner::BLEProximityScanner(){
    deviceFound=false; 
}
void BLEProximityScanner::init(){
    Serial.println("BLE: Begin Init.");
    BLEDevice::init("");
    Serial.println("BLE: Init Done.");
    bleScan= BLEDevice::getScan();
    bleScan->setActiveScan(true); 
    bleScan->setInterval(100);
    bleScan->setWindow(99); 
}

void BLEProximityScanner::update(){
    deviceFound = false;
    BLEScanResults foundDevices = bleScan->start(SCAN_TIME_SEC, false);
    for (int i=0;i< foundDevices.getCount();i++){
        BLEAdvertisedDevice advertisedDevice = foundDevices.getDevice(i);
        if ( advertisedDevice.getRSSI() > RSSI_THRESH ){
            deviceFound  = true;
            Serial.printf("BLE::Close Device: %s \n", advertisedDevice.toString().c_str());
        }
        else{
            Serial.printf("BLE::Too Far Device: %s \n", advertisedDevice.toString().c_str());
        }
    } 
    bleScan->clearResults(); 
}

boolean BLEProximityScanner::foundDevice(){
    return deviceFound;
} 


