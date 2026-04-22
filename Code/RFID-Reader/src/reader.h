/**
 * reader.h – Zustandsautomat des Lesegeräts
 *
 *  WaitForByte   → wartet auf Byte vom Tag
 *  WaitBeforeReply → millis()-Pause, dann Antwort senden
 */

#pragma once

void readerInit();
void readerUpdate();
