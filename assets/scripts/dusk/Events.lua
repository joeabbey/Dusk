
local IEventDispatcher = Dusk.Class(Dusk.Object, function(self, ptr)
    Dusk.Object.init(self, ptr)
end)

function IEventDispatcher:AddEventListener(eventId, callback)
    dusk_IEventDispatcher_AddEventListener(self.dusk_ptr, eventId, callback)
end

function IEventDispatcher:RemoveEventListener(eventId, callback)
    dusk_IEventDispatcher_RemoveEventListener(self.dusk_ptr, eventId, callback)
end

Dusk.IEventDispatcher = IEventDispatcher
