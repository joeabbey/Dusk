if not Dusk then Dusk = { } end

local Actor = Dusk.Class(Dusk.IEventDispatcher, function(self, ptr)
    Dusk.IEventDispatcher.init(self, ptr)
end)

Actor.Events = {
    LOAD_START    = 201,
    FREE_START    = 202,
    LOAD_FINISHED = 203,
    FREE_FINISHED = 204,
    UPDATE        = 205,
    RENDER        = 206
}

function Actor:GetPosition()
    return dusk_Actor_GetPosition(self.dusk_ptr)
end

function Actor:SetPosition(x, y, z)
    return dusk_Actor_SetPosition(self.dusk_ptr, x, y, z)
end

function Actor:GetRotation()
    return dusk_Actor_GetRotation(self.dusk_ptr)
end

function Actor:SetRotation(x, y, z)
    return dusk_Actor_SetRotation(self.dusk_ptr, x, y, z)
end

function Actor:GetScale()
    return dusk_Actor_GetScale(self.dusk_ptr)
end

function Actor:SetScale(x, y, z)
    return dusk_Actor_SetScale(self.dusk_ptr, x, y, z)
end

Dusk.Actor = Actor
