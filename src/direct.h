/* direct convolutions */
#include "audio_algorithm.h"
#include <iostream>
#include <vector>

static unsigned long wrap(unsigned long value, unsigned long max) {
  if (value <= max) {
    return max - value;
  }
  if (value < 0) {
    return max + value;
  }
  return value;
}

class DirectForLoopConvolution : public Algorithm {
public:
  DirectForLoopConvolution(std::vector<float> &impulse_response,
                           unsigned long block_size)
      : filter(impulse_response),
        delay_buffer(block_size + impulse_response.size()) {
    delay_pos = impulse_response.size();
  };

  bool tick(const float *in, float *out, unsigned long num_frames) override {
    // first clock the frames into the delay buffer
    for (int i = 0; i < num_frames; i++) {
      delay_buffer[wrap(delay_pos + i, delay_buffer.size())] = in[i];
    }
    // slide the flipped filter over the delay buffer
    float sample;
    for (int i = 0; i < num_frames; i++) {
      sample = 0;
      for (int j = 0; j < filter.size(); j++) {
        sample +=
            delay_buffer[wrap(delay_pos - j, delay_buffer.size())] * filter[j];
      }
      out[i] = sample;
    }

    delay_pos = wrap(delay_pos + num_frames, delay_buffer.size());
    return false;
  };

  std::string name = "direct/for-loop";

private:
  std::vector<float> filter;
  std::vector<float> delay_buffer;
  unsigned long delay_pos;
};
