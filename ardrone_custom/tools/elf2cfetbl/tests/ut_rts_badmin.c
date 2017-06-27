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
**  elf2cfetbl test: empty SCCommandList, bad SCPacketMinSize symbol
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


/* Helpful macros for command definition */
#include "ct_ccsds.h"


/*****************************
 * (end include files)
 *****************************/

/*****************************
 * Macros
 *****************************/

/*
 * The macro to force the compiler to place the symbol into the object
 * file symbol table.  (compiler specific) */
#define RETAIN_SYMBOL	__attribute__((__used__))   /* gcc */

/*****************************
 * (End of macros)
 *****************************/

/*****************************
 * Table ID and Type Information
 *****************************/
/*
 * Sample cFE Table Header
 * (Required)
 */
static CFE_TBL_FileDef_t CFE_TBL_FileDef RETAIN_SYMBOL = {
		"RTS_Table002", "SC.RTS_TBL002", "SC Sample RTS_TBL002",
		"ut_rts_badmin.tbl", (SC_RTS_BUFF_SIZE * sizeof(uint16)) };

/*
 * Sample RTS Table Data
 * (Required)
 * (Note, no data will be used from this table, it will be assembled
 *  with information below.)
 */
uint16 RTS_Table002[SC_RTS_BUFF_SIZE] = { };

/*
 * A single command entry with relative time tag.
 */
typedef struct RTS_List_Entry {
	uint16 TimeTag; /* relative time (sec) for command */
	char cmdSymbolName[255]; /* symbol name of the command data, above */
	uint16 byteSize; /* size of the entire message (bytes) */
} RTS_List_Entry;

/* The type of table (used by the post-processor).
 * This is used since the naming convention is under the control of sc_platform_cfg.h.
 * 0=ATS, 1=RTS */
static uint8 SCTableType RETAIN_SYMBOL = 1U; /* RTS */

/* Packet size limits (which can be checked by the post-processor) from sc_platform_cfg.h. */
static uint16 SCPacketMinSizeXYZ RETAIN_SYMBOL = (SC_PACKET_MIN_SIZE); /* BAD SYMBOL NAME */
static uint16 SCPacketMaxSize RETAIN_SYMBOL = (SC_PACKET_MAX_SIZE);

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
static RTS_List_Entry SCCommandList[NUM_CMDS] RETAIN_SYMBOL;

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
