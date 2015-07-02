/*

  VideoDxtDrawer
  ==============

  A simple DXT based video player drawer. We use the `VideoDxtPlayer` class which 
  takes care of all video player logic like pause, stop, play, etc.. and whenever
  we read a new frame we update a texture. Using draw() we draw it to screen. 

  Usage
  -----
  - call `init()` to initialize the player.
  - call `bindProgram()` then `setProjectionMatrix(),  `setViewMatrix()` and `setModelMatrix()`.
  - call `update()` at the (at least) the same speed as the framerate. 
  - call `draw()`.
  - when ready call `shutdown()` to free memory. 

  Portable
  ---------
  To make this player a bit more portable we added the following  defines which changes 
  some function calls and includes. We expect *YOU* to set these whenever necessary.

  DXT_PLAYER_USE_GLAD            When defined we include the cross platform "glad" opengl includer.
  DXT_PLAYER_USE_CINDER          When defined we include the GL header of libcinder. 
  DXT_PLAYER_USE_APPLE_VAO       When defined we use `glGenVertexArraysAPPLE` and `glBindVertexArraysAPPLE` to support GL 2.1 
  DXT_PLAYER_GLSL_VERSION_330    Define this when you want to use glsl 330, otherwise...
  DXT_PLAYER_GLSL_VERSION_120    .. define this one. We default to 330


  Return values of the `update*()` functions
  --------------------------------------------

  You should use the return values of the `update*()` functions to detect if  
  we read a new frame from the buffer. The returns should be interpreted as:

       0   = Everything okay, we didn't read a new frame.
       1   = We read a new frame.
       < 0 = We return < 0 on error. 

 */
#ifndef POLY_VIDEO_DXT_DRAWER_H
#define POLY_VIDEO_DXT_DRAWER_H

#if !defined(DXT_PLAYER_GLSL_VERSION_330) && !defined(DXT_PLAYER_GLSL_VERSION_120)
#  define DXT_PLAYER_GLSL_VERSION_330
#endif

#include <assert.h>
#if defined(DXT_PLAYER_USE_GLAD)
#  include <glad/glad.h>
#endif
#if defined(DXT_PLAYER_USE_CINDER)
#  include "cinder/gl/gl.h"
#endif
#include "VideoDxtPlayer.h"

namespace poly {

  /* --------------------------------------------------------------------------------- */



  /* --------------------------------------------------------------------------------- */

  class VideoDxtDrawer {
  public:
    VideoDxtDrawer();
    ~VideoDxtDrawer();
    int init(std::string filepath);
    int shutdown();
    int worker();                                                             /* Read a new frame from the buffer and upload the texture when necessary. This combines `updateBuffer()` and `updateTexture()` */
                                      /* Bind the texture on the given texture unit; we do: `GL_TEXTURE0 + unit`. */
    void uploadTexture();
    void draw();
    void draw(ci::Vec2f);
    void draw(ci::Rectf frame);                                                              /* Draw using the set projection-,view- and model matrices. */
    void play(int fps);                                                       /* Start playing with using `fps` as framerate (e.g. 24). */
    void loop();                                                              /* Enable looping. */
    void noloop();                                                            /* Set mode to normal */
    int getWidth();                                                           /* Get the width of the video. */
    int getHeight();                                                          /* Get the height of hte video. */
	
  private:
	  char * localBuffer;
	  int localBufferSize;
      GLuint texture;

  public:
    VideoDxtPlayer player;                                                   /* The `VideoDxtPlayer` handles all the video logic, like looping, reading new frames when necessary, pause, play, etc.. */ 
                                                              /* The texture ID. */
    float alpha;                                                             /* The alpha for the video. */  
    int has_new_frame;                                                       /* Is set to 0 when we read a new from from the DXT buffer, otherwise < -1. */
                                                      /* Texture uniform. */
                                                  
  };

  /* --------------------------------------------------------------------------------- */


  inline int VideoDxtDrawer::getWidth() {
    return player.getWidth();
  }

  inline int VideoDxtDrawer::getHeight() {
    return player.getHeight();
  }

  inline void VideoDxtDrawer::play(int fps) {
    player.play(fps);
  }

  inline void VideoDxtDrawer::loop() {
    player.loop();
  }

  inline void VideoDxtDrawer::noloop() {
    player.noloop();
  }

  
  
} /* namespace poly */

#endif
