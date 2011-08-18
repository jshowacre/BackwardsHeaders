if !svn then require( "svn" ) end
 
concommand.Add( "svn_cleanup", function( ply, cmd, args )
    if ( IsValid( ply ) and ply:IsAdmin() ) or ply == NULL then
        local successstr = svn.cleanup( "garrysmod/addons/mymod" ) and "successful" or "unsuccessful"
        if IsValid( ply ) then
            ply:ChatPrint( string.format( "[SVN] Cleanup: %s", successstr ) )
        end
    end
end )
 
concommand.Add( "svn_update", function( ply, cmd, args )
    if ( IsValid( ply ) and ply:IsAdmin() ) or ply == NULL then
        local latestversion = 0
        local status, err = pcall( function()
            latestversion = svn.update( "garrysmod/addons/mymod", args[1], args[2] )
        end )
        if !status and err then
            if IsValid( ply ) then
                ply:ChatPrint( string.format( "[SVN] Error: %s", err ) )
            end
        else
            if IsValid( ply ) then
                ply:ChatPrint( string.format( "[SVN] Updated mod to revision %s", latestversion ) )
            end
            timer.Simple( 5, function()
                RunConsoleCommand( "changegamemode", game.GetMap(), GAMEMODE.FolderName )
            end )
        end
    end
end )