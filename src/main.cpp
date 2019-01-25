/**
Main entrypoint.
**/

#include "audio_algorithm.h"
#include <gflags/gflags.h>
#include <iostream>
#include <sndfile.h>

static bool validate_input(const char *flagname, const std::string &value) {
  SF_INFO info;
  SNDFILE *result = sf_open(value.c_str(), SFM_READ, &info);
  if (result == nullptr) {
    return false;
  }
  sf_close(result);
  return true;
}

DEFINE_string(input, "", "input file to process");
DEFINE_validator(input, &validate_input);
DEFINE_uint64(block_size, 512, "block size for processing");
DEFINE_string(output, "./out.wav", "output file to process");

void scale_float_buffer(float *buffer, sf_count_t len, int bits) {
  float max = (1 << bits) - 1;
  for (int i = 0; i < len; i++) {
    buffer[i] /= max;
  }
}

void unscale_float_buffer(float *buffer, sf_count_t len, int bits) {
  float max = (1 << bits) - 1;
  for (int i = 0; i < len; i++) {
    buffer[i] *= max;
  }
}

/* Read a file, process it one block at a time and output to a file. Data is
always processed as `float` */
void process_file(sf_count_t block_size, SF_INFO *in_info, SNDFILE *input,
                  Algorithm *processor, SNDFILE *output) {
  float *input_buffer = new float[block_size];
  float *output_buffer = new float[block_size];
  sf_count_t samples_read = block_size;
  sf_count_t blocks = 0;
  bool status = false;
  while (samples_read == block_size) {
    samples_read = sf_readf_float(input, input_buffer, block_size);
    if (samples_read) {
      // scale_float_buffer(input_buffer, samples_read, 24);

      status = processor->tick(input_buffer, output_buffer, samples_read);
      blocks++;
      // unscale_float_buffer(output_buffer, samples_read, 24);
      sf_writef_float(output, output_buffer, samples_read);
    }
  }
  // and grab any remaining tail
  while (status) {
    status = processor->tick(nullptr, output_buffer, block_size);
    sf_writef_float(output, output_buffer, block_size);
  }
  std::cout << "processing complete after " << blocks << " blocks";

  delete[] input_buffer;
  delete[] output_buffer;
}

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  std::cout << "input: " << FLAGS_input << std::endl;
  std::cout << "-------" << std::endl;
  std::cout << "writing to: " << FLAGS_output << std::endl;

  SF_INFO in_info, out_info;
  SNDFILE *in_file = sf_open(FLAGS_input.c_str(), SFM_READ, &in_info);
  if (in_info.channels != 1) {
    std::cerr << "error, can only handle one channel, got " << in_info.channels
              << std::endl;
    return 1;
  }
  out_info.channels = 1;
  out_info.format = SF_FORMAT_WAV | (in_info.format & 0x00FFFF);
  out_info.frames = in_info.frames;
  out_info.sections = in_info.sections;
  out_info.samplerate = in_info.samplerate;

  SNDFILE *out_file = sf_open(FLAGS_output.c_str(), SFM_WRITE, &out_info);

  sf_command(in_file, SFC_SET_NORM_FLOAT, nullptr, SF_FALSE);
  sf_command(out_file, SFC_SET_NORM_FLOAT, nullptr, SF_FALSE);

  Algorithm algol;
  process_file(FLAGS_block_size, &in_info, in_file, &algol, out_file);
  sf_close(out_file);
  sf_close(in_file);

  return 0;
}
