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
** File Name:  ardwd_app.c
**
** Title:  Function Definitions for ARDWD Application
**
**
** Purpose:  This source file contains all necessary function definitions to run ARDWD
**           application.
**
** Functions Defined:
** ARDWD_InitApp()
** ARDWD_InitEvent()
** ARDWD_InitData()
** ARDWD_InitPipe()
** ARDWD_AppMain()
** ARDWD_CleanupCallback()
** ARDWD_RcvMsg()
** ARDWD_ProcessNewData()
** ARDWD_ProcessNewCmds()
** ARDWD_ProcessNewAppCmds()
** ARDWD_ProcessCItelemData()
** ARDWD_ReportHousekeeping()
** ARDWD_SendOutData(void)
** ARDWD_VerifyCmdLength()
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to all functions in the file.
**    2. List the external source(s) and event(s) that can cause the funcs in this
**       file to execute.
**    3. List known limitations that apply to the funcs in this file.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2016-05-05 | Michael Rosburg | Build #: Code Started
**
**=====================================================================================*/

/*
** Pragmas
*/

/*
** Include Files
*/
#include <string.h>

#include "cfe.h"

#include "ardwd_platform_cfg.h"
#include "ardwd_mission_cfg.h"
#include "ardwd_app.h"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/

/*
** Global Variables
*/
ARDWD_AppData_t  g_ARDWD_AppData;

/*
** Local Variables
*/

/*
** Local Function Definitions
*/
int32 ARDWD_SendGenCmd(int32 msgId, int32 cmdCode);
    
/*=====================================================================================
** Name: ARDWD_InitEvent
**
** Purpose: To initialize and register event table for ARDWD application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_EVS_Register
**    CFE_ES_WriteToSysLog
**
** Called By:
**    ARDWD_InitApp
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    g_ARDWD_AppData.EventTbl
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Your Name 
**
** History:  Date Written  2016-05-05
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 ARDWD_InitEvent()
{
    int32  iStatus=CFE_SUCCESS;

    /* Create the event table */
    memset((void*)g_ARDWD_AppData.EventTbl, 0x00, sizeof(g_ARDWD_AppData.EventTbl));

    g_ARDWD_AppData.EventTbl[0].EventID = ARDWD_RESERVED_EID;
    g_ARDWD_AppData.EventTbl[1].EventID = ARDWD_INF_EID;
    g_ARDWD_AppData.EventTbl[2].EventID = ARDWD_INIT_INF_EID;
    g_ARDWD_AppData.EventTbl[3].EventID = ARDWD_ILOAD_INF_EID;
    g_ARDWD_AppData.EventTbl[4].EventID = ARDWD_CDS_INF_EID;
    g_ARDWD_AppData.EventTbl[5].EventID = ARDWD_CMD_INF_EID;

    g_ARDWD_AppData.EventTbl[ 6].EventID = ARDWD_ERR_EID;
    g_ARDWD_AppData.EventTbl[ 7].EventID = ARDWD_INIT_ERR_EID;
    g_ARDWD_AppData.EventTbl[ 8].EventID = ARDWD_ILOAD_ERR_EID;
    g_ARDWD_AppData.EventTbl[ 9].EventID = ARDWD_CDS_ERR_EID;
    g_ARDWD_AppData.EventTbl[10].EventID = ARDWD_CMD_ERR_EID;
    g_ARDWD_AppData.EventTbl[11].EventID = ARDWD_PIPE_ERR_EID;
    g_ARDWD_AppData.EventTbl[12].EventID = ARDWD_MSGID_ERR_EID;
    g_ARDWD_AppData.EventTbl[13].EventID = ARDWD_MSGLEN_ERR_EID;

    /* Register the table with CFE */
    iStatus = CFE_EVS_Register(g_ARDWD_AppData.EventTbl,
                               ARDWD_EVT_CNT, CFE_EVS_BINARY_FILTER);
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("ARDWD - Failed to register with EVS (0x%08X)\n", iStatus);
    }

    return (iStatus);
}
    
/*=====================================================================================
** Name: ARDWD_InitPipe
**
** Purpose: To initialize all message pipes and subscribe to messages for ARDWD application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_SB_CreatePipe
**    CFE_SB_Subscribe
**    CFE_ES_WriteToSysLog
**
** Called By:
**    ARDWD_InitApp
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    g_ARDWD_AppData.usSchPipeDepth
**    g_ARDWD_AppData.cSchPipeName
**    g_ARDWD_AppData.SchPipeId
**    g_ARDWD_AppData.usCmdPipeDepth
**    g_ARDWD_AppData.cCmdPipeName
**    g_ARDWD_AppData.CmdPipeId
**    g_ARDWD_AppData.usTlmPipeDepth
**    g_ARDWD_AppData.cTlmPipeName
**    g_ARDWD_AppData.TlmPipeId
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Your Name 
**
** History:  Date Written  2016-05-05
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 ARDWD_InitPipe()
{
    int32  iStatus=CFE_SUCCESS;

    /* Init schedule pipe */
    g_ARDWD_AppData.usSchPipeDepth = ARDWD_SCH_PIPE_DEPTH;
    memset((void*)g_ARDWD_AppData.cSchPipeName, '\0', sizeof(g_ARDWD_AppData.cSchPipeName));
    strncpy(g_ARDWD_AppData.cSchPipeName, "ARDWD_SCH_PIPE", OS_MAX_API_NAME-1);

    /* Subscribe to Wakeup messages */
    iStatus = CFE_SB_CreatePipe(&g_ARDWD_AppData.SchPipeId,
                                 g_ARDWD_AppData.usSchPipeDepth,
                                 g_ARDWD_AppData.cSchPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        iStatus = CFE_SB_SubscribeEx(ARDWD_WAKEUP_MID,
        		g_ARDWD_AppData.SchPipeId, CFE_SB_Default_Qos, 1);

        if (iStatus != CFE_SUCCESS)
        {
        	CFE_ES_WriteToSysLog("ARDWD - Sch Pipe failed to subscribe to "
        			"ARDWD_WAKEUP_MID wakeup. (0x%08X)\n", iStatus);
        	goto ARDWD_InitPipe_Exit_Tag;
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("ARDWD - Failed to create SCH pipe (0x%08X)\n", iStatus);
        goto ARDWD_InitPipe_Exit_Tag;
    }

    /* Init command pipe */
    g_ARDWD_AppData.usCmdPipeDepth = ARDWD_CMD_PIPE_DEPTH;
    memset((void*)g_ARDWD_AppData.cCmdPipeName, '\0', sizeof(g_ARDWD_AppData.cCmdPipeName));
    strncpy(g_ARDWD_AppData.cCmdPipeName, "ARDWD_CMD_PIPE", OS_MAX_API_NAME-1);

    /* Subscribe to command messages */
    iStatus = CFE_SB_CreatePipe(&g_ARDWD_AppData.CmdPipeId,
                                 g_ARDWD_AppData.usCmdPipeDepth,
                                 g_ARDWD_AppData.cCmdPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        /* Subscribe to command messages */
        iStatus = CFE_SB_Subscribe(ARDWD_CMD_MID, g_ARDWD_AppData.CmdPipeId);

        if(iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("ARDWD - CMD Pipe failed to subscribe to "
                                 "ARDWD_CMD_MID. (0x%08X)\n",
                                 iStatus);
            goto ARDWD_InitPipe_Exit_Tag;
        }

        iStatus = CFE_SB_Subscribe(ARDWD_SEND_HK_MID, g_ARDWD_AppData.CmdPipeId);

        if(iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("ARDWD - CMD Pipe failed to subscribe to "
                                 "ARDWD_SEND_HK_MID. (0x%08X)\n",
                                 iStatus);
            goto ARDWD_InitPipe_Exit_Tag;
        } 
    }
    else
    {
        CFE_ES_WriteToSysLog("ARDWD - Failed to create CMD pipe (0x%08X)\n", iStatus);
        goto ARDWD_InitPipe_Exit_Tag;
    }

    /* Init telemetry pipe */
    g_ARDWD_AppData.usTlmPipeDepth = ARDWD_TLM_PIPE_DEPTH;
    memset((void*)g_ARDWD_AppData.cTlmPipeName, '\0', sizeof(g_ARDWD_AppData.cTlmPipeName));
    strncpy(g_ARDWD_AppData.cTlmPipeName, "ARDWD_TLM_PIPE", OS_MAX_API_NAME-1);

    /* Subscribe to telemetry messages on the telemetry pipe */
    iStatus = CFE_SB_CreatePipe(&g_ARDWD_AppData.TlmPipeId,
                                 g_ARDWD_AppData.usTlmPipeDepth,
                                 g_ARDWD_AppData.cTlmPipeName);
    if (iStatus == CFE_SUCCESS)
    {
    	/* Subscribe to CI_LAB_HK_TLM_MID telem for CI_LAB app, so that
    	   a check can be made against counter in its telem */
    	iStatus = CFE_SB_Subscribe(CI_LAB_HK_TLM_MID, g_ARDWD_AppData.TlmPipeId);
    }
    else
    {
        CFE_ES_WriteToSysLog("ARDWD - Failed to create TLM pipe (0x%08X)\n", iStatus);
        goto ARDWD_InitPipe_Exit_Tag;
    }

ARDWD_InitPipe_Exit_Tag:
    return (iStatus);
}
    
/*=====================================================================================
** Name: ARDWD_InitData
**
** Purpose: To initialize global variables used by ARDWD application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_SB_InitMsg
**
** Called By:
**    ARDWD_InitApp
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    g_ARDWD_AppData.InData
**    g_ARDWD_AppData.OutData
**    g_ARDWD_AppData.HkTlm
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Your Name 
**
** History:  Date Written  2016-05-05
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 ARDWD_InitData()
{
    int32  iStatus=CFE_SUCCESS;

    /* Init input data */
    memset((void*)&g_ARDWD_AppData.InData, 0x00, sizeof(g_ARDWD_AppData.InData));

    /* Init housekeeping packet */
    memset((void*)&g_ARDWD_AppData.HkTlm, 0x00, sizeof(g_ARDWD_AppData.HkTlm));
    CFE_SB_InitMsg(&g_ARDWD_AppData.HkTlm,
                   ARDWD_HK_TLM_MID, sizeof(g_ARDWD_AppData.HkTlm), TRUE);

    g_ARDWD_AppData.lastPassValueOfIngestPackets = 0;

    return (iStatus);
}
    
/*=====================================================================================
** Name: ARDWD_InitApp
**
** Purpose: To initialize all data local to and used by ARDWD application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_ES_RegisterApp
**    CFE_ES_WriteToSysLog
**    CFE_EVS_SendEvent
**    OS_TaskInstallDeleteHandler
**    ARDWD_InitEvent
**    ARDWD_InitPipe
**    ARDWD_InitData
**
** Called By:
**    ARDWD_AppMain
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Your Name 
**
** History:  Date Written  2016-05-05
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 ARDWD_InitApp()
{
    int32  iStatus=CFE_SUCCESS;

    g_ARDWD_AppData.uiRunStatus = CFE_ES_APP_RUN;

    iStatus = CFE_ES_RegisterApp();
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("ARDWD - Failed to register the app (0x%08X)\n", iStatus);
        goto ARDWD_InitApp_Exit_Tag;
    }

    if ((ARDWD_InitEvent() != CFE_SUCCESS) || 
        (ARDWD_InitPipe() != CFE_SUCCESS) || 
        (ARDWD_InitData() != CFE_SUCCESS))
    {
        iStatus = -1;
        goto ARDWD_InitApp_Exit_Tag;
    }

    /* Install the cleanup callback */
    OS_TaskInstallDeleteHandler((void*)&ARDWD_CleanupCallback);

ARDWD_InitApp_Exit_Tag:
    if (iStatus == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(ARDWD_INIT_INF_EID, CFE_EVS_INFORMATION,
                          "ARDWD - Application initialized");
    }
    else
    {
        CFE_ES_WriteToSysLog("ARDWD - Application failed to initialize\n");
    }

    return (iStatus);
}
    
/*=====================================================================================
** Name: ARDWD_CleanupCallback
**
** Purpose: To handle any neccesary cleanup prior to application exit
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    TBD
**
** Called By:
**    TBD
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Your Name 
**
** History:  Date Written  2016-05-05
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void ARDWD_CleanupCallback()
{
    /* TODO:  Add code to cleanup memory and other cleanup here */
}
    
/*=====================================================================================
** Name: ARDWD_RcvMsg
**
** Purpose: To receive and process messages for ARDWD application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization 
**
** Routines Called:
**    CFE_SB_RcvMsg
**    CFE_SB_GetMsgId
**    CFE_EVS_SendEvent
**    ARDWD_ProcessNewCmds
**    ARDWD_ProcessNewData
**    ARDWD_SendOutData
**    ARDWD_ReportHousekeeping
**
** Called By:
**    ARDWD_Main
**
** Global Inputs/Reads:
**    g_ARDWD_AppData.SchPipeId
**
** Global Outputs/Writes:
**    g_ARDWD_AppData.uiRunStatus
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Your Name 
**
** History:  Date Written  2016-05-05
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 ARDWD_RcvMsg(int32 iBlocking)
{
    int32           iStatus=CFE_SUCCESS;
    CFE_SB_Msg_t*   MsgPtr=NULL;
    CFE_SB_MsgId_t  MsgId;

    /* Wait for WakeUp messages from scheduler */
    CFE_ES_PerfLogExit(ARDWD_MAIN_TASK_PERF_ID);
 
    iStatus = CFE_SB_RcvMsg(&MsgPtr, g_ARDWD_AppData.SchPipeId, iBlocking);

    CFE_ES_PerfLogEntry(ARDWD_MAIN_TASK_PERF_ID);

    if (iStatus == CFE_SUCCESS)
    {
        MsgId = CFE_SB_GetMsgId(MsgPtr);
        switch (MsgId)
	{
            case ARDWD_WAKEUP_MID:
                ARDWD_ProcessNewCmds();
                ARDWD_ProcessNewData();

                /* TODO:  Add more code here to handle other things when app wakes up */

                /* The last thing to do at the end of this Wakeup cycle should be to
                   automatically publish new output. */
                /* We commented this call because our application is not publishing 
                   new output other than housekeeping.  That housekeeping data is published
                   in a different function.  If your function had other data to be published
                   for use by other apps then you could send out here */
                //ARDWD_SendOutData();
                break;

            default:
                CFE_EVS_SendEvent(ARDWD_MSGID_ERR_EID, CFE_EVS_ERROR,
                                  "ARDWD - Recvd invalid SCH msgId (0x%08X)", MsgId);
        }
    }
    else if (iStatus == CFE_SB_NO_MESSAGE)
    {
        /* If there's no incoming message, you can do something here, or nothing */
    }
    else
    {
        /* This is an example of exiting on an error.
        ** Note that a SB read error is not always going to result in an app quitting.
        */
        CFE_EVS_SendEvent(ARDWD_PIPE_ERR_EID, CFE_EVS_ERROR,
			  "ARDWD: SB pipe read error (0x%08X), app will exit", iStatus);
        g_ARDWD_AppData.uiRunStatus= CFE_ES_APP_ERROR;
    }

    return (iStatus);
}
    
/*=====================================================================================
** Name: ARDWD_ProcessNewData
**
** Purpose: To process incoming data subscribed by ARDWD application
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_SB_RcvMsg
**    CFE_SB_GetMsgId
**    CFE_EVS_SendEvent
**
** Called By:
**    ARDWD_RcvMsg
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    None
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Your Name 
**
** History:  Date Written  2016-05-05
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void ARDWD_ProcessNewData()
{
    int iStatus = CFE_SUCCESS;
    CFE_SB_Msg_t*   TlmMsgPtr=NULL;
    CFE_SB_MsgId_t  TlmMsgId;

    /* Process telemetry messages till the pipe is empty */
    while (1)
    {
        iStatus = CFE_SB_RcvMsg(&TlmMsgPtr, g_ARDWD_AppData.TlmPipeId, CFE_SB_POLL);
        if(iStatus == CFE_SUCCESS)
        {
            TlmMsgId = CFE_SB_GetMsgId(TlmMsgPtr);
            switch (TlmMsgId)
            {
                case CI_LAB_HK_TLM_MID:
            	   ARDWD_ProcessCItelemData(TlmMsgPtr);
            	   break;

                default:
                    CFE_EVS_SendEvent(ARDWD_MSGID_ERR_EID, CFE_EVS_ERROR,
                                      "ARDWD - Recvd invalid TLM msgId (0x%08X)", TlmMsgId);
                    break;
            }
        }
        else if (iStatus == CFE_SB_NO_MESSAGE)
        {
            break;
        }
        else
        {
            CFE_EVS_SendEvent(ARDWD_PIPE_ERR_EID, CFE_EVS_ERROR,
                  "ARDWD: CMD pipe read error (0x%08X)", iStatus);
            g_ARDWD_AppData.uiRunStatus = CFE_ES_APP_ERROR;
            break;
        }
    }
}

/*=====================================================================================
** Name: ARDWD_ProcessCItelemData
**
** Purpose: To process incoming CI_LAB Housekeeping Telemetry subscribed by ARDWD application
**
** Arguments:
**    CFE_SB_Msg_t* TlmMsgPtr - Generic Software Bus Message Type Definition
**
** Returns:
**    None
**
** Routines Called:
**    ARDWD_SendGenCmd()
**    CFE_EVS_SendEvent()
**    CFE_TIME_Subtract()
**    CFE_TIME_GetTime()
**
** Called By:
**    ARDWD_ProcessNewTlmData()
**
** Global Inputs/Reads:
**    ci_hk_tlm_t - (CI_Lab housekeeping)
**
** Global Outputs/Writes:
**    None
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Algorithm will check CI_LAB Housekeeping Telemetry for IngestPacket (ground command) updates.
**    If IngestPackets has not changed within 10 seconds of the last IngestPacket
**    the AR.Drone will land.
**
** Author(s):  Nasa
**
** History:  Date Written  2016-03-21
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void ARDWD_ProcessCItelemData(CFE_SB_Msg_t* TlmMsgPtr)
{
	int32 iStatus = CFE_SUCCESS;
	ci_hk_tlm_t       *ciHkTelemMsg;
	uint32             cmdIngestPackets;

    if (TlmMsgPtr != NULL)
    {
    	/* Pointer to CI_LAB Housekeeping data */
    	ciHkTelemMsg = (ci_hk_tlm_t *) TlmMsgPtr;

    	/* Get CI_LAB Housekeeping IngestPackets */
    	cmdIngestPackets = ciHkTelemMsg->IngestPackets;

    	/* Check if a CI_LAB reset has occurred */
    	if (0 < g_ARDWD_AppData.lastPassValueOfIngestPackets && 0 == cmdIngestPackets)
    	{
    		g_ARDWD_AppData.ciReset = TRUE;  /* CI_LAB Reset command sent */
    	}
    	else if (0 != cmdIngestPackets && 0 == g_ARDWD_AppData.lastPassValueOfIngestPackets)
    	{
    		g_ARDWD_AppData.ciReset = FALSE;
    	}

    	/* Don't do any work till we receive a command ingest packet or CI_Lab has been reset */
    	if (0 != cmdIngestPackets || g_ARDWD_AppData.ciReset)
    	{
			/* Check if CI_LAB received any command */
			if (cmdIngestPackets != g_ARDWD_AppData.lastPassValueOfIngestPackets)
			{
			    g_ARDWD_AppData.lastPassValueOfIngestPackets = cmdIngestPackets;
				g_ARDWD_AppData.cmdIngestUpdateTime = CFE_TIME_GetTime();
			}

			/* If CI_LAB reset is True, reset last pass value of ingest packets to 0 */
			if (g_ARDWD_AppData.ciReset)
			{
				g_ARDWD_AppData.lastPassValueOfIngestPackets = 0;
			}

			/* Compute time delta between current time in seconds and last time CI received a command */
			g_ARDWD_AppData.lastCmdDeltaTime = CFE_TIME_Subtract(g_ARDWD_AppData.currentTime,
					g_ARDWD_AppData.cmdIngestUpdateTime);

			/* Check if elapsed time is greater than or equal to 10 since last command. If so, land the drone.
			   The drone will land roughly 10 seconds after last command time if no new commands are received */
			if ((int)g_ARDWD_AppData.lastCmdDeltaTime.Seconds >= ARDWD_TIMEOUT)
			{
				/* Send command to land the drone */
				iStatus = ARDWD_SendGenCmd(ROTWING_GENERIC_CMD_MID, ROTWING_LAND_CC);

				if (iStatus == CFE_SUCCESS)
				{
					CFE_EVS_SendEvent(ARDWD_CMD_INF_EID, CFE_EVS_INFORMATION, "Landing Drone!");
				}
				else
				{
					/* Send an event message to report the invalid command status */
					CFE_EVS_SendEvent(ARDWD_CMD_ERR_EID, CFE_EVS_ERROR, "ARDWD: Command failed ");
				}
			}
    	}
    }
    else
    {
    	CFE_EVS_SendEvent(ARDWD_ERR_EID, CFE_EVS_ERROR, "ARDWD: TlmMsgPtr NULL ");
    }
}
    
/*=====================================================================================
** Name: ARDWD_ProcessNewCmds
**
** Purpose: To process incoming command messages for ARDWD application
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_SB_RcvMsg
**    CFE_SB_GetMsgId
**    CFE_EVS_SendEvent
**    ARDWD_ProcessNewAppCmds
**
** Called By:
**    ARDWD_RcvMsg
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    None
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Your Name 
**
** History:  Date Written  2016-05-05
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void ARDWD_ProcessNewCmds()
{
    CFE_SB_Msg_t*   CmdMsgPtr=NULL;
    CFE_SB_MsgId_t  CmdMsgId;
    boolean         bGotNewMsg=TRUE;

    while (bGotNewMsg)
    {
        if (CFE_SB_RcvMsg(&CmdMsgPtr, g_ARDWD_AppData.CmdPipeId, CFE_SB_POLL) == CFE_SUCCESS)
        {
            CmdMsgId = CFE_SB_GetMsgId(CmdMsgPtr);
            switch (CmdMsgId)
            {
                case ARDWD_CMD_MID:
                    ARDWD_ProcessNewAppCmds(CmdMsgPtr);
                    break;

                case ARDWD_SEND_HK_MID:
                    ARDWD_ReportHousekeeping();
                    break;

                default:
                    CFE_EVS_SendEvent(ARDWD_MSGID_ERR_EID, CFE_EVS_ERROR,
                                      "ARDWD - Recvd invalid CMD msgId (0x%08X)", CmdMsgId);
                    break;
            }
        }
        else
        {
            bGotNewMsg = FALSE;
        }
    }
}
    
/*=====================================================================================
** Name: ARDWD_ProcessNewAppCmds
**
** Purpose: To process command messages targeting ARDWD application
**
** Arguments:
**    CFE_SB_Msg_t*  MsgPtr - new command message pointer
**
** Returns:
**    None
**
** Routines Called:
**    CFE_SB_GetCmdCode
**    CFE_EVS_SendEvent
**
** Called By:
**    ARDWD_ProcessNewCmds
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    g_ARDWD_AppData.HkTlm.usCmdCnt
**    g_ARDWD_AppData.HkTlm.usCmdErrCnt
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Your Name 
**
** History:  Date Written  2016-05-05
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void ARDWD_ProcessNewAppCmds(CFE_SB_Msg_t* MsgPtr)
{
    uint32  uiCmdCode=0;

    if (MsgPtr != NULL)
    {
        uiCmdCode = CFE_SB_GetCmdCode(MsgPtr);
        switch (uiCmdCode)
        {
            case ARDWD_NOOP_CC:
                g_ARDWD_AppData.HkTlm.usCmdCnt++;
                CFE_EVS_SendEvent(ARDWD_CMD_INF_EID, CFE_EVS_INFORMATION,
                                  "ARDWD - Recvd NOOP cmd (%d)", uiCmdCode);
                break;

            case ARDWD_RESET_CC:
                g_ARDWD_AppData.HkTlm.usCmdCnt = 0;
                g_ARDWD_AppData.HkTlm.usCmdErrCnt = 0;
                CFE_EVS_SendEvent(ARDWD_CMD_INF_EID, CFE_EVS_INFORMATION,
                                  "ARDWD - Recvd RESET cmd (%d)", uiCmdCode);
                break;

            /* TODO:  Add code to process the rest of the ARDWD commands here */

            default:
                g_ARDWD_AppData.HkTlm.usCmdErrCnt++;
                CFE_EVS_SendEvent(ARDWD_MSGID_ERR_EID, CFE_EVS_ERROR,
                                  "ARDWD - Recvd invalid cmdId (%d)", uiCmdCode);
                break;
        }
    }
}
    
/*=====================================================================================
** Name: ARDWD_ReportHousekeeping
**
** Purpose: To send housekeeping message
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    TBD
**
** Called By:
**    ARDWD_ProcessNewCmds
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  GSFC, Your Name
**
** History:  Date Written  2016-05-05
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void ARDWD_ReportHousekeeping()
{
    /* TODO:  Add code to update housekeeping data, if needed, here.  */

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&g_ARDWD_AppData.HkTlm);
    CFE_SB_SendMsg((CFE_SB_Msg_t*)&g_ARDWD_AppData.HkTlm);
}
    
/*=====================================================================================
** Name: ARDWD_SendOutData
**
** Purpose: To publish 1-Wakeup cycle output data
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    TBD
**
** Called By:
**    ARDWD_RcvMsg
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Your Name
**
** History:  Date Written  2016-05-05
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void ARDWD_SendOutData()
{
    /* TODO:  Add code to update output data, if needed, here.  */

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&g_ARDWD_AppData.OutData);
    CFE_SB_SendMsg((CFE_SB_Msg_t*)&g_ARDWD_AppData.OutData);
}

/*=====================================================================================
** Name: ARDWD_SendGenCmd
**
** Purpose: To send a general ARDrone2 cmd
**
** Arguments:
**    int32 msgId
**    int32 cmdCode
**
** Returns:
**    iStatus - status of the CFE_SB_SendMsg function return
**
** Routines Called:
**    CFE_SB_InitMsg()
**    CFE_SB_SetCmdCode()
**    CFE_SB_SendMsg
**    OS_TaskDelay()
**
** Called By:
**    ARDWD_ProcessCItelemData
**
** Global Inputs/Reads:
**    CFE_SB_CmdHdr_t
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Pseudo-code or description of basic algorithm
**
** Author(s):  Nasa
**
** History:  Date Written  2016-03-24
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 ARDWD_SendGenCmd(int32 msgId, int32 cmdCode) {
    int32 iStatus = CFE_SUCCESS;
    CFE_SB_CmdHdr_t  cmdMsg;

    CFE_SB_InitMsg((void *) &cmdMsg, msgId, sizeof(cmdMsg), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &cmdMsg, cmdCode);
    iStatus = CFE_SB_SendMsg((CFE_SB_Msg_t *) &cmdMsg);

    OS_TaskDelay(ARDWD_CMD_DELAY_TIME);

    return iStatus;
}
    
/*=====================================================================================
** Name: ARDWD_VerifyCmdLength
**
** Purpose: To verify command length for a particular command message
**
** Arguments:
**    CFE_SB_Msg_t*  MsgPtr      - command message pointer
**    uint16         usExpLength - expected command length
**
** Returns:
**    boolean bResult - result of verification
**
** Routines Called:
**    TBD
**
** Called By:
**    ARDWD_ProcessNewCmds
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Your Name 
**
** History:  Date Written  2016-05-05
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
boolean ARDWD_VerifyCmdLength(CFE_SB_Msg_t* MsgPtr,
                           uint16 usExpectedLen)
{
    boolean bResult=FALSE;
    uint16  usMsgLen=0;

    if (MsgPtr != NULL)
    {
        usMsgLen = CFE_SB_GetTotalMsgLength(MsgPtr);

        if (usExpectedLen != usMsgLen)
        {
            CFE_SB_MsgId_t MsgId = CFE_SB_GetMsgId(MsgPtr);
            uint16 usCmdCode = CFE_SB_GetCmdCode(MsgPtr);

            CFE_EVS_SendEvent(ARDWD_MSGLEN_ERR_EID, CFE_EVS_ERROR,
                              "ARDWD - Rcvd invalid msgLen: msgId=0x%08X, cmdCode=%d, "
                              "msgLen=%d, expectedLen=%d",
                              MsgId, usCmdCode, usMsgLen, usExpectedLen);
            g_ARDWD_AppData.HkTlm.usCmdErrCnt++;
        }
    }

    return (bResult);
}
    
/*=====================================================================================
** Name: ARDWD_AppMain
**
** Purpose: To define ARDWD application's entry point and main process loop
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_ES_RegisterApp
**    CFE_ES_RunLoop
**    CFE_ES_PerfLogEntry
**    CFE_ES_PerfLogExit
**    CFE_ES_ExitApp
**    ARDWD_InitApp
**    ARDWD_RcvMsg
**
** Called By:
**    TBD
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Your Name 
**
** History:  Date Written  2016-05-05
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void ARDWD_AppMain()
{
    /* Register the application with Executive Services */
    CFE_ES_RegisterApp();

    /* Start Performance Log entry */
    CFE_ES_PerfLogEntry(ARDWD_MAIN_TASK_PERF_ID);

    /* Perform application initializations */
    if (ARDWD_InitApp() != CFE_SUCCESS)
    {
        g_ARDWD_AppData.uiRunStatus = CFE_ES_APP_ERROR;
    } else {
        // Don't want to do performance logging on the startup sync
        CFE_ES_PerfLogExit(ARDWD_MAIN_TASK_PERF_ID);
        CFE_ES_WaitForStartupSync(ARDWD_TIMEOUT_MSEC);
        CFE_ES_PerfLogEntry(ARDWD_MAIN_TASK_PERF_ID);
    }

    /* Application main loop */
    while (CFE_ES_RunLoop(&g_ARDWD_AppData.uiRunStatus) == TRUE)
    {
        ARDWD_RcvMsg(CFE_SB_PEND_FOREVER);
        g_ARDWD_AppData.currentTime = CFE_TIME_GetTime();
    }

    /* Stop Performance Log entry */
    CFE_ES_PerfLogExit(ARDWD_MAIN_TASK_PERF_ID);

    /* Exit the application */
    CFE_ES_ExitApp(g_ARDWD_AppData.uiRunStatus);
} 
    
/*=======================================================================================
** End of file ardwd_app.c
**=====================================================================================*/
    
