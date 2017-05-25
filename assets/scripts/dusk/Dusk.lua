require "Class"

Dusk = { }

Dusk.Object = Class(function(self, ptr)
    self.dusk_ptr = ptr
end)

require "Events"
require "Scene"
require "App"
