local args = { ... }

for i,v in ipairs( args ) do
	print( v )
end

assert( require "json.json" )