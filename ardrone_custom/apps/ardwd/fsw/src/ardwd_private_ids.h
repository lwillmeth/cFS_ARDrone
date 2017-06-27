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
** File Name:  ardwd_private_ids.h
**
** Title:  ID Header File for ARDWD Application
**
**
** Purpose:  This header file contains declarations and definitions of ARDWD's private IDs.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2016-05-05 | Michael Rosburg | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _ARDWD_PRIVATE_IDS_H_
#define _ARDWD_PRIVATE_IDS_H_

/*
** Pragmas
*/

/*
** Include Files
*/

/*
** Local Defines
*/

/* Event IDs */
#define ARDWD_RESERVED_EID  0

#define ARDWD_INF_EID        1
#define ARDWD_INIT_INF_EID   2
#define ARDWD_ILOAD_INF_EID  3
#define ARDWD_CDS_INF_EID    4
#define ARDWD_CMD_INF_EID    5

#define ARDWD_ERR_EID         51
#define ARDWD_INIT_ERR_EID    52
#define ARDWD_ILOAD_ERR_EID   53
#define ARDWD_CDS_ERR_EID     54
#define ARDWD_CMD_ERR_EID     55
#define ARDWD_PIPE_ERR_EID    56
#define ARDWD_MSGID_ERR_EID   57
#define ARDWD_MSGLEN_ERR_EID  58

#define ARDWD_EVT_CNT  14

/*
** Local Structure Declarations
*/

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

#endif /* _ARDWD_PRIVATE_IDS_H_ */

/*=======================================================================================
** End of file ardwd_private_ids.h
**=====================================================================================*/
    
