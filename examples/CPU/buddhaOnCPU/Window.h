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
    Window(short kind, GRECT r);
    virtual ~Window();
    
    Window(const Window&) = delete;     // delete copy constructor
    virtual void full(void);
    virtual void size(GRECT r);
    virtual void draw(GRECT r);
    virtual void clear(GRECT r);
    virtual void scroll();
    virtual void timer();
    virtual void do_redraw(GRECT r) {};
    virtual bool handle_message(short msgbuff[]);
};
#endif // WINDOW_H
