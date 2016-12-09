#pragma once

#include <fstream>

#include <lispp/istream_tokenizer.h>

namespace lispp {

class FileTokenizer : public IstreamTokenizer {
public:
  FileTokenizer(const std::string& filename)
      : input_file_(filename) {
    set_input_stream(input_file_);
  }

private:
  std::ifstream input_file_;
};

} // lispp
