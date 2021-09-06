if getgenv().Fatality then return getgenv().Fatality end

-- // Services
local Players = game:GetService("Players")
local Workspace = game:GetService("Workspace")
local GuiService = game:GetService("GuiService")
local RunService = game:GetService("RunService")

-- // Vars
local Heartbeat = RunService.Heartbeat
local LocalPlayer = Players.LocalPlayer
local CurrentCamera = Workspace.CurrentCamera
local Mouse = LocalPlayer:GetMouse()

-- // Optimisation Vars (ugly)
local Drawingnew = Drawing.new
local Color3fromRGB = Color3.fromRGB
local Vector2new = Vector2.new
local GetGuiInset = GuiService.GetGuiInset
local Randomnew = Random.new
local mathfloor = math.floor
local CharacterAdded = LocalPlayer.CharacterAdded
local CharacterAddedWait = CharacterAdded.Wait
local WorldToViewportPoint = CurrentCamera.WorldToViewportPoint
local RaycastParamsnew = RaycastParams.new
local EnumRaycastFilterTypeBlacklist = Enum.RaycastFilterType.Blacklist
local Raycast = Workspace.Raycast
local GetPlayers = Players.GetPlayers
local Instancenew = Instance.new
local IsDescendantOf = Instancenew("Part").IsDescendantOf
local FindFirstChildWhichIsA = Instancenew("Part").FindFirstChildWhichIsA
local FindFirstChild = Instancenew("Part").FindFirstChild
local tableremove = table.remove
local tableinsert = table.insert

-- // Silent Aim Vars
getgenv().Fatality = {
    Enabled = true,
    ShowFOV = true,
    FOVColour = Color3fromRGB(192,192,192),
    VisibleCheck = true,
    FOV = 60,
    HitChance = 100,
    Selected = LocalPlayer,
    SelectedPart = nil,
    TargetPart = {"Head", "RightLowerArm", "LeftLowerArm", "LeftLowerLeg", "RightLowerLeg", "RightUpperArm", "LeftUpperArm", "LeftUpperLeg", "RightUpperLeg", "UpperTorso"},
    Ignored = {
        Teams = {
            {
                Team = LocalPlayer.Team,
                TeamColor = LocalPlayer.TeamColor,
            },
        },
        Players = {
            LocalPlayer,
            91318356
        }
    }
}
local Fatality = getgenv().Fatality

-- // Show FOV
local circle = Drawingnew("Circle")
circle.Transparency = 1
circle.Thickness = 1.5
circle.Color = Fatality.FOVColour
circle.Filled = false
function Fatality.UpdateFOV()
    if (circle) then
        -- // Set Circle Properties
        circle.Visible = Fatality.ShowFOV
        circle.Radius = (Fatality.FOV * 3)
        circle.Position = Vector2new(Mouse.X, Mouse.Y + GetGuiInset(GuiService).Y)
        circle.Color = Fatality.FOVColour

        -- // Return circle
        return circle
    end
end

-- // Custom Functions
local CalcChance = function(percentage)
    percentage = mathfloor(percentage)
    local chance = mathfloor(Randomnew().NextNumber(Randomnew(), 0, 1) * 100) / 100
    return chance <= percentage / 100
end

-- // Customisable Checking Functions: Is a part visible
function Fatality.IsPartVisible(Part, PartDescendant)
    -- // Vars
    local Character = LocalPlayer.Character or CharacterAddedWait(CharacterAdded)
    local Origin = CurrentCamera.CFrame.Position
    local _, OnScreen = WorldToViewportPoint(CurrentCamera, Part.Position)

    -- // If Part is on the screen
    if (OnScreen) then
        -- // Vars: Calculating if is visible
        local raycastParams = RaycastParamsnew()
        raycastParams.FilterType = EnumRaycastFilterTypeBlacklist
        raycastParams.FilterDescendantsInstances = {Character, CurrentCamera}

        local Result = Raycast(Workspace, Origin, Part.Position - Origin, raycastParams)
        if (Result) then
            local PartHit = Result.Instance
            local Visible = (not PartHit or IsDescendantOf(PartHit, PartDescendant))

            -- // Return
            return Visible
        end
    end

    -- // Return
    return false
end

-- // Ignore player
function Fatality.IgnorePlayer(Player)
    -- // Vars
    local Ignored = Fatality.Ignored
    local IgnoredPlayers = Ignored.Players

    -- // Find player in table
    for i = 1, #IgnoredPlayers do
        local IgnoredPlayer = IgnoredPlayers[i]

        if (IgnoredPlayer == Player) then
            return false
        end
    end

    -- // Blacklist player
    tableinsert(IgnoredPlayers, Player)
    return true
end

-- // Unignore Player
function Fatality.UnIgnorePlayer(Player)
    -- // Vars
    local Ignored = Fatality.Ignored
    local IgnoredPlayers = Ignored.Players

    -- // Find player in table
    for i = 1, #IgnoredPlayers do
        local IgnoredPlayer = IgnoredPlayers[i]

        if (IgnoredPlayer == Player) then
            tableremove(IgnoredPlayers, i)
            return true
        end
    end

    -- //
    return false
end

-- // Ignore team
function Fatality.IgnoreTeam(Team, TeamColor)
    -- // Vars
    local Ignored = Fatality.Ignored
    local IgnoredTeams = Ignored.Teams

    -- // Find team in table
    for i = 1, #IgnoredTeams do
        local IgnoredTeam = IgnoredTeams[i]

        if (IgnoredTeam.Team == Team and IgnoredTeam.TeamColor == TeamColor) then
            return false
        end
    end

    -- // Ignore team
    tableinsert(IgnoredTeams, {Team, TeamColor})
    return true
end

-- // Unignore team
function Fatality.UnIgnoreTeam(Team, TeamColor)
    -- // Vars
    local Ignored = Fatality.Ignored
    local IgnoredTeams = Ignored.Teams

    -- // Find team in table
    for i = 1, #IgnoredTeams do
        local IgnoredTeam = IgnoredTeams[i]

        if (IgnoredTeam.Team == Team and IgnoredTeam.TeamColor == TeamColor) then
            -- // Remove
            tableremove(IgnoredTeams, i)
            return true
        end
    end

    -- // Return
    return false
end

-- //  Toggle team check
function Fatality.TeamCheck(Toggle)
    if (Toggle) then
        return Fatality.IgnoreTeam(LocalPlayer.Team, LocalPlayer.TeamColor)
    end

    return Fatality.UnIgnoreTeam(LocalPlayer.Team, LocalPlayer.TeamColor)
end

-- // Check teams
function Fatality.IsIgnoredTeam(Player)
    -- // Vars
    local Ignored = Fatality.Ignored
    local IgnoredTeams = Ignored.Teams

    -- // Check if team is ignored
    for i = 1, #IgnoredTeams do
        local IgnoredTeam = IgnoredTeams[i]

        if (Player.Team == IgnoredTeam.Team and Player.TeamColor == IgnoredTeam.TeamColor) then
            return true
        end
    end

    -- // Return
    return false
end

-- // Check if player (and team) is ignored
function Fatality.IsIgnored(Player)
    -- // Vars
    local Ignored = Fatality.Ignored
    local IgnoredPlayers = Ignored.Players

    -- // Loop
    for i = 1, #IgnoredPlayers do
        local IgnoredPlayer = IgnoredPlayers[i]

        -- // Check if Player Id
        if (typeof(IgnoredPlayer) == "number") then
            if (Player.UserId == IgnoredPlayer) then
                return true
            end
        end

        -- // Normal Player Instance
        if (IgnoredPlayer == Player) then
            return true
        end
    end

    -- // Team check
    return Fatality.IsIgnoredTeam(Player)
end

-- // Get the Direction, Normal and Material
function Fatality.Raycast(Origin, Destination, UnitMultiplier)
    if (typeof(Origin) == "Vector3" and typeof(Destination) == "Vector3") then
        -- // Handling
        if (not UnitMultiplier) then UnitMultiplier = 1 end

        -- // Vars
        local Direction = (Destination - Origin).Unit * UnitMultiplier
        local RaycastResult = Raycast(Workspace, Origin, Direction)

        if (RaycastResult ~= nil) then
            local Normal = RaycastResult.Normal
            local Material = RaycastResult.Material

            return Direction, Normal, Material
        end
    end

    -- // Return
    return nil
end

-- // Get Character
function Fatality.Character(Player)
    return Player.Character
end

-- // Check Health
function Fatality.CheckHealth(Player)
    local Character = Fatality.Character(Player)
    local Humanoid = FindFirstChildWhichIsA(Character, "Humanoid")

    local Health = (Humanoid and Humanoid.Health or 0)
    return Health > 0
end

-- // Check if silent aim can used
function Fatality.Check()
    return (Fatality.Enabled == true and Fatality.Selected ~= LocalPlayer and Fatality.SelectedPart ~= nil)
end
Fatality.checkSilentAim = Fatality.Check

-- // Get Closest Target Part
function Fatality.GetClosestTargetPartToCursor(Character)
    local TargetParts = Fatality.TargetPart

    -- // Vars
    local ClosestPart = nil
    local ClosestPartPosition = nil
    local ClosestPartOnScreen = false
    local ClosestPartMagnitudeFromMouse = nil
    local ShortestDistance = 1/0

    -- //
    local function CheckTargetPart(TargetPartName)
        local TargetPart = FindFirstChild(Character, TargetPartName)

        if (TargetPart) then
            local PartPos, onScreen = WorldToViewportPoint(CurrentCamera, TargetPart.Position)
            local Magnitude = (Vector2new(PartPos.X, PartPos.Y) - Vector2new(Mouse.X, Mouse.Y)).Magnitude

            if (Magnitude < ShortestDistance) then
                ClosestPart = TargetPart
                ClosestPartPosition = PartPos
                ClosestPartOnScreen = onScreen
                ClosestPartMagnitudeFromMouse = Magnitude
                ShortestDistance = Magnitude
            end
        end
    end

    -- // String check
    if (typeof(TargetParts) == "string") then
        -- // Check if it all
        if (TargetParts == "All") then
            -- // Loop through character children
            for _, v in ipairs(Character:GetChildren()) do
                -- // See if it a part
                if not (v:IsA("BasePart")) then
                    continue
                end

                -- // Check it
                CheckTargetPart(v.Name)
            end
        else
            -- // Individual
            CheckTargetPart(TargetParts)
        end
    end

    -- // Loop through all target parts
    if (typeof(TargetParts) == "table") then
        for i = 1, #TargetParts do
            local TargetPartName = TargetParts[i]
            CheckTargetPart(TargetPartName)
        end
    end

    -- //
    return ClosestPart, ClosestPartPosition, ClosestPartOnScreen, ClosestPartMagnitudeFromMouse
end

-- // Silent Aim Function
function Fatality.GetClosestPlayerToCursor()
    -- // Vars
    local TargetPart = nil
    local ClosestPlayer = nil
    local Chance = CalcChance(Fatality.HitChance)
    local ShortestDistance = 1/0

    -- // Chance
    if (not Chance) then
        Fatality.Selected = LocalPlayer
        Fatality.SelectedPart = nil

        return LocalPlayer
    end

    -- // Loop through all players
    local AllPlayers = GetPlayers(Players)
    for i = 1, #AllPlayers do
        local Player = AllPlayers[i]
        local Character = Fatality.Character(Player)

        if (Fatality.IsIgnored(Player) == false and Character) then
            local TargetPartTemp, _, _, Magnitude = Fatality.GetClosestTargetPartToCursor(Character)

            -- // Check if part exists and health
            if (TargetPartTemp and Fatality.CheckHealth(Player)) then
                -- // Check if is in FOV
                if (circle.Radius > Magnitude and Magnitude < ShortestDistance) then
                    -- // Check if Visible
                    if (Fatality.VisibleCheck and not Fatality.IsPartVisible(TargetPartTemp, Character)) then continue end

                    -- // Set vars
                    ClosestPlayer = Player
                    ShortestDistance = Magnitude
                    TargetPart = TargetPartTemp
                end
            end
        end
    end

    -- // End
    Fatality.Selected = ClosestPlayer
    Fatality.SelectedPart = TargetPart
end

-- // Heartbeat Function
Heartbeat:Connect(function()
    Fatality.UpdateFOV()
    Fatality.GetClosestPlayerToCursor()
end)

return Fatality

-- // Examples // --

--// Namecall Version // --

--[[
-- // Load Fatality Module
local Fatality = loadstring(game:HttpGet("https://raw.githubusercontent.com/Stefanuk12/ROBLOX/master/Universal/Fatality/Module.lua"))()

-- // Hook
local __namecall
__namecall = hookmetamethod(game, "__namecall", function(...)
    -- // Vars
    local args = {...}
    local self = args[1]
    local method = getnamecallmethod()

    -- // Checks
    if (method == "FireServer") then
        if (self.Name == "RemoteNameHere") then
            -- change args

            -- // Return changed arguments
            return __namecall(unpack(args))
        end
    end

    -- // Return
    return __namecall(...)
end)
]]--

-- // Index Version // --

--[[
-- // Load Fatality Module
local Fatality = loadstring(game:HttpGet("https://raw.githubusercontent.com/Stefanuk12/ROBLOX/master/Universal/Fatality/Module.lua"))()

-- // Hook
local __index
__index = hookmetamethod(game, "__index", function(t, k)
    -- // Check if it trying to get our mouse's hit or target
    if (t:IsA("Mouse") and (k == "Hit" or k == "Target")) then
        -- // If we can use the silent aim
        if (Fatality.Check()) then
            -- // Vars
            local TargetPart = Fatality.SelectedPart

            -- // Return modded val
            return (k == "Hit" and TargetPart.CFrame or TargetPart)
        end
    end

    -- // Return
    return __index(t, k)
end)
]]--
