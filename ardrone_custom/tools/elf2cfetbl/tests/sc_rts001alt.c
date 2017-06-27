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
 **  Author: Allen Brown, Odyssey Space Research, LLC
 **
 ** CFS Stored Command (SC) sample RTS table #1 (alternate method)
 **
 ** Note 1: The following source code demonstrates how to create a sample
 **         Stored Command RTS table.  While the preferred method for creating
 **         flight versions of RTS tables is to use custom ground system
 **         tools that output the binary table files (skipping this step
 **         altogether) this is an example of how to create table data
 **         with the build toolchain.  (See also elf2cfetbl.)
 **
 ** Note 2: This source file creates a sample RTS table that contains only
 **         the following commands that are scheduled as follows:
 **
 **         SC NOOP command, execution time relative to start of RTS = 0
 **         SC Enable RTS #2 command, execution time relative to prev cmd = 5
 **         SC Start RTS #2 command, execution time relative to prev cmd = 5
 **
 **         Commands are defined separately, then the commands and their times
 **         can be used together.  This approach makes it simple to send the
 **         same command at multiple different times, if desired.
 **
 ** Note 3: This table creation approach: This source file produces an object
 **         file containing additional symbols (and data) beyond CFE_TBL_FileDef.
 **         The elf2cfetbl host utility looks for the SCCommandList symbol
 **         in the .o, and is the hook for special processing.  If found,
 **         then the symbols starting with "SC*" are used to generate the
 **         final table from the SCCommandList and supporting data in the
 **         object file.  The commands are built into the resulting .tbl,
 **         message checksums are added, and messages are aligned in the table.
 **         Message lengths and table size are checked as the resulting .tbl
 **         table file is built.  The main build system alterations are
 **         additional include directories for the additional headers.
 **
 ** Note 4: As with many of these table examples, the __attribute__ ((__used__))
 **         is compiler-specific.  Its intent is to ensure that the symbol
 **         is always placed into the object file's symbol table, regardless
 **         of other compiler settings (e.g. optimization).  Adjust for other
 **         compilers as necessary.
 **
 *************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

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
 * Sample cFE Table Header
 * (Required)
 */
static CFE_TBL_FileDef_t CFE_TBL_FileDef RETAIN_SYMBOL = {
		"RTS_Table001", "SC.RTS_TBL001", "SC Sample RTS_TBL001",
		"sc_rts001alt.tbl", (SC_RTS_BUFF_SIZE * sizeof(uint16)) };

/*
 * Sample RTS Table Data
 * (Required)
 * (Note, no data will be used from this table, it will be assembled
 *  with information below.)
 */
uint16 RTS_Table001[SC_RTS_BUFF_SIZE] = { };

/* The type of table (used by the post-processor).
 * This is used since the naming convention is under the control of sc_platform_cfg.h.
 * 0=ATS, 1=RTS */
static uint8 SCTableType RETAIN_SYMBOL = 1U; /* RTS */

/*****************************
 * (End table ID and type info)
 *****************************/

/*****************************
 * Begin command definitions
 *****************************/

/*
 * SC No-OP Command (a command code only, no payload)
 */
#define CMD123_SIZE_BE     CFE_MAKE_BIG16((sizeof(CFE_SB_CmdHdr_t))-7) /* size */

static CFE_SB_CmdHdr_t sc_noop RETAIN_SYMBOL = { {
/* primary message header */
{ INPLACE_BE_HI(CFE_MAKE_BIG16(SC_CMD_MID)),
  INPLACE_BE_LO(CFE_MAKE_BIG16(SC_CMD_MID)) }, /* MID */
{ INPLACE_BE_HI(CMD_FLAGS_BE), INPLACE_BE_LO(CMD_FLAGS_BE) }, /* Seq/Flags */
{ INPLACE_BE_HI(CMD123_SIZE_BE), INPLACE_BE_LO(CMD123_SIZE_BE) } /* Length */
},
/* secondary command header */
{ (SC_NOOP_CC << 8) | 0x0000 } /* command code, but no checksum (yet) */
}; /* no payload */

/* Size for the following two commands */
#define RTSCMD_SIZE_BE     CFE_MAKE_BIG16((sizeof(SC_RtsCmd_t))-7) /* size */

/*
 * SC Enable RTS Command (a command with a payload/arg)
 */

static SC_StartAtsCmd_t sc_enarts RETAIN_SYMBOL = { {
/* primary message header */
INPLACE_BE_HI(CFE_MAKE_BIG16(SC_CMD_MID)),
INPLACE_BE_LO(CFE_MAKE_BIG16(SC_CMD_MID)), /* MID */
INPLACE_BE_HI(CMD_FLAGS_BE), INPLACE_BE_LO(CMD_FLAGS_BE), /* Seq/Flags */
INPLACE_BE_HI(RTSCMD_SIZE_BE), INPLACE_BE_LO(RTSCMD_SIZE_BE), /* Length */
/* secondary command header, native endian */
CC_NATIVE_1ST(SC_ENABLE_RTS_CC),
CC_NATIVE_2ND(SC_ENABLE_RTS_CC),
},
/* payload */
2U /* The ID of the RTS to enable */
};

/*
 * SC Start RTS Command (a command with a payload/arg)
 */

static SC_StartAtsCmd_t sc_startrts RETAIN_SYMBOL = { {
/* primary message header */
INPLACE_BE_HI(CFE_MAKE_BIG16(SC_CMD_MID)),
INPLACE_BE_LO(CFE_MAKE_BIG16(SC_CMD_MID)), /* MID */
INPLACE_BE_HI(CMD_FLAGS_BE), INPLACE_BE_LO(CMD_FLAGS_BE), /* Seq/Flags */
INPLACE_BE_HI(RTSCMD_SIZE_BE), INPLACE_BE_LO(RTSCMD_SIZE_BE), /* Length */
/* secondary command header, native endian */
CC_NATIVE_1ST(SC_START_RTS_CC),
CC_NATIVE_2ND(SC_START_RTS_CC),
},
/* payload */
2U /* The ID of the RTS to start */
};


/*****************************
 * (End command definitions)
 *****************************/

/*****************************
 * SC Table Command Content
 *****************************/

/* Set the number of commands to insert in the table here: */
#define NUM_CMDS (3U)

/* The command order and relative timing to place into the table.
 * Fields:
 *       The relative time (seconds)
 *       The complete command message (symbol) name (must match above)
 *       The size of the entire message
 * NOTE: The symbol "SCCommandList" is the trigger for external processing of the
 *       table. */
static RTS_List_Entry SCCommandList[NUM_CMDS] RETAIN_SYMBOL =
	{ { 0, "sc_noop",     sizeof(sc_noop) },
	  { 5, "sc_enarts",   sizeof(sc_enarts) },
	  { 5, "sc_startrts", sizeof(sc_startrts) } };

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
