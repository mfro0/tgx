#include "GEMApplication.h"


GEMApplication* theApplication;

GEMApplication::GEMApplication()
{
    ap_id = appl_init();
    
    
    for (auto i = 1; i < 10; i++)
        work_in[i] = 0;
    work_in[0] = 1;
    work_in[10] = 2;
    
    v_opnvwk(work_in, &vdi_handle, work_out);
    theApplication = this;
}

constexpr PXY GEMApplication::wh(void)
{
    PXY ret = {(short) (work_out[0] + 1), (short) (work_out[1] + 1)};
    
    return ret;
}

void GEMApplication::set_clipping(short handle, short x, short y, short w, short h, short on)
{
    short clip[4];
    
    clip[0] = x;
    clip[1] = y;
    clip[2] = clip[0] + w - 1;
    clip[3] = clip[1] + h - 1;
    
    vs_clip(handle, on, clip);
}


Window* GEMApplication::window_from_handle(short handle)
{
    return windows[handle];
}



void GEMApplication::event_loop(void) {
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
        if (event & MU_TIMER)
        {
            for (std::unordered_map<short, Window*>::iterator it = windows.begin(); it != windows.end(); it++)
                it->second->timer();
        }
        wind_update(END_UPDATE);
        
    } while (!quit);
}