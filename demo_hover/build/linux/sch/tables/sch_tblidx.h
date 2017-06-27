/*=======================================================================================
** File Name:  sch_tblidx.h
**
** Title:  Table indicies into the CFS scheduler's message table for DRONE mission
**
** Purpose:  This header file contains table indicies into the message table.
**
**=====================================================================================*/

#ifndef _SCH_TBL_IDX_H_
#define _SCH_TBL_IDX_H_

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
**  cFE housekeeping request messages
*/
#define CFE_ES_SEND_HK_MIDX    1
#define CFE_EVS_SEND_HK_MIDX   2
#define CFE_SB_SEND_HK_MIDX    3
#define CFE_TIME_SEND_HK_MIDX  4
#define CFE_TBL_SEND_HK_MIDX   5

/*
**  CFS housekeeping request messages
*/
#define CS_SEND_HK_MIDX      6
#define DS_SEND_HK_MIDX      7
#define FM_SEND_HK_MIDX      8
#define HK_SEND_HK_MIDX      9
#define HS_SEND_HK_MIDX     10
#define LC_SEND_HK_MIDX     11
#define MD_SEND_HK_MIDX     12
#define MM_SEND_HK_MIDX     13
#define SC_SEND_HK_MIDX     14
#define SCH_SEND_HK_MIDX    15

/*
**  CFS routine messages
*/
#define HK_SEND_COMBINED_PKT1_MIDX  16
#define HK_SEND_COMBINED_PKT2_MIDX  17
#define HK_SEND_COMBINED_PKT3_MIDX  18
#define HK_SEND_COMBINED_PKT4_MIDX  19
#define CS_BACKGROUND_CYCLE_MIDX    20
#define SC_1HZ_WAKEUP_MIDX          21
#define LC_SAMPLE_AP_MIDX           22
#define DS_1HZ_WAKEUP_MIDX          23
#define MD_WAKEUP_MIDX              24


#define CI_LAB_SEND_HK_MIDX         30
#define TO_LAB_SEND_HK_MIDX         31
#define ARDRONE2_SEND_HK_MIDX       32
#define ARDRONE2_WAKEUP_MIDX        33


#endif /* _SCH_TBL_IDX_H_ */

/*=======================================================================================
** End of file sch_tblidx.h
**=====================================================================================*/
    
