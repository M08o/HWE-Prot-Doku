/**
 * main.cpp - Einstiegspunkt des ESP32-Programms.
 *
 * Arduino ruft setup() genau einmal beim Start auf. Danach wird loop()
 * dauerhaft wiederholt. Die eigentliche RFID-Logik liegt in reader.cpp,
 * damit diese Datei uebersichtlich bleibt.
 */

#include <Arduino.h>
#include "config.h"
#include "reader.h"

void setup()
{
    // Serielle Verbindung zum PC starten. Darueber sehen wir Debug-Ausgaben
    // im PlatformIO/Arduino Serial Monitor.
    Serial.begin(MONITOR_BAUD);

    // Kurze Wartezeit, damit der serielle Monitor nach dem Reset bereit ist.
    delay(200);

    Serial.println();
    Serial.println("esp-lesegeraet startet...");

    // UART2 fuer den RFID-Tag initialisieren und Startinformationen ausgeben.
    readerInit();
}

void loop()
{
    // Nicht-blockierende Hauptlogik. Diese Funktion wird sehr oft aufgerufen
    // und prueft jedes Mal, ob neue Daten eingetroffen sind oder eine Antwort
    // faellig ist.
    readerUpdate();
}
