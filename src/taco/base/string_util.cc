#include "taco/base/string_util.h"

#include <cassert>

namespace taco {

void Tokenize(std::vector<StringPiece> &output, StringPiece str,
              const std::string &delimiters)
{
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos) {
    // Found a token, add it to the vector.
    output.push_back(str.substr(lastPos, pos-lastPos));
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

void TokenizeMultiCharSeparator(std::vector<StringPiece> &output,
                                StringPiece str,
                                const std::string &separator) {
  std::size_t pos = 0;
  // Find first "non-delimiter".
  std::string::size_type nextPos = str.find(separator, pos);

  while (nextPos != std::string::npos) {
    // Found a token, add it to the vector.
    output.push_back(str.substr(pos, nextPos - pos));
    Trim(output.back());
    // Skip delimiters.  Note the "not_of"
    pos = nextPos + separator.size();
    // Find next "non-delimiter"
    nextPos = str.find(separator, pos);
  }
  output.push_back(str.substr(pos, nextPos - pos));
  Trim(output.back());
}

void Trim(StringPiece &str, const std::string &chars)
{
  std::string::size_type first = str.find_first_not_of(chars);
  if (first == std::string::npos) {
    str.clear();
    return;
  }
  std::string::size_type last = str.find_last_not_of(chars);
  assert(last != std::string::npos);
  // TODO Check StringPiece definition.  I think there are functions to
  // shrink the string.
  str = str.substr(first, last-first+1);
}

}  // namespace taco
