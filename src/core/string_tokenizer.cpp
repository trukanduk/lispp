#include <lispp/string_tokenizer.h>

namespace lispp {

StringTokenizer::StringTokenizer() {
  set_input_stream(input_ss_);
}

StringTokenizer::StringTokenizer(const std::string& initial_value) {
  set_input_stream(input_ss_);
  append(initial_value);
}

void StringTokenizer::clear() {
  input_ss_.clear();
  next_token(); // NOTE: eat cached token
  next_token();
}

void StringTokenizer::append(const std::string& value) {
  /* NOTE: we should clear the stringstream to clear flags.
   *       This shouldn't break the underlying tokenizer
   *       if stream is already ended
   */
  if (!input_ss_.good()) {
    input_ss_.clear();
  }

  input_ss_<< value;
}

} // lispp
