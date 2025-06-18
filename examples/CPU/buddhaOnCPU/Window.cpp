#include "Window.h"

#include <utility>      // need std::pair
#include <iostream>

#include "GEMApplication.h"

extern GEMApplication* theApplication;

Window::Window(short kind, short x, short y, short w, short h) {
    this->kind = kind;
    wind_calc(WC_BORDER, kind, x, y, w, h, &x, &y, &w, &h);
    handle = wind_create(kind, x, y, w, h);
    wind_open(handle, x, y, w, h);
    
    rect = {x, y, w, h};
    wind_calc(WC_WORK, kind, x, y, w, h, &x, &y, &w, &h);
    work = {x, y, w, h};
    
    open = true;
    theApplication->windows.insert(std::pair<short, Window*>(handle, this));
    
}

Window::~Window()
{
    if (open)
    {
        wind_close(handle);
        open = false;
    } else
        
    wind_delete(handle);
}


void Window::full(void) {
    short x, y, w, h;
    wind_get(handle, WF_FULLXYWH, &x, &y, &w, &h);
    wind_set(handle, WF_CURRXYWH, x, y, w, h);
    wind_get_grect(handle, WF_CURRXYWH, &rect);
};

void Window::size(short x, short y, short w, short h)
{
    const short MIN_WIDTH = 100;
    const short MIN_HEIGHT = 100;
    
    if (w < MIN_WIDTH)
    {
        w = MIN_WIDTH;
    }
    
    if (h < MIN_HEIGHT)
    {
        h = MIN_HEIGHT;
    }
}

void Window::draw(short x, short y, short w, short h)
{
};

void Window::clear(short x, short y, short w, short h)
{
    
}

void Window::scroll()
{
    short sl_vpos;
    short sl_hpos;
    short sl_hsz;
    short sl_vsz;
    long dw, dh;
    
    /*
     * set sliders according to document area
     * If document size is smaller than window area, use the latter
     */
    dw = (doc_width > 0 ? doc_width : rect.g_w);
    dh = (doc_height > 0 ? doc_height : rect.g_h);
    
    sl_hsz = dw == 0 ? 1000 : 1000L * work.g_w / dw;
    sl_hsz = sl_hsz > 1000 ? 1000 : sl_hsz;
    sl_vsz = dh == 0 ? 1000 : 1000L * work.g_h / dh;
    sl_vsz = sl_vsz > 1000 ? 1000 : sl_vsz;
    
    if (dw - work.g_w == 0)
        sl_vpos = 0;
    else
        sl_vpos = 1000L * left / (dw - work.g_w);
    sl_vpos = sl_vpos > 1000 ? 1000 : sl_vpos;
    sl_vpos = sl_vpos < 0 ? 0 : sl_vpos;
    
    if (dh - work.g_h == 0)
        sl_hpos = 0;
    else
        sl_hpos = 1000L * top / (dh - work.g_h);
    sl_hpos = sl_hpos > 1000 ? 1000 : sl_hpos;
    sl_hpos = sl_hpos < 0 ? 0 : sl_hpos;
    wind_set(handle, WF_HSLIDE, sl_vpos, 0, 0, 0);
    wind_set(handle, WF_VSLIDE, sl_hpos, 0, 0, 0);
    wind_set(handle, WF_HSLSIZE, sl_hsz, 0, 0, 0);
    wind_set(handle, WF_VSLSIZE, sl_vsz, 0, 0, 0);        
}

void Window::timer()
{
    std::cout << "Window::timer()" << std::endl;
};


bool Window::handle_message(short *msgbuff)
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
        std::cout.setf(std::ios::hex);
        std::cout << "unknown window message " <<  msgbuff[0] << std::endl;
    }
    wind_update(END_UPDATE);
    return false;
}
