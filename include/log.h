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

#ifndef _LOG_H_
#define _LOG_H_

#define LOG_EMERG   0
#define LOG_ALERT   1
#define LOG_CRIT    2
#define LOG_ERR     3
#define LOG_WARNING 4
#define LOG_NOTICE  5
#define LOG_INFO    6
#define LOG_DEBUG   7
#define LOG_TRACE   8
#define LOG_VERBOSE 9

#define LOG_STACK 255

#include <stdio.h>

#define logprintf(prio, args...)       \
  {                                    \
    printf("rtl_433_ESP(%d): ", prio); \
    printf(args);                      \
  }
#define logprintfLn(prio, args...)     \
  {                                    \
    printf("rtl_433_ESP(%d): ", prio); \
    printf(args);                      \
    printf("\n");                      \
  }
#define alogprintf(prio, args...) \
  { printf(args); }
#define alogprintfLn(prio, args...) \
  {                                 \
    printf(args);                   \
    printf("\n");                   \
  }

#endif
