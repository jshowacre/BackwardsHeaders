-- Menu environment doesn't automatically include all of the vgui types..

for k,v in ipairs( file.FindInLua( "vgui/*.lua") ) do
	include( "vgui/" .. v )
end