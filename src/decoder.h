/*
  rtl_433_ESP - 433.92 MHz protocols library for ESP32

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with library. If not, see <http://www.gnu.org/licenses/>


  Project Structure

  rtl_433_ESP - Main Class
  decoder.cpp - Wrapper and interface for the rtl_433 classes
  receiver.cpp - Wrapper and interface for RadioLib
  rtl_433 - subset of rtl_433 package

*/

#ifndef rtl_433_DECODER_H
#define rtl_433_DECODER_H

#include "rtl_433_ESP.h"

extern "C"
{
#include "bitbuffer.h"
#include "pulse_detect.h"
#include "pulse_demod.h"
#include "list.h"
#include "r_api.h"
#include "r_private.h"
#include "rtl_433.h"
#include "rtl_433_devices.h"
#include "fatal.h"
}

#include "tools/aprintf.h"
#include "log.h"

    void rtlSetup();
    void _setCallback(rtl_433_ESPCallBack callback, char *messageBuffer, int bufferSize);
    void _setDebug(int debug);
    void processSignal(pulse_data_t *rtl_pulses);
    void rtl_433_DecoderTask(void *pvParameters);

#endif