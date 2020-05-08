/*
  Copyright 2020 by Angelo Naselli <anaselli at linux dot it>

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

  File:         YMGAQMenuBar.cc

  Author:       Angelo Naselli <anaselli@linux.it>

/-*/

#include <QMenu>
#include <QMenuBar>
#include <QHBoxLayout>
#include <QAction>
#define YUILogComponent "mga-qt-ui"
#include <yui/YUILog.h>

#include <yui/qt/utf8.h>

#include <yui/qt/YQUI.h>
#include <yui/YEvent.h>
#include <yui/qt/YQSignalBlocker.h>
#include <yui/YUIException.h>
#include <yui/YMenuItem.h>

#include "YMGAQMenuBar.h"
#include <yui/qt/YQApplication.h>

struct YMGAQMenuBar::Private
{
  QMenuBar *menubar;
};


class QItemAction : public QAction
{

private:
  YMenuItem *_yitem;
public:
  QItemAction(YMenuItem *item, QObject *parent = nullptr ) :
    QAction(item->label().c_str(), parent), _yitem(item)
  {

  }

  virtual ~QItemAction() {}

  void selectedItem()
  {
    YQUI::ui()->sendEvent( new YMenuEvent( _yitem ) );
  }
};

YMGAQMenuBar::YMGAQMenuBar( YWidget * parent )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YMGAMenuBar( parent ), d(new Private)
{
  YUI_CHECK_NEW ( d );

  setWidgetRep ( this );

  QHBoxLayout* layout = new QHBoxLayout ( this );
  layout->setSpacing ( 0 );
  setLayout ( layout );

  layout->setMargin ( YQWidgetMargin );

  d->menubar = new QMenuBar ( this );
  YUI_CHECK_NEW ( d->menubar );
  layout->addWidget ( d->menubar );
}


YMGAQMenuBar::~YMGAQMenuBar()
{
      delete d;
}

void YMGAQMenuBar::addAction(QMenu* menu, YItem* yitem)
{
  YMenuItem * item = dynamic_cast<YMenuItem *> ( yitem );
  YUI_CHECK_PTR ( item );

  // TODO icon from item
  QItemAction *action = new QItemAction(item, this);

  connect(action, &QAction::triggered, action, &QItemAction::selectedItem);
  menu->addAction(action);

}

void YMGAQMenuBar::addSubMenu(QMenu* menu, YItem* yitem)
{
  YMenuItem * item = dynamic_cast<YMenuItem *> ( yitem );
  YUI_CHECK_PTR ( item );

  // TODO icon from item
  QMenu *m=menu->addMenu(item->label().c_str());
  if (item->hasChildren())
  {
    for (YItemIterator miter = item->childrenBegin(); miter != item->childrenEnd(); miter++)
    {
      YMenuItem *m_item= dynamic_cast<YMenuItem *>(*miter);
      if (m_item->hasChildren())
      {
        addSubMenu(menu, m_item);
      }
      else
      {
        addAction(m, m_item);
      }
    }
  }
}


void YMGAQMenuBar::addItem(YItem* yitem)
{
  YMenuItem * item = dynamic_cast<YMenuItem *> ( yitem );
  YUI_CHECK_PTR ( item );

  // TODO icon from item
  QMenu *menu = d->menubar->addMenu(item->label().c_str());
  if (item->hasChildren())
  {
    for (YItemIterator miter = item->childrenBegin(); miter != item->childrenEnd(); miter++)
    {
      YMenuItem *m_item= dynamic_cast<YMenuItem *>(*miter);
      if (m_item->hasChildren())
      {
        addSubMenu(menu, m_item);
      }
      else
      {
        addAction(menu, m_item);
      }
    }
  }

  YMGAMenuBar::addItem(yitem);
}





int YMGAQMenuBar::preferredWidth()
{
    // Arbitrary value.
    // Use a MinSize widget to set a size that is useful for the application.

    return 20 + d->menubar->sizeHint().width();
}


int YMGAQMenuBar::preferredHeight()
{
    // Arbitrary value.
    // Use a MinSize widget to set a size that is useful for the application.
    return 20 + d->menubar->sizeHint().height();
}


void YMGAQMenuBar::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}

