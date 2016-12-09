#pragma once

#include <sstream>

#include <lispp/istream_tokenizer.h>

namespace lispp {

class StringTokenizer : public IstreamTokenizer {
public:
  StringTokenizer();
  StringTokenizer(const std::string& initial_value);

  void append(const std::string& initial_value);
  void clear() override;

  StringTokenizer& operator<<(const std::string& value) {
    append(value);
    return *this;
  }

  template<typename T>
  StringTokenizer& operator<<(const T& value) {
    // FIXME: OPTIMIZE:
    std::stringstream ss;
    ss << value;
    append(ss.str());
    return *this;
  }

private:
  std::stringstream input_ss_;
};

} // lispp
