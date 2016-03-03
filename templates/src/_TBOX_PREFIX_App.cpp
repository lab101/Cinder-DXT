#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Utilities.h"

#include "VideoDxtDrawer.h"


using namespace ci;
using namespace ci::app;
using namespace std;

class _TBOX_PREFIX_App : public App {
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
	player.update();
}

void _TBOX_PREFIX_App::draw()
{
	// clear out the window with black
	gl::clear( Color( 1, 0, 0 ) );


    player.draw(vec2(0,0));
//	player.alpha = 0.5;

	gl::drawString("video size: " + toString(player.getWidth()) + " x " + toString(player.getHeight()) + " current frame " + toString(player.player.curr_frame) ,vec2(10,20));

}

CINDER_APP( _TBOX_PREFIX_App, RendererGl )
