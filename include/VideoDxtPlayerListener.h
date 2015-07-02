#ifndef POLY_VIDEO_PLAYER_DXT_LISTENER_H
#define POLY_VIDEO_PLAYER_DXT_LISTENER_H

#define VIDEO_DXT_EVENT_PLAY 0
#define VIDEO_DXT_EVENT_PAUSE 1
#define VIDEO_DXT_EVENT_STOP 2

namespace poly {
  
  /* --------------------------------------------------------------------------------- */

  class VideoDxtPlayer;

  /* --------------------------------------------------------------------------------- */

  class VideoDxtPlayerEvent {
  public:
    int type;
    VideoDxtPlayer* player;
  };

  /* --------------------------------------------------------------------------------- */

  class VideoDxtPlayerListener {
  public:
    virtual void onVideoDxtPlayerFrame(unsigned char* pixels, size_t nbytes) { }
    virtual void onVideoDxtPlayerEvent(VideoDxtPlayerEvent& ev) {}
  };
 
} /* namespace poly */


#endif
