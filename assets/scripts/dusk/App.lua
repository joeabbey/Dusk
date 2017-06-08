if not Dusk then Dusk = { } end

Dusk.GetApp = function()
    return Dusk.App( dusk_App_GetInst() )
end

local App = Dusk.Class(Dusk.Object, function(self, ptr)
    Dusk.IEventDispatcher.init(self, ptr)
end)

App.Events = {
    UPDATE = 1,
    RENDER = 2
}

function App:GetScene()
    return Dusk.Scene( dusk_App_GetScene(self.dusk_ptr) )
end

function App:GetSceneByName(name)
    return Dusk.Scene( dusk_App_GetSceneByName(self.dusk_ptr, name) )
end

function App:PushScene(scene)
    dusk_App_PushScene(self.dusk_ptr, scene.dusk_ptr)
end

function App:PopScene()
    dusk_App_PopScene(self.dusk_ptr)
end

Dusk.App = App
