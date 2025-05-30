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
    
    bool handle_window_message(short* msgbuff);
    
    void event_loop(void)
    {
        short msgbuff[8];
        short keycode;
        short mx, my;
        short ret;
        bool quit = false;
        short butdown;
        short event;
        short keystate;
        short keyreturn;
        short mbreturn;
        long msec = 20;
        
        do
        {
            event = evnt_multi(MU_MESAG | MU_BUTTON | MU_KEYBD | MU_TIMER,
                               0x103, 3, butdown,
                               0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0,
                               msgbuff, msec, &mx, &my, &mbreturn, &keystate, &keyreturn, &ret);
            wind_update(BEG_UPDATE);
        
            if (event & MU_MESAG)
            {
                quit = handle_window_message(msgbuff);
            }
            wind_update(END_UPDATE);
            
        } while (!quit);
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
    
    int x_fac;
    int y_fac;
    
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
        
    virtual void full(void) {
        short x, y, w, h;
        wind_set(handle, WF_FULLXYWH, x, y, w, h);
    };
    
    virtual void size(short x, short y, short w, short h)
    {
        /*
        if (w < MIN_WIDTH)
        {
            w = MIN_WIDTH;
        }
        if (h < MIN_HEIGHT)
        {
            h = MIN_HEIGHT;
        }
        */
        
        wind_set(handle, WF_CURRXYWH, x, y, w, h);
        wind_get(handle, WF_WORKXYWH, &work.g_x, &work.g_y, &work.g_w, &work.g_h);
        wind_get(handle, WF_CURRXYWH, &rect.g_x, &rect.g_y, &rect.g_w, &rect.g_h);
        scroll(); /* fix slider sizes and positions */
        
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
    
    virtual void do_redraw(short xc, short yc, short wc, short hc)
    {
        GRECT t1, t2 = {xc, yc, wc, hc};
        graf_mouse(M_OFF, 0);
        
        wind_update(BEG_UPDATE);
        
        wind_get(handle, WF_FIRSTXYWH, &t1.g_x, &t1.g_y, &t1.g_w, &t1.g_h);
        
        while (t1.g_w || t1.g_h)
        {
            if (rc_intersect(&t2, &t1))
            {
                // set_clipping(theApplication->vh(), t1.g_x, t1.g_y, t1.g_w, t1.g_h, 1);
                draw(t1.g_x, t1.g_y, t1.g_w, t1.g_h);
            }
            wind_get(handle, WF_NEXTXYWH, &t1.g_x, &t1.g_y, &t1.g_w, &t1.g_h);
        }
        wind_update(END_UPDATE);
        graf_mouse(M_ON, NULL);
    }
    
    bool handle_message(short *msgbuff)
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
                return true;                
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
                    case WA_UPLINE:
                        top -= y_fac;
                        break;
                    case WA_DNLINE:
                        top += y_fac;
                        break;
                    case WA_LFPAGE:
                        left -= work.g_w;
                        break;
                    case WA_RTPAGE:
                        left += work.g_w;
                        break;
                    case WA_LFLINE:
                        left--;
                        break;
                    case WA_RTLINE:
                        left++;
                        break;
                }
                if (top > doc_height - work.g_h)
                    top = doc_height - work.g_h;
                if (top < 0) top = 0;
                if (left > doc_width - work.g_w)
                    left = doc_width - work.g_w;
                if (left < 0) left = 0;
                
                scroll();
                
                do_redraw(work.g_x, work.g_y, work.g_w, work.g_h);
                break;
            
            case WM_HSLID:
                left = msgbuff[4] * doc_width / 1000;
                
                if (left > doc_width - work.g_w)
                    left = doc_width - work.g_w;
                scroll();
                
                do_redraw(work.g_x, work.g_y, work.g_w, work.g_h);
                break;
            
            case WM_VSLID:
                top = msgbuff[4] * doc_height / 1000;
                
                if (top > doc_height - work.g_h)
                    top = doc_height - work.g_h;
                scroll();
                
                do_redraw(work.g_x, work.g_y, work.g_w, work.g_h);
                break;
        
            default:
                std::cout << "unknown window message " << msgbuff[0] << std::endl;
        }
        wind_update(END_UPDATE);
        return false;
    }
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
            0, 0,
            work.g_w - 1,
            work.g_h - 1,
            work.g_x, work.g_y,
            work.g_x + work.g_w - 1,
            work.g_y + work.g_h - 1 };

//            0,  20,       LX - 1, LY - 1,
//            100,  20, 100 +   LX - 1, LY - 1
//        };
        
        vro_cpyfm(theApplication->vh(), S_ONLY, pxy, &src_mfdb, &dst_mfdb);
        
        std::cout << "BuddhaWindow::draw()" << std::endl << std::flush;
    }
};


bool GEMApplication::handle_window_message(short *msgbuff)
{
    auto wi = theApplication->window_from_handle(msgbuff[3]);
    return wi->handle_message(msgbuff);    
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
    BuddhaWindow wi(NAME | SIZER | MOVER | CLOSER | HSLIDE | VSLIDE,
                    100,  40, 100 + LX - 1, 40 + LY - 1);
    
    theApplication->event_loop();
}

/** en of file */
