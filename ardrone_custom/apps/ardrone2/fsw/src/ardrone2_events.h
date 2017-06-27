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
** File Name:  ardrone2_events.h
**
** Title:  Event ID Header File for ardrone2 Application
**
**
** Purpose:  This file contains the definitions of all of the events send by the ardrone2 application
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2012-11-18 | Mathew Benson | Build #: Code Started
**
**=====================================================================================*/
#ifndef _EVENTS_H_
#define _EVENTS_H_

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
#define RESERVED_EID                0
#define INF_EID                     1
#define INIT_INF_EID                2
#define ILOAD_INF_EID               3
#define CDS_INF_EID                 4
#define CMD_INF_EID                 5

#define STATE_CHANGE_EID            6
#define TELEM_SOCKET_READ_ERR_EID   7
#define TELEM_CHECKSUM_ERR_EID      8
#define TELEM_SEQUENCE_ERR_EID      9
#define TELEM_TASK_ERR_EID          10
#define CMD_SOCKETCREATE_ERR_EID    11
#define ARDRONE2_INIT_ERR_EID       12

#define ERR_EID                     51
#define INIT_ERR_EID                52
#define ILOAD_ERR_EID               53
#define CDS_ERR_EID                 54
#define CMD_ERR_EID                 55
#define PIPE_ERR_EID                56
#define MSGID_ERR_EID               57
#define MSGLEN_ERR_EID              58
#define ACQ_PTR_ERR_EID             59
#define CAL_TBL_REG_ERR_EID         60
#define CAL_TBL_LOAD_ERR_EID        61
#define CAL_TBL_MNG_ERR_EID         62
#define CAL_TBL_GADR_ERR_EID        63

#define TTYPA2_OPEN_ERR_EID         64
#define FCNTL_ERR_EID               65
#define TCGEETATTR_ERR_EID          66
#define CTSETISPEED_ERR_EID         67
#define CTSETOSPEED_ERR_EID         68
#define TCSETATTR_ERR_EID           69
#define GPIOSET_ERR_EID             70
#define TTYPA2_WRITE_ERR_EID        71
#define SOCKETCREATE_ERR_EID        72
#define MSG_SEND_ERR_EID            73

#define EVT_CNT  36

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

#endif /* _EVENTS_H_ */

/*=======================================================================================
** End of file events.h
**=====================================================================================*/
    
