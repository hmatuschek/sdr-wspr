#include "wspr.hh"

void
sdr::wspr_decode(const Buffer<int16_t> &in, std::list<WSPRMessage> &msgs, double Fbfo)
{
  float psd[513];    // Average power spectrum
  Buffer< std::complex<float> > c2(65536);
  int npts = in.size(); int nbfo = Fbfo, jz=c2.size();
  mix162_(reinterpret_cast<int16_t *>(in.data()), &npts, &nbfo,
          reinterpret_cast<std::complex<float> *>(c2.data()), &jz, psd);

  for (size_t i=0; i<513; i++) { psd[i] = 0; }
  float sstf[5*275]; // 5x275 - column major
  int kz = 0;        // The number of signal candidates found
  jz = 45000;
  sync162_(reinterpret_cast<std::complex<float> *>(c2.data()), &jz, psd, sstf, &kz);

  if (0 >= kz) { return; }

  // For every candidate found
  for (int k=0; k<kz; k++)
  {
    float snrsync = sstf[5*k+0];
    float snrx    = sstf[5*k+1];
    float dtx     = sstf[5*k+2];
    float dfx     = sstf[5*k+3];
    float drift   = sstf[5*k+4];

    // Fix frequency drift, store result into c3
    float a[5] = { -dfx, -0.5*drift, 0.0, 0.0, 0.0 };
    Buffer< std::complex<float> > c3(45000);
    int jz = 45000;
    twkfreq_(reinterpret_cast<std::complex<float> *>(c2.data()),
             reinterpret_cast<std::complex<float> *>(c3.data()), &jz, a);

    // Decode signal
    char message[22];
    for (size_t i=0; i<22; i++) { message[i] = 0; }

    int minsync = 1; int nsync = snrsync;
    if (nsync < 0) { nsync = 0; }

    float minsnr = -33; int nsnrx = snrx;
    if (nsnrx < minsnr) { nsnrx = minsnr; }
    if ((nsync < minsync) || (nsnrx < minsnr)) { continue; }

    float dt = 1./375;
    for (int idt=0; idt<=128; idt++) {
      int ii = (idt)/2;
      if (idt%2) { ii = -ii; }
      int i1 = (dtx+2.0)/dt + ii; // Start index for synced symbols.
      Buffer< std::complex<float> > c4(45000);
      for (size_t i=0; i<45000; i++) { c4[i] = 0; }
      if (i1 >= 0) {
        for (int i=i1; i<jz; i++) { c4[i-i1] = c3[i]; }
      } else {
        for (int i=0; i<(jz+i1); i++) { c4[i+-i1+1] = c3[i]; }
      }
      int jz=45000, ncycles=0, nerr=0;
      int metric[512];
      decode162_(reinterpret_cast<std::complex<float> *>(c4.data()), &jz,
                 message, &ncycles, metric, &nerr);
      // Check message and store in list
      if (strncmp("      ", message, 6) && strncmp("000AAA", message, 6)) {
        WSPRMessage msg;
        msg.df = dfx; msg.dt = dtx; msg.snr = snrx;
        memcpy(msg.msg, message, 22);
        msgs.push_back(msg);
        break;
      }
    }
  }
}


double
sdr::loc_dist(const std::string &loc_a, const std::string &loc_b) {
  int Dmiles, Dkm, nAz=0, nEl=0, nHotAz, nHotABetter;
  double utch=0;
  char locA[6]; char locB[6];
  for (size_t i=0; i<6; i++) { locA[i] = locB[i] = ' '; }
  memcpy(locA, loc_a.c_str(), std::min(size_t(6),loc_a.size()));
  memcpy(locB, loc_b.c_str(), std::min(size_t(6),loc_b.size()));
  azdist_(locA, locB, &utch, &nAz, &nEl, &Dmiles, &Dkm, &nHotAz, &nHotABetter);
  return Dkm;
}
