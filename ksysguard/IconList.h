/*
    KTop, a taskmanager and cpu load monitor
   
    Copyright (C) 1997 Bernd Johannes Wuebben
                       wuebben@math.cornell.edu

    Copyright (C) 1998 Nicolas Leclercq
                       nicknet@planete.net

	Copyright (c) 1999 Chris Schlaeger <cs@axys.de>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef _IconList_h_
#define _IconList_h_

#include <qlist.h>
#include <qpalette.h>
#include <qcombo.h>
#include <qpainter.h>
#include <qpushbt.h>
#include <qtabdlg.h>
#include <qtimer.h>
#include <qmsgbox.h>
#include <qlabel.h>
#include <qobject.h>
#include <qlistbox.h>
#include <qgrpbox.h>
#include <qradiobt.h>
#include <qchkbox.h>
#include <qbttngrp.h>
#include <qpalette.h>
#include <qlined.h>
#include <qtabbar.h>
#include <qpopmenu.h>
#include <qfontmet.h>
#include <qslider.h>
#include <qlcdnum.h>

/*=============================================================================
 class  :  KtopIconListElem
 =============================================================================*/
class KtopIconListElem
{
public:
      KtopIconListElem(const char* fName,const char* iName);
     ~KtopIconListElem();

      const QPixmap* pixmap();
      const char*    name();

private:
    QPixmap *pm;
    char     icnName[128];
};

/*=============================================================================
 class  : KtopIconList
 =============================================================================*/
class KtopIconList
{
 public:
      KtopIconList();
     ~KtopIconList();

      const QPixmap* procIcon(const char*);

 private :
      static int                      instCounter;
      static QList<KtopIconListElem> *icnList;
      static const QPixmap*           defaultIcon;

      static void load();
};


#endif