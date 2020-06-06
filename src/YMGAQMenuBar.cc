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
#include <yui/mga/YMGAMenuItem.h>

typedef std::map<YItem*, QObject*> MenuEntryMap;
typedef std::pair<YItem*, QObject*> MenuEntryPair;

struct YMGAQMenuBar::Private
{
  QMenuBar *menubar;
  MenuEntryMap menu_entry;
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

  YMGAMenuItem *menuItem = dynamic_cast<YMGAMenuItem *>(yitem);
  if (menuItem)
  {
    action->setEnabled(menuItem->enabled());
    d->menu_entry.insert(MenuEntryPair(yitem, action));
    action->setVisible(!menuItem->hidden());
  }

}

void YMGAQMenuBar::addSubMenu(QMenu* menu, YItem* yitem)
{
  YMenuItem * item = dynamic_cast<YMenuItem *> ( yitem );
  YUI_CHECK_PTR ( item );

  // TODO icon from item
  QMenu *m=menu->addMenu(item->label().c_str());
  if (item->hasChildren())
  {
    YMGAMenuItem *menuItem = dynamic_cast<YMGAMenuItem *>(yitem);
    if (menuItem)
    {
      m->setEnabled(menuItem->enabled());
      d->menu_entry.insert(MenuEntryPair(yitem, m));
      m->menuAction()->setVisible(!menuItem->hidden());
    }
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

  YMGAMenuItem *menuItem = dynamic_cast<YMGAMenuItem *>(yitem);
  if (menuItem)
  {
    menu->setEnabled(menuItem->enabled());
    d->menu_entry.insert(MenuEntryPair(yitem, menu));
    menu->menuAction()->setVisible(!menuItem->hidden());
  }
  YMGAMenuBar::addItem(yitem);
  menu->update();
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

void YMGAQMenuBar::enableItem(YItem* menu_item, bool enable)
{
  YMGAMenuBar::enableItem(menu_item, enable);

  auto search = d->menu_entry.find( menu_item );
  if (search != d->menu_entry.end())
  {
    QMenu * menu_entry = dynamic_cast<QMenu*>(search->second);
    if (menu_entry)
      menu_entry->setEnabled(enable);
    else
    {
      QAction * menu_action = dynamic_cast<QAction*>(search->second);
      if (menu_action)
        menu_action->setEnabled(enable);
    }
  }
  else
  {
    yuiError() << menu_item->label() << " not found" << std::endl;
  }
}

void YMGAQMenuBar::hideItem(YItem* menu_item, bool invisible)
{
  YMGAMenuBar::hideItem(menu_item, invisible);

  auto search = d->menu_entry.find( menu_item );
  if (search != d->menu_entry.end())
  {
    QMenu * menu_entry = dynamic_cast<QMenu*>(search->second);
    if (menu_entry)
    {
      menu_entry->menuAction()->setVisible(!invisible);

//       resize( d->menubar->sizeHint() + QSize(20,20));
//       update();

    }
    else
    {
      QAction * menu_action = dynamic_cast<QAction*>(search->second);
      if (menu_action)
        menu_action->setVisible(!invisible);
    }
  }
  else
  {
    yuiError() << menu_item->label() << " not found" << std::endl;
  }
}

