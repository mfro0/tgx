#ifndef WINDOW_H
#define WINDOW_H

#include <gem.h>

class Window
{
protected:
    short handle;
    short kind;
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
    Window(short kind, short x, short y, short w, short h);
    virtual ~Window();
    
    Window(const Window&) = delete;     // delete copy constructor
    
    virtual void full(void);
    
    virtual void size(short x, short y, short w, short h);
    
    virtual void draw(short x, short y, short w, short h);
    
    virtual void clear(short x, short y, short w, short h);
    
    virtual void scroll();
    
    virtual void timer();
    
    virtual void do_redraw(short xc, short yc, short wc, short hc) {};
    
    bool handle_message(short *msgbuff);
};
#endif // WINDOW_H
