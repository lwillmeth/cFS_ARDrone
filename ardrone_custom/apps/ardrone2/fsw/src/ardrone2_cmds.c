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
**        Created by Michael Rosburg, mrosburg@odysseysr.com
**
******************************************************************************/

/*=======================================================================================
** File Name:  ardrone2_cmds.c
**
** Title:  Function Definitions ARDRONE2 App API commands
**
**
** Purpose:  Define all the API function calls for Drone control
**
** Functions Defined:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2013-10-15 | Guy de Carufel | Code Started
**   2016-01-16 | Michael Rosburg | Added include file ardrone2_telem.h for ARDrone2_ResetTlmConnection
**
**=====================================================================================*/

/*
** Include Files
*/
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>

#include "ardrone2_app.h"
#include "ardrone2_cmds.h"
#include "ardrone2_telem.h"

/*
** Local Defines
*/
int  ARDrone2_InitATCmdSockets(void);
void ARDrone2_RcvATCmds(void);
void ARDrone2_RotWingGenericCmds(CFE_SB_Msg_t*);
void ARDrone2_PlatformSpecificCmds(CFE_SB_Msg_t*);
void ARDrone2_customATCmd(int, char *);
void ARDrone2_modNavDataOpt(NavData_Tag_t, int flag);
void ARDrone2_setFlatTrim(void);
void ARDrone2_takeOff(void);
void ARDrone2_land(void);
void ARDrone2_eStop(void);
void ARDrone2_reset(void);
void ARDrone2_hover(void);
void ARDrone2_pitch(float pitch);
void ARDrone2_roll(float roll);
void ARDrone2_rollWithCombinedYaw(float roll);
void ARDrone2_yaw(float ySpeed);
void ARDrone2_climb(float vSpeed);
void ARDrone2_move(int, float, float, float, float);
void ARDrone2_yawAbs(float yawAngle, float yawAcu);
void ARDrone2_move_mag(int, float, float, float, float, float, float);
void ARDrone2_magCal(void);
void ARDrone2_LEDAnimate(int, int, float);
void ARDrone2_moveAnimate(int, int);
void ARDrone2_SendATCommand(char *command_string);
void ARDrone2_cmdComWdg(void);
void ARDrone2_resetCmdSeq(void);
int ARDrone2_incrAtCmdSeq(void);
void ARDrone2_ATCmdCleanupCallback(void);
void ARDrone2_Drift_Towards_Level();
float ARDrone2_Reduce_Angle(float);
int ARDrone2_Update_Attitude_Flag(void);

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/
extern AppData_t g_ARDrone2_AppData;

/*
** Global Variables
*/
const char *g_ARDrone2_configGenCmds[] =
{
    "GENERAL:num_version_config", 
    "GENERAL:num_version_mb",     
    "GENERAL:num_version_soft",   
    "GENERAL:soft_build_date",    
    "GENERAL:motor1_soft",        
    "GENERAL:motor2_soft",        
    "GENERAL:motor3_soft",        
    "GENERAL:motor4_soft",        
    "GENERAL:motor1_hard",        
    "GENERAL:motor2_hard",        
    "GENERAL:motor3_hard",        
    "GENERAL:motor4_hard",        
    "GENERAL:motor1_supplier",    
    "GENERAL:motor2_supplier",    
    "GENERAL:motor3_supplier",    
    "GENERAL:motor4_supplier",    
    "GENERAL:ardrone_name",       
    "GENERAL:flying_time",        
    "GENERAL:navdata_demo",       
    "GENERAL:com_watchdog",       
    "GENERAL:video_enable",       
    "GENERAL:vision_enable",      
    "GENERAL:vbat_min",           
    "GENERAL:navdata_options"    
};


const char *g_ARDrone2_configCtrlCmds[] =
{
    "CONTROL:accs_offset",
    "CONTROL:accs_gains",
    "CONTROL:gyros_offset",
    "CONTROL:gyros_gains",
    "CONTROL:gyros110_offset",
    "CONTROL:gyros110_gains",
    "CONTROL:gyros_offset_thr_x",
    "CONTROL:gyros_offset_thr_y",
    "CONTROL:gyros_offset_thr_z",
    "CONTROL:pwm_ref_gyros",
    "CONTROL:shield_enable",
    "CONTROL:altitude_max",
    "CONTROL:altitude_min",
    "CONTROL:outdoor",
    "CONTROL:flight_without_shell",
    "CONTROL:flight_anim",
    "CONTROL:control_level",
    "CONTROL:euler_angle_max",
    "CONTROL:indoor_euler_angle_max",
    "CONTROL:outdoor_euler_angle_max",
    "CONTROL:control_vz_max",
    "CONTROL:indoor_control_vz_max",
    "CONTROL:outdoor_control_vz_max",
    "CONTROL:control_yaw",
    "CONTROL:indoor_control_yaw",
    "CONTROL:outdoor_control_yaw",
    "CONTROL:flying_mode"
};

/*
** Local Variables
*/
struct sockaddr_in  CmdSocketAddress;
struct sockaddr_in  CmdARDroneAddress;
int                 CmdSequence;
int                 ATCmdSocketID;
ARDrone2_attitude	CurAttitude;


void delay(unsigned int mseconds)
{
	clock_t goal = mseconds + clock();
	while (goal > clock());
}


/* Main function of task */
void ARDrone2_ATCmdMain(void)
{
    /* Register this child task */
    if (CFE_ES_RegisterChildTask() != CFE_SUCCESS)
    {
    	CFE_ES_WriteToSysLog("CFE_ES_RegisterChildTask: Error: Cannot register Child Task.\n");
    	OS_printf("ARDrone2: Register Child Task failed.\n");
    	return;
    }

    /* Increment the Main task Execution Counter */
    CFE_ES_IncrementTaskCounter();
    
    OS_printf("ARDrone2: ATCmd task started.\n");

    /* Initialize sockets */
    if (ARDrone2_InitATCmdSockets() != 0) {
    	OS_printf("ARDrone2: Initialize sockets failed.\n");
    	return;
    }

    /* Install the cleanup callback */
    OS_TaskInstallDeleteHandler((void*)&ARDrone2_ATCmdCleanupCallback);

    OS_printf("ARDrone2: ATCmd task entering main loop.\n");

	/* Loop and check for a new AT cmd every 30ms */
    while (g_ARDrone2_AppData.ATCommandTaskShouldRun) 
    {
        ARDrone2_RcvATCmds();
        OS_TaskDelay(30);
    	ARDrone2_Drift_Towards_Level();
    }

    OS_printf("ARDrone2: ATCmd exiting main loop.\n");
    ARDrone2_ATCmdCleanupCallback();

    CFE_ES_ExitChildTask();
}


void ARDrone2_ATCmdCleanupCallback(void)
{
    if (ATCmdSocketID > 0)
    {
       OS_printf("ARDrone2: ATCmd cleanup socket.\n");
       close(ATCmdSocketID);
       ATCmdSocketID = 0;
    }
}


/* Initialize commanding socket */
int ARDrone2_InitATCmdSockets(void)
{
    int status = 0;
    
    /* Reset Cmd sequence */
    ARDrone2_resetCmdSeq();

	/* Create sockets */
    if ( (ATCmdSocketID = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        CFE_EVS_SendEvent(CMD_SOCKETCREATE_ERR_EID, CFE_EVS_ERROR,
                          "ARDrone2: Failed to create command socket.  errno: %d", errno);
        status = -1;
        goto end_of_function;
    }

    /* Set addresses */
    bzero((char *) &CmdSocketAddress, sizeof(CmdSocketAddress));
    CmdSocketAddress.sin_family      = AF_INET;
    CmdSocketAddress.sin_addr.s_addr = inet_addr("0.0.0.0");
    CmdSocketAddress.sin_port        = htons(0);

    bzero((char *) &CmdARDroneAddress, sizeof(CmdARDroneAddress));
    CmdARDroneAddress.sin_family      = AF_INET;
    CmdARDroneAddress.sin_addr.s_addr = inet_addr(ARDRONE2_WIFI_IP);
    CmdARDroneAddress.sin_port        = htons(ARDRONE2_AT_PORT);

    if ( bind(ATCmdSocketID, (struct sockaddr *) &CmdSocketAddress,
         sizeof(CmdSocketAddress)) < 0)
    {
        CFE_EVS_SendEvent(SOCKETCREATE_ERR_EID, CFE_EVS_ERROR, 
                          "ARDrone2: Failed to bind command socket.  errno: %d", errno);
        status = -1;
        goto end_of_function;
    }

end_of_function:
    return status;
}


/*=====================================================================================
** Name: ARDrone2_RcvATCmds
**
** Purpose: To Receive and process AT commands 
**
** Arguments:
**
** Returns:
**    None
**
** Routines Called:
**    CFE_SB_GetCmdCode
**    CFE_EVS_SendEvent
**
** Called By:
**    ARDrone2_ATCmdMain
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    g_ARDrone2_AppData.HkTlm.usCmdCnt
**    g_ARDrone2_AppData.HkTlm.usCmdErrCnt
**
** Limitations, Assumptions, External Events, and Notes:
**
** Algorithm:
**
** Author(s):  Guy de Carufel
**
** History:  Date Written  2013-10-15
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void ARDrone2_RcvATCmds(void)
{
    int32 iStatus = CFE_SUCCESS;
    CFE_SB_Msg_t*   CmdMsgPtr=NULL;
    CFE_SB_MsgId_t  CmdMsgId;

    iStatus = CFE_SB_RcvMsg(&CmdMsgPtr, g_ARDrone2_AppData.ATCmdPipeId,
                            CFE_SB_POLL);
    
    if (iStatus == CFE_SUCCESS)
    {
        CmdMsgId = CFE_SB_GetMsgId(CmdMsgPtr);
        switch (CmdMsgId)
        {
            /* Send a generic rotary wing command to the drone */
            case ROTWING_GENERIC_CMD_MID:
                OS_printf("ROTWING_GENERIC_CMD_MID\n");
                ARDrone2_RotWingGenericCmds(CmdMsgPtr);
                break;

            /* Send an ARDrone Platform specific command to the drone */
            case ARDRONE2_PLAT_CMD_MID:
                OS_printf("ARDRONE2_SPECIFIC_CMD_MID\n");
                ARDrone2_PlatformSpecificCmds(CmdMsgPtr);
                break;

            default:
                CFE_EVS_SendEvent(MSGID_ERR_EID, CFE_EVS_ERROR,
                                  "ARDrone2 - Recvd invalid CMD msgId (0x%08X)", CmdMsgId);
                break;
        }
    }
    else if (iStatus == CFE_SB_NO_MESSAGE)
    {
        /* stroke the watchdog */
        ARDrone2_cmdComWdg();
    }
    else
    {
        CFE_EVS_SendEvent(PIPE_ERR_EID, CFE_EVS_ERROR,
              "ARDrone2: AT CMD pipe read error (0x%08X)",
			  (unsigned int)iStatus);
        g_ARDrone2_AppData.uiRunStatus = CFE_ES_APP_ERROR;
    }
}
    

/*=====================================================================================
** Name: ARDrone2_RotWingGenericCmds
**
** Purpose: Process Generic Rotory Wing commands for A.R.Drone2
**
** Arguments:
**      [in] MsgPtr  Pointer to message received from AT Cmd pipe
**
** Returns:
**    None
**
** Routines Called:
**
** Called By:
**    ARDrone2_RcvATCmds()
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**
** Limitations, Assumptions, External Events, and Notes:
**
** Algorithm:
**
** Author(s):  Guy de Carufel
**
** History:  
**  2013-10-01 | Guy de Carufel | Code Started            
**=====================================================================================*/
void ARDrone2_RotWingGenericCmds(CFE_SB_Msg_t* MsgPtr)
{
    uint32  uiCmdCode = 0;

    if (MsgPtr != NULL)
    {
        uiCmdCode = CFE_SB_GetCmdCode(MsgPtr);
        switch (uiCmdCode)
        {
            case ROTWING_E_STOP_CC:
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "Emergency Stop Command");
                ARDrone2_eStop();
                break;

            case ROTWING_E_STOP_RESET_CC:
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "Emergency Stop Reset Command");
                ARDrone2_reset();
                break;

            case ROTWING_LAUNCH_CC:
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                			"Launch Command");
                ARDrone2_takeOff();
                break;
            
            case ROTWING_LAND_CC:
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "Land Command");
                ARDrone2_land();
                break;
            
            case ROTWING_HOVER_CC:
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "Hover Command");
                ARDrone2_hover();
                break;
            
            case ROTWING_ROLL_CC:
            {
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                ROTWING_move_cmd_t * pCmdMsg = 
                    (ROTWING_move_cmd_t *) MsgPtr;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "Roll Command");
                ARDrone2_roll(pCmdMsg->arg);
                break;
            }
                
            case ROTWING_PITCH_CC:
            {
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                ROTWING_move_cmd_t * pCmdMsg = 
                    (ROTWING_move_cmd_t *) MsgPtr;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "Pitch Command");
                ARDrone2_pitch(pCmdMsg->arg);
                break;
            }
            
            case ROTWING_YAW_CC:
            {
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                ROTWING_move_cmd_t * pCmdMsg = 
                    (ROTWING_move_cmd_t *) MsgPtr;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "Yaw Command");
                ARDrone2_yaw(pCmdMsg->arg);
                break;
            }
            
            case ROTWING_VERTICAL_CC:
            {
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                ROTWING_move_cmd_t * pCmdMsg = 
                    (ROTWING_move_cmd_t *) MsgPtr;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "Vertical Command");
                ARDrone2_climb(pCmdMsg->arg);
                break;
            }
            
            default:
                g_ARDrone2_AppData.HkTlm.usCmdErrCnt++;
                CFE_EVS_SendEvent(MSGID_ERR_EID, CFE_EVS_ERROR,
                                  "ARDrone2 - Recvd invalid AT cmd Code (%d)", 
                                  (int)uiCmdCode);
                break;
        }
    }
}


/*=====================================================================================
** Name: ARDrone2_PlatformSpecificCmds
**
** Purpose: Process Drone Specific commands for A.R.Drone2
**
** Arguments:
**      [in] MsgPtr  Pointer to message received from AT Cmd pipe
**
** Returns:
**    None
**
** Routines Called:
**
** Called By:
**    ARDrone2_RcvATCmds()
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**
** Limitations, Assumptions, External Events, and Notes:
**
** Algorithm:
**
** Author(s):  Guy de Carufel
**
** History:  
**  2013-10-01 | Guy de Carufel | Code Started            
**=====================================================================================*/
void ARDrone2_PlatformSpecificCmds(CFE_SB_Msg_t* MsgPtr)
{
    uint32  uiCmdCode = 0;
    uint32  opt = 0;

    if (MsgPtr != NULL)
    {
        uiCmdCode = CFE_SB_GetCmdCode(MsgPtr);
        switch (uiCmdCode)
        {
            case ARDRONE2_CONFIG_CC:
            {
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                ARDrone2_config_cmd_t * pCmdMsg = 
                    (ARDrone2_config_cmd_t *) MsgPtr;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "Config Command");
                ARDrone2_configATCmd(pCmdMsg->configCmd, pCmdMsg->param);
                break;
            }
            
            case ARDRONE2_CUSTOM_AT_CMD_CC:
            {
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                ARDrone2_custom_cmd_t * pCmdMsg = 
                    (ARDrone2_custom_cmd_t *) MsgPtr;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "Custom AT Command");
                ARDrone2_customATCmd(strlen(pCmdMsg->cmd), pCmdMsg->cmd);
                break;
            }

            case ARDRONE2_SET_FLAT_TRIM_CC:
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "Set Flat Trim Command");
                ARDrone2_setFlatTrim();
                break;

            case ARDRONE2_ROLL_W_COMB_YAW_CC:
            {
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                ROTWING_move_cmd_t * pCmdMsg = 
                    (ROTWING_move_cmd_t *) MsgPtr;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "Roll with Combined Yaw Command");
                ARDrone2_rollWithCombinedYaw(pCmdMsg->arg);
                break;
            }

            case ARDRONE2_AT_MOVE_CC:
            {
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                ARDrone2_move_cmd_t * pCmdMsg = 
                    (ARDrone2_move_cmd_t *) MsgPtr;
                // set the bitmask = 0x03
                opt = pCmdMsg->rollYawCombinedFlag*2 + 1;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "AT Move Command");
                ARDrone2_move(opt, pCmdMsg->roll, pCmdMsg->pitch, 
                              pCmdMsg->vSpeed, pCmdMsg->ySpeed);
                break;
            }

            case ARDRONE2_LED_ANIM_CC:
            {
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                ARDrone2_anim_cmd_t * pCmdMsg = 
                    (ARDrone2_anim_cmd_t *) MsgPtr;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "LED Animation Command");
                ARDrone2_LEDAnimate(pCmdMsg->animSeq, pCmdMsg->duration, 
                                    pCmdMsg->frequency); 
                break;
            }

            case ARDRONE2_MOVE_ANIM_CC:
            {
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                ARDrone2_anim_cmd_t * pCmdMsg = 
                    (ARDrone2_anim_cmd_t *) MsgPtr;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "Move Animation Command");
                ARDrone2_moveAnimate(pCmdMsg->animSeq, pCmdMsg->duration); 
                break;
            }

            case ARDRONE2_RESET_CONNECTION_CC:
            {
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "Reset Connection Command");
                ARDrone2_ResetTlmConnection();
                break;
            }

            case ARDRONE2_AT_MOVE_MAG_CC:
            {
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                ARDrone2_move_mag_cmd_t * pCmdMsg = 
                    (ARDrone2_move_mag_cmd_t *) MsgPtr;
                opt = pCmdMsg->absYawFlag*4 + pCmdMsg->rollYawCombinedFlag*2 + 1;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "AT Move Absolute Magnetometer Command");
                ARDrone2_move_mag(opt, pCmdMsg->roll, pCmdMsg->pitch, 
                                  pCmdMsg->vSpeed, pCmdMsg->ySpeed,
                                  pCmdMsg->yaw, pCmdMsg->yawAcu);
                break;
            }

            case ARDRONE2_MOD_NAVDATA_OPT_CC:
            {
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                ARDrone2_navdata_opt_cmd_t * pCmdMsg =
                    (ARDrone2_navdata_opt_cmd_t *) MsgPtr;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "MOD NAVDATA OPT Command");
                ARDrone2_modNavDataOpt(pCmdMsg->optionTag,
                                       pCmdMsg->setFlag);
                break;
            }

            case ARDRONE2_YAW_ABS_CC:
            {
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                ARDrone2_yaw_abs_cmd_t * pCmdMsg =
                    (ARDrone2_yaw_abs_cmd_t *) MsgPtr;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "Absolute Yaw Command");
                ARDrone2_yawAbs(pCmdMsg->yawAngle, pCmdMsg->yawAcu);
                break;
            }

            case ARDRONE2_MAG_CAL:
            {
                g_ARDrone2_AppData.HkTlm.usCmdCnt++;
                CFE_EVS_SendEvent(CMD_INF_EID,CFE_EVS_INFORMATION,
                            "Magnetometer Calibrate Command");
                ARDrone2_magCal();
                break;
            }
            
            default:
                g_ARDrone2_AppData.HkTlm.usCmdErrCnt++;
                CFE_EVS_SendEvent(MSGID_ERR_EID, CFE_EVS_ERROR,
                                  "ARDrone2 - Recvd invalid cmdId (%d)", (int)uiCmdCode);
                break;
        }
    }
}


/*!
 * @brief  Send a config AT command
 * @param  cmd_in  pointer to command message
 * @param  param   pointer to command parameter
 * @return Void.
 * */
void ARDrone2_configATCmd(char *cmd_in, char *param)
{
    int seq = 0;
    char cmd[100];

    memset(cmd, 0x0, sizeof(cmd));
    seq = ARDrone2_incrAtCmdSeq();
    sprintf(cmd, "AT*CONFIG=%d,\"%s\",\"%s\"\r", seq, cmd_in, param);
    ARDrone2_SendATCommand(cmd);
    OS_printf("Sending AT*CONFIG command:%s, %s\n", cmd_in,param);
    OS_TaskDelay(30);
}


/*!
 * @brief  Send a custom AT command
 * @param  length - length of the command string
 * @param  cmd_in - pointer to command message
 * @return Void.
 */
void ARDrone2_customATCmd(int length, char *cmd_in)
{
    char cmd[256];
    memset(cmd, 0x0, sizeof(cmd));
    memcpy(cmd, cmd_in, length);
    ARDrone2_SendATCommand(cmd);
}


/*!
 * @brief  Set the control mode
 * @param  controlMode - configuration control mode
 * @return Void.
 */
void ARDrone2_cmdSetControl(ARDrone2_ControlMode_t controlMode)
{
    int seq = 0;
    char cmd[100];
    memset(cmd, 0x0, sizeof(cmd));
    seq = ARDrone2_incrAtCmdSeq();
    OS_printf("Setting control.\n");
    sprintf(cmd, "AT*CTRL=%d,%d,0\r", seq, controlMode);
    ARDrone2_SendATCommand(cmd);
}


/*!
 * @brief Set configuration IDs
 * @return Void.
 */
void ARDrone2_configIds(void)
{
    ARDrone2_configATCmd("custom:session_id", "1");
    OS_TaskDelay(30);
    ARDrone2_configATCmd("custom:profile_id", "1");
    OS_TaskDelay(30);
    ARDrone2_configATCmd("custom:application_id", "1");
}


/*!
 * @brief Change navData options
 * @param optionTag - The navdata option tag
 * @param flag      - boolean (0 - false, 1 - true)
 * @return Void.
 */
void ARDrone2_modNavDataOpt(NavData_Tag_t optionTag, int flag)
{
    char cmd_param[11];
    
    OS_MutSemTake(g_ARDrone2_AppData.NavDataMutexId);
    if (flag == TRUE)
    {
        /* Perform bitwize OR to turn on bit */
        g_ARDrone2_AppData.lastNavDataOptions |= 1 << (int) optionTag;
    }
    else
    {
        /* Perform bitwise XAND to turn off bit */
        g_ARDrone2_AppData.lastNavDataOptions &= ~(1 << (int) optionTag);
    }
    
    sprintf(cmd_param, "%lu", g_ARDrone2_AppData.lastNavDataOptions);

    ARDrone2_configATCmd("general:navdata_options", cmd_param);
    OS_MutSemGive(g_ARDrone2_AppData.NavDataMutexId);
}

/*!
 * @brief  Flat trim forces horizontality of the drone. Tell the drone it is lying horizontally.
 * @note   Must be called before flying
 * @return Void.
 */
void ARDrone2_setFlatTrim(void)
{
    char cmd[100];
    int seq;

    // Reset desired movements to zero
    CurAttitude.isTilted 	= 1;
    CurAttitude.roll 		= 0;
    CurAttitude.pitch 		= 0;
    CurAttitude.vSpeed 		= 0;
    CurAttitude.ySpeed		= 0;

    /* Unknown setting, but needed apparently */
    memset(cmd, 0x0, sizeof(cmd));
    seq = ARDrone2_incrAtCmdSeq();
    sprintf(cmd, "AT*PMODE=%d,%d\r", seq, 2);
    ARDrone2_SendATCommand(cmd);

    OS_TaskDelay(30);

    /* Set limits ? */
    memset(cmd, 0x0, sizeof(cmd));
    seq = ARDrone2_incrAtCmdSeq();
    sprintf(cmd, "AT*MISC=%d,%d,%d,%d,%d\r", seq, 2, 20, 2000, 3000);
    ARDrone2_SendATCommand(cmd);

    OS_TaskDelay(30);
    memset(cmd, 0x0, sizeof(cmd));
    seq = ARDrone2_incrAtCmdSeq();
    sprintf(cmd, "AT*FTRIM=%d\r", seq);
    ARDrone2_SendATCommand(cmd);
}

/*!
 * @brief Take off and hover at pre-defined level (default: 1m, set in config)
 * @return Void.
 */
void ARDrone2_takeOff(void)
{
    int seq = 0;
    char cmd[100];
    memset(cmd, 0x0, sizeof(cmd));

    /* Bits 18, 20, 22, 24 and 28 set to 1 */
    int arg2 = 0x11540000;

    /* For take-off, set bit 9 to 1 */
    arg2 |= 0x200;

    seq = ARDrone2_incrAtCmdSeq();
    sprintf(cmd, "AT*REF=%d,%d\r", seq, arg2);
    ARDrone2_SendATCommand(cmd);
}


/*!
 * @brief Land the drone
 * @return Void.
 */
void ARDrone2_land(void)
{
    char cmd[100];
    memset(cmd, 0x0, sizeof(cmd));
    int seq;

    /* Bits 18, 20, 22, 24 and 28 set to 1.  Bit 9 is 0 */
    int arg2 = 0x11540000;

    seq = ARDrone2_incrAtCmdSeq();
    sprintf(cmd, "AT*REF=%d,%d\r", seq, arg2);
    ARDrone2_SendATCommand(cmd);
}

/*!
 * @brief Emergency stop.
 * @note  Kills engines and falls if flying.
 * @return Void.
 */
void ARDrone2_eStop(void)
{
    int seq = ARDrone2_incrAtCmdSeq();
    
    char cmd[100];
    memset(cmd, 0x0, sizeof(cmd));

    /* Bits 18, 20, 22, 24 and 28 set to 1 */
    int arg2 = 0x11540000;

    /* For emergency stop / reset, set bit 8 to 1 */
    arg2 |= 0x100;  

    sprintf(cmd, "AT*REF=%d,%d\r", seq, arg2);
    ARDrone2_SendATCommand(cmd);
}

/*!
 * @brief  Reset the e-stop. Can then take off again.
 * @return Void.
 */
void ARDrone2_reset(void)
{
    /* Same AT command as e-stop */
    ARDrone2_eStop();
}

/*!
 * @brief  Make drone hover over same point on ground (null translation)
 * @return Void.
 */
void ARDrone2_hover(void)
{
    int opt = 0;        // Set to 0 to enter hover mode
    float roll = 0;     // Roll
    float pitch = 0;    // Pitch
    float vSpeed = 0;   // Vertical speed
    float ySpeed = 0;   // Yaw / spin

    // Reset desired movements to zero
    CurAttitude.isTilted 	= 0;
    CurAttitude.roll 		= 0;
    CurAttitude.pitch 		= 0;
    CurAttitude.vSpeed 		= 0;
    CurAttitude.ySpeed		= 0;

    ARDrone2_move(opt, roll, pitch, vSpeed, ySpeed);
}


/*!
 * @brief  Make ARDrone pitch
 * @param  pitch - command pitch unit, range: -1.0 to 1.0
 * @note   "nose down" move forward (-%) / "nose up" move backward (+%)
 * @return Void.
 */
void ARDrone2_pitch(float pitch)
{
//    int opt = 1;         // Set to consider other params
//    float roll = 0;      // Roll
//    float vSpeed = 0;    // Vertical speed
//    float ySpeed = 0;    // Yaw / spin

    /* Out of range > 100% of max inclination value */
    if(pitch > 1 || pitch < -1)
    {
        return;
    }

    CurAttitude.pitch = pitch;
//    ARDrone2_move(opt, roll, pitch, vSpeed, ySpeed);
//    OS_TaskDelay(500);
//    ARDrone2_move(opt, roll, 0, vSpeed, ySpeed);
}


/*!
 * @brief  Make ARDrone roll
 * @param  roll command unit, range: -1.0 to 1.0
 * @note   move left (-%) / move right (+%)
 * @return Void.
 */
void ARDrone2_roll(float roll)
{
//    int opt = 1;           // Set to consider other params
//    float pitch = 0;       // Pitch
//    float vSpeed = 0;      // Vertical speed
//    float ySpeed = 0;      // Yaw / spin

    /* Out of range > 100% of max inclination value */
    if(roll > 1 || roll < -1)
    {
        return;                              
    }

    CurAttitude.roll = roll;
//    ARDrone2_move(opt, roll, pitch, vSpeed, ySpeed);
//    OS_TaskDelay(200);
//    ARDrone2_move(opt, 0, pitch, vSpeed, ySpeed);
}


/*!
 * @brief  Make ARDrone roll
 * @param  roll command roll unit , range: -1.0 to 1.0
 * @note   left (-%) / right (+%)
 *         ARDrone specific roll command used in racing
 * @return Void.
 */
void ARDrone2_rollWithCombinedYaw(float roll)
{
//    int opt = 3;          // Set bits 1 and 2 = 1
//    float pitch = 0;      // Pitch
//    float vSpeed = 0;     // Vertical speed
//    float ySpeed = 0;     // Yaw / spin

    /* Out of range > 100% of max inclination value */
    if(roll > 1 || roll < -1)
    {
        return;                              
    }

//    ARDrone2_move(opt, roll, pitch, vSpeed, ySpeed);
//    OS_TaskDelay(200);
//    ARDrone2_move(opt, 0, pitch, vSpeed, ySpeed);
}


/*!
 *  @brief  Make ARDrone spin
 *  @param  ySpeed  yaw speed, range -1.0 to 1.0
 *  @note   left-cw (-%) / right-ccw (+%), sets spin rate,
 *          scaled by config max speed value
 *  @return Void.
 */
void ARDrone2_yaw(float ySpeed)
{
//    int opt = 1;         // Set to consider other params
//    float roll = 0;      // Roll
//    float pitch = 0;     // Pitch
//    float vSpeed = 0;    // Vertical speed

    /* Out of range > 100% of max speed value */
    if(ySpeed > 1 || ySpeed < -1)
    {
        return;                              
    }

    CurAttitude.ySpeed = ySpeed;
//    ARDrone2_move(opt, roll, pitch, vSpeed, ySpeed);
//    OS_TaskDelay(150);
//    ARDrone2_move(opt, roll, pitch, vSpeed, 0);
}


/*!
 * @brief  Make drone climb
 * @param  vSpeed  vertical speed, range -1.0 to 1.0
 * @note   (+%) or drop (-%), sets rate
 *         scaled by config max speed value
 * @return Void.
 */
void ARDrone2_climb(float vSpeed)
{
//    int opt = 1;         // Set to consider other params
//    float roll = 0;      // Roll
//    float pitch = 0;     // Pitch
//    float ySpeed = 0;    // Yaw / spin

    /* Out of range > 100% of max speed value */
    if(vSpeed > 1 || vSpeed < -1)
    {
        return;
    }

    CurAttitude.vSpeed = vSpeed;
//    ARDrone2_move(opt, roll, pitch, vSpeed, ySpeed);
//    OS_TaskDelay(200);
//    ARDrone2_move(opt, roll, pitch, 0, ySpeed);
}


/*!
 * @brief  General ARDrone function to move
 * @param  opt   0 = hover only (nulls out all rates, pitch, yaw, and roll),
 *               1 = normal maneuver based on the other arguments
 * @param  roll
 * @param  pitch
 * @param  vSpeed
 * @param  ySpeed
 *
 * @note   ex: opt=0 and all other args=0.0 then it will hover and null out all rates and lateral velocity (with respect to ground)
 *         ex: opt=1 and all other args=0.0 then it will hover but not null the rates and continue with lateral velocity (with respect to ground)
 *         ex: opt=3 maneuver with combined yaw
 * @return Void.
 */
void ARDrone2_move(int opt, float roll, float pitch, float vSpeed, float ySpeed)
{
    char cmd[100];
    int seq = 0;
    memset(cmd, 0x0, sizeof(cmd));

    seq = ARDrone2_incrAtCmdSeq();

    OS_printf("Sending moving command: Opt=%d, roll=%f, Pitch=%f, "
              "vSpeedr=%f, ySpeed=%f\n", opt, roll, pitch, vSpeed, ySpeed);

    sprintf(cmd, "AT*PCMD=%d,%d,%d,%d,%d,%d\r", seq, opt, 
            *(int*) &roll, *(int*) &pitch, *(int*) &vSpeed, *(int*) &ySpeed);

    OS_printf("PCMD command: %s\n", cmd);
    ARDrone2_SendATCommand(cmd);
}


/*!
 * @brief  General ARDrone function to drift towards level, from current desired angles
 * @param  opt   0 = hover only (nulls out all rates, pitch, yaw, and roll),
 *               1 = normal maneuver based on the other arguments
 * @param  roll
 * @param  pitch
 * @param  vSpeed
 * @param  ySpeed
 *
 * @note   ex: opt=0 and all other args=0.0 then it will hover and null out all rates and lateral velocity (with respect to ground)
 *         ex: opt=1 and all other args=0.0 then it will hover but not null the rates and continue with lateral velocity (with respect to ground)
 *         ex: opt=3 maneuver with combined yaw
 * @return Void.
 */
void ARDrone2_Drift_Towards_Level()
{
	if ( CurAttitude.isTilted ){

		CurAttitude.isTilted	= ARDrone2_Update_Attitude_Flag();

		int opt = 1;

		char cmd[100];
		int seq = 0;
		memset(cmd, 0x0, sizeof(cmd));

		seq = ARDrone2_incrAtCmdSeq();

		sprintf(cmd, "AT*PCMD=%d,%d,%d,%d,%d,%d\r", seq, opt,
				*(int*) &CurAttitude.roll, *(int*) &CurAttitude.pitch,
				*(int*) &CurAttitude.vSpeed, *(int*) &CurAttitude.ySpeed);

		OS_printf("PCMD command: %s\n", cmd);
		ARDrone2_SendATCommand(cmd);

		// Reduce the attitude angles for the next iteration
		CurAttitude.roll 		= ARDrone2_Reduce_Angle(CurAttitude.roll);
		CurAttitude.pitch 		= ARDrone2_Reduce_Angle(CurAttitude.pitch);
		CurAttitude.vSpeed 		= ARDrone2_Reduce_Angle(CurAttitude.vSpeed);
		CurAttitude.ySpeed		= ARDrone2_Reduce_Angle(CurAttitude.ySpeed);

		// If drone WAS tilted but this made it level, it will need to loop 1 more time to finish leveling out
		if(CurAttitude.isTilted == 0){
					OS_printf("Drone has leveled out: roll=%f, Pitch=%f, vSpeedr=%f, ySpeed=%f\n",
							CurAttitude.roll, CurAttitude.pitch,
							CurAttitude.vSpeed, CurAttitude.ySpeed);
		}
	}
}


/*!
 * @brief  Set the CurAttitude.isTilted flag based on other fields in CurAttitude
 * @return new flag status
 */
int ARDrone2_Update_Attitude_Flag(void)
{
	if(CurAttitude.roll || CurAttitude.pitch || CurAttitude.vSpeed || CurAttitude.ySpeed){
		// If any of the target attitudes still exist, we need to keep tilting back to level
		CurAttitude.isTilted = 1;
	} else {
		CurAttitude.isTilted = 0;
	}
	return CurAttitude.isTilted;
}


/*!
 * @brief  Gently reduce angle back towards zero
 * @param  angle from -1 to 1
 * @return Float angle
 */
float ARDrone2_Reduce_Angle(float angle)
{
	if(angle > 0)
	{
		angle -= ARDRONE2_DRIFT_AMOUNT;
		if(angle < ARDRONE2_DRIFT_AMOUNT)
		{
			return 0;
		}
	}else{
		angle += ARDRONE2_DRIFT_AMOUNT;
		if(angle > ARDRONE2_DRIFT_AMOUNT)
		{
			return 0;
		}
	}
	return angle;
}


/*!
 * @brief  Make drone yaw to set angle (deg) from North (+ Est, - West)
 * @param  yawAngle yaw degrees, range -180 to 180 with  0:North
 * @param  yawAcu   accuracy of the yaw angle, range 0 to 1
 * @return Void.
 */
void ARDrone2_yawAbs(float yawAngle, float yawAcu)
{
    int opt = 5;       // Set to consider other params and abs. ySpeed 
    float roll = 0;    // Roll
    float pitch = 0;   // Pitch
    float vSpeed = 0;  // Vertical speed
    float ySpeed = 0;  // Yaw speed
    float yaw = yawAngle / 180.0;

    /* Out of range > 100% of ySpeed angle (0 = North, -1/1 = South)
       yawAcu in degrees. Must be positive */
    if(yaw > 1 || yaw < -1 || yawAcu < 0)
    {
        return;                              
    }

    ARDrone2_move_mag(opt, roll, pitch, vSpeed, ySpeed, yaw, yawAcu);
}


/*!
 * @brief  General ARDrone function to move with absolute magnetometer
 * @param  opt
 * @param  roll
 * @param  pitch
 * @param  vSpeed
 * @param  ySpeed
 * @param  yaw
 * @param  yawAcu
 * @return Void.
 */
void ARDrone2_move_mag(int opt, float roll, float pitch, float vSpeed, 
                       float ySpeed, float yaw, float yawAcu)
{
    char cmd[100];
    int seq = 0;
    memset(cmd, 0x0, sizeof(cmd));

    seq = ARDrone2_incrAtCmdSeq();

    sprintf(cmd, "AT*PCMD_MAG=%d,%d,%d,%d,%d,%d,%d,%d\r", seq, opt, 
            *(int*) &roll, *(int*) &pitch, *(int*) &vSpeed, *(int*) &ySpeed,
            *(int*) &yaw, *(int*) &yawAcu);

    OS_printf("Sending moving command: %s\n", cmd);
    ARDrone2_SendATCommand(cmd);
}

/*!
 * @brief  ARDrone2 In-flight magnetometer calibration.
 * @note   Must be done in-flight ONLY
 * @return Void.
 */
void ARDrone2_magCal(void) 
{
    char cmd[100];
    int seq;
    int magDeviceID = 0; /* magnetometer is 0 */

    /*
     * Calibrate the magnetometer by hovering inflight, then
     * spinning around.  This MUST be called in-flight.
     */
    ARDrone2_hover();

    OS_TaskDelay(30);

    memset(cmd, 0x0, sizeof(cmd));
    seq = ARDrone2_incrAtCmdSeq();
    sprintf(cmd, "AT*CALIB=%d,%d\r", seq, magDeviceID);
    ARDrone2_SendATCommand(cmd);
}


/*!
 * @brief  ARDrone2 LED animation sequence
 * @param  animation Pre-defined LED animation, for colors, duration, and location, range (0-20)
 * @param  duration  Total duration in seconds of the animation
 * @param  freq      Frequency of the animation
 * @return Void.
 */
void ARDrone2_LEDAnimate(int animation, int duration, float freq)
{
    int seq = ARDrone2_incrAtCmdSeq();

    char cmd[100];
    memset(cmd, 0x0, sizeof(cmd));

    sprintf(cmd, "AT*LED=%d,%d,%d,%d\r", seq, animation, *(int*) &freq, duration);
    ARDrone2_SendATCommand(cmd);
}

/*!
 * @brief  ARDrone2 execute predefined movement - animation
 * @param  animation  animation to play, range (0-15)
 * @param  duration   total duration in milliseconds of the animation
 * @return Void.
 */
void ARDrone2_moveAnimate(int animation, int duration)
{
    int seq = ARDrone2_incrAtCmdSeq();
    
    char cmd[100];
    memset(cmd, 0x0, sizeof(cmd));

    sprintf(cmd, "AT*ANIM=%d,%d,%d\r", seq, animation, duration); 
    ARDrone2_SendATCommand(cmd);
}


// PRIVATE FUNCTIONS:

/*!
 * @brief  Send AT command directly to ARDrone
 * @param  command_string  pointer to actual command sent to ARDrone
 * @note   Private function
 * @return Void.
 */
void ARDrone2_SendATCommand(char *command_string)
{
	sendto(ATCmdSocketID,
           command_string, strlen(command_string), 0,
		   (struct sockaddr *) &CmdARDroneAddress, sizeof(CmdARDroneAddress) );
}

/*!
 * @brief  Send Watchdog command
 * @return Void.
 */
void ARDrone2_cmdComWdg(void)
{
    int seq = 0;
    char cmd[100];
    memset(cmd, 0x0, sizeof(cmd));
    seq = ARDrone2_incrAtCmdSeq();
	
    sprintf(cmd, "AT*COMWDG=%u\r", seq);
    ARDrone2_SendATCommand(cmd);
}

/*!
 * @brief  Reset the Cmd sequence
 * @return Void.
 */
void ARDrone2_resetCmdSeq(void)
{
    OS_MutSemTake(g_ARDrone2_AppData.CmdSeqMutexId);
    CmdSequence = ARDRONE2_NAVDATA_SEQUENCE_DEFAULT-1;
    OS_MutSemGive(g_ARDrone2_AppData.CmdSeqMutexId);
}


/*!
 * @brief  Increment the Cmd sequence that goes with every command string
 * @return Void.
 */
int ARDrone2_incrAtCmdSeq(void)
{
   int seq;

   OS_MutSemTake(g_ARDrone2_AppData.CmdSeqMutexId);
   CmdSequence++;
   seq = CmdSequence;
   OS_MutSemGive(g_ARDrone2_AppData.CmdSeqMutexId);

   return seq;
}

/*=======================================================================================
** End of file ardron2_cmds.c
**=====================================================================================*/

