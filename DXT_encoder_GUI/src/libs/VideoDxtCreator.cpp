#define STB_DXT_IMPLEMENTATION
#include "stb_dxt.h"
#include "VideoDxtCreator.h"

namespace poly { 

  VideoDxtCreator::VideoDxtCreator()
    :version(VIDEO_DXT_VERSION_0_0_1)
  {
  }

  VideoDxtCreator::~VideoDxtCreator() {
    close();
  }

  int VideoDxtCreator::open(std::string outpath) {
    
    if (0 == outpath.size()) {
      CI_LOG_E("Given outpath is empty.");
      return -1;
    }

    ofs.open(outpath.c_str(), std::ios::binary | std::ios::out);
    if (!ofs.is_open()) {
      CI_LOG_E("Failed to open: " << outpath.c_str());
      return -2;
    }else{
        

        
    }

    return 0;
  }

  int VideoDxtCreator::create(std::string inpath,
                              int startFrame,
                              int endFrame)
  {

    char name[2048] = { 0 } ;
//    int allocated = 0;
    int channels = 0;
//    int r;
    int prev_w = 0;
    int prev_h = 0;
    int num_frames = 0;
    //    std::ofstream ofs(outpath.c_str(), std::ios::binary | std::ios::out);

//    if (VIDEO_DXT_VERSION_0_0_0 != version
//        && VIDEO_DXT_VERSION_0_0_1 != version)
//      {
//        CI_LOG_E("The version you've set is invalid. ");
//        return -1;
//      }

    /*
    if (0 == outpath.size()) {
      SX_ERROR("Given output path is invalid.");
      return -1;
    }
    */

//    if (0 == inpath.size()) {
//      CI_LOG_E("Given input path is invalid, size if 0.");
//      return -2;
//    }
//  
//    if (0 > startFrame) {
//      CI_LOG_E("Invalid start frame: " << startFrame);
//      return -4;
//    }
//
//    if (0 > endFrame || startFrame > endFrame) {
//      CI_LOG_E("Invalid endframe: " << endFrame);
//      return -5;
//    }
//
//    if (!ofs.is_open()) {
//      CI_LOG_E("Output file stream not open, did you call open()?");
//      return -6;
//    }
    

  //  CI_LOG_I("inpath: " << inpath.c_str());

    for (int i = startFrame; i <= endFrame; ++i) {

  //    sprintf(name, inpath.c_str(), i);

//      if (false == rx_file_exists(name)) {
//        CI_LOG_E("Cannot find " << name);
//        continue;
//      }
//
//      if ("png" != rx_get_file_ext(name)) {
//        CI_LOG_E("We only support png files.");
//        return -6;
//      }

//      uint64_t n = rx_hrtime();
//      r = rx_load_png(name, &pixels, w, h, channels, &allocated, RX_FLAG_LOAD_AS_RGBA);
//      if (0 >= r) {
//        SX_ERROR("Failed to load:" <<  name);
//        continue;
//      }

//      CI_LOG_I("Loaded " << w << " x " << h << ", channels: " << channels << " " << name);
//
//      if (0 != prev_w && prev_w != w) {
//        CI_LOG_E("Incorrect width, previous file had: " << prev_w << " now we have: " << w);
//        goto error;
//      }

//      if (0 == num_frames && VIDEO_DXT_VERSION_0_0_1 == version) {
//        if (0 != writeU32(0xCAFEBABE)) {
//          goto error;
//        }
//        
//        if (0 != writeU32(version)) {
//          goto error;
//        }
//        
//        if (0 != writeU32(w)) {
//          goto error;
//        }
//        
//        if (0 != writeU32(h)) {
//          goto error;
//        }
//        
//        if (0 != writeU32(0)) {
//          goto error;
//        }
//      }

      /* @todo somehow rygCompress crashes when not allocating a new buffer :/ */
      dxt = (unsigned char*) malloc(w * h);
      if (NULL == dxt) {
        CI_LOG_E("Failed to allocate the output buffer.");
        return -6;
      }
    
      rygCompress(dxt, pixels, w, h, 1);
    
      if (!ofs.write((const char*)dxt, w * h)) {
        CI_LOG_E("Failed to write DXT frame.");
        goto error;
      }

      free(dxt);
      dxt = NULL;

      if (!ofs) {
        CI_LOG_E("Failed to write dxt to output file.");
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


//  int VideoDxtCreator::close() {
//
//    if (ofs.is_open()) {
//      ofs.flush();
//      ofs.close();
//    }
//    
//    return 0;
//  }
    
    
    

    
    
    bool VideoDxtCreator::writePixels(unsigned char* pixels,int frames){
     
        
        if(frames == 0){
            writeU32(0xCAFEBABE);
            writeU32(version);
            writeU32(w);
            writeU32(h);
            writeU32(0);

        }
        
        dxt = (unsigned char*) malloc(w * h);
        if (NULL == dxt) {
            CI_LOG_E("Failed to allocate the output buffer.");
           // return -6;
        }
        
        rygCompress(dxt, pixels, w, h, 1);
        
        if (!ofs.write((const char*)dxt, w * h)) {
            CI_LOG_E("Failed to write DXT frame.");
           // goto error;
        }
        
        free(dxt);
        dxt = NULL;

        rewriteU32(16, frames);

        
        
        
        return true;

    }
    
    bool VideoDxtCreator::close(){
     
            if (ofs.is_open()) {
                ofs.flush();
                ofs.close();
                ofs.clear();
            
            }

        return true;

    }



} /* namespace poly */
