#ifndef WSPR_HH
#define WSPR_HH

#include <complex>
#include <cstdlib>

/* C Interfaces to FORTRAN routines. */

extern "C" {

typedef struct {
  float re, im;
} cmplx_t;

/** Down-sampling by 32 and computation of avg psd.
 * @param in (in) Pointer to the signal (2min @ 12kHz = 144000 samples).
 * @param npts (in) Number of samples in @c in.
 * @param nbfo (in) BFO frequency.
 * @param out (out) Down-sampled and shifted (-Fbfo) signal.
 * @param jz (out) Number of samples in @c out.
 * @param psd (out) flaot[513] Avg. PSD. */
void mix162_(int16_t *in, int *npts, int *nbfo, cmplx_t *out, int *jz, float *psd);

/** Searches for signal candidates in the time-series.
 * @param c2 (in) A vector of size 45000, the complex time-series centered at band-center.
 * @param jz (in) The length of the vector c2?
 * @param ps (in) The average power-spectrum (vector of length 513)?
 * @param sstf (out) The vector of signal-candidate properties found dim=(5,275).
 * @param kz (out) The number of signals found. */
void sync162_(cmplx_t *c2, int *jz, float *ps, float *sstf, int *kz);

/** Corrects the frequency drift of the given signal.
 * @param in A vector of (max) size 45000.
 * @param out A vector of (max) size 45000.
 * @param N The vector length.
 * @param coef The 3 parameters. */
void twkfreq_(cmplx_t *in, cmplx_t *out, int *N, float *coef);

/** Decodes a corrected signal.
 * @param sig The corrected signal (complex vector of (max) length 45000).
 * @param N The size of vector @c sig.
 * @param message The decoded massage (at least 22 bytes long).
 * @param ncycles ???
 * @param metric ???. 256x2 integer table.
 * @param nerr The number of deconding errors? */
void decode162_(cmplx_t *sig, int *N, char *message, int *Ncycles, int *metric, int *nerr);

/** Computes the distance between two locators (up to 6 char locators,
 * the last two chars can be ' '). */
void azdist_(char *loc_a, char *loc_b,
             double *utch, int *nAz, int *nEl,
             int *nDmiles, int *nDkm,
             int *nHotAz, int *nHotABetter);

void grid2deg_(char *loc, float *lon, float *lat);
void deg2grid_(float *lon, float *lat, char *loc);
}


#include "buffer.hh"

namespace sdr {

/** The WSPR message type. */
class WSPRMessage {
public:
  float df;     ///< Frequency of the signal w.r.t the center frequency. (Hz)
  float dt;     ///< Time-delay of signal (s).
  float snr;    ///< Signal strength in db SNR.
  char msg[22]; ///< Message text (CALLSIGN [SPACE] LOCATOR [SPACE] TXPOWER), where TXPOWER is
                ///  given in dBm.

  /** Returns true if the message body is a valid WSPRnet message. */
  bool isValid() const;
  /** Returns the calls sign. */
  std::string callsign() const;
  /** Returns the locator. */
  std::string locator() const;
  /** Returns the tx power in dB mW. */
  int power() const;
  /** Returns the tx power in W. */
  float powerW() const;
};

/** Decodes the WSPR signals in real buffer @c in +/- 100Hz around @c Fbfo. The signal is required
 * to have 12 kHz sample rate. Decoded messages are appended to @c msgs. */
void wspr_decode(const Buffer<int16_t> &in, std::list<WSPRMessage> &msgs, double Fbfo=1.5e3);
/** Computes the distance between the locators. */
double loc_dist(const std::string &loc_a, const std::string &loc_b);
void loc2deg(const std::string &loc, float &lon, float &lat);
std::string deg2loc(float lon, float lat);
}

#endif // WSPR_HH
