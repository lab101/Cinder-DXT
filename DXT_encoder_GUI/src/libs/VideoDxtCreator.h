/* 

   VideoDxtCreator
   ===============

   This class is used to create a fat DXT file with multiple files concatenated 
   in it. We make use of the fantastic DXT repository of [Yann Collet](https://github.com/Cyan4973/RygsDXTc).
   Tested using commit `8b983d43c85f84c1c16665c1661b16acd1f6f429` of this repository.

   +---------+------------+-------------------------------------+----------------+
   | Offset  | Datatype   | Info                                | Since version  |
   +---------+------------+-------------------------------------+----------------+
   |   0     | uint32_t   | Magic number                        | 0.0.1          |
   +---------+------------+-------------------------------------+----------------+
   +   4     | uint32_t   | Version                             | 0.0.1          |
   +---------+------------+-------------------------------------+----------------+
   |   8     | uint32_t   | Width of the frames                 | 0.0.1          |
   +---------+------------+-------------------------------------+----------------+
   |   12    | uint32_t   | Height of the frames                | 0.0.1          |           
   +---------+------------+-------------------------------------+----------------+
   |   16    | uint32_t   | Number of frames stored in the file | 0.0.1          |
   +---------+------------+-------------------------------------+----------------+

   Usage:
   ------
   
   Call `create()` to compress the PNG files in the `inPath` into a DXT5 file in 
   `outPath`.

   ````c++

    std::string outfile = rx_to_data_path("out_vid_bunny.dxt5");
    std::string indir = rx_to_data_path("frames_bunny/out_%04d.png");
    int vid_width = 1280;
    int vid_height = 720;

    VideoDxtCreator video_creator;
    if (0 != video_creator.create(outfile, indir, 1, 100)) {
      SX_ERROR("Failed to create dxt5 video.");
      exit(EXIT_FAILURE);
    }

   ````

 */
#ifndef ROXLU_VIDEO_CREATOR_DXT5_H
#define ROXLU_VIDEO_CREATOR_DXT5_H

//#define ROXLU_USE_PNG

#include "cinder/log.h"
#include "tinylib.h"
#include <stdint.h>
#include <fstream>



#define VIDEO_DXT_VERSION_0_0_0 0      /* The first release; simply wrotes every file concatenated into the output. */
#define VIDEO_DXT_VERSION_0_0_1 1      /* Added a bit more logic and storing the width, height and number of encoded frames. */

/* --------------------------------------------------------------------------------- */

namespace poly {
  
  class VideoDxtCreator {
  public:
    VideoDxtCreator();
    ~VideoDxtCreator();
    int open(std::string outpath);
    int create(std::string inpath, int startFrame, int endFrame);
   // int close();
    int writeU32(uint32_t v);                        /* Write big endian uint32_t. */  
    int rewriteU32(uint32_t offset, uint32_t v);     /* Rewrite at the given file offset. */
    
      bool writePixels(unsigned char* pixels,int frame);
      bool close();
      
      
  public:
    int version;
    std::ofstream ofs;
      
      
      unsigned char* dxt = NULL;
      unsigned char* pixels = NULL;
      int w = 0;
      int h = 0;

  };

  /* --------------------------------------------------------------------------------- */

  inline int VideoDxtCreator::writeU32(uint32_t v) {
    
    uint8_t* p = (uint8_t*)&v;
 
    if (!ofs.is_open()) {
      CI_LOG_E("Cannot writeU32 because the file isn't opened.");
      return -1;
    }
    
    if (!ofs.write((const char*)p + 3, 1)) {
      return -2;
    }
    
    if (!ofs.write((const char*)p + 2, 1)) {
      return -3;
    }
    
    if (!ofs.write((const char*)p + 1, 1)) {
      return -4;
    }
    
    if (!ofs.write((const char*)p + 0, 1)) {
      return -5;
    }

    ofs.flush();
    
    return 0;
  }

  inline int VideoDxtCreator::rewriteU32(uint32_t offset, uint32_t v) {

    uint8_t* p = (uint8_t*)&v;
    long curr_offset = 0;
    
    if (!ofs.is_open()) {
      CI_LOG_E("Cannot rewriteU32 because the output file hasn't been opened.");
      return -1;
    }

    curr_offset = ofs.tellp();
    ofs.seekp(offset, std::ios_base::beg);
    
    if (!ofs) {
      CI_LOG_E("Failed to set the write position.");
      return -2;
    }

    if (!ofs.write((const char*)p + 3, 1)) {
      return -3;
    }
    
    if (!ofs.write((const char*)p + 2, 1)) {
      return -4;
    }
    
    if (!ofs.write((const char*)p + 1, 1)) {
      return -5;
    }
    
    if (!ofs.write((const char*)p + 0, 1)) {
      return -6;
    }

    ofs.flush();

    ofs.seekp(curr_offset, std::ios_base::beg);
    if (!ofs) {
      CI_LOG_E("Failed to jump back to the rewrite start position.");
      return -7;
    }

    return 0;
  }

  /* --------------------------------------------------------------------------------- */
  
} /* namespace poly */
  
#endif
