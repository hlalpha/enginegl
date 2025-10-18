#if !defined( IN_CAMERA_H )
#define IN_CAMERA_H

// in_camera.c
cvar_t	cam_command;
cvar_t	cam_snapto;
cvar_t	cam_idealyaw;
cvar_t	cam_idealpitch;
cvar_t	cam_idealdist;
cvar_t	cam_contain;
cvar_t	c_maxpitch;
cvar_t	c_minpitch;
cvar_t	c_maxyaw;
cvar_t	c_minyaw;
cvar_t	c_maxdistance;
cvar_t	c_mindistance;

qboolean	cam_thirdperson;
vec3_t		cam_ofs;

int			cam_mousemove;

void CAM_Think (void);
void CAM_CamPitchUpDown (void);
void CAM_CamPitchUpUp (void);
void CAM_CamPitchDownDown (void);
void CAM_CamPitchDownUp (void);
void CAM_CamYawLeftDown (void);
void CAM_CamYawLeftUp (void);
void CAM_CamYawRightDown (void);
void CAM_CamYawRightUp (void);
void CAM_CamInDown (void);
void CAM_CamInUp (void);
void CAM_CamOutDown (void);
void CAM_CamOutUp (void);
void CAM_ToThirdPerson (void);
void CAM_ToFirstPerson (void);
void CAM_ToggleSnapto (void);
void CAM_ClearStates (void);
void CAM_StartMouseMove (void);
void CAM_EndMouseMove (void);
void CAM_StartDistance (void);
void CAM_EndDistance (void);

#endif
