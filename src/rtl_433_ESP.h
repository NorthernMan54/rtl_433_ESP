#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <ArduinoLog.h>

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_TRACE
#endif

#ifdef DEBUG
#define Debug(x) Serial.print(x)
#define DebugLn(x) Serial.println(x)
#else
#define Debug(x)
#define DebugLn(x)
#endif

#ifndef CC1101_FREQUENCY
#define CC1101_FREQUENCY 433.92
#endif

#ifndef MINRSSI
#define MINRSSI -75
#endif

#ifndef RECEIVER_BUFFER_SIZE
#define RECEIVER_BUFFER_SIZE 10
#endif

#ifndef MAXPULSESTREAMLENGTH
#define MAXPULSESTREAMLENGTH 1024
#endif

#define minpulselen 50
#define maxpulselen 10000000
#define mingaplen 0
#define minrawlen 1
#define maxrawlen 300

bool receiveMode = false;
unsigned long signalStart = micros();


typedef struct PulseTrain_t
{
    uint16_t pulses[MAXPULSESTREAMLENGTH];
    boolean pins[MAXPULSESTREAMLENGTH];
    uint16_t length = 0;
    unsigned long duration = 0;
} PulseTrain_t;

volatile PulseTrain_t _pulseTrains[RECEIVER_BUFFER_SIZE];
static volatile uint8_t _actualPulseTrain = 0;
static uint8_t _avaiablePulseTrain;
static volatile unsigned long _lastChange; // Timestamp of previous edge
static volatile uint16_t _nrpulses;
static int16_t _interrupt;

// rtl_433

extern "C" {
#include <pulse_detect.h>
#include <rfraw.h>
// #include <r_api.h>
// #include <rtl_433.h>
#include <r_private.h>
#include <rtl_433_devices.h>
#include <bitbuffer.h>
#include <r_device.h>
#include <term_ctl.h>
#include <pulse_detect_fsk.h>
#include <fatal.h>

struct dm_state *demod;
}