require("fuckoff")

hook.Add( "ClientConnect", "fuckoff1", function( name, steamid, ip, index )
	print( "ClientConnect", name, steamid, ip, index )
end )

hook.Add( "SteamIDValidated", "fuckoff2", function( name, steamid, index )
	print(  "SteamIDValidated", name, steamid, index )
	return "Fuck off " .. name
end )