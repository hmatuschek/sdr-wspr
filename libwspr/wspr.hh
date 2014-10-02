#ifndef WSPR_HH
#define WSPR_HH

#include <complex>

extern "C" {

void mix162_(int16_t *in, int *npts, int *nbfo, std::complex<float> *out, int *jz, float *psd);

/** Searches for signal candidates in the time-series.
 * @param c2 A vector of size 45000, the complex time-series centered at band-center.
 * @param jz The length of the vector c2?
 * @param ps The average power-spectrum (vector of length 512)?
 * @param sstf The vector of signal-candidate properties found dim=(5,275).
 * @param kz The number of signals found. */
void sync162_(std::complex<float> *c2, int *jz, float *ps, float *sstf, int *kz);

/** Corrects the frequency drift of the given signal.
 * @param in A vector of (max) size 45000.
 * @param out A vector of (max) size 45000.
 * @param N The vector length.
 * @param coef The 3 parameters. */
void twkfreq_(std::complex<float> *in, std::complex<float> *out, int *N, float *coef);

/** Decodes a corrected signal.
 * @param sig The corrected signal (complex vector of (max) length 45000).
 * @param N The size of vector @c sig.
 * @param message The decoded massage (at least 22 bytes long).
 * @param ncycles ???
 * @param metric ???. 256x2 integer table.
 * @param nerr The number of deconding errors? */
void decode162_(std::complex<float> *sig, int *N, char *message, int *Ncycles, int *metric, int *nerr);

/** Computes the distance between two locators (up to 6 char locators, the last two char can be ' '). */
void azdist_(char *loc_a, char *loc_b,
             double *utch, int *nAz, int *nEl,
             int *nDmiles, int *nDkm,
             int *nHotAz, int *nHotABetter);

}

#include "buffer.hh"

namespace sdr {

typedef struct {
  float df;     ///< Frequency of the signal w.r.t the center frequency. (Hz)
  float dt;     ///< Time-delay of signal (s).
  float snr;    ///< Signal strength in db over noise.
  char msg[22]; ///< Message text (CALLSIGN [SPACE] LOCATOR [SPACE] TXPOWER), where TXPOWER is given
                ///  in dBm.
} WSPRMessage;

void wspr_decode(const Buffer<int16_t> &in, std::list<WSPRMessage> &msgs, double Fbfo=1.5e3);

double loc_dist(const std::string &loc_a, const std::string &loc_b);

}

#endif // WSPR_HH
