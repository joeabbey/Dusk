if not Dusk then Dusk = { } end

Dusk.GetComponent = function()
    return Dusk.Component( dusk_current_ScriptComponent )
end

local Component = Dusk.Class(Dusk.Object, function(self, ptr)
    Dusk.IEventDispatcher.init(self, ptr)
end)

function Component:GetActor()
    return Dusk.Actor( dusk_Component_GetActor(self.dusk_ptr) )
end

Dusk.Component = Component
