
local Scene = Dusk.Class(Dusk.IEventDispatcher, function(self, ptr)
    Dusk.IEventDispatcher.init(self, ptr)
end)

Scene.Events = {
    LOAD_START    = 101,
    FREE_START    = 102,
    LOAD_FINISHED = 103,
    FREE_FINISHED = 104,
    START         = 105,
    STOP          = 106,
    UPDATE        = 107,
    RENDER        = 108
}

function Scene:GetActorByName(name)
    return Dusk.Actor(dusk_Scene_GetActorByName(self.dusk_ptr, name))
end

Dusk.Scene = Scene
