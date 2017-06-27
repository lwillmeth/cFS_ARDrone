/************************************************************************
**
** $Id: sc_rts004.c 1.9 2016/01/20 13:01:56EST mrosburg Exp  $
**
**  Copyright � 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** CFS Stored Command (SC) sample RTS table #4
**
*************************************************************************/

#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_platform_cfg.h"    /* defines table buffer size */
#include "sc_msgdefs.h"         /* defines SC command code values */
#include "sc_msgids.h"          /* defines SC packet msg ID's */

#include "ardrone2_msgids.h"    /* defines ARDRONE2 pack msg ID's */
#include "ardrone2_cmds.h"      /* defines ARDRONE2 command code values */


/*
** Execution time for each sample command
*/
#define CMD1_TIME     0
#define CMD2_TIME     10
#define CMD3_TIME     15

/*
** Optional command data values
*/
#define CMD2_ARG1      0x0003
#define CMD2_ARG2      0x0000
#define CMD2_ARG3      0x000A
#define CMD2_ARG4      0x0000
#define CMD2_ARG5      0x0002


/*
** Calculate checksum for each sample command
*/
#define CMD1_XSUM     0x008F
#define CMD2_XSUM     0x006C


/*
** Command packet segment flags and sequence counter
** - 2 bits of segment flags (0xC000 = start and end of packet)
** - 14 bits of sequence count (unused for command packets)
*/
#define PKT_FLAGS     0xC000

/*
** Length of cmd pkt data (in bytes minus one) that follows primary header (thus, 0xFFFF = 64k)
*/
#define CMD1_LENGTH   1
#define CMD2_LENGTH   13

/*
** Sample cFE Table Header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "RTS_Table004", "SC.RTS_TBL004", "SC Sample RTS_TBL004",
    "sc_rts004.tbl", (SC_RTS_BUFF_SIZE * sizeof(uint16))
};


/*
** Sample RTS Table Data
*/
uint16 RTS_Table004[SC_RTS_BUFF_SIZE] =
{
  /*  cmd time,  <---------------------------- cmd pkt primary header ---------------------------->  <----- cmd pkt 2nd header ---->   <-- opt data ---> */
     CMD2_TIME,  CFE_MAKE_BIG16(ARDRONE2_PLAT_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD2_LENGTH), ((ARDRONE2_LED_ANIM_CC << 8) | CMD2_XSUM), CMD2_ARG1, CMD2_ARG2, CMD2_ARG3, CMD2_ARG4, CMD2_ARG5

};

/************************/
/*  End of File Comment */
/************************/
