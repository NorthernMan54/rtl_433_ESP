#include "bitbuffer.h"
#include "pulse_detect.h"
#include "pulse_demod.h"
#include "list.h"
#include "rtl_devices.h"
#include "r_api.h"
#include "r_private.h"
#include "rtl_433.h"
#include "rtl_433_devices.h"
#include "log.h"

void rtlSetup(r_cfg_t *cfg);

#define MAXIMUM_JSON_MESSAGE 256

typedef struct protocol_t {
  char *id;
  uint8_t rawlen;
  uint8_t minrawlen;
  uint8_t maxrawlen;
  uint16_t mingaplen;
  uint16_t maxgaplen;
  uint8_t txrpt;
  uint8_t rxrpt;
  //short multipleId;
  //short config;
  //short masterOnly;
  //struct options_t *options;
  struct JsonNode *message;

  uint8_t repeats;
  unsigned long first;
  unsigned long second;

  uint16_t *raw;

  // hwtype_t hwtype;
  // devtype_t devtype;
  //struct protocol_devices_t *devices;
  //struct protocol_threads_t *threads;

  union {
    void (*parseCode)(void);
    void (*parseCommand)(struct JsonNode *code);
  };
  int (*validate)(void);
//  int (*createCode)(JsonNode *code);
//  int (*checkValues)(JsonNode *code);
  //struct threadqueue_t *(*initDev)(JsonNode *device);
  void (*printHelp)(void);
  void (*gc)(void);
  //void (*threadGC)(void);

  /* ESPiLight special, used to compare repeated messages*/
  char *old_content;
} protocol_t;

#ifdef MY_DEVICES
#define NUMOFDEVICES 4
#else
#define NUMOFDEVICES 178
#endif