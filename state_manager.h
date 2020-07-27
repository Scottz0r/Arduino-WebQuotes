#ifndef _SCOTTZ0R_STATE_MANAGER_H_INCLUDE_GUARD
#define _SCOTTZ0R_STATE_MANAGER_H_INCLUDE_GUARD

namespace scottz0r
{
namespace state
{
    struct State
    {
        int last_idx;
        int count;
    };

    bool get_state(State& state);

    bool set_state(const State& state);
}    
}

#endif // _SCOTTZ0R_STATE_MANAGER_H_INCLUDE_GUARD