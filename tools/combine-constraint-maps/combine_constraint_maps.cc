#include "combine_constraint_maps.h"

#include "input_reader.h"
#include "options.h"

#include "tools-common/text-formats/constraint_map_parser.h"

#include "taco/base/string_piece.h"

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>
#include <sstream>

namespace taco {
namespace tool {

int CombineConstraintMaps::Main(int argc, char *argv[]) {
  // Process command-line options.
  Options options;
  ProcessOptions(argc, argv, options);

  const std::size_t num_inputs = options.input_files.size();

  // Open the input streams.
  std::vector<boost::shared_ptr<std::ifstream> > map_streams;
  for (std::size_t i = 0; i < num_inputs; ++i) {
    boost::shared_ptr<std::ifstream> map_stream(new std::ifstream());
    OpenNamedInputOrDie(options.input_files[i], *map_stream);
    map_streams.push_back(map_stream);
  }

  // Open the output stream.
  std::ostream &output = OpenOutputOrDie(options.output_file);

  // Initialise the constraint map readers.
  std::vector<boost::shared_ptr<InputReader> > cm_readers;
  for (std::size_t i = 0; i < num_inputs; ++i) {
    std::ostringstream desc;
    desc << "map file " << i;
    boost::shared_ptr<InputReader> reader(
        new InputReader(ConstraintMapParser(*map_streams[i]),
                        ConstraintMapParser(),
                        desc.str()));
    cm_readers.push_back(reader);
  }

  std::vector<InputReader *> lowest_readers;
  std::vector<int> indices;
  while (true) {
    // Determine the lowest key value and the corresponding set of InputReaders.
    const std::string *lowest_key = 0;
    lowest_readers.clear();
    indices.clear();
    for (std::size_t i = 0; i < num_inputs; ++i) {
      InputReader &reader = *cm_readers[i];
      if (!reader) {
        continue;
      }
      if (lowest_key == 0) {
        lowest_key = &reader.key;
        lowest_readers.push_back(&reader);
        indices.push_back(i);
        continue;
      }
      int ret = reader.key.compare(*lowest_key);
      if (ret == 0) {  // Key is same as lowest_key.
        lowest_readers.push_back(&reader);
        indices.push_back(i);
        continue;
      }
      if (ret < 0) {  // Key is lower than lowest_key.
        lowest_key = &reader.key;
        lowest_readers.clear();
        lowest_readers.push_back(&reader);
        indices.clear();
        indices.push_back(i);
      }
    }
    // Are all input readers at EOF?
    if (lowest_key == 0) {
      break;
    }
    // Produce a combined output line from the lowest readers.
    CombineEntries(lowest_readers, indices, output);
    // Advance the lowest readers by one line.
    for (std::vector<InputReader *>::const_iterator p = lowest_readers.begin();
         p != lowest_readers.end(); ++p) {
      (*p)->ReadLine();
    }
  }

  return 0;
}

void CombineConstraintMaps::CombineEntries(
    const std::vector<InputReader *> &readers,
    const std::vector<int> &indices,
    std::ostream &out) const {
  assert(!readers.empty());
  assert(readers.size() == indices.size());
  out << readers[0]->key << " |||";
  for (std::size_t i = 0; i < readers.size(); ++i) {
    InputReader &reader = *readers[i];
    WriteIds(reader->ids, indices[i], out);
  }
  out << std::endl;
}

void CombineConstraintMaps::WriteIds(const std::vector<StringPiece> &ids,
                                     int type_id,
                                     std::ostream &out) const {
  for (std::vector<StringPiece>::const_iterator p = ids.begin();
       p != ids.end(); ++p) {
    out << " " << type_id << ":" << *p;
  }
}

void CombineConstraintMaps::ProcessOptions(int argc, char *argv[],
                                           Options &options) const {
  namespace po = boost::program_options;

  std::ostringstream usage_top;
  usage_top << "Usage: " << name() << " [OPTION]... MAP...\n\n"
            << "Combine multiple single-type constraint maps to form one multi-type map. The\ninput map files must be sorted on the first column using the \"C\" locale.\n"
            << "Options";

  // Construct the 'bottom' of the usage message.
  std::ostringstream usage_bottom;  // Empty for now.

  // Declare the command line options that are visible to the user.
  po::options_description visible(usage_top.str());
  visible.add_options()
    ("help",
        "print help message and exit")
    ("output,o",
        po::value(&options.output_file),
        "write to arg instead of standard output")
  ;

  // Declare the command line options that are hidden from the user
  // (these are used as positional options).
  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("input-files",
        po::value(&options.input_files),
        "input files")
  ;

  // Compose the full set of command-line options.
  po::options_description cmd_line_options;
  cmd_line_options.add(visible).add(hidden);

  // Register the positional options.
  po::positional_options_description p;
  p.add("input-files", -1);

  // Process the command-line.
  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv).style(CommonOptionStyle()).
              options(cmd_line_options).positional(p).run(), vm);
    po::notify(vm);
  } catch (const std::exception &e) {
    std::ostringstream msg;
    msg << e.what() << "\n\n" << visible << usage_bottom.str();
    Error(msg.str());
  }

  if (vm.count("help")) {
    std::cout << visible << usage_bottom.str() << std::endl;
    std::exit(0);
  }

  // Check positional options were given.
  if (!vm.count("input-files")) {
    std::ostringstream msg;
    msg << "no input files given\n\n" << visible << usage_bottom.str()
        << std::endl;
    Error(msg.str());
  }
}

}  // namespace tool
}  // namespace taco
