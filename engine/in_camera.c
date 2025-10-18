#include "quakedef.h"

void KeyDown (kbutton_t *b);
void KeyUp (kbutton_t *b);

#define CAM_DIST_DELTA 1.0
#define CAM_ANGLE_DELTA 2.5
#define CAM_ANGLE_SPEED 2.5
#define CAM_MIN_DIST 30.0
#define CAM_ANGLE_MOVE .5

cvar_t		cam_command = { "cam_command", "0" };
cvar_t		cam_snapto = { "cam_snapto", "0" };
cvar_t		cam_idealyaw = { "cam_idealyaw", "0" };
cvar_t		cam_idealpitch = { "cam_idealpitch", "0" };
cvar_t		cam_idealdist = { "cam_idealdist", "30" };
cvar_t		cam_contain = { "cam_contain", "0" };
cvar_t		c_maxpitch = { "c_maxpitch", "90.0" };
cvar_t		c_minpitch = { "c_minpitch", "0.0" };
cvar_t		c_maxyaw = { "c_maxyaw", "135.0" };
cvar_t		c_minyaw = { "c_minyaw", "-135.0" };
cvar_t		c_maxdistance = { "c_maxdistance", "200.0" };
cvar_t		c_mindistance = { "c_mindistance", "30.0" };

kbutton_t	cam_pitchup;
kbutton_t	cam_pitchdown;
kbutton_t	cam_yawleft;
kbutton_t	cam_yawright;
kbutton_t	cam_in;
kbutton_t	cam_out;

qboolean	cam_thirdperson;
vec3_t		cam_ofs;

int			cam_mousemove;
int			cam_distancemove;

POINT		cam_mouse;

int			cam_old_mouse_x;
int			cam_old_mouse_y;

extern int	window_center_x, window_center_y, window_x, window_y, window_width, window_height;
extern trace_t SV_ClipMoveToEntity (edict_t *ent, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end);

float MoveToward( float cur, float goal, float maxspeed ) // HLSDK
{
	if( cur != goal )
	{
		if( abs( cur - goal ) > 180.0 )
		{
			if( cur < goal )
				cur += 360.0;
			else
				cur -= 360.0;
		}

		if( cur < goal )
		{
			if( cur < goal - 1.0 )
				cur += ( goal - cur ) / 4.0;
			else
				cur = goal;
		}
		else
		{
			if( cur > goal + 1.0 )
				cur -= ( cur - goal ) / 4.0;
			else
				cur = goal;
		}
	}


	// bring cur back into range
	if( cur < 0 )
		cur += 360.0;
	else if( cur >= 360 )
		cur -= 360;

	return cur;
}

typedef struct
{
	vec3_t		boxmins, boxmaxs;// enclose the test object along entire move
	float		*mins, *maxs;	// size of the moving object
	vec3_t		mins2, maxs2;	// size when clipping against mosnters
	float		*start, *end;
	trace_t		trace;
	int			type;
	edict_t		*passedict;
} moveclip_t;

void CAM_Think (void)
{
	vec3_t ext, pnt, camForward, camRight, camUp;
	moveclip_t	clip;
	float dist;
	vec3_t camAngles;
	int i;
	entity_t *ent;

	switch ( (int)cam_command.value )
	{
	case 1:
		CAM_ToThirdPerson();
		break;

	case 2:
		CAM_ToFirstPerson();
		break;
	}

	if ( !cam_thirdperson )
		return;

	if ( cam_contain.value )
	{
		ext[0] = ext[1] = ext[2] = 0.0;
		ent = &cl_entities[cl.viewentity];
	}

	camAngles[PITCH] = cam_idealpitch.value;
	camAngles[YAW] = cam_idealyaw.value;
	dist = cam_idealdist.value;

	//
	//movement of the camera with the mouse
	//
	if ( cam_mousemove )
	{
		//get windows cursor position
		GetCursorPos ( &cam_mouse );

		//check for X delta values and adjust accordingly
		//eventually adjust YAW based on amount of movement
		//don't do any movement of the cam using YAW/PITCH if we are zooming in/out the camera	
		if ( !cam_distancemove )
		{
			//keep the camera within certain limits around the player (ie avoid certain bad viewing angles)  
			if ( cam_mouse.x > window_center_x )
			{
				//if ((camAngles[YAW]>=225.0)||(camAngles[YAW]<135.0))
				if ( camAngles[YAW] < c_maxyaw.value )
				{
					camAngles[YAW] += (CAM_ANGLE_MOVE) * ((cam_mouse.x - window_center_x) / 2);
				}

				if ( camAngles[YAW] > c_maxyaw.value )
				{
					camAngles[YAW] = c_maxyaw.value;
				}
			}
			else if ( cam_mouse.x < window_center_x )
			{
				//if ((camAngles[YAW]<=135.0)||(camAngles[YAW]>225.0))
				if ( camAngles[YAW] > c_minyaw.value )
				{
					camAngles[YAW] -= (CAM_ANGLE_MOVE) * ((window_center_x - cam_mouse.x) / 2);
				}

				if ( camAngles[YAW] < c_minyaw.value )
				{
					camAngles[YAW] = c_minyaw.value;
				}
			}

			//check for y delta values and adjust accordingly
			//eventually adjust PITCH based on amount of movement
			//also make sure camera is within bounds
			if ( cam_mouse.y > window_center_y )
			{
				if ( camAngles[PITCH] < c_maxpitch.value )
				{
					camAngles[PITCH] += (CAM_ANGLE_MOVE) * ((cam_mouse.y - window_center_y) / 2);
				}
				if ( camAngles[PITCH] > c_maxpitch.value )
				{
					camAngles[PITCH] = c_maxpitch.value;
				}
			}
			else if ( cam_mouse.y < window_center_y )
			{
				if ( camAngles[PITCH] > c_minpitch.value )
				{
					camAngles[PITCH] -= (CAM_ANGLE_MOVE) * ((window_center_y - cam_mouse.y) / 2);
				}
				if ( camAngles[PITCH] < c_minpitch.value )
				{
					camAngles[PITCH] = c_minpitch.value;
				}
			}

			//set old mouse coordinates to current mouse coordinates
			//since we are done with the mouse
			cam_old_mouse_x = cam_mouse.x * sensitivity.value;
			cam_old_mouse_y = cam_mouse.y * sensitivity.value;

			SetCursorPos ( window_center_x, window_center_y );
		}
	}

	//Nathan code here
	if ( CL_KeyState( &cam_pitchup ) )
		camAngles[PITCH] += CAM_ANGLE_DELTA;
	else if ( CL_KeyState( &cam_pitchdown ) )
		camAngles[PITCH] -= CAM_ANGLE_DELTA;

	if ( CL_KeyState( &cam_yawleft ) )
		camAngles[YAW] -= CAM_ANGLE_DELTA;
	else if ( CL_KeyState( &cam_yawright ) )
		camAngles[YAW] += CAM_ANGLE_DELTA;

	if ( CL_KeyState( &cam_in ) )
	{
		dist -= CAM_DIST_DELTA;
		if ( dist < CAM_MIN_DIST )
		{
			// If we go back into first person, reset the angle
			camAngles[PITCH] = 0;
			camAngles[YAW] = 0;
			dist = CAM_MIN_DIST;
		}

	}
	else if ( CL_KeyState( &cam_out ) )
		dist += CAM_DIST_DELTA;

	if ( cam_distancemove )
	{
		if ( cam_mouse.y > window_center_y )
		{
			if ( dist < c_maxdistance.value )
			{
				dist += CAM_DIST_DELTA * ((cam_mouse.y - window_center_y) / 2);
			}
			if ( dist > c_maxdistance.value )
			{
				dist = c_maxdistance.value;
			}
		}
		else if ( cam_mouse.y < window_center_y )
		{
			if ( dist > c_mindistance.value )
			{
				dist -= (CAM_DIST_DELTA) * ((window_center_y - cam_mouse.y) / 2);
			}
			if ( dist < c_mindistance.value )
			{
				dist = c_mindistance.value;
			}
		}

		//set old mouse coordinates to current mouse coordinates
		//since we are done with the mouse
		cam_old_mouse_x = cam_mouse.x * sensitivity.value;
		cam_old_mouse_y = cam_mouse.y * sensitivity.value;
		SetCursorPos ( window_center_x, window_center_y );
	}

	if ( cam_contain.value )
	{
		// check new ideal
		VectorCopy( ent->origin, pnt );
		AngleVectors( camAngles, camForward, camRight, camUp );
		for ( i = 0; i < 3; i++ )
			pnt[i] += -dist * camForward[i];

		// check line from r_refdef.vieworg to pnt
		memset( &clip, 0, sizeof( moveclip_t ) );
		clip.trace = SV_ClipMoveToEntity( sv.edicts, r_refdef.vieworg, ext, ext, pnt );
		if ( clip.trace.fraction == 1.0 )
		{
			// update ideal
			cam_idealpitch.value = camAngles[PITCH];
			cam_idealyaw.value = camAngles[YAW];
			cam_idealdist.value = dist;
		}
	}
	else
	{
		// update ideal
		cam_idealpitch.value = camAngles[PITCH];
		cam_idealyaw.value = camAngles[YAW];
		cam_idealdist.value = dist;
	}

	// Move towards ideal
	VectorCopy( cam_ofs, camAngles );

	if ( cam_snapto.value )
	{
		camAngles[YAW] = cam_idealyaw.value + cl.viewangles[YAW];
		camAngles[PITCH] = cam_idealpitch.value + cl.viewangles[PITCH];
		camAngles[ROLL] = cam_idealdist.value;
	}
	else
	{
		if ( camAngles[YAW] - cl.viewangles[YAW] != cam_idealyaw.value )
			camAngles[YAW] = MoveToward( camAngles[YAW], cam_idealyaw.value + cl.viewangles[YAW], CAM_ANGLE_SPEED );

		if ( camAngles[PITCH] - cl.viewangles[PITCH] != cam_idealpitch.value )
			camAngles[PITCH] = MoveToward( camAngles[PITCH], cam_idealpitch.value + cl.viewangles[PITCH], CAM_ANGLE_SPEED );

		if ( abs( camAngles[2] - cam_idealdist.value ) < 2.0 )
			camAngles[2] = cam_idealdist.value;
		else
			camAngles[2] += (cam_idealdist.value - camAngles[2]) / 4.0;
	}

	if ( cam_contain.value )
	{
		// Test new position
		dist = camAngles[ROLL];
		camAngles[ROLL] = 0;

		VectorCopy( ent->origin, pnt );
		AngleVectors( camAngles, camForward, camRight, camUp );
		for ( i = 0; i < 3; i++ )
			pnt[i] += -dist * camForward[i];

		// check line from r_refdef.vieworg to pnt
		memset( &clip, 0, sizeof( moveclip_t ) );
		ext[0] = ext[1] = ext[2] = 0.0;
		clip.trace = SV_ClipMoveToEntity( sv.edicts, r_refdef.vieworg, ext, ext, pnt );
		if ( clip.trace.fraction != 1.0 )
			return;
	}

	cam_ofs[PITCH] = camAngles[PITCH];
	cam_ofs[YAW] = camAngles[YAW];
	cam_ofs[ROLL] = dist;
}

void CAM_CamPitchUpDown (void) {KeyDown(&cam_pitchup);}
void CAM_CamPitchUpUp (void) {KeyUp(&cam_pitchup);}
void CAM_CamPitchDownDown (void) {KeyDown(&cam_pitchdown);}
void CAM_CamPitchDownUp (void) {KeyUp(&cam_pitchdown);}
void CAM_CamYawLeftDown (void) {KeyDown(&cam_yawleft);}
void CAM_CamYawLeftUp (void) {KeyUp(&cam_yawleft);}
void CAM_CamYawRightDown (void) {KeyDown(&cam_yawright);}
void CAM_CamYawRightUp (void) {KeyUp(&cam_yawright);}
void CAM_CamInDown (void) {KeyDown(&cam_in);}
void CAM_CamInUp (void) {KeyUp(&cam_in);}
void CAM_CamOutDown (void) {KeyDown(&cam_out);}
void CAM_CamOutUp (void) {KeyUp(&cam_out);}

void CAM_ToThirdPerson (void)
{
	if ( !cam_thirdperson )
	{
		cam_ofs[PITCH] = cl.viewangles[PITCH];
		cam_ofs[YAW] = cl.viewangles[YAW];
		cam_ofs[ROLL] = CAM_MIN_DIST;

		cam_thirdperson = true;
	}

	Cvar_SetValue ( "cam_command", 0.f );
}
void CAM_ToFirstPerson (void)
{
	cam_thirdperson = false;

	Cvar_SetValue ( "cam_command", 0.f );
}

void CAM_ToggleSnapto (void)
{
	cam_snapto.value = !cam_snapto.value; // TODO(SanyaSho): Use Cvar_SetValue here?
}

void CAM_ClearStates (void) // FIXME(SanyaSho): Unused
{
	cam_pitchup.state = 0;
	cam_pitchdown.state = 0;
	cam_yawleft.state = 0;
	cam_yawright.state = 0;
	cam_in.state = 0;
	cam_out.state = 0;

	cam_thirdperson = false;
	cam_command.value = 0.0;
	cam_mousemove = 0;

	cam_snapto.value = 0.0;
	cam_distancemove = 0;

	cam_ofs[0] = 0.0;
	cam_ofs[1] = 0.0;
	cam_ofs[2] = CAM_MIN_DIST;

	cam_idealpitch.value = cl.viewangles[PITCH];
	cam_idealyaw.value = cl.viewangles[YAW];
	cam_idealdist.value = CAM_MIN_DIST;
}

void CAM_StartMouseMove (void)
{
	if ( !cam_thirdperson )
	{
		cam_mousemove = 0;
		return;
	}

	if ( !cam_mousemove )
	{
		cam_mousemove = 1;

		GetCursorPos ( &cam_mouse );

		cam_old_mouse_x = (int)(cam_mouse.x * sensitivity.value);
		cam_old_mouse_y = (int)(cam_mouse.y * sensitivity.value);
	}
}
void CAM_EndMouseMove (void)
{
	cam_mousemove = 0;
}

void CAM_StartDistance (void)
{
	if ( !cam_thirdperson )
	{
		cam_distancemove = 0;
		cam_mousemove = 0;
		return;
	}

	if ( !cam_distancemove )
	{
		cam_distancemove = 1;

		cam_mousemove = 1;

		GetCursorPos ( &cam_mouse );

		cam_old_mouse_x = (int)(cam_mouse.x * sensitivity.value);
		cam_old_mouse_y = (int)(cam_mouse.y * sensitivity.value);
	}
}
void CAM_EndDistance (void)
{
	cam_distancemove = 0;
	cam_mousemove = 0;
}