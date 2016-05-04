#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Surface.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/ip/Resize.h"
#include "cinder/ip/Fill.h"

#include "Lab101Utils.h"


#include "VideoDxtCreator.h"

#include <iostream>
using namespace ci;
using namespace ci::app;
using namespace std;
using namespace poly;

class DXTencoderApp : public App {
    
    void loadMovieFile( const fs::path &path );

    qtime::MovieSurfaceRef	mMovie;
    SurfaceRef              mSurface;
    
    Surface scaledSurface;
    Surface correctedSurface;
    VideoDxtCreator         mDxtCreator;
    
    int currentFrame;
    int endFrame;
    bool isStarted;
    
    ivec2   correctedSize;
    ivec2    scaledSize;
    
    Font				mFont;
    gl::TextureFontRef	mTextureFont;

    
  public:
	void setup() override;
    void mouseDown( MouseEvent event ) override;
   // void keyDown( KeyEvent event ) override;
    void fileDrop( FileDropEvent event ) override;
    void movieLoadedSetParameters();

	void update() override;
	void draw() override;
};

void DXTencoderApp::setup()
{
    isStarted = false;
    currentFrame = 0;
    endFrame = -1;
    
    #if defined( CINDER_COCOA )
        mFont = Font( "Helvetica Neue", 24 );
    #else
        mFont = Font( "Arial", 24 );
    #endif
        mTextureFont = gl::TextureFont::create( mFont );

}

void DXTencoderApp::mouseDown( MouseEvent event )
{
}

void DXTencoderApp::movieLoadedSetParameters(){
    endFrame = mMovie->getNumFrames();
}

void DXTencoderApp::loadMovieFile( const fs::path &moviePath )
{
    try {
        mMovie = qtime::MovieSurface::create( moviePath );
        
        console() << "Dimensions:" << mMovie->getWidth() << " x " << mMovie->getHeight() << std::endl;
        console() << "Duration:  " << mMovie->getDuration() << " seconds" << std::endl;
        console() << "Frames:    " << mMovie->getNumFrames() << std::endl;
        console() << "Framerate: " << mMovie->getFramerate() << std::endl;
        console() << "Has audio: " << mMovie->hasAudio() << " Has visuals: " << mMovie->hasVisuals() << std::endl;

        mMovie->setLoop( false );
        mMovie->seekToStart();
        //mMovie->play();
        isStarted = true;
        currentFrame = 0;

        
        std::string basePath = moviePath.parent_path().string();
        string newFilename =   moviePath.filename().string();
        strReplace(newFilename, moviePath.extension().string(), ".dxt5");
                                                  
        mDxtCreator.open(basePath + "/" + newFilename);

    }
    catch( ci::Exception &exc ) {
        console() << "Exception caught trying to load the movie from path: " << moviePath << ", what: " << exc.what() << std::endl;
    }
    
    
    
}

void DXTencoderApp::fileDrop( FileDropEvent event )
{
    loadMovieFile( event.getFile( 0 ) );
}

void DXTencoderApp::update()
{
    
    bool hasNewFrame = false;
        
    if( mMovie && isStarted){
        
        //scaledSize.x= 196;
        scaledSize.x=mMovie->getWidth();

        
        hasNewFrame = mMovie->stepForward();
        
        
        scaledSize.y = mMovie->getSize().y * scaledSize.x / mMovie->getSize().x;

        
        correctedSize = scaledSize;
        correctedSize.y = correctedSize.y  - (correctedSize.y % 4);

        
        mDxtCreator.w = correctedSize.x;
        mDxtCreator.h = correctedSize.y;
        
        
        mSurface = mMovie->getSurface();
        if (mSurface) {

            if (currentFrame == 0) {
                movieLoadedSetParameters();
                isStarted = true;
            }
            
            if(currentFrame  < endFrame){
                                
                
                scaledSurface = ip::resizeCopy(*mSurface, Area(0, 0, mMovie->getSize().x, mMovie->getSize().y), scaledSize);
                
//                // make a copy but crop some pixels.
                correctedSurface = Surface(correctedSize.x, correctedSize.y, true);
                correctedSurface.copyFrom(scaledSurface, Area(0,0,correctedSize.x,correctedSize.y));

                mDxtCreator.writePixels(correctedSurface.getData(),currentFrame);
                currentFrame++;
            }else if(currentFrame == endFrame){
                mDxtCreator.close();
                mMovie->stop();
                isStarted = false;


            }
            
            
            
        }    }
}

void DXTencoderApp::draw()
{
    gl::clear( Color( 0, 1.0, 1.0 ) );
    gl::color(1, 1, 1, 1);

    gl::enableAlphaBlending( true );
    
    if( ( ! mMovie ) || ( ! mSurface ) || !isStarted ){

        gl::color(0, 0, 0, 1);

        gl::color(1, 1, 1, 1);
        mTextureFont->drawString("DRAG VIDEO FILE IN THIS WINDOW", getWindowCenter() - vec2(210,0));

        return;
    }
    
    if(scaledSurface.getWidth() < getWindowWidth()){
        gl::draw( gl::Texture::create( scaledSurface ),vec2(0,100) );
        gl::draw( gl::Texture::create( correctedSurface ),vec2(scaledSize.x + 10,100) );
    }else{
        gl::color(1, 1, 1, 1);
        mTextureFont->drawString("NO PREVIEW VIDEO IS TO LARGE", getWindowCenter() - vec2(210,0));

    }

    gl::color(0, 0, 0, 0.6);
    gl::drawSolidRect(Rectf(0,0,getWindowWidth(),100));

    gl::color(1, 1, 1, 1);
    mTextureFont->drawString("ENCODING FRAME: " + toString(currentFrame) + " / " + toString(endFrame), ivec2(20,35));
    
    gl::drawString("original size: " + toString(mMovie->getSize()) , ivec2(20,70));
    gl::drawString("scaled size: " + toString(scaledSize) , ivec2(20,85));
    gl::drawString("corrected %4 size: " + toString(correctedSize) , ivec2(scaledSize.x + 20,85));

    gl::color(1, 1, 1, 1);
    float width = lmap(currentFrame, 0, endFrame, 0, getWindowWidth());
    gl::drawSolidRect(Rectf(0,45,width ,60));
    

    gl::color(1, 1.0, 1.0, 1.0);

    float lineHeight = 100 + correctedSize.y;
    gl::drawLine(vec2(0,lineHeight), vec2(getWindowWidth(),lineHeight));

    gl::color(1, .4, .7, 0.8);

    gl::drawLine(vec2(0,100 + 256), vec2(getWindowWidth(),100 + 256));
    gl::drawString("screen bottom " , ivec2(scaledSize.x +correctedSize.x + 20,90 + 256 ), Color(1,0.4,0.8));
    
}

CINDER_APP( DXTencoderApp, RendererGl )
