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
** File Name:  ardrone2_app.h
**
** Title:  Header File for ARDRONE2 Application.
**
**
** Purpose:  To define ARDRONE2's internal macros, data types, global variables and
**           function prototypes.  The ARDRONE2 application is the driver application
**           to communicate with the drone via sockets, according to the A.R.Drone2
**           SDK 2.0.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2012-11-18 | Mathew Benson | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _ARDRONE2_APP_H_
#define _ARDRONE2_APP_H_

/*
** Pragmas
*/

/*
** Include Files
*/
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "network_includes.h"

#include "ardrone2_platform_cfg.h"
#include "ardrone2_mission_cfg.h"
#include "ardrone2_msgids.h"
#include "ardrone2_perfids.h"
#include "ardrone2_events.h"
#include "ardrone2_cmds.h"
#include "ardrone2_msgs.h"
#include "ardrone2_telem.h"
#include "ardrone2_navdata.h"

/*
** Local Defines
*/
#define ARDRONE2_SCH_PIPE_DEPTH  	10
#define ARDRONE2_CMD_PIPE_DEPTH  	10
#define ARDRONE2_AT_CMD_PIPE_DEPTH  20
#define ARDRONE2_TLM_PIPE_DEPTH  	10

#define ARDRONE2_AT_CMD_PIPE_MAX_PLAT_CMDS       20
#define ARDRONE2_AT_CMD_PIPE_MAX_ROTWING_CMDS    4

#define ARDRONE2_SESSION_ID "1"
#define ARDRONE2_PROFILE_ID "1"
#define ARDRONE2_APPLICATION_ID "1"

#define ARDRONE2_TIMEOUT_MSEC    1000 // Wait startup sync



typedef struct
{
	uint32 prev_update_count;

	ARDrone2_NavData_Demo_Msg_t	msg;
} OS_PACK ARDrone2_NavData_Demo_Sample_t;


typedef struct
{
    /* Child task IDs */
    uint32           ATCommandTaskId;
    uint32           TelemetryTaskId;
    /* These "ShouldRun" booleans are only set by the main app, but read by the
     * child threads. */
    boolean          ATCommandTaskShouldRun;
    boolean          TelemetryTaskShouldRun;

    /* CFE Event table */
    CFE_EVS_BinFilter_t  EventTbl[EVT_CNT];

    /* CFE scheduling pipe */
    CFE_SB_PipeId_t  SchPipeId; 
    uint16           usSchPipeDepth;
    char             cSchPipeName[OS_MAX_API_NAME];

    /* CFE command pipe */
    CFE_SB_PipeId_t  CmdPipeId;
    uint16           usCmdPipeDepth;
    char             cCmdPipeName[OS_MAX_API_NAME];
    
    /* CFE AT command pipe */
    CFE_SB_PipeId_t  ATCmdPipeId;
    uint16           usATCmdPipeDepth;
    char             cATCmdPipeName[OS_MAX_API_NAME];
    
    /* CFE telemetry pipe */
    CFE_SB_PipeId_t  TlmPipeId;
    uint16           usTlmPipeDepth;
    char             cTlmPipeName[OS_MAX_API_NAME];

    /* Task-related */
    uint32  uiRunStatus;

    /* Housekeeping telemetry */
    ARDrone2_HK_Tlm_t    HkTlm;

	CFE_TIME_SysTime_t		TimeStamp;
	CFE_TIME_SysTime_t		TimeSinceLastSample;

    uint32                  NavDataMutexId;
    uint32                  CmdSeqMutexId;
    
    uint32                  AtCmdChildTaskMutexId;
    uint32                  AtTelemChildTaskMutexId;

    uint32                          lastNavDataOptions;
    boolean                         NavDataStale;
	
    ARDrone2_NavData_Demo_Msg_t		NavDataDemoMsg;
    ARDrone2_NavData_Msg_t          NavDataMsg;
    NavData_Unpacked_t              NavDataUnpacked;
    ARDrone2_NavData_Select_Msg_t   NavDataSelectMsg;

} AppData_t;



/*
** External Global Variables
*/


/*
** Global Variables
*/

/*
** Local Variables
*/
int32 ARDrone2_nav_fd;

/*
** Local Function Prototypes
*/
int32 	ARDrone2_InitApp(void);
int32 	ARDrone2_InitEvent(void);
int32 	ARDrone2_InitData(void);
int32 	ARDrone2_InitPipe(void);

void  	ARDrone2_AppMain(void);
void  	ARDrone2_CleanupCallback(void);
int32 	ARDrone2_RcvMsg(int32 iBlocking);

void  	ARDrone2_ProcessNewCmds(void);
void  	ARDrone2_ProcessNewAppCmds(CFE_SB_Msg_t*);

void  	ARDrone2_ReportHousekeeping(void);
int32  	ARDrone2_SendOutData(void);
boolean	ARDrone2_VerifyCmdLength(CFE_SB_Msg_t*, uint16);

#endif /* _ARDRONE2_APP_H_ */

/*=======================================================================================
** End of file ARDRONE2_app.h
**=====================================================================================*/
    
