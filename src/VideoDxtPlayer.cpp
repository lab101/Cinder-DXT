#include "VideoDxtPlayer.h"
#include <stdint.h>

/* 
  --------------------------------------------------------------------------------- 

  Easy embeddable cross-platform high resolution timer function. For each 
  platform we select the high resolution timer. You can call the 'ns()' 
  function in your file after embedding this. 

  See: http://roxlu.com/2014/047/high-resolution-timer-function-in-c-c-- 

  --------------------------------------------------------------------------------- 
*/
#if defined(__linux)
  #  define HAVE_POSIX_TIMER
  #  include <time.h>
  #  ifdef CLOCK_MONOTONIC
  #     define CLOCKID CLOCK_MONOTONIC
  #  else
  #     define CLOCKID CLOCK_REALTIME
  #  endif
#elif defined(__APPLE__)
  #  define HAVE_MACH_TIMER
  #  include <mach/mach_time.h>
#elif defined(_WIN32)
  #  define WIN32_LEAN_AND_MEAN
  #  include <windows.h>
#endif

  static uint64_t ns() {
    static uint64_t is_init = 0;
#if defined(__APPLE__)
    static mach_timebase_info_data_t info;
    if (0 == is_init) {
      mach_timebase_info(&info);
      is_init = 1;
    }
    uint64_t now;
    now = mach_absolute_time();
    now *= info.numer;
    now /= info.denom;
    return now;
#elif defined(__linux)
    static struct timespec linux_rate;
    if (0 == is_init) {
      clock_getres(CLOCKID, &linux_rate);
      is_init = 1;
    }
    uint64_t now;
    struct timespec spec;
    clock_gettime(CLOCKID, &spec);
    now = spec.tv_sec * 1.0e9 + spec.tv_nsec;
    return now;
#elif defined(_WIN32)
    static LARGE_INTEGER win_frequency;
    if (0 == is_init) {
      QueryPerformanceFrequency(&win_frequency);
      is_init = 1;
    }
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (uint64_t) ((1e9 * now.QuadPart)  / win_frequency.QuadPart);
    #endif
  }

/* --------------------------------------------------------------------------------- */

namespace poly {
  
  VideoDxtPlayer::VideoDxtPlayer() 
    :video_width(0)
    ,video_height(0)
    ,num_frames(0)
    ,version(0)
    ,num_bytes_in_header(0)
    ,filesize(0)
    ,bytes_per_frame(0)
    ,time_started(0)
    ,time_per_frame(0)
    ,curr_frame(0)
    ,prev_frame(-1)
    ,mode(VIDEO_DXT_MODE_NORMAL)
    ,state(VIDEO_DXT_STATE_NONE)
    ,frame_buffer(NULL)
  {
  }

  VideoDxtPlayer::~VideoDxtPlayer() {

    shutdown();

    video_listeners.clear();
  }

  int VideoDxtPlayer::init(std::string filepath) {

    uint32_t magic;

    if (true == ifs.is_open()) {
      SX_ERROR("Already loaded %s, call shutdown if you want to reload.", filepath.c_str());
      return -1;
    }

    if (0 == filepath.size()) {
      SX_ERROR("Invalid filepath; size is 0.");
      return -2;
    }
  
    ifs.open(filepath.c_str(), std::ios::binary | std::ios::in);
    if (!ifs.is_open()) {
      SX_ERROR("Failed to open: %s", filepath.c_str());
      return -6;
    }

    ifs.seekg(0, std::ifstream::end);
    filesize = ifs.tellg();
    ifs.seekg(0, std::ios_base::beg);

    if (0 == filesize) {
      ifs.close();
      SX_ERROR("File size is 0 (%s).", filepath.c_str());
      return -7;
    }
    
    /* Read the header. */
    if (0 != readU32(magic)) {
      SX_ERROR("Failed to read the magic.");
      ifs.close();
      return -8;
    }
    
    if (0xCAFEBABE != magic) {
      ifs.close();
      SX_ERROR("Failed to read the magic cookie. Did you create the file using VideoDxtCreator?");
      return -8;
    }

    if (0 != readU32(version)) {
      SX_ERROR("Failed to read the version.");
      ifs.close();
      return -9;
    }
    
    if (1 != version) {
      ifs.close();
      SX_ERROR("The version of the DXT file is not supported by the player. Read version: %d", version);
      return -9;
    }

    if (0 != readU32(video_width)) {
      SX_ERROR("Failed to read the video width.");
      ifs.close();
      return -10;
    }
    
    if (0 != readU32(video_height)) {
      SX_ERROR("Failed to read the video height.");
      ifs.close();
      return -11;
    }
    
    if (0 != readU32(num_frames)) {
      SX_ERROR("Failed to read the number of frames.");
      ifs.close();
      return -12;
    }

    if (0 == video_width || 4096 < video_width) {
      SX_ERROR("Video width is invalid. Either 0 or bigger then 4096 what we don't support yet. Video width: %u", video_width);
      ifs.close();
      return -13;
    }

    if (0 == video_height || 4096 < video_height) {
      SX_ERROR("Video height is invalid. either 0 or bigger then 4096 what we don't support yet. Video height: %u", video_height);
      ifs.close();
      return -14;
    }
    
    /* Ready reading the header... */
    num_bytes_in_header = ifs.tellg();
    
    bytes_per_frame = video_width * video_height;

#if defined(NDEBUG)    
  /*  SX_VERBOSE("%s: video width: %u, video height: %u, num_frames: %u, bytes_per_frame: %u, version: %u, read position: %lu",
               filepath.c_str(),
               video_width,
               video_height,
               num_frames,
               bytes_per_frame,
               version,
               (int)ifs.tellg()
               );*/
#endif


    if (filesize <= 20 || ((filesize - 20) < (bytes_per_frame * num_frames)) ) {
      SX_ERROR("Invalid filesize.");
      ifs.close();
      return -15;
    }
    
    /* Allocate the frame buffer. */
    frame_buffer = (unsigned char*)malloc(bytes_per_frame);
    if (NULL == frame_buffer) {
      SX_ERROR("Failed to allocate the frame buffer.");
      ifs.close();
      return -8;
    }
  
    /* Read the first frame */
    if (0 != readCurrentFrame()) {
      SX_ERROR("Failed to read the first frame.");
      ifs.close();
      return -17;
    }
  
    return 0;
  }

  void VideoDxtPlayer::shutdown() {
  
    if (ifs.is_open()) {
      ifs.close();
    }

    if (NULL != frame_buffer) {
      free(frame_buffer);
      frame_buffer = NULL;
    }

    video_width = 0;
    video_height = 0;
    version = 0;
    num_bytes_in_header = 0;
    filesize = 0;
    bytes_per_frame = 0;
    num_frames = 0;
    time_started = 0;
    time_per_frame = 0;
    curr_frame = 0;
    prev_frame = -1;
    state = VIDEO_DXT_STATE_NONE;
  }

  void VideoDxtPlayer::play(int fps) {

#if !defined(NDEBUG)
    if (!ifs.is_open()) {
      SX_ERROR("Trying to play, but the file stream is not opened. Did you call init()?");
      return;
    }
#endif
  
    if (0 == isPlaying()) {
      SX_ERROR("Already playing.");
      return;
    }

    time_per_frame = double(1.0 / fps) * 1e9;
    time_started = ns() - (curr_frame * time_per_frame) ;

    state |= VIDEO_DXT_STATE_PLAYING;
    state &= ~VIDEO_DXT_STATE_PAUSED;
    state &= ~VIDEO_DXT_STATE_STOPPED;

    notifyVideoPlayerEvent(VIDEO_DXT_EVENT_PLAY);
  }

  void VideoDxtPlayer::pause() {

    if (0 != isPlaying()) {
      SX_ERROR("Cannot pause because we're not playing.");
      return;
    }

    if (0 == isPaused()) {
#if !defined(NDEBUG)
      SX_VERBOSE("Calling pause() on a video that's already paused.");
#endif
      return;
    }

    state |= VIDEO_DXT_STATE_PAUSED;
    state &= ~VIDEO_DXT_STATE_PLAYING;

    notifyVideoPlayerEvent(VIDEO_DXT_EVENT_PAUSE);
  }

  void VideoDxtPlayer::stop() {

    if (0 != isPlaying() && 0 != isPaused()) {
      SX_ERROR("Cannot stop because we're not paying or paused.");
      return;
    }

    curr_frame = 0;
    prev_frame = -1;

    state |= VIDEO_DXT_STATE_STOPPED;
    state &= ~VIDEO_DXT_STATE_PLAYING;
    state &= ~VIDEO_DXT_STATE_PAUSED;

    notifyVideoPlayerEvent(VIDEO_DXT_EVENT_STOP);
  }

  /*
    Return values:

       0   = Everything okay, we didn't read a new frame.
       1   = We read a new frame, you can use `getBufferPtr()` to get a pointer to the new frame. 
       < 0 = We return < 0 on error. 
   */
  int VideoDxtPlayer::update() {
 
    uint64_t now;
    uint64_t delta;
    int64_t new_frame;

#if !defined(NDEBUG)
    if (!ifs.is_open()) {
      SX_ERROR("Trying to update, but the file stream is not opened. Did you call init()?");
      return -1;
    }
#endif

    if (state & VIDEO_DXT_STATE_PAUSED) {
      return 0;
    }
    else if (state & VIDEO_DXT_STATE_STOPPED) {
      return 0;
    }
    else if ( (state & VIDEO_DXT_STATE_PLAYING) != VIDEO_DXT_STATE_PLAYING) {
      return 0;
    }

    now = ns();
    delta = now - time_started;
    new_frame = delta / time_per_frame;
    
    if (new_frame <= prev_frame) {

      return 0;
    }

    prev_frame = new_frame;

    /* When we reach the last frame, we set the next frame depending on the play mode. */
    if (new_frame >= num_frames) {


      if (VIDEO_DXT_MODE_NORMAL == mode) {
        stop();
      }
      else if (VIDEO_DXT_MODE_PALINDROME == mode) {
        prev_frame = -1;
        curr_frame = 0;
        time_started = ns();
      }
      else {
        SX_ERROR("Unhandled play mode.");
      }
      return 0;
    }

    curr_frame = new_frame;

    if (0 != readCurrentFrame()) {
      return -2;
    }

    return 1;
  }

} /* namespace poly */
