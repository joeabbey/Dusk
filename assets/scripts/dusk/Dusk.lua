print(package.path)
package.path = package.path .. ";assets/scripts/?.lua"

require "dusk/Class"

Dusk = { }

Dusk.Object = Class(function(self, ptr)
    self.dusk_ptr = ptr
end)

require "dusk/Events"
require "dusk/Scene"
require "dusk/App"
