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
 **
 ** Description:
 ** This file provides macro definitions, types, and defined symbols
 ** required for creating compile-time SC table object files for
 ** post-procesing with elf2cfetbl.  Note not all the SC required symbols
 ** are here.  Most make sense to define in your table .c for clarity.
 ** 
 ** For completeness the required symbols for SC elf2cfetbl processing are
 ** the following:
 ** For all cFS table processing:
 **   CFE_TBL_FileDef = for cFS table processing (your table .c)
 **   The table itself = for cFS table processing (your table .c), name 
 **      defined in CFE_TBL_FileDef
 ** For SC table processing by elf2cfetbl:
 **    SCCommandList, SCTableType, SCCommandList_NUM_CMDS = your table .c
 **    SCPacketMinSize, SCPacketMaxSize = in this file
 **
 ** Dependencies: 
 ** sc_platform_cfg.h
 **
 *************************************************************************/

#ifndef CT_SC_TABLE_H
#define CT_SC_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sc_platform_cfg.h"

/*****************************
 * Macros
 *****************************/

/*
 * The macro to force the compiler to place the symbol into the object
 * file symbol table.  (compiler specific) 
 * 
 * Usage: After the declaration and before any assignment, e.g.
 *        static my_type_t MySymbolName RETAIN_SYMBOL = {};
 */
#define RETAIN_SYMBOL	__attribute__((__used__))   /* gcc */

/*****************************
 * (End of macros)
 *****************************/

/*****************************
 * Types
 *****************************/

/*
 * A single command entry with relative time tag.
 */
typedef struct RTS_List_Entry {
	uint16 TimeTag; /* relative time (sec) for command */
	char cmdSymbolName[255]; /* symbol name of the command data, above */
	uint16 byteSize; /* size of the entire message (bytes) */
} RTS_List_Entry;

/*
 * A single command entry with absolute time tag.
 */
typedef struct ATS_List_Entry {
	uint32 TimeTag; /* execution time for command (spacecraft time) */
	char cmdSymbolName[255]; /* symbol name of the command data, above */
	uint16 byteSize; /* size of the entire message (bytes) */
} ATS_List_Entry;

/*****************************
 * (End of types)
 *****************************/

/*****************************
 * Required Symbols
 *****************************/

/* Packet size limits (which can be checked by the post-processor) from sc_platform_cfg.h. */
static uint16 SCPacketMinSize RETAIN_SYMBOL = (SC_PACKET_MIN_SIZE);
static uint16 SCPacketMaxSize RETAIN_SYMBOL = (SC_PACKET_MAX_SIZE);

/*****************************
 * (End of required symbols)
 *****************************/

#ifdef __cplusplus
}
#endif

#endif /* CT_SC_TABLE_H */

/************************/
/*  End of File Comment */
/************************/
