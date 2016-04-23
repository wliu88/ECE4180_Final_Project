#include "mbed.h"
 
class Nav_Switch
{
public:
    Nav_Switch(PinName up,PinName down,PinName left,PinName right,PinName fire,
               volatile bool* u, volatile bool* d, volatile bool* l, volatile bool* r, volatile bool* f, volatile int* counter);
//boolean functions to test each switch
    void up();
    void down();
    void left();
    void right();
    void fire();
private:
    InterruptIn _up;
    InterruptIn _down;
    InterruptIn _left;
    InterruptIn _right;
    InterruptIn _fire;
    volatile bool* _u;
    volatile bool* _d;
    volatile bool* _l;
    volatile bool* _r;
    volatile bool* _f;
    volatile int* _counter;
 
};
Nav_Switch::Nav_Switch (PinName up,PinName down,PinName left,PinName right,PinName fire,
                        volatile bool* u, volatile bool* d, volatile bool* l, volatile bool* r, volatile bool* f,
                        volatile int* counter):
    _up(up), _down(down), _left(left), _right(right), _fire(fire),
    _u(u), _d(d), _l(l), _r(r), _f(f),
    _counter(counter)
{
    _up.mode(PullUp); //needed if pullups not on board or a bare nav switch is used - delete otherwise
    _down.mode(PullUp);
    _left.mode(PullUp);
    _right.mode(PullUp);
    _fire.mode(PullUp);
    wait(0.001); //delays just a bit for pullups to pull inputs high
    _up.fall(this, &Nav_Switch::up);
    _down.fall(this, &Nav_Switch::down);
    _left.fall(this, &Nav_Switch::left);
    _right.fall(this, &Nav_Switch::right);
    _fire.fall(this, &Nav_Switch::fire);
}
inline void Nav_Switch::up()
{
    (*_u) = true;
    (*_counter) = ((*_counter) + 3 - 1) % 3;
}
inline void Nav_Switch::down()
{
    (*_d) = true;
    (*_counter) = ((*_counter) + 1) % 3;
}
inline void Nav_Switch::left()
{
    (*_l) = true;
}
inline void Nav_Switch::right()
{
    (*_r) = true;
}
inline void Nav_Switch::fire()
{
    (*_f) = true;
}
