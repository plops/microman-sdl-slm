/*
This device adapter opens an SDL window that can be drawn to from
Micromanager.  The size of the window and the initial position are
hardcoded in GetHeight(), GetWidth() for noww.

compile on linux with:
g++ -g -Wall -W -fPIC -shared -o libmmgr_dal_sdl-slm.so.0 -I/home/martin/src/micromanager1.3 sdl-slm.cpp /home/martin/src/micromanager1.3/MMDevice/.libs/libMMDevice.a -lSDL
sudo cp libmmgr_dal_sdl-slm.so.0 /usr/lib/micro-manager

see http://www.libsdl.org/intro.en/usingvideo.html for documentation
on libSDL
*/

#include <MMDevice/MMDevice.h>
#include <MMDevice/DeviceBase.h>
#include <SDL/SDL.h>

using namespace std;

const char* g_sdl_slm_name = "sdl-slm";
const char* g_sdl_slm_keyword = "Spatial Light Modulator controlled through computer graphics output using the SDL library";

class SDLSLM : public CSLMBase<SDLSLM>
{
public:
  SDLSLM(){
    initialized_=0;
    CreateProperty(MM::g_Keyword_Name,
		   g_sdl_slm_name, MM::String, true);
    
    CreateProperty(MM::g_Keyword_Description, 
		   g_sdl_slm_keyword, MM::String, true);
  }
  
  ~SDLSLM(){ 
    if (initialized_)
      Shutdown();
  }
  
  int Initialize(){
    if(initialized_)
      return DEVICE_OK;
	
    SDL_Init(SDL_INIT_VIDEO);\

    // apparently SDL1.3 has SDL_SetWindowPosition()
    // I use the old way:
    //SDL_putenv("SDL_VIDEO_WINDOW_POS=412, 134");

    sdl_screen_=SDL_SetVideoMode(GetWidth(),GetHeight(),
				 24,
				 SDL_SWSURFACE
				 //|SDL_NOFRAME // we don't want frame decorations
				 );
    surface_=SDL_CreateRGBSurface(SDL_SWSURFACE,GetWidth(),
				  GetHeight(),24,8,8,8,0);
    initialized_=true;
    return DEVICE_OK;
  }

  int Shutdown(){
    if (initialized_){
      SDL_FreeSurface(sdl_screen_);
      SDL_FreeSurface(surface_);
      SDL_Quit();

      initialized_ = false;
    }
    return DEVICE_OK;
  }
  
  void GetName(char* name) const {
    //assert(name_.length() < CDeviceUtils::GetMaxStringLength());
    CDeviceUtils::CopyLimitedString(name, g_sdl_slm_name);
  }
  
  bool Busy(){return false;};
  
  int SetImage(unsigned char* pixels)
  {
    int i, j, W=GetWidth(), H=GetHeight();
    for(j=0;j<H;j++)
      for(i=0;i<W;i++){
	((unsigned char*)(surface_->pixels))[3*(i+W*j)+0]=pixels[i+W*j];
      }
   
    return DEVICE_OK;
  }

  int SetImage(unsigned int* pixels)
  {
    int i, j, W=GetWidth(), H=GetHeight();
    for(j=0;j<H;j++)
      for(i=0;i<W;i++){
	((unsigned char*)(surface_->pixels))[3*(i+W*j)+0]=pixels[i+W*j];
      }
    
    return DEVICE_OK;
  }

  int SetPixelsTo(unsigned char red, unsigned char green, unsigned char blue)
  {
    int i, j, W=GetWidth(), H=GetHeight();
    for(j=0;j<H;j++)
      for(i=0;i<W;i++){
	((unsigned char*)(surface_->pixels))[3*(i+W*j)+0]=red;
	((unsigned char*)(surface_->pixels))[3*(i+W*j)+1]=green;
	((unsigned char*)(surface_->pixels))[3*(i+W*j)+2]=blue;
      }
   
    return DEVICE_OK;
  }

  int SetPixelsTo(unsigned char intensity)
  {
    SetPixelsTo(intensity,intensity,intensity);
    return DEVICE_OK;
  }

  int DisplayImage()
  {
    int W=GetWidth(),
      H=GetHeight();
    SDL_Rect r={0,0,W,H};
    SDL_BlitSurface(surface_,&r,sdl_screen_,&r);
    SDL_UpdateRect(sdl_screen_,0,0,W,H);
 
    return DEVICE_OK;
  }
  
  unsigned int GetWidth(){return 1024;}
  unsigned int GetHeight(){return 768;}
  unsigned int GetNumberOfComponents(){return 1;}
  unsigned int GetBytesPerPixel(){return 8;}
  
private:
  bool initialized_;
  SDL_Surface *sdl_screen_,*surface_;
}; 


// MMDevice API
MODULE_API void InitializeModuleData()
{
  AddAvailableDeviceName(g_sdl_slm_name, 
			 g_sdl_slm_keyword);
}

MODULE_API MM::Device* CreateDevice(const char* deviceName)
{
  if (deviceName == 0)
    return 0;
  
  if (strcmp(deviceName, g_sdl_slm_name) == 0){ 
    return new SDLSLM();
  }
  return 0;
}

MODULE_API void DeleteDevice(MM::Device* pDevice)
{
  delete pDevice;
}
