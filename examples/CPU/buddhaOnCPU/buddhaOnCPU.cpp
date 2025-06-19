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
// #include "teapot.h"
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

#include "Window.h"
#include "GEMApplication.h"

class BuddhaWindow : public Window
{
public:
    BuddhaWindow(short what, GRECT r) : Window(what, r)
    {
        doc_width = LX;
        doc_height = LY;
        left = top = 0;
    }
    
    ~BuddhaWindow(){
    }
    
    virtual void draw(GRECT r)
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
            .fd_addr = NULL     // nothing else required if target is screen
        };
        
        wind_get_grect(handle, WF_WORKXYWH, &r);
        
        short pxy[] = {
            static_cast<short>(left),
            static_cast<short>(top),
            static_cast<short>(left + work.g_w - 1),
            static_cast<short>(top + work.g_h - 1),
            work.g_x, work.g_y,
            static_cast<short>(work.g_x + work.g_w - 1),
            static_cast<short>(work.g_y + work.g_h - 1)
        };
        
        theApplication->set_clipping(theApplication->vh(), r, 1);
        vro_cpyfm(theApplication->vh(), S_ONLY, pxy, &src_mfdb, &dst_mfdb);
    }
    
    virtual void timer(void)
    {
        do_redraw(work);
    }
    
    virtual void size(GRECT r) {
        // move this into BuddhaWindow::size()
        // don't let the window size become larger than our drawing canvas
        
        wind_calc_grect(WC_WORK, kind, &r, &work);
        if (work.g_w > LX) work.g_w = LX;
        if (work.g_h > LY) work.g_h = LY;
        wind_calc_grect(WC_BORDER, kind, &work, &r);
        
        wind_set_grect(handle, WF_CURRXYWH, &r);
        wind_get_grect(handle, WF_WORKXYWH, &work);
        wind_get_grect(handle, WF_CURRXYWH, &rect);
        scroll(); /* fix slider sizes and positions */
    }
    
    void do_redraw(GRECT r)
    {
        GRECT t1, t2 = r;
        graf_mouse(M_OFF, 0);
        
        static float angle = -45.0f;
        
        wind_update(BEG_UPDATE);
        
        
        // draw the mesh 
        im.clear(tgx::RGB565_Gray);  // clear the image
        renderer.clearZbuffer(); // and the zbuffer.
        angle += 5.0f;
        renderer.setModelPosScaleRot({ 0, 0.5f, -36 }, { 13,13,13 }, angle); // set the position of the mesh
        renderer.drawMesh(&buddha, false); // and then draw it !
        //renderer.drawMesh(&teapot_1, false);
        //renderer.drawMesh(&teapot_2, false);
        wind_get_grect(handle, WF_FIRSTXYWH, &t1);
        
        while (t1.g_w || t1.g_h)
        {
            if (rc_intersect(&t2, &t1))
            {
                theApplication->set_clipping(theApplication->vh(), t1, 1);
                draw(t1);
            }
            wind_get_grect(handle, WF_NEXTXYWH, &t1);
        }
        wind_update(END_UPDATE);
        graf_mouse(M_ON, NULL);
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
    renderer.setMaterial(tgx::RGBf(0.85f, 0.55f, 0.25f), 0.2f, 0.7f, 0.8f, 64); // set material properties
    renderer.setShaders(tgx::SHADER_GOURAUD); // draw with Gouraud shaders
    
    // draw the mesh 
    im.clear(tgx::RGB565_Gray);  // clear the image
    renderer.clearZbuffer(); // and the zbuffer.
    renderer.setModelPosScaleRot({ 0, 0.5f, -36 }, { 13,13,13 }, -45); // set the position of the mesh
    renderer.drawMesh(&buddha, false);
    //renderer.drawMesh(&teapot_1, false); // and then draw it !
    //renderer.drawMesh(&teapot_2, false);
    
    GEMApplication ap;
    BuddhaWindow wi(NAME | SIZER | MOVER | CLOSER | FULLER |
                    HSLIDE | VSLIDE |
                    UPARROW | DNARROW | LFARROW | RTARROW,
                    {100,  40, 100 + 200 - 1, 40 + 200 - 1});
    wi.size(GRECT{100, 40, 100 + 200 - 1, 40 + 200 - 1});
    
    theApplication->event_loop();
}

/** en of file */
