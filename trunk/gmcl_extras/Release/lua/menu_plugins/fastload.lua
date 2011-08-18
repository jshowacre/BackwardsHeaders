for k, v in pairs( file.FindInLua( "menu_plugins/custom_menu/*.lua" ) ) do
	include( "menu_plugins/custom_menu/" .. v )
end