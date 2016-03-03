/*

  VideoDxtPlayer
  ==============

  The `VideoDxtPlayer` will playback a fat dxt file that was created using `VideoCreatorDxt`.
  To use this player, you first use the `VideoCreatorDxt` to create a big fat 
  dxt file with the contents of a PNG sequences (only support png). Then you load it
  and call `play(25)`. This will start loading frames at 25fps and it will call the 
  listeners whenever a new frame has been read. It's up to the user to draw the frames
  using e.g. OpenGL and DXT5 textures. Make sure to call `update()` often because it will
  make sure that it fetches a new frame whenever necessary. 

*/
#ifndef POLY_VIDEO_PLAYER_DXT_H
#define POLY_VIDEO_PLAYER_DXT_H

#include <string>
#include <vector>
#include <fstream>
#include <stdint.h>
#include "VideoDxtPlayerListener.h"
#include "cinder/log.h"

using namespace poly;

#define VIDEO_DXT_STATE_NONE 0
#define VIDEO_DXT_STATE_PLAYING (1 << 0)
#define VIDEO_DXT_STATE_PAUSED (1 << 1)
#define VIDEO_DXT_STATE_STOPPED (1 << 2)

#define VIDEO_DXT_MODE_NORMAL 0                                                      /* Plays once, then stops. */ 
#define VIDEO_DXT_MODE_PALINDROME 1                                                  /* Will continuously play, when the end frame has been reached we switch back tot he first one. */

/* --------------------------------------------------------------------------------- */
namespace poly {
  
  class VideoDxtPlayer {
  public:
    VideoDxtPlayer();                                                                   /* Sets the defaults. */
    ~VideoDxtPlayer();                                                                  /* Cleans up everything, closes file if it was opened and stops the video. */
    int init(std::string filepath);                                                     /* Load the DXT5 file that was created using `VideoDxtCreator`. */
    void loop();                                                                        /* Set the mode to VIDEO_DXT_MODE_PALINDROME. */
    void noloop();                                                                      /* Set the mode to VIDEO_DXT_MODE_NORMAL */
    void shutdown();                                                                    /* Closes the file, cleans up all allocated bytes, stops playing. */
    void addVideoPlayerListener(VideoDxtPlayerListener* lis);                           /* Adds a listener that will receive video frames whenever it needs to be drawn. */
    void notifyVideoPlayerEvent(int type);                                              /* Notifies all listeners for the given event. */
    void play(int fps);                                                                 /* Play the loaded video file. */
    void pause();                                                                       /* Pause playback. */
    void stop();                                                                        /* Stop video playback. */
    int update();                                                                       /* Call this often so we can notify the listeners when they need to handle a new video frame. Returns 0 on success and when we didn't read a new frame. Returns 1 when we read a new frame and returns < 0 on error.*/

    /* Get properties. */
    int getWidth();                                                                     /* We keep track of the given width/height that were passed into init(). This returns the width of the video as passed into `init()`. */
    int getHeight();                                                                    /* Get the height of the video as passed into `init()`. */
    int getBytesPerFrame();                                                             /* The number of bytes for one video frame; with DXT5 this will always be the same: `w * h`. */
    unsigned char* getBufferPtr();                                                      /* We copy each frame into a buffer, before calling the listeners. You can use this function get a pointer to the current DXT5 buffer. */
    int64_t getCurrentFrameNumber();                                                   /* Returns the last loaded frame number. */

    /* Check state. */
    int isPlaying();                                                                    /* Returns 0 if it's playing, otherwise -1. */ 
    int isPaused();                                                                     /* Returns 0 if it's paused, otherwise -1. */ 
    int isStopped();                                                                    /* Returns 0 if it's stopped, otherwise -1.*/ 
    
  private:
    int readCurrentFrame();                                                             /* This will read the currently set frame form the file (using `curr_frame` and `bytes_per_frame`) */ 
    int readU32(uint32_t& v);                                                           /* Reads a uint32_t which and assumes it's stored in BE format. Returns 0 on success, otherwise < 0.*/
  
  public:
    std::ifstream ifs;                                                                  /* The filestream that reads the DXT5 file.*/
    uint32_t video_width;                                                               /* The video width passed to `init()`. */
    uint32_t video_height;                                                              /* The video height passed to `init()`. */
    uint32_t num_frames;                                                                /* The total number of frames in the loaded files. */
    uint32_t version;                                                                   /* The version of the VideoDxtCreator. */
    uint32_t num_bytes_in_header;                                                       /* Based on the creator version this will be set to e.g. 20 (for version 1, used to calculate the read offset). */
    size_t filesize;                                                                    /* The total file size; used to calculate the total number of frames.*/
    size_t bytes_per_frame;                                                             /* The number of bytes per frames, `w * h` for DXT5. */
    uint64_t time_started;                                                              /* The time in ns. when you called `play()`. */
    uint64_t time_per_frame;                                                            /* The time that we need to show a frame, based on the `fps` given into `play()`.*/
    int64_t curr_frame;                                                                /* The currently used/read frame number. */
    int64_t prev_frame;
    int mode;                                                                           /* The mode, when set to VIDEO_DXT_MODE_NORMAL we will playback and stop at the end, when VIDEO_DXT_MODE_PALINDROME is used (see loop()), we start over. */
    int state;                                                                          /* The state of the player, see above. */
    unsigned char* frame_buffer;                                                        /* The buffer that we use to read the frame into. */
    std::vector<VideoDxtPlayerListener*> video_listeners;                               /* The listeners that will be notified when we read a new frame. */
  };

  /* --------------------------------------------------------------------------------- */

  inline int VideoDxtPlayer::isPlaying() {
    return (state & VIDEO_DXT_STATE_PLAYING) ? 0 : -1;
  }

  inline int VideoDxtPlayer::isPaused() {
    return (state & VIDEO_DXT_STATE_PAUSED) ? 0 : -1;
  }

  inline int VideoDxtPlayer::isStopped() {
    return (state & VIDEO_DXT_STATE_STOPPED) ? 0 : -1;
  }

  inline void VideoDxtPlayer::addVideoPlayerListener(VideoDxtPlayerListener* lis) {

    if (NULL == lis) {
      CI_LOG_E("Listener is NULL.");
      return;
    }

    video_listeners.push_back(lis);
  }

  inline void VideoDxtPlayer::notifyVideoPlayerEvent(int type) {
  
    VideoDxtPlayerEvent ev;
    ev.type = type;
    ev.player = this;

    for (size_t i = 0; i < video_listeners.size(); ++i) {
      video_listeners[i]->onVideoDxtPlayerEvent(ev);
    }
  }

  inline int VideoDxtPlayer::getWidth() {
    return video_width;
  }

  inline int VideoDxtPlayer::getHeight() {
    return video_height;
  }

  inline int VideoDxtPlayer::getBytesPerFrame() {
    return bytes_per_frame;
  }

  inline unsigned char* VideoDxtPlayer::getBufferPtr() {
    return frame_buffer;
  }

  inline int VideoDxtPlayer::readCurrentFrame() {

    int64_t byte_offset;
    size_t i = 0;

    /* Seek to the new position. */
    byte_offset = num_bytes_in_header + (curr_frame * bytes_per_frame);
    ifs.seekg(byte_offset, ifs.beg);

    if (!ifs) {
        CI_LOG_E("Failed to seek to " << byte_offset);
      return -1;
    }

    ifs.read((char*)frame_buffer, bytes_per_frame); /* from HDD this takes about 4ms */
    if (!ifs) {
		CI_LOG_E("Failed to read a frame! Not supposed to happen!.");
      return -2;
    }

    for (i = 0; i < video_listeners.size(); ++i) {
      video_listeners[i]->onVideoDxtPlayerFrame(frame_buffer, bytes_per_frame);
    }

    return 0;
  }

  inline void VideoDxtPlayer::loop() {
    mode = VIDEO_DXT_MODE_PALINDROME;
  }

  inline void VideoDxtPlayer::noloop() {
    mode = VIDEO_DXT_MODE_NORMAL;
  }

  inline int VideoDxtPlayer::readU32(uint32_t& result) {
  
    uint8_t* p = (uint8_t*)&result;

    if (false == ifs.is_open()) {
		CI_LOG_E("Cannot read because the file hasn't been opened.\n");
      return -1;
    }
    
    if (!ifs.read((char*)(p + 3), 1)) {
      return -2;
    }
    
    if (!ifs.read((char*)(p + 2), 1)) {
      return -3;
    }
    
    if (!ifs.read((char*)(p + 1), 1)) {
      return -4;
    }
    
    if (!ifs.read((char*)(p + 0), 1)) {
      return -5;
    }
    
    return 0;
  }

  inline int64_t VideoDxtPlayer::getCurrentFrameNumber() {
    return curr_frame;
  }

} /* namespace poly */

#endif
