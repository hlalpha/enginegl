#if !defined( RENDERMODE_H )
#define RENDERMODE_H

typedef enum
{
	kRenderModeNormal = 0,
	kRenderModeColor,
	kRenderModeTexture,
	kRenderModeBlur,
	kRenderModeSolid,
	kRenderModeAdditive
} eRenderMode;

typedef enum
{
	kRenderFXNormal = 0,
	kRenderFXSlowPulse,
	kRenderFXFastPulse,
	kRenderFXSlowWidePulse,
	kRenderFXFastWidePulse,
	kRenderFXSlowFadeAway,
	kRenderFXFastFadeAway,
	kRenderFXSlowBecomeSolid,
	kRenderFXFastBecomeSolid,
	kRenderFXSlowStrobe,
	kRenderFXFastStrobe,
	kRenderFXFasterStrobe,
	kRenderFXSlowFlicker,
	kRenderFXFastFlicker
} eRenderFX;

#endif // RENDERMODE_H
