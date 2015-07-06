#define STB_DXT_IMPLEMENTATION
#include <stb_dxt.h>
#include <poly/VideoDxtCreator.h>

namespace poly { 

  VideoDxtCreator::VideoDxtCreator(int version)
    :version(version)
  {
  }

  VideoDxtCreator::~VideoDxtCreator() {
    close();
  }

  int VideoDxtCreator::open(std::string outpath) {
    
    if (0 == outpath.size()) {
      SX_ERROR("Given outpath is empty.");
      return -1;
    }

    ofs.open(outpath.c_str(), std::ios::binary | std::ios::out);
    if (!ofs.is_open()) {
      SX_ERROR("Failed to open: `%s`.", outpath.c_str());
      return -2;
    }

    return 0;
  }

  int VideoDxtCreator::create(std::string inpath,
                              int startFrame,
                              int endFrame)
  {

    char name[2048] = { 0 } ;
    int w = 0;
    int h = 0; 
    int allocated = 0;
    int channels = 0;
    unsigned char* pixels = NULL;
    int r;
    int prev_w = 0;
    int prev_h = 0;
    int num_frames = 0;
    unsigned char* dxt = NULL;
    //    std::ofstream ofs(outpath.c_str(), std::ios::binary | std::ios::out);

    if (VIDEO_DXT_VERSION_0_0_0 != version
        && VIDEO_DXT_VERSION_0_0_1 != version)
      {
        SX_ERROR("The version you've set is invalid. ");
        return -1;
      }

    /*
    if (0 == outpath.size()) {
      SX_ERROR("Given output path is invalid.");
      return -1;
    }
    */

    if (0 == inpath.size()) {
      SX_ERROR("Given input path is invalid, size if 0.");
      return -2;
    }
  
    if (0 > startFrame) {
      SX_ERROR("Invalid start frame: %d.", startFrame);
      return -4;
    }

    if (0 > endFrame || startFrame > endFrame) {
      SX_ERROR("Invalid endframe: %d", endFrame);
      return -5;
    }

    if (!ofs.is_open()) {
      SX_ERROR("Output file stream not open, did you call open()?");
      return -6;
    }
    

    SX_VERBOSE("inpath: %s", inpath.c_str());

    for (int i = startFrame; i <= endFrame; ++i) {

      sprintf(name, inpath.c_str(), i);

      if (false == rx_file_exists(name)) {
        SX_ERROR("Cannot find %s", name);
        continue;
      }

      if ("png" != rx_get_file_ext(name)) {
        SX_ERROR("We only support png files.");
        return -6;
      }

      uint64_t n = rx_hrtime();
      r = rx_load_png(name, &pixels, w, h, channels, &allocated, RX_FLAG_LOAD_AS_RGBA);
      if (0 >= r) {
        SX_ERROR("Failed to load: %s (%d)", name, r);
        continue;
      }

      SX_VERBOSE("Loaded %d x %d, channels: %d (%s)", w, h, channels, name);

      if (0 != prev_w && prev_w != w) {
        SX_ERROR("Incorrect width, previous file had: %d now we have: %d", prev_w, w);
        goto error;
      }

      if (0 == num_frames && VIDEO_DXT_VERSION_0_0_1 == version) {
        if (0 != writeU32(0xCAFEBABE)) {
          goto error;
        }
        
        if (0 != writeU32(version)) {
          goto error;
        }
        
        if (0 != writeU32(w)) {
          goto error;
        }
        
        if (0 != writeU32(h)) {
          goto error;
        }
        
        if (0 != writeU32(0)) {
          goto error;
        }
      }

      /* @todo somehow rygCompress crashes when not allocating a new buffer :/ */
      dxt = (unsigned char*) malloc(w * h);
      if (NULL == dxt) {
        SX_ERROR("Failed to allocate the output buffer.");
        return -6;
      }
    
      rygCompress(dxt, pixels, w, h, 1);
    
      if (!ofs.write((const char*)dxt, w * h)) {
        SX_ERROR("Failed to write DXT frame.");
        goto error;
      }

      free(dxt);
      dxt = NULL;

      if (!ofs) {
        SX_ERROR("Failed to write dxt to output file.");
        goto error;
      }

      prev_w = w;
      prev_h = h;
      num_frames++;
    }

    if (VIDEO_DXT_VERSION_0_0_1 == version) {
      rewriteU32(16, num_frames);
    }

    if (pixels) {
      free(pixels);
      pixels = NULL;
    }

    return 0;

  error:
    if (NULL != pixels) {
      free(pixels);
      pixels = NULL;
    }

    if (NULL != dxt) {
      free(dxt);
      dxt = NULL;
    }
    
    close();

    return -666;
  }


  int VideoDxtCreator::close() {

    if (ofs.is_open()) {
      ofs.flush();
      ofs.close();
    }
    
    return 0;
  }


} /* namespace poly */
