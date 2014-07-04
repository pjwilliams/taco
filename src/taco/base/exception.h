#ifndef TACO_SRC_TACO_BASE_EXCEPTION_H_
#define TACO_SRC_TACO_BASE_EXCEPTION_H_

#include <string>

namespace taco {

class Exception {
 public:
  Exception(const char *msg) : msg_(msg) {}
  Exception(const std::string &msg) : msg_(msg) {}

  const std::string &msg() const { return msg_; }

 private:
  std::string msg_;
};

}  // namespace taco

#endif
