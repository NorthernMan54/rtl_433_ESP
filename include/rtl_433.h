/** @file
    Definition of r_cfg application structure.
*/

#ifndef INCLUDE_RTL_433_H_
#define INCLUDE_RTL_433_H_

#include <stdint.h>
#include "list.h"
#include <time.h>
#include <signal.h>

#define DEFAULT_SAMPLE_RATE     250000
#define DEFAULT_FREQUENCY       433920000
#define DEFAULT_HOP_TIME        (60*10)
#define DEFAULT_ASYNC_BUF_NUMBER    0 // Force use of default value (librtlsdr default: 15)
#define DEFAULT_BUF_LENGTH      (16 * 32 * 512) // librtlsdr default
#define FSK_PULSE_DETECTOR_LIMIT 800000000

#define MINIMAL_BUF_LENGTH      1
#define MAXIMAL_BUF_LENGTH      1
#define SIGNAL_GRABBER_BUFFER   (12 * DEFAULT_BUF_LENGTH)
#define MAX_FREQS               1

#define INPUT_LINE_MAX 8192 /**< enough for a complete textual bitbuffer (25*256) */

struct sdr_dev;
struct r_device;
struct mg_mgr;

typedef enum {
    CONVERT_NATIVE,
    CONVERT_SI,
    CONVERT_CUSTOMARY
} conversion_mode_t;

typedef enum {
    REPORT_TIME_DEFAULT,
    REPORT_TIME_DATE,
    REPORT_TIME_SAMPLES,
    REPORT_TIME_UNIX,
    REPORT_TIME_ISO,
    REPORT_TIME_OFF,
} time_mode_t;

typedef struct r_cfg {
//    char *dev_query;
//    char const *dev_info;
//    char *gain_str;
//    char *settings_str;
//    int ppm_error;
//    uint32_t out_block_size;
//    char const *test_data;
//    list_t in_files;
//    char const *in_filename;
//    volatile sig_atomic_t hop_now;
//    volatile sig_atomic_t exit_async;
//    volatile sig_atomic_t exit_code; ///< 0=no err, 1=params or cmd line err, 2=sdr device read error, 3=usb init error, 5=USB error (reset), other=other error
//    int frequencies;
//    int frequency_index;
//    uint32_t frequency[MAX_FREQS];
//    uint32_t center_frequency;
//    int fsk_pulse_detect_mode;
//    int hop_times;
//    int hop_time[MAX_FREQS];
//    time_t hop_start_time;
//    int duration;
//    time_t stop_time;
//    int after_successful_events_flag;
//    uint32_t samp_rate;
//    uint64_t input_pos;
//    uint32_t bytes_to_read;
//    struct sdr_dev *dev;
//    int grab_mode; ///< Signal grabber mode: 0=off, 1=all, 2=unknown, 3=known
//    int raw_mode; ///< Raw pulses printing mode: 0=off, 1=all, 2=unknown, 3=known
    int verbosity; ///< 0=normal, 1=verbose, 2=verbose decoders, 3=debug decoders, 4=trace decoding.
//    int verbose_bits;
    conversion_mode_t conversion_mode;
//    int report_meta;
//    int report_protocol;
//    time_mode_t report_time;
//    int report_time_hires;
//    int report_time_tz;
//    int report_time_utc;
//    int report_description;
//    int report_stats;
//    int stats_interval;
//    volatile sig_atomic_t stats_now;
//    time_t stats_time;
//    int no_default_devices;
    uint16_t num_r_devices;
//    char *output_key;
//    char *output_tag;
    list_t output_handler;
    struct dm_state *demod;
//    char const *sr_filename;
//    int sr_execopen;
//    int old_model_keys;
    /* stats*/
//    time_t frames_since; ///< stats start time
//    unsigned frames_count; ///< stats counter for interval
//    unsigned frames_fsk; ///< stats counter for interval
//    unsigned frames_events; ///< stats counter for interval
//    struct mg_mgr *mgr;
    struct r_device *devices;

    char *messageBuffer;        // message buffer for message callback
    int bufferSize;             // size of message buffer for message callback
    /**
     * callback to controlling program to be executed when a message is received.  
     * Object point passsed is a pointer to a JSON formatted message for publishing.
    */
    void (*callback)(char * message);
} r_cfg_t;

#define PD_MAX_PULSES 1200      // Maximum number of pulses before forcing End Of Package
#define PD_MIN_PULSES 16        // Minimum number of pulses before declaring a proper package
#define PD_MIN_PULSE_SAMPLES 10 // Minimum number of samples in a pulse for proper detection
#define PD_MIN_GAP_MS 10        // Minimum gap size in milliseconds to exceed to declare End Of Package
#define PD_MAX_GAP_MS 100       // Maximum gap size in milliseconds to exceed to declare End Of Package
#define PD_MAX_GAP_RATIO 10     // Ratio gap/pulse width to exceed to declare End Of Package (heuristic)
#define PD_MAX_PULSE_MS 100     // Pulse width in ms to exceed to declare End Of Package (e.g. for non OOK packages)


/// Data for a compact representation of generic pulse train.
typedef struct pulse_data {
//    uint64_t offset;            ///< Offset to first pulse in number of samples from start of stream.
    uint32_t sample_rate;       ///< Sample rate the pulses are recorded with.
//    unsigned depth_bits;        ///< Sample depth in bits.
//    unsigned start_ago;         ///< Start of first pulse in number of samples ago.
//    unsigned end_ago;           ///< End of last pulse in number of samples ago.
    unsigned int num_pulses;
    int pulse[PD_MAX_PULSES];   ///< Width of pulses (high) in number of samples.
    int gap[PD_MAX_PULSES];     ///< Width of gaps between pulses (low) in number of samples.
#ifdef RSSI
    int rssi[PD_MAX_PULSES];
#endif
//    int ook_low_estimate;       ///< Estimate for the OOK low level (base noise level) at beginning of package.
//    int ook_high_estimate;      ///< Estimate for the OOK high level at end of package.
//    int fsk_f1_est;             ///< Estimate for the F1 frequency for FSK.
//    int fsk_f2_est;             ///< Estimate for the F2 frequency for FSK.
    float freq1_hz;
//    float freq2_hz;
//    float centerfreq_hz;
//    float range_db;
//    float rssi_db;
//    float snr_db;
//    float noise_db;
    int signalRssi;
    unsigned long signalDuration;
} pulse_data_t;

#endif /* INCLUDE_RTL_433_H_ */
