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

/*==============================================================================
** File Name:  ardrone2_cmds.h
**
** Title:  Commands Header File for ARDrone2 Application
**
**
** Purpose:  This header file contains declarations and definitions of all 
**           ARDrone2's commands.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2012-11-18 | Mathew Benson | Code Started
**   2013-10-01 | Guy de Carufel | Revised for all API and AT commands
**
**===========================================================================*/

#ifndef _ARDRONE2_CMDS_H_
#define _ARDRONE2_CMDS_H_

/*
** Include Files
*/
#include "cfe.h"
#include "ardrone2_navdata.h"

/*
** AR Drone App command codes (ARDRONE2_CMD_MID)
*/
#define ARDRONE2_NOOP_CC				0
#define ARDRONE2_RESET_COUNTERS_CC		1

/*
** ROTARY WING Generic command codes (ROTWING_GENERIC_CMD_MID)
*/
#define ROTWING_E_STOP_CC				1  // Emergency Stop, if called a 2nd time could reset
#define ROTWING_E_STOP_RESET_CC		    2  // Reset out of Emergency Stop condition
#define ROTWING_LAUNCH_CC               3
#define ROTWING_LAND_CC                 4
#define ROTWING_HOVER_CC                5
#define ROTWING_ROLL_CC                 6
#define ROTWING_PITCH_CC                7
#define ROTWING_YAW_CC                  8  // yaw based on spin rate
#define ROTWING_VERTICAL_CC             9

/*
** ARDRONE2 Specific command codes (ARDRONE2_PLAT_CMD_MID)
*/
#define ARDRONE2_CONFIG_CC              100
#define ARDRONE2_CUSTOM_AT_CMD_CC       101
#define ARDRONE2_SET_FLAT_TRIM_CC       102
#define ARDRONE2_ROLL_W_COMB_YAW_CC     103  // experimental
#define ARDRONE2_AT_MOVE_CC             104
#define ARDRONE2_LED_ANIM_CC            105
#define ARDRONE2_MOVE_ANIM_CC           106  // experimental
//#define ARDRONE2_HOVER_ON_ROUNDEL_CC  107  // experimental
#define ARDRONE2_RESET_CONNECTION_CC    108  // reinit CFS app socket from AR Drone
#define ARDRONE2_AT_MOVE_MAG_CC         109  // move to absolute target, known problematic due to mag noise & error?
#define ARDRONE2_MOD_NAVDATA_OPT_CC     110
#define ARDRONE2_YAW_ABS_CC             111  // yaw based on absolute magnetometer
#define ARDRONE2_MAG_CAL                112  // in-flight calibrate magnetometer

// Amount that the drone drifts back towards being level
#define ARDRONE2_DRIFT_AMOUNT           0.05

typedef enum
{
  NO_CONTROL_MODE = 0,          /**< Doing nothing */
  ARDRONE_UPDATE_CONTROL_MODE,  /**< Not used */
  PIC_UPDATE_CONTROL_MODE,      /**< Not used */
  LOGS_GET_CONTROL_MODE,        /**< Not used */
  CFG_GET_CONTROL_MODE,         /**< Send active configuration file to a client through the 'control' socket UDP 5559 */
  ACK_CONTROL_MODE,             /**< Reset command mask in navdata */
  CUSTOM_CFG_GET_CONTROL_MODE   /**< Requests the list of custom config. IDs */
} ARDrone2_ControlMode_t;


/* Amount of movement remaining, used to smooth out movement commands
 *
 */
typedef struct
{
    int isTilted;          	 /** flag indication if the drone needs correction, 0 = drone is level, 1 = needs to level out */
    float roll;
    float pitch;
    float vSpeed;
	float ySpeed;
} ARDrone2_attitude;


/*
** SB API CMD Structure Declarations
*/
typedef struct
{
    CFE_SB_CmdHdr_t cmdHdr;
    float arg;               /**< (--) [-1 to 1] Motion argument (pitch, roll, etc.) */
} ROTWING_move_cmd_t;

typedef struct
{
    CFE_SB_CmdHdr_t cmdHdr;
    float yawAngle;          /**< (deg) [-180 to 180] angle. 0:North */
    float yawAcu;            /**< (deg) [0 to 1] Accuracy of yaw angle */
} ARDrone2_yaw_abs_cmd_t;

typedef struct
{
    CFE_SB_CmdHdr_t cmdHdr;
    char cmd[256];           /**< (--) Command message */
} ARDrone2_custom_cmd_t;

typedef struct
{
    CFE_SB_CmdHdr_t cmdHdr;
    int rollYawCombinedFlag; /**< (--) flag. 1 = Combined roll with yaw, 0=no combined move */
    float roll;              /**< (--) % of max roll incl. -left, +right */
    float pitch;             /**< (--) % of max pitch incl. -front, +back */
    float vSpeed;            /**< (--) % of max vert. speed -down, +up */
    float ySpeed;            /**< (--) % of max yaw/spin speed -left, +right */
} ARDrone2_move_cmd_t;

typedef struct
{
    CFE_SB_CmdHdr_t cmdHdr;
    int rollYawCombinedFlag; /**< (--) flag. 1 = Combined roll with yaw */
    int absYawFlag;          /**< (--) flag. 1 = Abs yaw, 0 = rel yaw */
    float roll;              /**< (--) % of max roll incl. -left, +right */
    float pitch;             /**< (--) % of max pitch incl. -front, +back */
    float vSpeed;            /**< (--) % of max vert. speed -down, +up */
    float ySpeed;            /**< (--) % of max yaw/spin speed -left, +right */
    float yaw;               /**< (--) [-1 to 1] angle. 0:North 1/-1:South */
    float yawAcu;            /**< (deg) [0 to 1] Accuracy of yaw angle */
} ARDrone2_move_mag_cmd_t;

typedef struct
{
    CFE_SB_CmdHdr_t cmdHdr;
    int animSeq;             /**< (--) Animation sequence */
    int duration;            /**< (s)  Animation duration */
    float frequency;         /**< (Hz) Animation frequency (LED anim only) */
} ARDrone2_anim_cmd_t;

typedef struct
{
    CFE_SB_CmdHdr_t cmdHdr;
    char configCmd[64];      /**< (--) Config command (can use cmd arrays) */
    char param[64];          /**< (--) Parameter to pass */
} ARDrone2_config_cmd_t;

typedef struct
{
    CFE_SB_CmdHdr_t cmdHdr;
    NavData_Tag_t   optionTag;  /**< (--) The navdata option tag */
    int             setFlag;    /**< (--) 1:true, 0:false */
} ARDrone2_navdata_opt_cmd_t;


typedef enum
{
    NUM_VERSION_CONFIG  = 0,   /**< (--) [R] Version of config */
    NUM_VERSION_MB      = 1,   /**< (--) [R] Hardware version of M/B */
    NUM_VERSION_SOFT    = 2,   /**< (--) [R] Firmware version of drone */
    SOFT_BUILD_DATE     = 3,   /**< (--) [R] Date of f/w compilation */
    MOTOR1_SOFT         = 4,   /**< (--) [R] Get motor S/W version */
    MOTOR2_SOFT         = 5,   /**< (--) [R] Get motor S/W version */
    MOTOR3_SOFT         = 6,   /**< (--) [R] Get motor S/W version */
    MOTOR4_SOFT         = 7,   /**< (--) [R] Get motor S/W version */
    MOTOR1_HARD         = 8,   /**< (--) [R] Get motor H/W version */
    MOTOR2_HARD         = 9,   /**< (--) [R] Get motor H/W version */
    MOTOR3_HARD         = 10,  /**< (--) [R] Get motor H/W version */
    MOTOR4_HARD         = 11,  /**< (--) [R] Get motor H/W version */
    MOTOR1_SUPPLIER     = 12,  /**< (--) [R] Get motor supplier version */
    MOTOR2_SUPPLIER     = 13,  /**< (--) [R] Get motor supplier version */
    MOTOR3_SUPPLIER     = 14,  /**< (--) [R] Get motor supplier version */
    MOTOR4_SUPPLIER     = 15,  /**< (--) [R] Get motor supplier version */
    ARDRONE_NAME        = 16,  /**< (--) [R/W] Set drone param "name" */
    FLYING_TIME         = 17,  /**< (--) [R] Lifetime flying time */
    NAVDATA_DEMO        = 18,  /**< (--) [R/W] param "TRUE" = Reduced set, "FALSE" = full set. */
    COM_WATCHDOG        = 19,  /**< (--) [R/W] Watchdog wait time. (disabled) Fixed delay of 250ms */
    VIDEO_ENABLE        = 20,  /**< (--) [R/W] default: TRUE. (disabled) */
    VISION_ENABLE       = 21,  /**< (--) [R/W] default: TRUE. (disabled) */
    VBAT_MIN            = 22,  /**< (--) [R] Min batt. level before shutdown */
    NAVDATA_OPTIONS     = 23   /**< (--) [R/W] Ask for navdata pkt (ex:"65537") packets defined in navdata_common.h */
} ARDrone2_conf_gen_cmd_list_t;           

typedef enum
{
    /* CAT_COMMON config */
    ACCS_OFFSET             = 0,  /**< (--) [R] Parrot internal debug info */
    ACCS_GAINS              = 1,  /**< (--) [R] Parrot internal debug info */
    GYROS_OFFSET            = 2,  /**< (--) [R] Parrot internal debug info */
    GYROS_GAINS             = 3,  /**< (--) [R] Parrot internal debug info */
    GYROS110_OFFSET         = 4,  /**< (--) [R] Parrot internal debug info */
    GYROS110_GAINS          = 5,  /**< (--) [R] Parrot internal debug info */
    GYROS_OFFSET_THR_X      = 6,  /**< (--) [R] Parrot internal debug info */
    GYROS_OFFSET_THR_Y      = 7,  /**< (--) [R] Parrot internal debug info */
    GYROS_OFFSET_THR_Z      = 8,  /**< (--) [R] Parrot internal debug info */
    PWM_REF_GYROS           = 9,  /**< (--) [R] Parrot internal debug info */
    SHIELD_ENABLE           = 10, /**< (--) [R/W] future use. */
    ALTITUDE_MAX            = 11, /**< (mm) [R/W] Limit if 500-5000, inf. if>10000 */
    ALTITUDE_MIN            = 12, /**< (mm) [R/W] Leave default for stability. */
    OUTDOOR                 = 13, /**< (--) [R/W] Set to TRUE if outdoor. */
    FLIGHT_WITHOUT_SHELL    = 14, /**< (--) [R/W] Flying without indoor shell */
    FLIGHT_ANIM             = 15, /**< (--) [R/W] Launch drone anim. "<seq>,<dur>" */

    /* CAT APPLI config */
    CONTROL_LEVEL           = 16, /**< (--) [R/W] set to 1 (default) to allow combined roll/yaw maneuvers */

    /* CAT_USER configs */
    EULER_ANGLE_MAX         = 17, /**< (rad)   [R/W] Max incl. angle for pitch/roll */
    INDOOR_EULER_ANGLE_MAX  = 18, /**< (rad)   [R/W] if outdoor is false */
    OUTDOOR_EULER_ANGLE_MAX = 19, /**< (rad)   [R/W] if outdoor is true */
    CONTROL_VZ_MAX          = 20, /**< (mm/s)  [R/W] Max vert. speed Recom:200-2000 */
    INDOOR_CONTROL_VZ_MAX   = 21, /**< (mm/s)  [R/W] if outdoor is false */
    OUTDOOR_CONTROL_VZ_MAX  = 22, /**< (mm/s)  [R/W] if outdoor is true */
    CONTROL_YAW             = 23, /**< (rad/s) [R/W] Max yaw speed Recom:0.7-6.11 */
    INDOOR_CONTROL_YAW      = 24, /**< (rad/s) [R/W] if outdoor is false */
    OUTDOOR_CONTROL_YAW     = 25, /**< (rad/s) [R/W] if outdoor is true */

    /* CAT_SESSION */
    FLYING_MODE             = 26  /**< (--) [R/W] see ardrone_api.h for mode */
} ARDrone2_conf_ctrl_cmd_list_t;

//TODO Add the other types of commands later: NETWORK, PIC, VIDEO, DETECT, CUSTOM 


/* cFS App function prototypes */
void delay(unsigned int);
void ARDrone2_ATCmdMain(void);
void ARDrone2_configATCmd(char *, char *);
void ARDrone2_cmdSetControl(ARDrone2_ControlMode_t controlMode);

#endif /* ARDRONE2_CMDS_H_ */
