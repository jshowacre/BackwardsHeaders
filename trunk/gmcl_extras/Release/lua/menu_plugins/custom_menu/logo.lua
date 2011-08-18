local PANEL = {}
local Logo = Material( "console/logo_text" )

function PANEL:Init()

	self:SetMouseInputEnabled( false )
	self:SetKeyboardInputEnabled( false )
	
end

function PANEL:Paint()

	surface.SetMaterial( Logo )
	surface.SetDrawColor( 255, 255, 255, 255 )
	surface.DrawTexturedRect( 0, 0, self:GetWide(), self:GetTall() )

	return true

end

function PANEL:PerformLayout()
	local w = ScrH() * 0.85
	local h = w / 4
	local y = ScrH() * 0.2
	
	self:SetPos( 0, y )
	self:SetSize( w, h )
	self:CenterHorizontal()
end

local GLogo = vgui.CreateFromTable( vgui.RegisterTable( PANEL, "Panel" ) )