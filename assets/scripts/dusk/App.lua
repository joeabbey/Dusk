if not Dusk then Dusk = { } end

Dusk.GetApp = function()
    return Dusk.App( dusk_App_GetInst() )
end

local App = Dusk.Class(Dusk.Object, function(self, ptr)
    Dusk.IEventDispatcher.init(self, ptr)
end)

function App:GetScene()
    return Dusk.Scene( dusk_App_GetScene(self.dusk_ptr) )
end

Dusk.App = App
