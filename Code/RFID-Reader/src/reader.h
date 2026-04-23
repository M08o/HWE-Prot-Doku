/**
 * reader.h - Oeffentliche Schnittstelle des RFID-Lesegeraets.
 *
 * Andere Dateien muessen nicht wissen, wie der Reader intern arbeitet.
 * Sie rufen nur readerInit() einmal beim Start und readerUpdate() in loop()
 * auf. Die eigentliche UART- und Zustandslogik bleibt in reader.cpp gekapselt.
 */

#pragma once

// Initialisiert UART2 und gibt die wichtigsten Startinformationen aus.
void readerInit();

// Fuehrt einen Schritt des Zustandsautomaten aus. Diese Funktion blockiert
// nicht und soll daher dauerhaft aus loop() aufgerufen werden.
void readerUpdate();
