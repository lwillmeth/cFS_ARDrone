/************************************************************************
**
** $Id: sc_rts005.c 1.9 2016/01/20 13:01:56EST mrosburg Exp  $
**
**  Copyright � 2007-2016 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.
**
** CFS Stored Command (SC) sample RTS table #5
**
** The stored commands in this file are as follows:
**
**   1)  Set Flat Trim
**   2)  Launch
**   3)  Issue Calibrate Magnometer
**   4)  Hover
**   5)  LED Animate "Blink Orange for 15 seconds
**   6)  Land
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

#define CMD1_TIME     0         /* for set flat trim commad */
#define CMD2_TIME     5         /* for launch command */
#define CMD3_TIME     8         /* for calibration mag command */
#define CMD4_TIME     10        /* for the hover command */
#define CMD5_TIME     15        /* for the led animate command */
#define CMD6_TIME     30        /* for the land command */

/*
** Optional command data values
*/
#define CMD5_ARG1      0x0004
#define CMD5_ARG2      0x0000
#define CMD5_ARG3      0x000F
#define CMD5_ARG4      0x0000
#define CMD5_ARG5      0x0002

/*
** Calculate checksum for each sample command
*/
#define CMD1_XSUM     0x0064
#define CMD2_XSUM     0x0000
#define CMD3_XSUM     0x0072
#define CMD4_XSUM     0x0006
#define CMD5_XSUM     0x0068
#define CMD6_XSUM     0x0007


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
#define CMD2_LENGTH   1
#define CMD3_LENGTH   1
#define CMD4_LENGTH   1
#define CMD5_LENGTH   11
#define CMD6_LENGTH   1

/*
** Sample cFE Table Header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "RTS_Table005", "SC.RTS_TBL005", "SC Sample RTS_TBL005",
    "sc_rts005.tbl", (SC_RTS_BUFF_SIZE * sizeof(uint16))
};

/*
** Sample RTS Table Data
*/
uint16 RTS_Table005[SC_RTS_BUFF_SIZE] =
{
  /*  cmd time,  <---------------------------- cmd pkt primary header ---------------------------->  <----- cmd pkt 2nd header ---->   <-- opt data ---> */
     CMD1_TIME,  CFE_MAKE_BIG16(ARDRONE2_PLAT_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD1_LENGTH), ((ARDRONE2_SET_FLAT_TRIM_CC << 8) | CMD1_XSUM),
     CMD2_TIME,  CFE_MAKE_BIG16(ROTWING_GENERIC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD2_LENGTH), ((ROTWING_LAUNCH_CC << 8) | CMD2_XSUM),
     CMD3_TIME,  CFE_MAKE_BIG16(ARDRONE2_PLAT_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD3_LENGTH), ((ARDRONE2_MAG_CAL << 8) | CMD3_XSUM),
     CMD4_TIME,  CFE_MAKE_BIG16(ROTWING_GENERIC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD4_LENGTH), ((ROTWING_HOVER_CC << 8) | CMD4_XSUM),
     CMD5_TIME,  CFE_MAKE_BIG16(ARDRONE2_PLAT_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD5_LENGTH), ((ARDRONE2_LED_ANIM_CC << 8) | CMD5_XSUM), CMD5_ARG1, CMD5_ARG2, CMD5_ARG3, CMD5_ARG4, CMD5_ARG5,
     CMD6_TIME,  CFE_MAKE_BIG16(ROTWING_GENERIC_CMD_MID), CFE_MAKE_BIG16(PKT_FLAGS), CFE_MAKE_BIG16(CMD6_LENGTH), ((ROTWING_LAND_CC << 8) | CMD6_XSUM)

};

/************************/
/*  End of File Comment */
/************************/
