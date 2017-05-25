#include "dusk/Event.hpp"

namespace dusk {

const EventData EventData::Empty;

int EventData::PushToLua(lua_State * L) const
{
    return 0;
}

} // namespace dusk
