hook.Add( "OnLoadingURLChanged", "OnURLChanged", function( url ) -- Can override the loading URL
	return url
end )

hook.Add( "OnStringTableChanged", "OnTableChanged", function( stringTable, stringNumber, newString )	
	if stringTable:GetName() == "OgfGiyegPNbFOf" and newString == "loading_url" then
		local oldurl = stringTable:GetString( stringNumber )
		local newurl = hook.Call( "OnLoadingURLChanged", nil, oldurl )
		if newurl and newurl != oldurl then
			stringTable:SetString( stringNumber, newurl )
		end
	end
end )

hook.Add( "InstallStringTableCallback", "InstallCallback", function( tableName )
	if tableName == "OgfGiyegPNbFOf" then
		return true
	end
end )

hook.Add( "InstallStringTableCallback", "InstallCallback", function( tableName )
	local tbl = StringTable( tableName )
	print( tableName )
	PrintTable( tbl:GetTable() )
	if tableName == "OgfGiyegPNbFOf" then
		return true
	end
end )