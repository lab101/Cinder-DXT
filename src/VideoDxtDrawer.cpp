#include "VideoDxtDrawer.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"


using namespace ci;
using namespace ci::gl;
using namespace ci::app;
namespace poly {

	/* --------------------------------------------------------------------------------- */



	VideoDxtDrawer::VideoDxtDrawer()
		:has_new_frame(-1)
		, localBuffer(nullptr)
	{
		alpha = 1;
	}

	VideoDxtDrawer::~VideoDxtDrawer() {

		if (localBuffer)delete[] localBuffer;
		alpha = 1.0f;
		has_new_frame = -1;

		player.shutdown();

	}

	bool VideoDxtDrawer::isInitialized(){
		return mIsInitialized;
	}


	int VideoDxtDrawer::init(std::string filepath) {

		if (0 == filepath.size()) {
			return -1;
		}

		if (0 != player.init(filepath)) {
			return -4;
		}

        
        
//        mGlsl = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "shader.vert" ) )
//                                       .fragment( loadAsset( "shader.frag" ) ) );


    
		mGlsl = gl::getStockShader(gl::ShaderDef().texture().color());
		mGlsl->bind();
	
		/* Create our backing texture. */
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, player.getWidth(), player.getHeight(), 0, player.getBytesPerFrame(), player.getBufferPtr());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


		checkError();

		mIsInitialized = true;

		return 0;
	}


	void VideoDxtDrawer::update(){
		has_new_frame = player.update();
		if (has_new_frame == 1){
			uploadTexture();
		}
	
	}


	void VideoDxtDrawer::uploadTexture(){

		glBindTexture(GL_TEXTURE_2D, texture);
		glCompressedTexSubImage2D(GL_TEXTURE_2D,
			0,
			0,
			0,
			player.getWidth(),
			player.getHeight(),
			GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
			player.getBytesPerFrame(),
			player.getBufferPtr());
	}

	void VideoDxtDrawer::draw(vec2 p) {
		draw(Rectf(p, p + vec2(player.getWidth(), player.getHeight())));
	}

	void VideoDxtDrawer::draw(Rectf frame) {


		mGlsl->bind();
        
        


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		
		gl::VertBatch vb(GL_TRIANGLES);

		// left top
		vb.texCoord(0.0, 0.0);
		vb.color(1.0, 1.0, 1.0, alpha);
		vb.vertex(frame.x1, frame.y1);


		// left bottom
		vb.texCoord(0.0, 1);
		vb.color(1.0, 1.0, 1.0, alpha);
		vb.vertex(frame.x1, frame.y2);

		// right bottom
		vb.texCoord(1, 1);
		vb.color(1.0, 1.0, 1.0, alpha);
		vb.vertex(frame.x2, frame.y2);

		//left top
		vb.texCoord(0.0, 0.0);
		vb.color(1.0, 1.0, 1.0, alpha);
		vb.vertex(frame.x1, frame.y1);

		// right bottom
		vb.texCoord(1, 1);
		vb.color(1.0, 1.0, 1.0, alpha);
		vb.vertex(frame.x2, frame.y2);

		// right top
		vb.texCoord(1, 0.0);
		vb.color(1.0, 1.0, 1.0, alpha);
		vb.vertex(frame.x2, frame.y1);


		vb.draw();

	}



} /* namespace poly */
