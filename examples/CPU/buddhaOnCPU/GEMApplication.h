#ifndef GEMAPPLICATION_H
#define GEMAPPLICATION_H

#include <gem.h>
#include <unordered_map>
#include "Window.h"

class GEMApplication
{
    short ap_id;
    short work_in[11];
    short work_out[57];
    short vdi_handle;
public:
    GEMApplication();
    
    constexpr PXY wh(void);
    constexpr short vh() { return vdi_handle; }
    
    bool handle_window_message(short* msgbuff);
    
    void event_loop(void);
    
    void set_clipping(short handle, short x, short y, short w, short h, short on);
    
    std::unordered_map<short, Window*> windows;
    
    Window *window_from_handle(short handle);
};

extern GEMApplication* theApplication;

#endif // GEMAPPLICATION_H
