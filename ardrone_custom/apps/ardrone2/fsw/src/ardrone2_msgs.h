/******************************************************************************
**
**        Copyright (c) 2016, Odyssey Space Research, LLC.
**
**        Software developed under contract NNJ14HA64B, subcontract
**        NNJ14HA64B-ODY1.
**
**        All rights reserved.  Odyssey Space Research grants
**        to the Government, and other acting on its behalf, a paid-up,
**        nonexclusive, irrevocable, worldwide license in such copyrighted
**        computer software to reproduce, prepare derivative works, and perform
**        publicly and display publicly (but not to distribute copies to the
**        public) by or on behalf of the Government.
**
**        Created by Mathew Benson, mbenson@odysseysr.com
**
******************************************************************************/

/*=======================================================================================
** File Name:  ardrone2_msg.h
**
** Title:  Message Definition Header File for ardrone2 Application
**
**
** Purpose:  To define ardrone2's command and telemetry message definitions
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2012-12-27 | Mathew Benson | Build #: Code Started
**
**=====================================================================================*/
#ifndef _ARDRONE2_MSG_H_
#define _ARDRONE2_MSG_H_

/*
** Pragmas
*/

/*
** Include Files
*/
#include "cfs_utils.h"
#include "ardrone2_platform_cfg.h"
#include "ardrone2_navdata.h"

/*
** Type definition (ARDrone2 housekeeping)...
*/
typedef struct
{
    uint8		TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8		usCmdCnt;
    uint8		usCmdErrCnt;
}   OS_PACK ARDrone2_HK_Tlm_t  ;

typedef enum 
{
    CTRL_DEFAULT = 0,
    CTRL_INIT = 1,
    CTRL_LANDED = 2,
    CTRL_FLYING = 3,
    CTRL_HOVERING = 4,
    CTRL_TEST = 5,
    CTRL_TRANS_TAKEOFF = 6,
    CTRL_TRANS_GOTOFIX = 7,
    CTRL_TRANS_LANDING = 8,
    CTRL_TRANS_LOOPING = 9,
    CTRL_NUM_STATES = 10
} ARDrone2_ctrl_state_t ;

typedef enum 
{
    FLYING_OK = 0,
    FLYING_LOST_ALT = 1,
    FLYING_LOST_ALT_GO_DOWN = 2,
    FLYING_ALT_OUT_ZONE = 3,
    FLYING_COMBINED_YAW = 4,
    FLYING_BRAKE = 5,
    FLYING_NO_VISION = 6
} ARDrone2_ctrl_flying_state_t;
    
typedef enum
{
    HOVERING_OK = 0,
    HOVERING_YAW = 1,
    HOVERING_YAW_LOST_ALT = 2,
    HOVERING_YAW_LOST_ALT_GO_DOWN = 3,
    HOVERING_ALT_OUT_ZONE = 4,
    HOVERING_YAW_ALT_OUT_ZONE = 5,
    HOVERING_LOST_ALT = 6,
    HOVERING_LOST_ALT_GO_DOWN  = 7,
    HOVERING_LOST_COM  = 8,
    LOST_COM_LOST_ALT = 9,
    LOST_COM_LOST_ALT_TOO_LONG = 10,
    LOST_COM_ALT_OK = 11,
    HOVERING_MAGNETO_CALIB = 12,
    HOVERING_DEMO = 13
} ARDrone2_ctrl_hovering_state_t;

typedef enum
{
    TAKEOFF_GROUND = 0,
    TAKEOFF_AUTO = 1
} ARDrone2_ctrl_takeoff_trans_state_t;

typedef enum
{
    GOTOFIX_OK = 0,
    GOTOFIX_LOST_ALT = 1,
    GOTOFIX_YAW = 2
} ARDrone2_ctrl_gotofix_trans_state_t;

typedef enum
{
    LANDING_CLOSED_LOOP = 0,
    LANDING_OPEN_LOOP = 1,
    LANDING_OPEN_LOOP_FAST = 2
} ARDrone2_ctrl_landing_trans_state_t;

typedef enum
{
    LOOPING_IMPUSION = 0,
    LOOPING_OPEN_LOOP_CTRL = 1,
    LOOPING_PLANIF_CTRL = 2
} ARDrone2_ctrl_looping_trans_state_t;

typedef struct
{
    int16       minor_ctrl_state;           /*!< Minor Control State */
    int16       ctrl_state;                 /*!< Major Control State */
    uint32    	vbat_flying_percentage; 	/*!< battery voltage filtered (mV) */

	float   	theta;                  	/*!< UAV's pitch in milli-degrees */
	float   	phi;                    	/*!< UAV's roll  in milli-degrees */
	float   	psi;                    	/*!< UAV's yaw   in milli-degrees */

	int32     	altitude;               	/*!< UAV's altitude in millimeters */

	float   	vx;                     	/*!< UAV's estimated linear velocity mm/s in rotated vehicle ref frame */
	float   	vy;                     	/*!< UAV's estimated linear velocity mm/s in rotated vehicle ref frame */
	float   	vz;                     	/*!< UAV's estimated linear velocity mm/s in rotated vehicle ref frame */

	uint32    	num_frames;			  		/*!< streamed frame index */ // Not used -> To integrate in video stage.

	// Camera parameters compute by detection
	float  		detection_camera_rot_m11;
	float  		detection_camera_rot_m12;
	float  		detection_camera_rot_m13;
	float  		detection_camera_rot_m21;
	float  		detection_camera_rot_m22;
	float  		detection_camera_rot_m23;
	float  		detection_camera_rot_m31;
	float  		detection_camera_rot_m32;
	float  		detection_camera_rot_m33;

	float	  	detection_camera_trans_x;
	float	  	detection_camera_trans_y;
	float	  	detection_camera_trans_z;

	uint32	  	detection_tag_index;
	uint32	  	detection_camera_type;  	/*!<  Type of tag searched in detection */

	// Camera parameters compute by drone
	float	  	drone_camera_rot_m11;
	float	  	drone_camera_rot_m12;
	float	  	drone_camera_rot_m13;
	float	  	drone_camera_rot_m21;
	float	  	drone_camera_rot_m22;
	float	  	drone_camera_rot_m23;
	float	  	drone_camera_rot_m31;
	float	  	drone_camera_rot_m32;
	float	  	drone_camera_rot_m33;

	float  		drone_camera_trans_x;
	float  		drone_camera_trans_y;
	float  		drone_camera_trans_z;
} OS_PACK ARDrone2_NavData_Demo_t;

typedef struct
{
    CFE_SB_TlmHdr_t             TlmHeader;
    ARDrone2_NavData_Demo_t		payload;
} OS_PACK ARDrone2_NavData_Demo_Msg_t;   

//FIXME: Temporary, replace with packed message instead
typedef struct
{
    CFE_SB_TlmHdr_t             TlmHeader;
    ARDrone2_NavData_Demo_t     demo;
    NavData_Time_t              time;
    NavData_Magneto_t           mag;
} ARDrone2_NavData_Select_Msg_t;


typedef struct
{
    CFE_SB_TlmHdr_t             TlmHeader;
    int8                        buffer[ARDRONE2_MAX_NAV_BUFFER_SIZE];
} ARDrone2_NavData_Msg_t;


/*
** Local Defines
*/
#define ARDRONE2_HK_TLM_LNGTH           sizeof ( ARDrone2_HK_Tlm_t )
#define ARDRONE2_NAVDATA_DEMO_LNGTH     sizeof ( ARDrone2_NavData_Demo )

#endif /* _ARDRONE2_MSG_H_ */

/*=======================================================================================
** End of file ardrone2_msg.h
**=====================================================================================*/
