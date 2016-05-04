#define ROXLU_USE_PNG
#define ROXLU_IMPLEMENTATION
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <tinylib.h>
#include <poly/Log.h>

#include <poly/VideoDxtCreator.h>

using namespace poly;

int main(int argc, char** argv) {

  int begin_frame;
  int end_frame;
  std::string tmp;
  std::string out_filepath;
  std::string in_filepath;
  std::stringstream ss;
  VideoDxtCreator creator(VIDEO_DXT_VERSION_0_0_1);

  printf("\n\n\nDXT file creator\n\n");
  printf("-----------------------------------------------------------------------\n");
  printf("This utility will convert a PNG sequence into a fat DXT5 file that\n");
  printf("can be used with the VideoDxt* classes for video playback.\n");
  printf("The video width and height needs to a multiple of 4.\n");
  printf("-----------------------------------------------------------------------\n\n");

  printf("\nUsage:\n\n");
  printf("      %s in_filepath out_filepath start_frame end_frame\n", argv[0]);
  printf("\n\n");
  printf("Example:\n\n");
  printf("      %s ./data/sequence_%%04d.png ./data/video.dxt5 0 512\n", argv[0]);
  printf("\n\n");
  printf("-----------------------------------------------------------------------\n\n");

  poly_log_init();

  if (5 != argc) {
    printf("\nError: invalid arguments.\n\n");
    exit(EXIT_FAILURE);
  }

#if !defined(NDEBUG)  
  for (int i = 0; i < argc; ++i) {
    printf("%d = %s\n\n", i, argv[i]);
  }
#endif

  ss << argv[1];
  ss >> in_filepath;
  ss.clear();
  
  ss << argv[2];
  ss >> out_filepath;
  ss.clear();

  ss << argv[3];
  ss >> begin_frame;
  ss.clear();

  ss << argv[4];
  ss >> end_frame;
  ss.clear();

  printf("\n");
  printf("-----------------------------------------------------------------------\n");
  printf("begin_frame: %d\n", begin_frame);
  printf("end_frame: %d\n", end_frame);
  printf("out_filepath: %s\n", out_filepath.c_str());
  printf("in_filepath: %s\n", in_filepath.c_str());
  printf("-----------------------------------------------------------------------\n\n");

  if (0 != creator.open(out_filepath)) {
    exit(EXIT_FAILURE);
  }
  
  if (0 != creator.create(in_filepath, begin_frame, end_frame)) {
    printf("\nError: failed to create DXT file, see log.\n\n");
  }

  if (0 != creator.close()) {
    printf("\nError: failed to close teh DXT creator.\n\n");
  }

  return 0;
}
