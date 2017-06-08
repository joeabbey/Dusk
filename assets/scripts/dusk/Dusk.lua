package.path = package.path .. ";assets/scripts/?.lua"

Dusk = { }

require "dusk/Class"

Dusk.Object = Dusk.Class(function(self, ptr)
    self.dusk_ptr = ptr
end)

require "dusk/Events"
require "dusk/Actor"
require "dusk/Component"
require "dusk/Scene"
require "dusk/App"
