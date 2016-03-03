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

#include "VideoDxtPlayer.h"

#include "cinder/gl/GlslProg.h"
#include "cinder/Timeline.h"
#include "cinder/Log.h"
#include "cinder/gl/gl.h"


namespace poly {

	class VideoDxtDrawer {
	public:
		VideoDxtDrawer();
		~VideoDxtDrawer();
		int init(std::string filepath);
		int shutdown();
		int worker();                                                             /* Read a new frame from the buffer and upload the texture when necessary. This combines `updateBuffer()` and `updateTexture()` */
		/* Bind the texture on the given texture unit; we do: `GL_TEXTURE0 + unit`. */
		void uploadTexture();
		void update();
		

		void draw(cinder::vec2);
		void draw(ci::Rectf frame);                                                              /* Draw using the set projection-,view- and model matrices. */
		void play(int fps);                                                       /* Start playing with using `fps` as framerate (e.g. 24). */
		void loop();                                                              /* Enable looping. */
		void noloop();                                                            /* Set mode to normal */
		int getWidth();                                                           /* Get the width of the video. */
		int getHeight();                                                          /* Get the height of hte video. */

		bool isInitialized();

	private:
		char * localBuffer;
		int localBufferSize;
		GLuint texture;
		bool mIsInitialized = false;
		cinder::gl::GlslProgRef mGlsl;

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
