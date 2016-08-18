{
  Copyright 2016-2016 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Part of CastleGLUtils unit: DrawPrimitive2D and friends. }

{$ifdef read_interface}

type
  { Primitive to draw using DrawPrimitive2D.
    The modes correspond to OpenGL drawing modes, see
    https://www.opengl.org/wiki/Primitive
    https://www.opengl.org/sdk/docs/man2/xhtml/glBegin.xml }
  TPrimitiveMode = (
    pmPoints,
    pmLineStrip,
    pmLineLoop,
    pmLines,
    pmTriangleStrip,
    pmTriangleFan,
    pmTriangles
  );
  TBlendingSourceFactor = (
    bsSrcAlpha,
    bsOneMinusSrcAlpha,
    bsZero,
    bsOne,

    bsDstColor,
    bsSrcColor, //< As a source factor only since GL 1.4, check @code(GLFeatures.Version_1_4)
    bsDstAlpha,
    bsOneMinusDstColor,
    bsOneMinusSrcColor, //< As a source factor only since GL 1.4, check @code(GLFeatures.Version_1_4)
    bsOneMinusDstAlpha,

    bsSrcAlphaSaturate,

    bsConstantColor,
    bsOneMinusConstantColor,
    bsConstantAlpha,
    bsOneMinusConstantAlpha
  );
  TBlendingDestinationFactor = (
    bdSrcAlpha,
    bdOneMinusSrcAlpha,
    bdZero,
    bdOne,

    bdDstColor, //< As a destination factor only since GL 1.4, check @code(GLFeatures.Version_1_4)
    bdSrcColor,
    bdDstAlpha,
    bdOneMinusDstColor, //< As a destination factor only since GL 1.4, check @code(GLFeatures.Version_1_4)
    bdOneMinusSrcColor,
    bdOneMinusDstAlpha,

    // not supported by OpenGL for destination factor: bsSrcAlphaSaturate
    { }
    bdConstantColor,
    bdOneMinusConstantColor,
    bdConstantAlpha,
    bdOneMinusConstantAlpha
  );

const
  DefaultCircleSlices = 32;

{ Draw a rectangle filled with a color.

  Like with DrawPrimitive2D:
  Blending is automatically used if Color alpha < 1.
  ForceBlending forces the usage of blending. When it is @false,
  we use blending only if Color[3] (alpha) < 1.  }
procedure DrawRectangle(const R: TRectangle; const Color: TCastleColor;
  const BlendingSourceFactor: TBlendingSourceFactor = bsSrcAlpha;
  const BlendingDestinationFactor: TBlendingDestinationFactor = bdOneMinusSrcAlpha;
  const ForceBlending: boolean = false);

{ Draw a rectangle frame.

  Like with DrawPrimitive2D:
  Blending is automatically used if Color alpha < 1.
  ForceBlending forces the usage of blending. When it is @false,
  we use blending only if Color[3] (alpha) < 1.  }
procedure DrawRectangleOutline(const R: TRectangle; const Color: TCastleColor;
  const LineWidth: Single = 1;
  const BlendingSourceFactor: TBlendingSourceFactor = bsSrcAlpha;
  const BlendingDestinationFactor: TBlendingDestinationFactor = bdOneMinusSrcAlpha;
  const ForceBlending: boolean = false);

{ Draw a circle. It may be scaled differently in X than Y, thus resembling an ellipse.

  Like with DrawPrimitive2D:
  Blending is automatically used if Color alpha < 1.
  ForceBlending forces the usage of blending. When it is @false,
  we use blending only if Color[3] (alpha) < 1.  }
procedure DrawCircle(const Middle: TVector2Integer;
  const HorizontalRadius, VerticalRadius: Integer;
  const Color: TCastleColor;
  const CircleSlices: Cardinal = DefaultCircleSlices;
  const BlendingSourceFactor: TBlendingSourceFactor = bsSrcAlpha;
  const BlendingDestinationFactor: TBlendingDestinationFactor = bdOneMinusSrcAlpha;
  const ForceBlending: boolean = false);

{ Draw a circle outline. It may be scaled differently in X than Y, thus resembling an ellipse.

  Like with DrawPrimitive2D:
  Blending is automatically used if Color alpha < 1.
  ForceBlending forces the usage of blending. When it is @false,
  we use blending only if Color[3] (alpha) < 1.  }
procedure DrawCircleOutline(const Middle: TVector2Integer;
  const HorizontalRadius, VerticalRadius: Integer;
  const Color: TCastleColor;
  const LineWidth: Single = 1;
  const CircleSlices: Cardinal = DefaultCircleSlices;
  const BlendingSourceFactor: TBlendingSourceFactor = bsSrcAlpha;
  const BlendingDestinationFactor: TBlendingDestinationFactor = bdOneMinusSrcAlpha;
  const ForceBlending: boolean = false);

{ Draw a simple 2D primitive with a given color.
  This can be used to draw a series of points, lines or triangles,
  depending on the @code(Mode) parameter.

  Blending is automatically used if Color alpha < 1.
  ForceBlending forces the usage of blending. When it is @false,
  we use blending only if Color[3] (alpha) < 1.

  The LineWidth is only used when Mode indicates lines.
  The PointSize is only used when Mode indicates points,
  and only on desktop OpenGL (not available on mobile OpenGLES).
  Moreover, their interpretation may be limited by the implementation
  if anti-aliasing is enabled (and may be even limited to 1,
  which is common on OpenGLES).
  See https://www.opengl.org/sdk/docs/man2/xhtml/glPointSize.xml ,
  https://www.opengl.org/sdk/docs/man/html/glLineWidth.xhtml . }
procedure DrawPrimitive2D(const Mode: TPrimitiveMode;
  const Points: array of TVector2SmallInt;
  const Color: TCastleColor;
  const BlendingSourceFactor: TBlendingSourceFactor = bsSrcAlpha;
  const BlendingDestinationFactor: TBlendingDestinationFactor = bdOneMinusSrcAlpha;
  const ForceBlending: boolean = false;
  const LineWidth: Single = 1;
  const PointSize: Cardinal = 1);

{ Draw a rectangle that modulates colors underneath,
  making nice animation to FadeColor while FadeIntensity changes from 1.0
  down to 0.0.

  The GLFadeRectangleLight version makes a flash to FadeColor,
  then goes back to normal.
  The GLFadeRectangle version makes additional flash to blackness
  in the middle (so it goes from no modulation, to FadeColor,
  to pure black, and then back to normal).
  So it's a little more impressive when you're flashing with a dark color.

  These are nice as a screen effect, to flash some color (e.g. flash
  red color when the player is hurt).

  Only RGB portion of FadeColor is used. }
procedure GLFadeRectangle(const X1, Y1, X2, Y2: Integer;
  const FadeColor: TVector3Single;
  const FadeIntensity: Single); deprecated 'use TFlashEffect';
procedure GLFadeRectangle(const Rect: TRectangle;
  const FadeColor: TCastleColor;
  const FadeIntensity: Single); deprecated 'use TFlashEffect';
procedure GLFadeRectangleLight(const Rect: TRectangle;
  const FadeColor: TCastleColor;
  const FadeIntensity: Single); deprecated 'use TFlashEffect';

{ Draw a rectangle with blending.
  @deprecated Deprecated, use DrawRectangle instead. }
procedure GLBlendRectangle(const X1, Y1, X2, Y2: Integer;
  const SourceFactor: TBlendingSourceFactor;
  const DestinationFactor: TBlendingDestinationFactor;
  const Color: TVector4Single); deprecated 'use DrawRectangle';
procedure GLBlendRectangle(const Rect: TRectangle;
  const Color: TVector4Single); deprecated 'use DrawRectangle';

procedure GLBlendFunction(const SourceFactor: TBlendingSourceFactor;
  const DestinationFactor: TBlendingDestinationFactor);

{$endif read_interface}

{$ifdef read_implementation}

{ DrawPrimitive2D ---------------------------------------------------------------- }

const
  BlendingSourceFactorToGL: array [TBlendingSourceFactor] of TGLEnum = (
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_ZERO,
    GL_ONE,

    GL_DST_COLOR,
    GL_SRC_COLOR,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_COLOR,
    GL_ONE_MINUS_SRC_COLOR,
    GL_ONE_MINUS_DST_ALPHA,

    GL_SRC_ALPHA_SATURATE,

    GL_CONSTANT_COLOR,
    GL_ONE_MINUS_CONSTANT_COLOR,
    GL_CONSTANT_ALPHA,
    GL_ONE_MINUS_CONSTANT_ALPHA
  );
  BlendingDestinationFactorToGL: array [TBlendingDestinationFactor] of TGLEnum = (
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_ZERO,
    GL_ONE,

    GL_DST_COLOR,
    GL_SRC_COLOR,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_COLOR,
    GL_ONE_MINUS_SRC_COLOR,
    GL_ONE_MINUS_DST_ALPHA,

    // GL_SRC_ALPHA_SATURATE, // not supported as destination factor

    GL_CONSTANT_COLOR,
    GL_ONE_MINUS_CONSTANT_COLOR,
    GL_CONSTANT_ALPHA,
    GL_ONE_MINUS_CONSTANT_ALPHA
  );
  PrimitiveModeToGL: array [TPrimitiveMode] of TGLEnum = (
    GL_POINTS,
    GL_LINE_STRIP,
    GL_LINE_LOOP,
    GL_LINES,
    GL_TRIANGLE_STRIP,
    GL_TRIANGLE_FAN,
    GL_TRIANGLES
  );

var
  {$ifdef GLImageUseShaders}
  Primitive2DProgram: TGLSLProgram;
  Primitive2DUniformViewportSize, Primitive2DUniformColor: TGLSLUniform;
  Primitive2DAttribVertex: TGLSLAttribute;
  {$endif}
  Primitive2DVbo: TGLuint;
  Primitive2DPointPtr: Pointer;
  Primitive2DPointPtrSize: Integer;

procedure DrawPrimitive2D(const Mode: TPrimitiveMode;
  const Points: array of TVector2SmallInt; const Color: TCastleColor;
  const BlendingSourceFactor: TBlendingSourceFactor;
  const BlendingDestinationFactor: TBlendingDestinationFactor;
  const ForceBlending: boolean;
  const LineWidth: Single;
  const PointSize: Cardinal);
var
  Blending: boolean;
  I, RequiredPrimitive2DPointPtrSize: Integer;
begin
  {$ifdef GLImageUseShaders}
  if Primitive2DProgram = nil then
  begin
    Primitive2DProgram := TGLSLProgram.Create;
    Primitive2DProgram.AttachVertexShader({$I primitive_2.vs.inc});
    Primitive2DProgram.AttachFragmentShader({$I primitive_2.fs.inc});
    Primitive2DProgram.Link;

    Primitive2DUniformViewportSize := Primitive2DProgram.Uniform('viewport_size');
    Primitive2DUniformColor := Primitive2DProgram.Uniform('color');
    Primitive2DAttribVertex := Primitive2DProgram.Attribute('vertex');
  end;
  {$endif}

  { apply LineWidth, PointSize.
    Their setters avoid doing anything when they already have the requested
    values, so we can just assign them here not worrying about performance. }
  RenderContext.LineWidth := LineWidth;
  RenderContext.PointSize := PointSize;

  Blending := ForceBlending or (Color[3] < 1);
  if Blending then
  begin
    GLBlendFunction(BlendingSourceFactor, BlendingDestinationFactor);
    glEnable(GL_BLEND);
  end;

  if (Primitive2DVbo = 0) and GLFeatures.VertexBufferObject then
    glGenBuffers(1, @Primitive2DVbo);

  { make Primitive2DPointPtr have necessary size }
  RequiredPrimitive2DPointPtrSize := SizeOf(TVector2SmallInt) * (High(Points) + 1);
  if Primitive2DPointPtrSize <> RequiredPrimitive2DPointPtrSize then
  begin
    if Primitive2DPointPtr <> nil then
      FreeMem(Primitive2DPointPtr);
    Primitive2DPointPtr := GetMem(RequiredPrimitive2DPointPtrSize);
    Primitive2DPointPtrSize := RequiredPrimitive2DPointPtrSize;
  end;

  { copy Points to Primitive2DPointPtr }
  for I := 0 to High(Points) do
    Move(Points[I],
      PVector2SmallInt(PtrUInt(Primitive2DPointPtr) + SizeOf(TVector2SmallInt) * I)^,
      SizeOf(TVector2SmallInt));

  if GLFeatures.VertexBufferObject then
  begin
    glBindBuffer(GL_ARRAY_BUFFER, Primitive2DVbo);
    glBufferData(GL_ARRAY_BUFFER, Primitive2DPointPtrSize, Primitive2DPointPtr, GL_STREAM_DRAW);
  end;

  {$ifdef GLImageUseShaders}
  CurrentProgram := Primitive2DProgram;
  Primitive2DAttribVertex.EnableArray(0, 2, GL_SHORT, GL_FALSE, SizeOf(TVector2SmallInt), 0);
  Primitive2DUniformViewportSize.SetValue(Viewport2DSize);
  Primitive2DUniformColor.SetValue(Color);

  {$else}
  CurrentProgram := nil;
  glLoadIdentity();
  glColorv(Color);

  glEnableClientState(GL_VERTEX_ARRAY);
  if GLFeatures.VertexBufferObject then
    glVertexPointer(2, GL_SHORT, SizeOf(TVector2SmallInt), nil) else
    glVertexPointer(2, GL_SHORT, SizeOf(TVector2SmallInt), Primitive2DPointPtr);
  {$endif}

  glDrawArrays(PrimitiveModeToGL[Mode], 0, High(Points) + 1);

  {$ifdef GLImageUseShaders}
  // Primitive2DProgram.Disable; // no need to disable, keep it enabled to save speed
  { attribute arrays are enabled independent from GLSL program, so we need
    to disable them separately }
  Primitive2DAttribVertex.DisableArray;
  {$else}
  glDisableClientState(GL_VERTEX_ARRAY);
  {$endif}

  if GLFeatures.VertexBufferObject then
  begin
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  end;

  if Blending then
    glDisable(GL_BLEND);
end;

{ DrawRectangle ---------------------------------------------------------------- }

procedure DrawRectangle(const R: TRectangle; const Color: TCastleColor;
  const BlendingSourceFactor: TBlendingSourceFactor;
  const BlendingDestinationFactor: TBlendingDestinationFactor;
  const ForceBlending: boolean);
var
  RectanglePoint: array [0..3] of TVector2SmallInt;
begin
  RectanglePoint[0] := Vector2SmallInt(R.Left          , R.Bottom);
  RectanglePoint[1] := Vector2SmallInt(R.Left + R.Width, R.Bottom);
  RectanglePoint[2] := Vector2SmallInt(R.Left + R.Width, R.Bottom + R.Height);
  RectanglePoint[3] := Vector2SmallInt(R.Left          , R.Bottom + R.Height);

  DrawPrimitive2D(pmTriangleFan, RectanglePoint,
    Color, BlendingSourceFactor, BlendingDestinationFactor, ForceBlending);
end;

procedure DrawRectangleOutline(const R: TRectangle; const Color: TCastleColor;
  const LineWidth: Single;
  const BlendingSourceFactor: TBlendingSourceFactor;
  const BlendingDestinationFactor: TBlendingDestinationFactor;
  const ForceBlending: boolean);
var
  RectanglePoint: array [0..3] of TVector2SmallInt;
begin
  RectanglePoint[0] := Vector2SmallInt(R.Left          , R.Bottom);
  RectanglePoint[1] := Vector2SmallInt(R.Left + R.Width, R.Bottom);
  RectanglePoint[2] := Vector2SmallInt(R.Left + R.Width, R.Bottom + R.Height);
  RectanglePoint[3] := Vector2SmallInt(R.Left          , R.Bottom + R.Height);

  DrawPrimitive2D(pmLineLoop, RectanglePoint,
    Color, BlendingSourceFactor, BlendingDestinationFactor, ForceBlending, LineWidth);
end;

{ DrawCircle ----------------------------------------------------------------- }

procedure DrawCircle(const Middle: TVector2Integer;
  const HorizontalRadius, VerticalRadius: Integer;
  const Color: TCastleColor;
  const CircleSlices: Cardinal = DefaultCircleSlices;
  const BlendingSourceFactor: TBlendingSourceFactor = bsSrcAlpha;
  const BlendingDestinationFactor: TBlendingDestinationFactor = bdOneMinusSrcAlpha;
  const ForceBlending: boolean = false);
var
  CirclePoint: array of TVector2SmallInt;
  PreviousSin, PreviousCos, NextSin, NextCos: Float;
  I: Integer;
begin
  SetLength(CirclePoint, CircleSlices * 3);
  SinCos(0, PreviousSin, PreviousCos);
  for I := 1 to CircleSlices do
  begin
    if I = CircleSlices then
      { do not depend that SinCos(2 * Pi, ...) is precisely equal to SinCos(0, ...) }
      SinCos(0, NextSin, NextCos)
    else
      SinCos(2 * Pi * I / CircleSlices, NextSin, NextCos);
    CirclePoint[(I - 1) * 3    ] := Vector2SmallInt(Middle[0], Middle[1]);
    CirclePoint[(I - 1) * 3 + 1] := Vector2SmallInt(
      Middle[0] + Round(PreviousSin * HorizontalRadius),
      Middle[1] + Round(PreviousCos * VerticalRadius));
    CirclePoint[(I - 1) * 3 + 2] := Vector2SmallInt(
      Middle[0] + Round(NextSin     * HorizontalRadius),
      Middle[1] + Round(NextCos     * VerticalRadius));
    PreviousSin := NextSin;
    PreviousCos := NextCos;
  end;

  DrawPrimitive2D(pmTriangleFan, CirclePoint,
    Color, BlendingSourceFactor, BlendingDestinationFactor, ForceBlending);
end;

procedure DrawCircleOutline(const Middle: TVector2Integer;
  const HorizontalRadius, VerticalRadius: Integer;
  const Color: TCastleColor;
  const LineWidth: Single = 1;
  const CircleSlices: Cardinal = DefaultCircleSlices;
  const BlendingSourceFactor: TBlendingSourceFactor = bsSrcAlpha;
  const BlendingDestinationFactor: TBlendingDestinationFactor = bdOneMinusSrcAlpha;
  const ForceBlending: boolean = false);
var
  CirclePoint: array of TVector2SmallInt;
  S, C: Float;
  I: Integer;
begin
  SetLength(CirclePoint, CircleSlices);
  for I := 0 to CircleSlices - 1 do
  begin
    SinCos(2 * Pi * I / (CircleSlices - 1), S, C);
    CirclePoint[I] := Vector2SmallInt(
      Middle[0] + Round(S * HorizontalRadius),
      Middle[1] + Round(C * VerticalRadius));
  end;

  DrawPrimitive2D(pmLineLoop, CirclePoint,
    Color, BlendingSourceFactor, BlendingDestinationFactor, ForceBlending, LineWidth);
end;

{ GLFadeRectangle ------------------------------------------------------------ }

procedure GLFadeRectangle(const X1, Y1, X2, Y2: Integer;
  const FadeColor: TVector3Single; const FadeIntensity: Single);
begin
  {$warnings off}
  GLFadeRectangle(Rectangle(X1, Y1, X2 - X1, Y2 - Y1),
    Vector4Single(FadeColor, 1.0), FadeIntensity);
  {$warnings on}
end;

procedure GLFadeRectangle(const Rect: TRectangle;
  const FadeColor: TCastleColor; const FadeIntensity: Single);
const
  FullWhiteEnd = 0.9;
  FullBlack = 0.3;
  { We assume that MinScale is small enough that difference between
    "FadeColor * MinScale * screen color" and
    "MinScale * screen color" is not noticeable. }
  MinScale = 0.1;
  { Constants below make resulting screen color = glColor * previous screen color.
    Note that as long as all components of FadeColor are <= 1,
    then all components of our glColor are also always <= 1,
    and this means that we will always make the screen darker (or equal,
    but never brighter). }
  SourceFactor = bsZero;
  DestinationFactor = bdSrcColor;
var
  Color: TCastleColor;
begin
  if FadeIntensity > 0 then
  begin
    { for FadeIntensity in 1...FullWhiteEnd (going down):
      screen color := FadeColor * original screen color }
    if FadeIntensity > FullWhiteEnd then
      Color := FadeColor else
    { for FadeIntensity in FullWhiteEnd...FullBlack (going down):
      final screen color changes:
      - from screen color := FadeColor * original screen color
      - to   screen color := FadeColor * MinScale * original screen color }
    if FadeIntensity > FullBlack then
      Color := FadeColor * MapRange(FadeIntensity, FullWhiteEnd, FullBlack, 1, MinScale) else
    { for FadeIntensity in FullBlack...0 (going down):
      final screen color changes:
      - from screen color := MinScale * original screen color
      - to   screen color := original screen color }
      Color := White * MapRange(FadeIntensity, FullBlack, 0, MinScale, 1);

    Color[3] := 1.0; { alpha always 1.0 in this case }
    DrawRectangle(Rect, Color, SourceFactor, DestinationFactor, true);
  end;
end;

procedure GLFadeRectangleLight(const Rect: TRectangle;
  const FadeColor: TCastleColor; const FadeIntensity: Single);
const
  FullTime = 0.9;
  SourceFactor = bsZero;
  DestinationFactor = bdSrcColor;
var
  Color: TCastleColor;
  Intensity: Single;
begin
  if FadeIntensity > 0 then
  begin
    if FadeIntensity < FullTime then
      Intensity := MapRange(FadeIntensity, 0, FullTime, 0, 1) else
      Intensity := MapRange(FadeIntensity, FullTime, 1, 1, 0);
    Color := FadeColor;
    Color[3] := Intensity; { alpha always 1.0 in this case }
    DrawRectangle(Rect, Color, bsSrcAlpha, bdOneMinusSrcAlpha, true);
  end;
end;

{ GLBlendRectangle ----------------------------------------------------------- }

procedure GLBlendRectangle(const X1, Y1, X2, Y2: Integer;
  const SourceFactor: TBlendingSourceFactor;
  const DestinationFactor: TBlendingDestinationFactor;
  const Color: TVector4Single);
begin
  DrawRectangle(Rectangle(X1, Y1, X2 - X1, Y2 - Y1), Color,
    SourceFactor, DestinationFactor, true);
end;

procedure GLBlendRectangle(const Rect: TRectangle;
  const Color: TVector4Single);
begin
  DrawRectangle(Rect, Color, bsOne, bdSrcAlpha, true);
end;

{ GLBlendFunction ------------------------------------------------------------ }

procedure GLBlendFunction(const SourceFactor: TBlendingSourceFactor;
  const DestinationFactor: TBlendingDestinationFactor);
begin
  glBlendFunc(
    BlendingSourceFactorToGL[SourceFactor],
    BlendingDestinationFactorToGL[DestinationFactor]);
end;

{$endif read_implementation}