require("fuckoff")

-- fuckoff.Drop( ply:EntIndex(), "Some reason" )

-- Don't rely on this hook for a valid steamid
hook.Add( "ClientConnect", "fuckoff1", function( name, steamid, ip, index )
	print( "ClientConnect", name, steamid, ip, index )
    --return false -- Deny connection
end )

-- You can rely on this hook for a valid steamid
hook.Add( "SteamIDValidated", "fuckoff2", function( name, steamid, index )
	print(  "SteamIDValidated", name, steamid, index )
	return "Fuck off " .. name
end )