#include <rtl_433_ESP.h>

uint16_t nextPulseTrainLength()
{
    return _pulseTrains[_avaiablePulseTrain].length;
}

uint16_t receivePulseTrain(uint16_t *pulses, boolean *pins)
{
    uint16_t length = nextPulseTrainLength();

    if (length > 0)
    {
        volatile PulseTrain_t &pulseTrain = _pulseTrains[_avaiablePulseTrain];
        _avaiablePulseTrain = (_avaiablePulseTrain + 1) % RECEIVER_BUFFER_SIZE;
        for (uint16_t i = 0; i < length; i++)
        {
            pulses[i] = pulseTrain.pulses[i];
            pins[i] = pulseTrain.pins[i];
        }
        pulseTrain.length = 0;
    }
    return length;
}

void resetReceiver()
{
    for (unsigned int i = 0; i < RECEIVER_BUFFER_SIZE; i++)
    {
        _pulseTrains[i].length = 0;
    }
    _avaiablePulseTrain = 0;
    _actualPulseTrain = 0;
    _nrpulses = 0;
}

void ICACHE_RAM_ATTR interruptHandler()
{
    volatile PulseTrain_t &pulseTrain = _pulseTrains[_actualPulseTrain];
    volatile uint16_t *codes = pulseTrain.pulses;
    volatile boolean *pins = pulseTrain.pins;

    const unsigned long now = micros();
    const unsigned int duration = now - _lastChange;
    // Debug(duration);  Debug(",");

    /* We first do some filtering (same as pilight BPF) */
    if (duration > minpulselen)
    {
        //        if (duration < maxpulselen)
        //        {
        /* All codes are buffered */
        codes[_nrpulses] = (uint16_t)duration;
        if (digitalRead(RF_RECEIVER_GPIO))
        {
            pins[_nrpulses] = true;
            // Debug("+");
        }
        else
        {
            pins[_nrpulses] = false;
            // Debug("-");
        }
        _nrpulses = (uint16_t)((_nrpulses + 1) % MAXPULSESTREAMLENGTH);
        //        }
        _lastChange = now;
    }
}

void enableReceive(byte inputPin)
{
    int16_t interrupt = digitalPinToInterrupt(inputPin);
    if (_interrupt == interrupt)
    {
        return;
    }
    if (_interrupt >= 0)
    {
        detachInterrupt((uint8_t)_interrupt);
    }
    _interrupt = interrupt;

    if (interrupt >= 0)
    {
        attachInterrupt((uint8_t)interrupt, interruptHandler, CHANGE);
    }
}

void setup()
{
    Serial.begin(115200);
    Log.begin(LOG_LEVEL, &Serial);
    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.SetRx(CC1101_FREQUENCY);
    pinMode(RF_EMITTER_GPIO, OUTPUT);
    resetReceiver();

    Log.notice(F("CC1101 Frequency Scanner Ready %d" CR), ESP.getFreeHeap());
    Log.trace(F("Pre rtlSetup %d" CR), ESP.getFreeHeap());

    Log.trace(F("Setup Completed %d" CR), ESP.getFreeHeap());
}

void loop()
{
    if (ELECHOUSE_cc1101.getRssi() > MINRSSI)
    {
        enableReceive(RF_RECEIVER_GPIO);
        if (!receiveMode)
        {
            receiveMode = true;
            signalStart = micros();
        }
    }
    else
    {
        if (receiveMode)
        {
            DebugLn();
            Log.trace(F("Signal length: %d, train: %d, pulses: %d" CR), micros() - signalStart, _actualPulseTrain, _nrpulses);
            receiveMode = false;
            enableReceive(-1);
            if (_nrpulses > 30 && (micros() - signalStart > 40000))
            {

                _pulseTrains[_actualPulseTrain].length = _nrpulses;
                _pulseTrains[_actualPulseTrain].duration = micros() - signalStart;
                _actualPulseTrain = (_actualPulseTrain + 1) % RECEIVER_BUFFER_SIZE;

                _nrpulses = 0;
            }
            else
            {
                // _pulseTrains[_actualPulseTrain].duration = micros() - signalStart;
                // _actualPulseTrain = (_actualPulseTrain + 1) % RECEIVER_BUFFER_SIZE;

                _nrpulses = 0;
            }
        }
    }

    uint16_t pulses[MAXPULSESTREAMLENGTH];
    boolean pins[MAXPULSESTREAMLENGTH];

    uint16_t length = receivePulseTrain(pulses, pins);

    if (length > 0)
    {
        Debug("RAW (");
        Debug(length);
        Debug("): ");

        for (int i = 0; i < length; i++)
        {
            if (pins[i])
            {
                Debug("+");
            }
            else
            {
                Debug("-");
            }
            Debug(pulses[i]);
        }
        DebugLn("");
    }
}
