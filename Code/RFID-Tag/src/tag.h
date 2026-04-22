/**
 * tag.h – Zustandsautomat des Tags
 *
 *  WaitForByte     → wartet auf Byte vom Reader
 *  WaitBeforeReply → millis()-Pause, dann Antwort senden
 */

#pragma once

void tagInit();
void tagUpdate();
