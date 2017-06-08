
local Scene = Dusk.Class(Dusk.IEventDispatcher, function(self, ptr)
    Dusk.IEventDispatcher.init(self, ptr)
end)

Scene.Events = {
    LOAD   = 101,
    FREE   = 102,
    UPDATE = 103,
    RENDER = 104
}

function Scene:GetActorByName(name)
    return Dusk.Actor(dusk_Scene_GetActorByName(self.dusk_ptr, name))
end

Dusk.Scene = Scene
