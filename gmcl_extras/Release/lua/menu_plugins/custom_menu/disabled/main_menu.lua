// THIS SCRIPT FUNCTIONS, BUT FOR WHATEVER REASON, OVERRIDING THE DEFAULT MENU OPTIONS CAUSES THE GAME TO CRASH ON GAME EXIT

SHOW_ALWAYS = 1
SHOW_INGAME = 2
SHOW_NOGAME = 3

local mainlist = vgui.Create( "DPanelList" )
mainlist:SetSize( ScrW() / 6, ScrH() / 2 )
mainlist:SetPos( 0, ScrH()/2 - mainlist:GetTall()/2 )
mainlist:SetSpacing( 16 )
mainlist:SetPadding( 0 )
mainlist:EnableHorizontal( false )
mainlist:EnableVerticalScrollbar( false )
mainlist:SetAutoSize( true )
mainlist:SetBottomUp( true )
function mainlist:Paint() end

local texGradient = surface.GetTextureID( "gui/gradient" )

surface.CreateFont( "coolvetica", 26, 0, true, false, "Menu_Option")

local Options = {
	{
		{
			label = "Start Game",
			cmd = "menu_startgame",
			when = SHOW_ALWAYS,
		},
		{
			label = "Leave Game",
			cmd = "disconnect",
			when = SHOW_INGAME,
		},
		{
			label = "Find Servers",
			menucmd = "OpenServerBrowser",
			when = SHOW_ALWAYS,
		},
		{
			label = "Load Save",
			menucmd = "OpenLoadGameDialog",
			when = SHOW_ALWAYS,
		},
	},
	{
		{
			label = "Player List",
			menucmd = "OpenPlayerlistDialog",
			when = SHOW_INGAME,
		},
	},
	{
		{
			label = "Configuration",
			menucmd = "OpenOptionsDialog",
			when = SHOW_ALWAYS,
		},
		{
			label = "Extensions",
			cmd = "menu_extensions",
			when = SHOW_ALWAYS,
		},
		{
			label = "Achievements",
			cmd = "menu_achievements",
			when = SHOW_ALWAYS,
		},
	},
	{
		{
			label = "Tutorials",
			cmd = "SchoolMe",
			when = SHOW_ALWAYS,
		},
		{
			label = "Quit",
			menucmd = "quit",
			when = SHOW_ALWAYS,
		},
	}
}

local function ReBuildMenuList( nopopup )

	mainlist:Clear()

	for k,section in pairs( Options ) do

		local panel = vgui.Create( "DPanelList" )
		//panel:SetPos( 16, ScrH()/2 - panel:GetTall()/2 )
		panel:SetSpacing( 5 )
		panel:SetPadding( 24 )
		panel:EnableHorizontal( false )
		panel:EnableVerticalScrollbar( false )
		panel:SetAutoSize( true )
		panel:SetBottomUp( true )
		function panel:Paint()
			surface.SetDrawColor( 0, 0, 0, 100 )
			surface.SetTexture( texGradient )
			surface.DrawTexturedRect( 0, 0, self:GetWide(), self:GetTall() )
		end
		
		local enough = false

		for k,settings in pairs( section ) do
			if settings.when == SHOW_ALWAYS || ( settings.when == SHOW_INGAME && client.IsInGame() ) || ( settings.when == SHOW_NOGAME && !client.IsInGame() ) then
			
				enough = true
		
				local text = vgui.Create( "DLabel" )
				text:SetFont( "Menu_Option" )
				text:SetText( settings.label )
				text:SizeToContents()
				
				text.cmd = settings.cmd
				text.menucmd = settings.menucmd

				function text:OnMousePressed()
					if self.cmd then
						console.Command( self.cmd )
					elseif self.menucmd then
						menu.Command( self.menucmd )
					end
					surface.PlaySound( "UI/buttonclickrelease.wav" )
				end

				function text:OnCursorEntered()
					self:SetTextColor( Color( 153, 217, 234, 255 ) )
					surface.PlaySound( "UI/buttonrollover.wav" )
				end

				function text:OnCursorExited()
					self:SetTextColor( Color( 255, 255, 255, 255 ) )
				end
				
				panel:AddItem( text )
			end
		end
		
		if enough then
			mainlist:AddItem( panel )
		else
			panel:Remove()
		end
	end
	
	if !nopopup then
		mainlist:MakePopup()
	end
end

hook.Add( "OnLoadingStarted", "RebuildConnected", function( )
	ReBuildMenuList( true )
end )

hook.Add( "OnLoadingStopped", "RebuildCancelLoad", function( bool1, something1, something2 )
	ReBuildMenuList( !bool1 )
end )

hook.Add( "OnDisconnectFromServer", "RebuildDisconnect", function( reasonNum )
	ReBuildMenuList()
end )

ReBuildMenuList()

//Something is wrong with this, closing the game causes a crash in vgui2.dll ONLY if we override the panel.
menu.SetPanelOverride( mainlist )