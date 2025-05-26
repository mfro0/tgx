/********************************************************************************
 * @file buddhaOnCPU.cpp 
 *
 * Example: using the TGX to draw a 3D mesh (displayed using the CImg library).
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


// The CImg library is used only to create a window and display the image.
// It is lightweight (single header file!) and cross-platform (Windows/Linux/MacOS)
//#include "CImg.h" 

// the tgx library. 
#include "tgx.h" 

// the mesh we will draw. 
#include "buddha.h"
#include <gem.h>

#include <iostream>

const int LX = 400; // image dimension
const int LY = 400; //

tgx::RGB565 im_buffer[LX * LY];                  // image memory buffer...
tgx::Image<tgx::RGB565> im(im_buffer, LX, LY);   // ...and associated tgx::Image object that encapsulated it. 

float zbuffer[LX * LY];                         // z-buffer (same sie as the image)

tgx::Renderer3D<tgx::RGB565> renderer;           // the 3D renderer (loads all shaders). 


int main()
{
    // setup the 3D renderer.
    renderer.setViewportSize(LX, LY);   // viewport = image...
    renderer.setOffset(0, 0);           //  ...so no offset
    renderer.setImage(&im);             // set the image to draw onto
    renderer.setZbuffer(zbuffer);       // set the z buffer for depth testing
    renderer.setPerspective(45, ((float)LX) / LY, 1.0f, 100.0f);  // set the perspective projection matrix.     
    renderer.setMaterial(tgx::RGBf(0.85f, 0.55f, 0.25f), 0.2f, 0.7f, 0.8f, 64); // set material properties
    renderer.setShaders(tgx::SHADER_GOURAUD); // draw with Gouraud shaders

    // draw the mesh 
    im.clear(tgx::RGB565_Gray);  // clear the image
    renderer.clearZbuffer(); // and the zbuffer.
    renderer.setModelPosScaleRot({ 0, 0.5f, -36 }, { 13,13,13 }, 0); // set the position of the mesh
    renderer.drawMesh(&buddha, false); // and then draw it !
    
    short ap_id = appl_init();
    
    short work_in[10], work_out[57];
    
    for (short i = 1; i < 10; i++)
        work_in[i] = 0;
    work_in[0] = 1;
    work_in[10] = 2;
    
    short vdi_handle;
    
    v_opnvwk(work_in, &vdi_handle, work_out);
    
    short w = work_out[0] + 1;
    short h = work_out[1] + 1;
    
    std::cout << "width=" << w <<
        std::endl << "height=" << h << std::endl;
    
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
        .fd_w = w,
        .fd_h = h,
        .fd_wdwidth = (w + 15) / 16,
        .fd_stand = 0,
        .fd_nplanes = 16,
        .fd_r1 = 0,
        .fd_r2 = 0,
        .fd_r3 = 0
    };
    
    short pxy[] = {0, 0, LX - 1, LY - 1, 0, 0, LX - 1, LY - 1};
    vro_cpyfm(vdi_handle, S_ONLY, pxy, &src_mfdb, &dst_mfdb);
    
    // while (1);
}

/** en of file */
