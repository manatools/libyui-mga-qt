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

  File:         YMGAQMenuBar.h

  Author:       Angelo Naselli <anaselli@linux.it>

/-*/


#ifndef YMGAQMenuBar_h
#define YMGAQMenuBar_h
#include <QWidget>
#include <QMenu>

#include <yui/mga/YMGAMenuBar.h>


class YMGAQMenuBar : public QWidget, public YMGAMenuBar
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YMGAQMenuBar( YWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YMGAQMenuBar();

    /**
     * Add an YMenuItem first item represents the menu name, other sub items menu entries
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void addItem( YItem * item );

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    /**
    * Enable YMGAMenuItem (menu name or menu entry) to enable/disable it into menubar or menu
    *
    * Reimplemented from YMGAMenuBar.
    **/
    virtual void enableItem(YItem * menu_item, bool enable=true);


protected:

    /**
     * Add a submenu
     *
     **/
    virtual void addSubMenu( QMenu * menu, YItem * item );

    /**
     * Add an action (a menu entry without submenus)
     *
     **/
    virtual void addAction( QMenu * menu, YItem * item );

private:
    struct Private;
    Private *d;
};

#endif //YMGAQMenuBar_h
