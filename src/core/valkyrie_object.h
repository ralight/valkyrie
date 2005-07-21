/* --------------------------------------------------------------------- 
 * Definition of class Valkyrie                        valkyrie_object.h
 * Valkyrie-specific options / flags / fns
 * ---------------------------------------------------------------------
 * This file is part of Valkyrie, a front-end for Valgrind
 * Copyright (c) 2000-2005, Donna Robinson <donna@valgrind.org>
 * This program is released under the terms of the GNU GPL v.2
 * See the file LICENSE.GPL for the full license details.
 */

#ifndef __VALKYRIE_OBJECT_H
#define __VALKYRIE_OBJECT_H


#include <qobject.h>
#include <qstringlist.h>

#include "vk_objects.h"
#include "valkyrie_options_page.h"


class Valgrind;
class ToolObject;

/* class Valkyrie ------------------------------------------------------
   Note: the very first option must be > 0, otherwise it conflicts
   with arg_flags in popt. */
class Valkyrie : public VkObject
{
  Q_OBJECT
public:
  Valkyrie();
  ~Valkyrie();

  void init();        /* set usingGui + get ptrs to all tools */
  bool runTool( ToolObject* activeTool=0 );
  void stopTool( ToolObject* activeTool=0 );

  /* modeNotSet:      no cmd-line options given
   * modeParseLog:    read <logfile> from disk
   * modeMergeLogs:   read <file-list> from disk and merge the contents
   * modeParseOutput: read output direct from valgrind 
   */
  enum RunMode { modeNotSet=0, modeParseLog, modeMergeLogs, modeParseOutput };
  void setRunMode( Valkyrie::RunMode rm );

  /* returns a '\n' separated list of current relevant flags */
  QString currentFlags( ToolObject* tool_obj );
  /* flags relating only to valkyrie */
  QStringList modifiedFlags();

  int checkOptArg( int optid, const char* argval, bool use_gui=false );

  enum vkOpts {
    HELP_OPT,    TOOLTIP,   PALETTE,    ICONTXT,
    FONT_SYSTEM, FONT_USER, SRC_EDITOR, SRC_LINES,
    VG_EXEC,      /* path to valgrind executable (/usr/bin/valgrind) */
    VG_SUPPS_DIR, /* path to supp. files dir [def = /usr/lib/valgrind/] */
    /* FIRST_CMD_OPT */
    BINARY, BIN_FLAGS, VIEW_LOG, MERGE_LOGS, USE_GUI,
    LAST_CMD_OPT  = USE_GUI
  };

  OptionsPage* createOptionsPage( OptionsWindow* parent ) {
    return (OptionsPage*)new ValkyrieOptionsPage( parent, this );
  }

public slots:
  void quit();

private:
  RunMode runMode;
  QStringList flags;

  Valgrind* valgrind;
};


#endif