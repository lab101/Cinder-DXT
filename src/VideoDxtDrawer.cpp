#include "VideoDxtDrawer.h"
#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::gl;
using namespace ci::app;
namespace poly {

  /* --------------------------------------------------------------------------------- */


 
  

  /* --------------------------------------------------------------------------------- */

  
  /* --------------------------------------------------------------------------------- */

  VideoDxtDrawer::VideoDxtDrawer()
    :has_new_frame(-1)
	, localBuffer(nullptr)
  {
  }
  
  VideoDxtDrawer::~VideoDxtDrawer() {

	  if (localBuffer)delete[] localBuffer;
	  alpha = 1.0f;
	  has_new_frame = -1;

	  player.shutdown();

  }

  int VideoDxtDrawer::init(std::string filepath) {
    
    if (0 == filepath.size()) {
      return -1;
    }

    if (0 != player.init(filepath)) {
      return -4;
    }
    
      
      glGenTextures(1, &texture);



    return 0;
  }

  int VideoDxtDrawer::worker() {

	  has_new_frame = player.update();
      
	  if (has_new_frame == 1)
	  {
		  if (!localBuffer)
		  {
			  localBuffer = new char[player.getBytesPerFrame()];
			  localBufferSize = player.getBytesPerFrame();
		  }
		  
		  std::copy(player.getBufferPtr(), player.getBufferPtr() + player.getBytesPerFrame(), localBuffer);
	  }
	  return 1;
  }
 
    void VideoDxtDrawer::uploadTexture(){
        glBindTexture(GL_TEXTURE_2D, texture);
        
        if (has_new_frame){
            glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, getWidth(), player.getHeight(), 0, player.getBytesPerFrame(), player.getBufferPtr());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
    }
    
    void VideoDxtDrawer::draw(ci::Vec2f position){
        draw(Rectf(position.x, position.y, player.getWidth(), player.getHeight()));
    }


    void VideoDxtDrawer::draw(){
        draw(Rectf(0, 0, player.getWidth(), player.getHeight()));
    }
    
    
  void VideoDxtDrawer::draw(Rectf frame) {

	
	  SaveTextureBindState saveBindState(GL_TEXTURE_2D);
	  BoolState saveEnabledState(GL_TEXTURE_2D);
		  ClientBoolState vertexArrayState(GL_VERTEX_ARRAY);
		  ClientBoolState texCoordArrayState(GL_TEXTURE_COORD_ARRAY);
		  glActiveTexture(GL_TEXTURE0);
		  glEnable(GL_TEXTURE_2D);
		  glBindTexture(GL_TEXTURE_2D, texture);

		  glEnableClientState(GL_VERTEX_ARRAY);
		  GLfloat verts[8];
		  glVertexPointer(2, GL_FLOAT, 0, verts);
		  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		  GLfloat texCoords[8];
		  glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
		
		
		  verts[0 * 2 + 0] = frame.getX2(); verts[0 * 2 + 1] = frame.getY1();
		  verts[1 * 2 + 0] = frame.getX1(); verts[1 * 2 + 1] = frame.getY1();
		  verts[2 * 2 + 0] = frame.getX2(); verts[2 * 2 + 1] = frame.getY2();
		  verts[3 * 2 + 0] = frame.getX1(); verts[3 * 2 + 1] = frame.getY2();

		  const Rectf srcCoords = Rectf(0, 0, 1, 1);
		  texCoords[0 * 2 + 0] = srcCoords.getX2(); texCoords[0 * 2 + 1] = srcCoords.getY1();
		  texCoords[1 * 2 + 0] = srcCoords.getX1(); texCoords[1 * 2 + 1] = srcCoords.getY1();
		  texCoords[2 * 2 + 0] = srcCoords.getX2(); texCoords[2 * 2 + 1] = srcCoords.getY2();
		  texCoords[3 * 2 + 0] = srcCoords.getX1(); texCoords[3 * 2 + 1] = srcCoords.getY2();

		  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		  GLenum err;
		  while ((err = glGetError()) != GL_NO_ERROR) {
			console() << "OpenGL error: " << err << std::endl;
		  }

  }
  


} /* namespace poly */
