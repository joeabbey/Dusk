if not Dusk then Dusk = { } end

Dusk.GetApp = function()
    return Dusk.App( dusk_AppInst() )
end

local App = Class(Dusk.Object, function(self, ptr)
    Dusk.Object.init(self, ptr)
end)

function App:GetScene()
    return Dusk.Scene( dusk_App_GetScene() )
end

Dusk.App = App
