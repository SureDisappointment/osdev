#include "key.h"

inline Key new_key()
{
    Key k = {0, 0, 0, 0, 0, 0, 0, 0, 0 ,0};
    return k;
}

inline bool key_valid(Key key)
{
    return key.scancode != 0;
}
inline void invalidate_key(Key key)
{
    key.scancode = 0;
}

inline bool key_alt(Key key)
{
    return key.alt_left | key.alt_right;
}
inline bool key_ctrl(Key key)
{
    return key.ctrl_left | key.ctrl_right;
}
