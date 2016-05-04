#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Utilities.h"

#include "VideoDxtDrawer.h"


using namespace ci;
using namespace ci::app;
using namespace std;

class BasicPlayerApp : public App {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    VideoDxtDrawer player;

};

void BasicPlayerApp::setup()
{
    
    if(player.init(ci::app::getAssetPath("/").string() +  "bunny.dxt5")==0){
        player.play(25);
        player.loop();
        setWindowSize(player.getWidth(), player.getHeight());
    }else{
        console() << "error reading file" << std::endl;
    }

}

void BasicPlayerApp::mouseDown( MouseEvent event )
{
}

void BasicPlayerApp::update()
{
	player.update();
}

void BasicPlayerApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 1, 0, 0 ) );


    player.draw(vec2(0,0));

	gl::drawString("video size: " + toString(player.getWidth()) + " x " + toString(player.getHeight()) + " current frame " + toString(player.player.curr_frame) ,vec2(10,20));

}

CINDER_APP( BasicPlayerApp, RendererGl )
