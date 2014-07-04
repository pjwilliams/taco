#include "m1_extract_vocab.h"

int main(int argc, char *argv[]) {
  taco::tool::m1::ExtractVocab tool;
  return tool.Main(argc, argv);
}
