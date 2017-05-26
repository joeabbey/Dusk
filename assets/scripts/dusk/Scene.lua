
local Scene = Dusk.Class(Dusk.IEventDispatcher, function(self, ptr)
    Dusk.IEventDispatcher.init(self, ptr)
end)

function Scene:GetActorByName(name)
    return Dusk.Actor(dusk_Scene_GetActorByName(self.dusk_ptr, name))
end

Dusk.Scene = Scene
