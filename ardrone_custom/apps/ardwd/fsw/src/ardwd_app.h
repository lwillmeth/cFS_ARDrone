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
** File Name:  ardwd_app.h
**
** Title:  Header File for ARDWD Application
**
**
** Purpose:  To define ARDWD's internal macros, data types, global variables and
**           function prototypes
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2016-05-05 | Michael Rosburg | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _ARDWD_APP_H_
#define _ARDWD_APP_H_

/*
** Pragmas
*/

/*
** Include Files
*/
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "ardwd_platform_cfg.h"
#include "ardwd_mission_cfg.h"
#include "ardwd_private_ids.h"
#include "ardwd_private_types.h"
#include "ardwd_perfids.h"
#include "ardwd_msgids.h"
#include "ardwd_msg.h"

#include "ci_lab_msg.h"
#include "ci_lab_msgids.h"
#include "ardrone2_cmds.h"
#include "ardrone2_msgids.h"

/*
** Local Defines
*/

#define ARDWD_TIMEOUT         10
#define ARDWD_CMD_DELAY_TIME  100  // 100ms
#define ARDWD_TIMEOUT_MSEC    1000 // Wait startup sync

/*
** Local Structure Declarations
*/
typedef struct
{
    /* CFE Event table */
    CFE_EVS_BinFilter_t  EventTbl[ARDWD_EVT_CNT];

    /* CFE scheduling pipe */
    CFE_SB_PipeId_t  SchPipeId; 
    uint16           usSchPipeDepth;
    char             cSchPipeName[OS_MAX_API_NAME];

    /* CFE command pipe */
    CFE_SB_PipeId_t  CmdPipeId;
    uint16           usCmdPipeDepth;
    char             cCmdPipeName[OS_MAX_API_NAME];
    
    /* CFE telemetry pipe */
    CFE_SB_PipeId_t  TlmPipeId;
    uint16           usTlmPipeDepth;
    char             cTlmPipeName[OS_MAX_API_NAME];

    /* Time when ARDWD receives information regarding command ingest */
    CFE_TIME_SysTime_t cmdIngestUpdateTime;

    /* Current Time from CFE_TIME_GetTime() */
    CFE_TIME_SysTime_t currentTime;

    /* Last pass value for CI_LAB IngestPackets */
    uint32 lastPassValueOfIngestPackets;

    /* Delta time since a command was received by CI_LAB */
    CFE_TIME_SysTime_t lastCmdDeltaTime;

    /* Task-related */
    uint32  uiRunStatus;
    
    /* Flag indicating if CI_LAB Reset was commanded */
    boolean ciReset;

    /* Input data - from I/O devices or subscribed from other apps' output data.
       Data structure should be defined in ardwd/fsw/src/ardwd_private_types.h */
    ARDWD_InData_t   InData;

    /* Output data - to be published at the end of a Wakeup cycle.
       Data structure should be defined in ardwd/fsw/src/ardwd_private_types.h */
    ARDWD_OutData_t  OutData;

    /* Housekeeping telemetry - for downlink only.
       Data structure should be defined in ardwd/fsw/src/ardwd_msg.h */
    ARDWD_HkTlm_t  HkTlm;

    /* TODO:  Add declarations for additional private data here */
} ARDWD_AppData_t;

/*
** External Global Variables
*/

/*
** Global Variables
*/

/*
** Local Variables
*/

/*
** Local Function Prototypes
*/
int32  ARDWD_InitApp(void);
int32  ARDWD_InitEvent(void);
int32  ARDWD_InitData(void);
int32  ARDWD_InitPipe(void);

void  ARDWD_AppMain(void);

void  ARDWD_CleanupCallback(void);

int32  ARDWD_RcvMsg(int32 iBlocking);

void  ARDWD_ProcessNewData(void);
void  ARDWD_ProcessNewCmds(void);
void  ARDWD_ProcessNewAppCmds(CFE_SB_Msg_t*);
void  ARDWD_ProcessCItelemData(CFE_SB_Msg_t*);

void  ARDWD_ReportHousekeeping(void);
void  ARDWD_SendOutData(void);

boolean  ARDWD_VerifyCmdLength(CFE_SB_Msg_t*, uint16);

#endif /* _ARDWD_APP_H_ */

/*=======================================================================================
** End of file ardwd_app.h
**=====================================================================================*/
    
