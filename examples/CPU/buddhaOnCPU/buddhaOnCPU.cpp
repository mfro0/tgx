/********************************************************************************
 * @file buddhaOnCPU.cpp 
 *
 * Example: using the TGX to draw a 3D mesh 
 * 
 * For Windows/Linuc/MacOS.
 *
 * Building the example:
 *
 * 1. Install CMake (version 3.10 later). 
 *
 * 2. Open a shell/terminal inside the directory that contains this file. 
 *
 * 3. Install CImg (only for Linux/MacOS)
 *    - If on Linux/Debian, run: "sudo apt install cimg-dev"
 *    - If on MacOS/homebrew, run: "brew install cimg"
 *
 * 4. run the following commands:
 *       mkdir build
 *       cd build
 *       cmake ..
 *
 * 4. This will create the project files into the /build directory which can now 
 *    be used to build the example. .
 *
 *    -> On Windows. Open the Visual Studio solution file "buddhaOnCPU.sln" and 
 *       build the example from within the IDE. 
 * 
 *    -> On Linux/MacOS. Run "make" to build the example. 
 *
 *******************************************************************************/


// the tgx library. 
#include "tgx.h" 

// the mesh we will draw. 
#include "buddha.h"
#include <unordered_map>

#include <gem.h>

#include <iostream>

const int LX = 400; // image dimension
const int LY = 400; //

tgx::RGB565 im_buffer[LX * LY];                     // image memory buffer...
tgx::Image<tgx::RGB565> im(im_buffer, LX, LY);      // ...and associated tgx::Image object that encapsulated it. 

float zbuffer[LX * LY];                             // z-buffer (same sie as the image)

tgx::Renderer3D<tgx::RGB565> renderer;              // the 3D renderer (loads all shaders).

class Window;
class GEMApplication;

GEMApplication *theApplication;

class GEMApplication
{
    short ap_id;
    short work_in[11];
    short work_out[57];
    short vdi_handle;
public:
    GEMApplication()
    {
        ap_id = appl_init();
        
        
        for (auto i = 1; i < 10; i++)
            work_in[i] = 0;
        work_in[0] = 1;
        work_in[10] = 2;
        
        v_opnvwk(work_in, &vdi_handle, work_out);
        theApplication = this;
    }
    
    constexpr PXY wh(void)
    {
        PXY ret = {(short) (work_out[0] + 1), (short) (work_out[1] + 1)};
        
        return ret;
    }
    constexpr short vh() { return vdi_handle; }
    
    void handle_window_message(short* msgbuff);
    
    void event_loop(void)
    {
        short msgbuff[8];
        short keycode;
        short mx, my;
        short ret;
        bool quit;
        short butdown;
        short event;
        short keystate;
        short keyreturn;
        short mbreturn;
        long msec = 20;
        
        event = evnt_multi(MU_MESAG | MU_BUTTON | MU_KEYBD | MU_TIMER,
                           0x103, 3, butdown,
                           0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0,
                           msgbuff, msec, &mx, &my, &mbreturn, &keystate, &keyreturn, &ret);
        wind_update(BEG_UPDATE);
        
        if (event & MU_MESAG)
        {
            handle_window_message(msgbuff);
        }
    }
            
    
    std::unordered_map<short, Window*> windows;
    
    Window *window_from_handle(short handle)
    {
        return windows[handle];
    }
};

class Window
{
protected:
    short handle;
    
    GRECT rect;
    GRECT work;
    GRECT old;
    
    int left;
    int top;
    
    int doc_width;
    int doc_height;
    
    bool open;
    bool topped;
    bool fulled;
    
    char name[200];
    char info[200];
    
public:
    Window(short kind, short x, short y, short w, short h) {
        wind_calc(WC_BORDER, kind, x, y, w, h, &x, &y, &w, &h);
        handle = wind_create(kind, x, y, w, h);
        wind_open(handle, x, y, w, h);
        
        rect = {x, y, w, h};
        
        std::cout << "rect = {" << x << ", " << y << ", " << w << ", " << h << "}" << std::endl;
        wind_calc(WC_WORK, kind, x, y, w, h, &x, &y, &w, &h);
        work = {x, y, w, h};
        std::cout << "work = {" << x << ", " << y << ", " << w << ", " << h << "}" << std::endl;
        
        open = true;
        theApplication->windows.insert(std::pair<short, Window*>(handle, this));
                
        std::cout << "Window::Window: window handle=" << handle << std::endl;
    }
    
    virtual ~Window()
    {
        if (open)
        {
            std::cout << "window was open" << std::endl;
            wind_close(handle);
            open = false;
        } else
            std::cout << "window was not open" << std::endl;
        
        wind_delete(handle);
        std::cout << "Window destructor" << std::endl << std::flush;
    }
    
    Window(const Window&) = delete;     // delete copy constructor
    
    void handle_message(short *msgbuff)
    {
        switch (msgbuff[0])
        {
            case WM_REDRAW:
                draw(msgbuff[4], msgbuff[5], msgbuff[6], msgbuff[7]);
                break;
            
            case WM_ONTOP:
            case WM_NEWTOP:
            case WM_TOPPED:
                wind_set(handle, WF_TOP, 0, 0, 0, 0);
                topped = true;
                break;
            
            case WM_UNTOPPED:
                topped = false;
                break;
        
            case WM_SIZED:
            case WM_MOVED:
                size(msgbuff[4], msgbuff[5], msgbuff[6], msgbuff[7]);
                break;
        
            case WM_FULLED:
                full();
                break;
        
            case WM_CLOSED:
                //del();
                break;
        
            case WM_ARROWED:
                switch (msgbuff[4])
                {
                    case WA_UPPAGE:
                        top -= work.g_h;
                        break;
                    case WA_DNPAGE:
                        top += work.g_h;
                        break;
                }
            
            default:
                std::cout << "unknown window message " << msgbuff[0] << std::endl;
        }
        wind_update(END_UPDATE);
    }
        
    virtual void full(void) {
        short x, y, w, h;
        wind_set(handle, WF_FULLXYWH, x, y, w, h);
    };
    
    virtual void size(short x, short y, short w, short h)
    {
        wind_set(handle, WF_WORKXYWH, x, y, w, h);
    }
    
    virtual void draw(short x, short y, short w, short h)
    {
        std::cout << "Window::draw()" << std::endl << std::flush;
    };
    
    virtual void clear(short x, short y, short w, short h)
    {
        
    }
    
    virtual void scroll()
    {
        
    }
    
    virtual void timer()
    {
        
    };
};

class BuddhaWindow : public Window
{
public:
    BuddhaWindow(short what, short x, short y, short w, short h) : Window(what, x, y, w, h)
    {
        std::cout << "BuddhaWindow constructor" << std::endl << std::flush;
    }
    
    ~BuddhaWindow(){
        std::cout << "BuddhaWindow destructor" << std::endl << std::flush;
    }
    
    virtual void draw(short x, short y, short w, short h)
    {
        MFDB src_mfdb = {
            .fd_addr= im_buffer,
            .fd_w = LX,
            .fd_h = LY,
            .fd_wdwidth = (LX + 15) / 16,
            .fd_stand = 0,
            .fd_nplanes = 16,
            .fd_r1 = 0,
            .fd_r2 = 0,
            .fd_r3 = 0
        };
        
        
        MFDB dst_mfdb = {
            .fd_addr = NULL,
            .fd_w = theApplication->wh().p_x,
            .fd_h = theApplication->wh().p_y,
            .fd_wdwidth = (short) ((theApplication->wh().p_x + 15) / 16),
            .fd_stand = 0,
            .fd_nplanes = 16,
            .fd_r1 = 0,
            .fd_r2 = 0,
            .fd_r3 = 0
        };
        
        short pxy[] = {
            (short) (x - work.g_x), (short)(y - work.g_y), work.g_w, work.g_h,
            work.g_x, work.g_y, work.g_w, work.g_h};

//            0,  20,       LX - 1, LY - 1,
//            100,  20, 100 +   LX - 1, LY - 1
//        };
        
        vro_cpyfm(theApplication->vh(), S_ONLY, pxy, &src_mfdb, &dst_mfdb);
        
        std::cout << "BuddhaWindow::draw()" << std::endl << std::flush;
    }
};


void GEMApplication::handle_window_message(short *msgbuff)
{
    auto wi = theApplication->window_from_handle(msgbuff[3]);
    wi->handle_message(msgbuff);    
}

int main()
{
    // setup the 3D renderer.
    renderer.setViewportSize(LX, LY);   // viewport = image...
    renderer.setOffset(0, 0);           //  ...so no offset
    renderer.setImage(&im);             // set the image to draw onto
    renderer.setZbuffer(zbuffer);       // set the z buffer for depth testing
    renderer.setPerspective(45, ((float) LX) / LY, 1.0f, 100.0f);  // set the perspective projection matrix.
    renderer.setMaterial(tgx::RGBf(/*0.85f */0.25f, 0.55f, /*0.25f*/ 0.85f), 0.2f, 0.7f, 0.8f, 64); // set material properties
    renderer.setShaders(tgx::SHADER_GOURAUD); // draw with Gouraud shaders
    
    // draw the mesh 
    im.clear(tgx::RGB565_Gray);  // clear the image
    renderer.clearZbuffer(); // and the zbuffer.
    renderer.setModelPosScaleRot({ 0, 0.5f, -36 }, { 13,13,13 }, 45); // set the position of the mesh
    renderer.drawMesh(&buddha, false); // and then draw it !
    
    
    GEMApplication ap;
    BuddhaWindow wi(NAME|SIZER|MOVER|CLOSER, 100,  40, 100 + LX - 1, 40 + LY - 1);
    wi.clear(0, 0, LX, LY);
    wi.draw(0, 0, 200, 200);
    
    while (!evnt_keybd());
}

/** en of file */
