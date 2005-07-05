/* ---------------------------------------------------------------------- 
 * This file is only included by vk_config.cpp               vk_include.h
 * Automatically generated by configure on Tue Jul  5 12:54:29 BST 2005
 * Do not edit this file - all changes will be lost
 * ----------------------------------------------------------------------
 * This file is part of Valkyrie, a front-end for Valgrind
 * Copyright (c) 2000-2005, Donna Robinson <donna@valgrind.org>
 * This program is released under the terms of the GNU GPL v.2
 * See the file LICENSE.GPL for the full license details.
 */

#ifndef __VK_INCLUDE_H
#define __VK_INCLUDE_H

/* valkyrie install paths ---------------------------------------------- */
#define PREFIX        "/home/de/Programs/valkyrie/trunk"
#define VK_DOC_PATH   "/doc/"
#define VG_DOC_PATH   "/doc/vg-html/"
#define VK_ICONS_PATH "/icons/"
#define VK_DBASE_DIR  "/dbase/"
#define VK_LOGS_DIR   "/logs/"
#define VK_SUPPS_DIR  "/suppressions/"
/* valgrind paths ------------------------------------------------------ */
#define VG_EXEC_PATH  "/home/sewardj/Vg3LINE/trunk/Inst/bin/valgrind"
#define VG_SUPP_PATH  "/home/sewardj/Vg3LINE/trunk/Inst/lib/valgrind"

/* names --------------------------------------------------------------- */
#define Vk_Name      "valkyrie"
#define VK_NAME      "Valkyrie"

/* version, as 'MAJOR-NUMBER.MINOR-NUMBER[.PATCHLEVEL]'.
   A version in the form 'YYYY-MM-DD' is a release-of-the-day, 
   i.e. a snapshot of the current development tree --------------------- */
#define VK_VERSION    "1.2.0"
#define VK_COPYRIGHT  "(c) 2003-2005"
#define VK_AUTHOR     "Donna Robinson"
#define VK_EMAIL      "donna@valgrind.org"
/* so we do not have to update hard-wired-in dates for valgrind */
#define VG_COPYRIGHT  "(c) 2000-2005, and GNU GPL'd, by Julian Seward et al."

#endif
