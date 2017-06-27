/************************************************************************
**
**  Copyright 2016 United States Government as represented by the
**  Administrator of the National Aeronautics and Space Administration.
**  All Other Rights Reserved.
**
**  This software was created at NASA's Johnson Space Center.
**  This software is governed by the NASA Open Source Agreement and may be
**  used, distributed and modified only pursuant to the terms of that
**  agreement.
**
**  elf2cfetbl test: bad symbol in command table
**
**  Author: Allen Brown, Odyssey Space Research, LLC
**
*************************************************************************/

#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_platform_cfg.h"    /* defines table buffer size */
#include "sc_tbldefs.h"

/*****************************
 * Include files (for the app commands)
 *****************************/

/* For SC APP */
#include "sc_msgdefs.h"         /* defines SC command code values */
#include "sc_msgids.h"          /* defines SC packet msg ID's */
#include "sc_msg.h"             /* defines SC command structs */

/* Required items for SC table creation */
#include "ct_sc_table.h"

/* Helpful macros for command definition */
#include "ct_ccsds.h"

/*****************************
 * (end include files)
 *****************************/

/*****************************
 * Table ID and Type Information
 *****************************/
/*
 * Sample ATS_TBL1 Table Header
 * (Required)
 */
static CFE_TBL_FileDef_t CFE_TBL_FileDef RETAIN_SYMBOL = {
		"ATS_Table1", "SC.ATS_TBL1", "SC Sample ATS_TBL1",
		"ut_ats_badcmd.tbl", (SC_ATS_BUFF_SIZE * sizeof(uint16)) };

/*
 * Sample ATS Table Data
 * (Required)
 * (Note, no data will be used from this table, it will be assembled
 *  with information below.)
 */
uint16 ATS_Table1[SC_ATS_BUFF_SIZE] = { };

/* The type of table (used by the post-processor).
 * This is used since the naming convention is under the control of sc_platform_cfg.h.
 * 0=ATS, 1=RTS */
static uint8 SCTableType RETAIN_SYMBOL = 0; /* ATS */

/*****************************
 * (End table ID and type info)
 *****************************/

/*****************************
 * Begin command definitions
 *****************************/

/*
 * SC No-OP Command (a command code only, no payload)
 */
#define CMDONLY_SIZE_BE     CFE_MAKE_BIG16((sizeof(CFE_SB_CmdHdr_t))-7) /* size */

static CFE_SB_CmdHdr_t sc_noop RETAIN_SYMBOL = { {
/* primary message header */
{ INPLACE_BE_HI(CFE_MAKE_BIG16(SC_CMD_MID)),
  INPLACE_BE_LO(CFE_MAKE_BIG16(SC_CMD_MID)) }, /* MID */
{ INPLACE_BE_HI(CMD_FLAGS_BE), INPLACE_BE_LO(CMD_FLAGS_BE) }, /* Seq/Flags */
{ INPLACE_BE_HI(CMDONLY_SIZE_BE), INPLACE_BE_LO(CMDONLY_SIZE_BE) } /* Length */
},
/* secondary command header, native endian */
{ (SC_NOOP_CC << 8) | 0x0000 } /* command code, but no checksum (yet) */
}; /* no payload */


/*****************************
 * (End command definitions)
 *****************************/

/*****************************
 * SC Table Command Content
 *****************************/

/* Set the number of commands to insert in the table here: */
#define NUM_CMDS (1U)

/*
** Arbitrary spacecraft time for start of sample ATS
*/
#define TEST_TIME     1000000


/* The command order and relative timing to place into the table.
 * Fields:
 *       The execution time for each command (spacecraft time)
 *       The complete command message (symbol) name (must match above)
 *       The size of the entire message
 * NOTE: The symbol "SCCommandList" is the trigger for external processing of the
 *       table. */
static ATS_List_Entry SCCommandList[NUM_CMDS] RETAIN_SYMBOL =
	{ { (TEST_TIME + 30),  "sc_noopBADSYM",     sizeof(sc_noop)     } };

/* The number of commands in the above table (the post-processor needs this size). */
static const uint16 SCCommandList_NUM_CMDS RETAIN_SYMBOL = (NUM_CMDS);

/*****************************
 * (end SC Table Command Content)
 *****************************/
 
#ifdef __cplusplus
}
#endif

/************************/
/*  End of File Comment */
/************************/
