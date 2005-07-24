/* ---------------------------------------------------------------------- 
 * Definition of class VkObject                              vk_objects.h
 * 
 * Essential functionality is contained within a VkObject.
 * ---------------------------------------------------------------------
 * This file is part of Valkyrie, a front-end for Valgrind
 * Copyright (c) 2000-2005, OpenWorks LLP <info@open-works.co.uk>
 * This program is released under the terms of the GNU GPL v.2
 * See the file LICENSE.GPL for the full license details.
 */

/* TODO: don't have enum values for the objOpts; instead, init an int
   array in the constructor.  This means will have to implement addOpt
   somewhat differently, as won't have enums available */

#ifndef __VK_OBJECTS_H
#define __VK_OBJECTS_H


#include <qkeysequence.h>
#include <qobject.h>
#include <qptrlist.h>
#include <qmainwindow.h>

#include "vk_option.h"          /* class Option */
#include "vk_popt_option.h"
#include "vk_utils.h"
#include "options_window.h"
#include "options_page.h"

class VkObject;
typedef QPtrList<VkObject> VkObjectList;

/* class VkObject ------------------------------------------------------ */
class VkObject : public QObject 
{
 Q_OBJECT
public:
  VkObject( const QString& capt, const QString& txt,
            const QKeySequence& key, bool is_tool=true );
  ~VkObject();

  bool isTool()           { return is_Tool;         }
  QString name()          { return caption.lower(); }
  QString title()         { return caption;         }
  QString accelTitle()    { return accelText;       }
  QKeySequence accelKey() { return accel_Key;       }

  /* called by parseCmdArgs() in parse_cmd_args.cpp,
     and from the gui options pages */
  virtual int checkOptArg(int optid, const char* argval, bool gui=false) = 0; 

  /* returns a list of non-default flags to pass to valgrind */
  virtual QStringList modifiedFlags();
  /* returns a list of options to be written to the config file */
  virtual QString configEntries();
  /* command-line + optionsWindow help and parsing stuff */
  vkPoptOption * poptOpts();
  void freePoptOpts( vkPoptOption * );
  /* also called by OptionsPage::optionWidget() */
  Option * findOption( int optid );
  /* called by OptionsPage::optionWidget() to if should use ^2 spin widget */
  virtual bool optionUsesPwr2( int /*optId*/ ) { return false; }

  virtual OptionsPage* createOptionsPage( OptionsWindow* parent ) = 0;

protected:
  /* writes the value of the option to vkConfig */
  void writeOptionToConfig( Option* opt, QString argval );

  void addOpt( int key, Option::ArgType arg_type, Option::WidgetType w_type,
               QString cfg_group,  QChar   short_flag, QString long_flag, 
               QString flag_desc,  QString poss_vals,  QString default_val,
               QString shelp,      QString lhelp,      const char* url );

protected:
  bool usingGui;             /* whether in gui || non-gui mode */
  bool is_Tool;              /* not valkyrie or valgrind-core */

  QString caption;           /* eg. Memcheck */

  QString accelText;         /* eg. &Memcheck */
  QKeySequence accel_Key;    /* accelerator key */

  QPtrList<Option> optList;  /* list of options for this object */
};


#endif // #ifndef __VK_OBJECTS_H


