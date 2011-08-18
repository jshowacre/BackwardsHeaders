//=============================================================================//
//  ___  ___   _   _   _    __   _   ___ ___ __ __
// |_ _|| __| / \ | \_/ |  / _| / \ | o \ o \\ V /
//  | | | _| | o || \_/ | ( |_n| o ||   /   / \ / 
//  |_| |___||_n_||_| |_|  \__/|_n_||_|\\_|\\ |_|  2007
//										 
//=============================================================================//


local pnlDownloadType = vgui.RegisterFile( "download_type.lua" )
local pnlRunnerType = vgui.RegisterFile( "runner.lua" )

PANEL.Base = "Panel"

/*---------------------------------------------------------

---------------------------------------------------------*/
function PANEL:Init()

	self.lblHeadline 	= vgui.Create( "DLabel", self )
	self.lblHeadline:SetFont( "LoadingProgress" )
	self.lblHeadline:SetContentAlignment( 5 )
	
	self.Downloads = {}
	
	self.Downloads[ 'other' ] 		= self:DownloadType( "Other", 		"gui/silkicons/box", 		25 )
	self.Downloads[ 'lua' ] 		= self:DownloadType( "Lua Cache", 	"gui/silkicons/page", 		30 )
	self.Downloads[ 'textures' ] 	= self:DownloadType( "Textures", 	"gui/silkicons/palette", 	40 )
	self.Downloads[ 'models' ] 		= self:DownloadType( "Models", 		"gui/silkicons/brick_add", 	50 )
	self.Downloads[ 'maps' ] 		= self:DownloadType( "Maps", 		"gui/silkicons/world", 		60 )
	self.Downloads[ 'sounds' ] 		= self:DownloadType( "Sounds", 		"gui/silkicons/sound", 		35 )
	self.Downloads[ 'text' ] 		= self:DownloadType( "Text", 		"gui/silkicons/table_edit", 30 )

end

/*---------------------------------------------------------

---------------------------------------------------------*/
function PANEL:DownloadType( strName, strTexture, speed )

	local ctrl = vgui.CreateFromTable( pnlDownloadType, self )
	ctrl:SetText( strName )
	ctrl:SetIcon( strTexture )
	ctrl:SetSpeed( speed )
	return ctrl

end

/*---------------------------------------------------------

---------------------------------------------------------*/
function PANEL:PerformLayout()

	self.lblHeadline:SetPos( 0, 20 )
	self.lblHeadline:SetSize( ScrW(), 20 )
	
	y = 50
	
	for k, panel in pairs( self.Downloads ) do
	
		if ( panel:ShouldBeVisible() ) then
		
			panel:SetVisible( true )
			panel:InvalidateLayout( true )
			panel:CenterHorizontal()
			panel.y = y
			y = y + panel:GetTall() + 2
			
		else
		
			panel:SetVisible( false )
			
		end
	
	end
	
end

/*---------------------------------------------------------

---------------------------------------------------------*/
function PANEL:RefreshDownloadables()

	self.Downloadables = GetDownloadables()
	if ( !self.Downloadables ) then return end
	
	self:ClearDownloads()
	
	local iDownloading = 0
	for k, v in pairs( self.Downloadables ) do
	
		v = string.gsub( v, ".bz2", "" )
		v = string.gsub( v, ".ztmp", "" )
		v = string.gsub( v, "\\", "/" )
	
		iDownloading = iDownloading + self:ClassifyDownload( v )
			
	end
	
	if ( iDownloading == 0 ) then return end
	
	self.lblHeadline:SetText( Format( "%i files needed from server", iDownloading ) )
	self:InvalidateLayout()

end

/*---------------------------------------------------------

---------------------------------------------------------*/
function PANEL:ClearDownloads()

	for k, panel in pairs( self.Downloads ) do
		panel:Clean()
		panel:SetVisible( false )
	end

end

/*---------------------------------------------------------

---------------------------------------------------------*/
function PANEL:ClassifyDownload( filename )

	local ctrl = self.Downloads[ 'other' ]
	
	if ( string.find( filename, ".dua" ) ) then
		ctrl = self.Downloads[ 'lua' ]
	elseif ( string.find( filename, ".vtf" ) || string.find( filename, ".vmt" ) ) then
		ctrl = self.Downloads[ 'textures' ]
	elseif ( string.find( filename, "models/" ) || string.find( filename, "models\\" ) || string.find( filename, ".mdl" ) ) then
		ctrl = self.Downloads[ 'models' ]
	elseif ( string.find( filename, "sound/" ) || string.find( filename, "sound\\" ) || string.find( filename, ".wav" ) || string.find( filename, ".mp3" ) ) then
		ctrl = self.Downloads[ 'sounds' ]
	elseif ( string.find( filename, ".bsp" ) ) then
		ctrl = self.Downloads[ 'maps' ]
	elseif ( string.find( filename, ".txt" ) ) then
		ctrl = self.Downloads[ 'text' ]
	end
	
	return ctrl:AddFile( filename )
	
end

/*---------------------------------------------------------

---------------------------------------------------------*/
function PANEL:CheckDownloadTables()

	// The string table might have only partially come through
	// So if the number of strings changed, we need to update
	local NumDownloadables = NumDownloadables()
	if ( !NumDownloadables ) then return end
	
	if ( !self.NumDownloadables || NumDownloadables != self.NumDownloadables ) then
		
		self.NumDownloadables = NumDownloadables
		self:RefreshDownloadables()
	
	end

end

/*---------------------------------------------------------

---------------------------------------------------------*/
function PANEL:Clean()

	self.NumDownloadables = nil
	self.Downloadables = nil
	self.FilesToDownload = nil
	
	self:ClearDownloads()
	
	self.lblHeadline:SetText( "" )
	
end


/*---------------------------------------------------------
	We finished downloading the current file (probably)
---------------------------------------------------------*/
function PANEL:CurrentDownloadFinished()

	if (!self.strCurrentDownload) then return end
	
	for k, panel in pairs( self.Downloads ) do
		panel:Downloaded( self.strCurrentDownload )
	end
	
	if ( self.CurrentRunner ) then
		if ( self.CurrentRunner:IsValid() ) then
			self.CurrentRunner:SetRepeat( false )
		end
		self.CurrentRunner = nil
	end
	
	self.strCurrentDownload = nil

end

/*---------------------------------------------------------
	
---------------------------------------------------------*/
function PANEL:DownloadingFile( filename )

	self:CheckDownloadTables()
	self:CurrentDownloadFinished()
	self.strCurrentDownload = filename
	
	if ( self.CurrentRunner ) then
		if ( self.CurrentRunner:IsValid() ) then
			self.CurrentRunner:SetRepeat( false )
		end
		self.CurrentRunner = nil
	end
	
	for k, panel in pairs( self.Downloads ) do
		self.CurrentRunner = panel:MakeRunner( self.strCurrentDownload )
		if ( self.CurrentRunner ) then
			self.CurrentRunner:SetRepeat( true )
			break
		end
	end

end

/*---------------------------------------------------------

---------------------------------------------------------*/
function PANEL:StatusChanged( strNewStatus )

	// The status might have changed because a file has been downloaded.
	// So check if the files we're waiting to download have been downloaded
	self:CurrentDownloadFinished()
	self:CheckDownloadTables()
		
end

/*---------------------------------------------------------

---------------------------------------------------------*/
function PANEL:AddRunner( icon, speed )

	local Runner = vgui.CreateFromTable( pnlRunnerType, self )
		Runner:SetUp( icon, speed )
	
	return Runner
	
end
