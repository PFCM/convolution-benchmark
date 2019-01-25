/*
Base processing algorithm
*/
#ifndef AUDIO_ALGORITHM
#define AUDIO_ALGORITHM
#include <memory>
#include <string>

class Algorithm {
public:
  /* process a batch of samples, return true if there's still a tail */
  virtual bool tick(const float *in, float *out, unsigned long blocksize) {
    if (in == nullptr) {
      return false;
    }
    std::memcpy(out, in, blocksize * sizeof(float));
    return false;
  }

  std::string name = "identity";
};
#endif
