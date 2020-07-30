#ifndef _SCOTTZ0R_STATE_MANAGER_H_INCLUDE_GUARD
#define _SCOTTZ0R_STATE_MANAGER_H_INCLUDE_GUARD

#include <cstdint>

namespace scottz0r
{
namespace state
{
    struct State
    {
        uint16_t last_idx_0;
        uint16_t last_idx_1;
        uint16_t last_idx_2;
        uint16_t last_idx_3;
        uint16_t last_idx_4;
        uint16_t count;
    };

    bool get_state(State& state);

    bool set_state(const State& state);

    bool has_been_shown_lately(uint16_t number, const State& state);

    void set_shown_lately(uint16_t index, State& state);
}    
}

#endif // _SCOTTZ0R_STATE_MANAGER_H_INCLUDE_GUARD
