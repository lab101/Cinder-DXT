#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "VideoDxtDrawer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class _TBOX_PREFIX_App : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    VideoDxtDrawer player;

};

void _TBOX_PREFIX_App::setup()
{
    
    if(player.init(ci::app::getAssetPath("/").string() +  "video.dxt5")==0){
        player.play(24);
        player.loop();
        setWindowSize(player.getWidth(), player.getHeight());
    }else{
        console() << "error reading file" << std::endl;
    }

}

void _TBOX_PREFIX_App::mouseDown( MouseEvent event )
{
}

void _TBOX_PREFIX_App::update()
{
    // could be done in a thread.
    player.worker();
    // uploading should be done on the main thread.
    player.uploadTexture();

}

void _TBOX_PREFIX_App::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    player.draw();

}

CINDER_APP_NATIVE( _TBOX_PREFIX_App, RendererGl )
