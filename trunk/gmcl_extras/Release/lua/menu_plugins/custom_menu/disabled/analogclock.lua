local PANEL = {}

function PANEL:Init()

	--self:SetParent( GetOverlayPanel() )
	self:SetMouseInputEnabled( false )
	self:SetKeyboardInputEnabled( false )
	
end

local clockMat = Material( "blackops/clock" )

local smoothsecond = tonumber( os.time() )

local scale = ScrH() / 6
local radius = scale * 0.5

local dblSecond, dblMinute, dblHour
local strSec, strMin, strHr

local lineSecondX1, lineSecondX2, lineMinuteX1, lineMinuteX2, lineHourX1, lineHourX2
local lineSecondY1, lineSecondY2, lineMinuteY1, lineMinuteY2, lineHourY1, lineHourY2

local seondlinelength = radius * 0.80
local minutelinelength = radius * 0.65
local hourlinelength = radius * 0.35
local x,y

local halfpi = math.pi / 2

function PANEL:Paint()

	if tonumber( os.time() ) - smoothsecond >= 5 then
		smoothsecond = tonumber( os.time() )
	else
		smoothsecond = math.Approach( smoothsecond, tonumber( os.time() ), RealFrameTime() * 3 ) --if you remove the * 3 it makes it continuously smooth
	end
	dblSecond = ( smoothsecond * 6 - 90 ) * -1 + 180
	
    dblMinute = ( os.date( "%M" ) + os.date( "%S" )  / 60 ) * 6 - 90
    dblHour   = ( os.date( "%H" ) + os.date( "%M" )  / 60 ) * 30 - 90
	
	lineSecondX1, lineMinuteX1, lineHourX1 = radius, radius, radius
	lineSecondY1, lineMinuteY1, lineHourY1 = radius, radius, radius
	
    lineSecondX2 = seondlinelength * -math.cos( dblSecond * math.pi / 180 ) + lineSecondX1
    lineSecondY2 = seondlinelength * math.sin( dblSecond * math.pi / 180 ) + lineSecondY1
    lineMinuteX2 = minutelinelength * math.cos( dblMinute * math.pi / 180 ) + lineMinuteX1
    lineMinuteY2 = minutelinelength * math.sin( dblMinute * math.pi / 180 ) + lineMinuteY1
    lineHourX2 = hourlinelength * math.cos( dblHour   * math.pi / 180 ) + lineHourX1
    lineHourY2 = hourlinelength * math.sin( dblHour   * math.pi / 180 ) + lineHourY1
	
	surface.SetMaterial( clockMat )
	surface.SetDrawColor( 255, 255, 255, 255 )
	surface.DrawTexturedRect( 0, 0, self:GetWide(), self:GetTall() )
	
	surface.SetDrawColor( 255, 0, 0, 255 )
	surface.DrawLine( lineSecondX1, lineSecondY1, lineSecondX2, lineSecondY2 )
	
	surface.SetDrawColor( 255, 255, 255, 255 )
	surface.DrawLine( lineMinuteX1, lineMinuteY1, lineMinuteX2, lineMinuteY2 )
	
	surface.SetDrawColor( 255, 255, 255, 255 )
	surface.DrawLine( lineHourX1, lineHourY1, lineHourX2, lineHourY2 )
	
	for i = 1, 12 do

		x = -math.cos( math.rad( i / 12 * 360 ) + halfpi ) * ( radius * 0.9 ) + lineHourX1
		y = -math.sin( math.rad( i / 12 * 360 ) + halfpi ) * ( radius * 0.9 ) + lineHourY1

		draw.SimpleText( i, "TargetIDSmall", x, y, Color( 0, 0, 0, 255 ), 1, 1 )
	end
	
	draw.SimpleText( os.date( "%I" ) .. ":" .. os.date( "%M" ) .. ":" .. os.date( "%S" ), "TargetIDSmall", radius, radius-15, Color( 255, 255, 255, 255 ), 1, 1 )
	draw.SimpleText( os.date( "%p" ), "TargetIDSmall", radius, radius+15, Color( 255, 255, 255, 255 ), 1, 1 )

end

function PANEL:PerformLayout()
	
	self:SetPos( ScrW() - scale - 10, 10 )
	self:SetSize( scale, scale )
	
end

vgui.CreateFromTable( vgui.RegisterTable( PANEL, "Panel" ) )