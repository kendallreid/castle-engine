{
  Copyright 2020-2020 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Main "playing game" state, where most of the game logic takes place. }
unit GameStatePlay;

interface

uses Classes,
  CastleUIState, CastleComponentSerialize, CastleUIControls, CastleControls,
  CastleKeysMouse, CastleViewport, CastleScene, CastleVectors, CastleCameras,
  CastleTransform, CastleInputs, CastleThirdPersonNavigation, CastleDebugTransform,
  GameEnemy;

type
  { Main "playing game" state, where most of the game logic takes place. }
  TStatePlay = class(TUIState)
  private
    { Enemies behaviours }
    Enemies: TEnemyList;

    DebugAvatar: TDebugTransform;

    { Components designed using CGE editor, loaded from state-main.castle-user-interface. }
    LabelFps: TCastleLabel;
    MainViewport: TCastleViewport;
    ThirdPersonNavigation: TCastleThirdPersonNavigation;
    SceneAvatar: TCastleScene;
    CheckboxAimAvatar: TCastleCheckbox;
    CheckboxDebugAvatarColliders: TCastleCheckbox;
    CheckboxImmediatelyFixBlockedCamera: TCastleCheckbox;
    procedure ChangeCheckboxAimAvatar(Sender: TObject);
    procedure ChangeCheckboxDebugAvatarColliders(Sender: TObject);
    procedure ChangeCheckboxImmediatelyFixBlockedCamera(Sender: TObject);
  public
    procedure Start; override;
    procedure Stop; override;
    procedure Update(const SecondsPassed: Single; var HandleInput: Boolean); override;
    function Press(const Event: TInputPressRelease): Boolean; override;
  end;

var
  StatePlay: TStatePlay;

implementation

uses SysUtils, Math, StrUtils,
  CastleSoundEngine, CastleLog, CastleStringUtils, CastleFilesUtils, CastleUtils,
  GameStateMenu;

{ TStatePlay ----------------------------------------------------------------- }

procedure TStatePlay.Start;
var
  UiOwner: TComponent;
  SoldierScene: TCastleScene;
  Enemy: TEnemy;
  I: Integer;
begin
  inherited;

  { Load designed user interface }
  InsertUserInterface('castle-data:/state_play.castle-user-interface', FreeAtStop, UiOwner);

  { Find components, by name, that we need to access from code }
  LabelFps := UiOwner.FindRequiredComponent('LabelFps') as TCastleLabel;
  MainViewport := UiOwner.FindRequiredComponent('MainViewport') as TCastleViewport;
  ThirdPersonNavigation := UiOwner.FindRequiredComponent('ThirdPersonNavigation') as TCastleThirdPersonNavigation;
  SceneAvatar := UiOwner.FindRequiredComponent('SceneAvatar') as TCastleScene;
  CheckboxAimAvatar := UiOwner.FindRequiredComponent('CheckboxAimAvatar') as TCastleCheckbox;
  CheckboxDebugAvatarColliders := UiOwner.FindRequiredComponent('CheckboxDebugAvatarColliders') as TCastleCheckbox;
  CheckboxImmediatelyFixBlockedCamera := UiOwner.FindRequiredComponent('CheckboxImmediatelyFixBlockedCamera') as TCastleCheckbox;

  { Create TEnemy instances, add them to Enemies list }
  Enemies := TEnemyList.Create(true);
  for I := 1 to 4 do
  begin
    SoldierScene := UiOwner.FindRequiredComponent('SceneSoldier' + IntToStr(I)) as TCastleScene;
    Enemy := TEnemy.Create(SoldierScene);
    Enemies.Add(Enemy);
  end;

  CheckboxAimAvatar.OnChange := @ChangeCheckboxAimAvatar;
  CheckboxDebugAvatarColliders.OnChange := @ChangeCheckboxDebugAvatarColliders;
  CheckboxImmediatelyFixBlockedCamera.OnChange := @ChangeCheckboxImmediatelyFixBlockedCamera;

  { Make SceneAvatar collide using a sphere.
    Sphere is more useful than default bounding box for avatars and creatures
    that move in the world, look ahead, can climb stairs etc. }
  SceneAvatar.MiddleHeight := 0.9;
  SceneAvatar.CollisionSphereRadius := 0.5;

  { Gravity means that object tries to maintain a constant height
    (SceneAvatar.PreferredHeight) above the ground.
    GrowSpeed means that object raises properly (makes walking up the stairs work).
    FallSpeed means that object falls properly (makes walking down the stairs,
    falling down pit etc. work). }
  SceneAvatar.Gravity := true;
  SceneAvatar.GrowSpeed := 10.0;
  SceneAvatar.FallSpeed := 10.0;

  { Visualize SceneAvatar bounding box, sphere, middle point, direction etc. }
  DebugAvatar := TDebugTransform.Create(FreeAtStop);
  DebugAvatar.Attach(SceneAvatar);

  { Configure ThirdPersonNavigation, some things that cannot be yet adusted using CGE editor.
    In particular assign some keys that are not assigned by default. }
  ThirdPersonNavigation.Input_LeftStrafe.Assign(keyQ);
  ThirdPersonNavigation.Input_RightStrafe.Assign(keyE);
  ThirdPersonNavigation.Input_CameraCloser.Assign(keyNone, keyNone, '', false, mbLeft, mwUp);
  ThirdPersonNavigation.Input_CameraFurther.Assign(keyNone, keyNone, '', false, mbLeft, mwDown);
  ThirdPersonNavigation.MouseLook := true; // by default use mouse look
  ThirdPersonNavigation.Init;
end;

procedure TStatePlay.Stop;
begin
  FreeAndNil(Enemies);
  inherited;
end;

procedure TStatePlay.Update(const SecondsPassed: Single; var HandleInput: Boolean);

  // Test: use this to make AimAvatar only when *holding* right mouse button.
  (*
  procedure UpdateAimAvatar;
  begin
    if mbRight in Container.MousePressed then
      ThirdPersonNavigation.AimAvatar := aaHorizontal
    else
      ThirdPersonNavigation.AimAvatar := aaNone;

    { In this case CheckboxAimAvatar only serves to visualize whether
      the right mouse button is pressed now. }
    CheckboxAimAvatar.Checked := ThirdPersonNavigation.AimAvatar <> aaNone;
  end;
  *)

begin
  inherited;
  { This virtual method is executed every frame.}
  LabelFps.Caption := 'FPS: ' + Container.Fps.ToString;
  // UpdateAimAvatar;
end;

function TStatePlay.Press(const Event: TInputPressRelease): Boolean;

  function AvatarRayCast: TCastleTransform;
  var
    AvatarMiddle: TVector3;
  begin
    { Make avatar temporarily non-existing for ray-cast,
      to avoid hitting SceneAvatar geometry with the WorldRayCast.
      Save SceneAvatar.Middle value first, as non-existing avatar would have lower Middle. }
    AvatarMiddle := SceneAvatar.Middle;
    SceneAvatar.Disable;
    try
      Result := MainViewport.Items.WorldRayCast(AvatarMiddle, SceneAvatar.Direction);
    finally SceneAvatar.Enable end;
  end;

var
  HitByAvatar: TCastleTransform;
  HitEnemy: TEnemy;
begin
  Result := inherited;
  if Result then Exit; // allow the ancestor to handle keys

  { This virtual method is executed when user presses
    a key, a mouse button, or touches a touch-screen.

    Note that each UI control has also events like OnPress and OnClick.
    These events can be used to handle the "press", if it should do something
    specific when used in that UI control.
    The TStatePlay.Press method should be used to handle keys
    not handled in children controls.
  }

  if Event.IsMouseButton(mbLeft) then
  begin
    SoundEngine.Sound(SoundEngine.SoundFromName('shoot_sound'));

    { We clicked on enemy if
      - HitByAvatar indicates we hit something
      - This something has, as 1st child, a TEnemy instance.

      We depend here on our TEnemy behaviour:
      - TEnemy instance adds itself as child of TCastleScene
      - TEnemy has no collidable things by itself, so it's not returned itself by WorldRayCast.
    }
    HitByAvatar := AvatarRayCast;
    if (HitByAvatar <> nil) and
       (HitByAvatar.Count <> 0) and
       (HitByAvatar.Items[0] is TEnemy) then
    begin
      HitEnemy := HitByAvatar.Items[0] as TEnemy;
      HitEnemy.Hurt;
    end;

    Exit(true);
  end;

  if Event.IsKey(keyM) then
  begin
    ThirdPersonNavigation.MouseLook := not ThirdPersonNavigation.MouseLook;
    Exit(true);
  end;

  if Event.IsKey(keyF5) then
  begin
    Container.SaveScreenToDefaultFile;
    Exit(true);
  end;

  if Event.IsKey(keyEscape) then
  begin
    TUIState.Current := StateMenu;
    Exit(true);
  end;

  if Event.IsMouseButton(mbRight) then
  begin
    CheckboxAimAvatar.Checked := not CheckboxAimAvatar.Checked;
    ChangeCheckboxAimAvatar(CheckboxAimAvatar); // update ThirdPersonNavigation.AimAvatar
    Exit(true);
  end;
end;

procedure TStatePlay.ChangeCheckboxAimAvatar(Sender: TObject);
begin
  if CheckboxAimAvatar.Checked then
    ThirdPersonNavigation.AimAvatar := aaHorizontal
  else
    ThirdPersonNavigation.AimAvatar := aaNone;

  { The 3rd option, aaFlying, doesn't make sense for this case,
    when avatar walks on the ground and has Gravity = true. }
end;

procedure TStatePlay.ChangeCheckboxDebugAvatarColliders(Sender: TObject);
begin
  DebugAvatar.Exists := CheckboxDebugAvatarColliders.Checked;
end;

procedure TStatePlay.ChangeCheckboxImmediatelyFixBlockedCamera(Sender: TObject);
begin
  ThirdPersonNavigation.ImmediatelyFixBlockedCamera := CheckboxImmediatelyFixBlockedCamera.Checked;
end;

end.
