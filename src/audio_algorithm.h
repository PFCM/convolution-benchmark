/*
Base processing algorithm
*/
#include <memory>

class Algorithm {
public:
  virtual void tick(const float *in, float *out, unsigned long blocksize) {
    std::memcpy(out, in, blocksize * sizeof(float));
  }
};
