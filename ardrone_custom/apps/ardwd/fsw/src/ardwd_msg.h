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
** File Name:  ardwd_msg.h
**
** Title:  Message Definition Header File for ARDWD Application
**
**
** Purpose:  To define ARDWD's command and telemetry message definitions
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2016-05-05 | Michael Rosburg | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _ARDWD_MSG_H_
#define _ARDWD_MSG_H_

/*
** Pragmas
*/

/*
** Include Files
*/



/*
** Local Defines
*/

/*
** ARDWD command codes
*/
#define ARDWD_NOOP_CC                 0
#define ARDWD_RESET_CC                1

/*
** Local Structure Declarations
*/
typedef struct
{
    uint32  counter;

    /* TODO:  Add input data to this application here, such as raw data read from I/O
    **        devices or data subscribed from other apps' output data.
    */

} ARDWD_InData_t;

/*
** Local Structure Declarations
*/
typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
} ARDWD_NoArgCmd_t;

typedef struct
{
    uint8   ucTlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint16  usCmdCnt;
    uint16  usCmdErrCnt;
} ARDWD_HkTlm_t;

typedef struct
{
    uint8   ucTlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint32  uiCounter;
} ARDWD_OutData_t;


#endif /* _ARDWD_MSG_H_ */

/*=======================================================================================
** End of file ardwd_msg.h
**=====================================================================================*/
    
