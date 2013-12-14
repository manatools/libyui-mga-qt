/*
  Copyright (C) 2000-2012 Novell, Inc
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

  File:	      YMGA_QCBTable.h

  Author:       Angelo Naselli <anaselli@linux.it>

/-*/

#ifndef YMGA_QCBTable_h
#define YMGA_QCBTable_h
#include <QFrame>

#include <yui/mga/YMGA_CBTable.h>
#include <yui/qt/QY2ListView.h>


// class QY2ListView;
class QTreeWidgetItem;
class YQListViewItem;


class YMGA_QCBTable : public QFrame, public YMGA_CBTable
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YMGA_QCBTable( YWidget * parent, YTableHeader * header, YCBTableMode checkboxMode );

    /**
     * Destructor.
     **/
    virtual ~YMGA_QCBTable();

    /**
     * Switch between sorting by item insertion order (keepSorting: true) or
     * allowing the user to sort by an arbitrary column (by clicking on the
     * column header).
     *
     * Reimplemented from YTable.
     **/
    virtual void setKeepSorting( bool keepSorting );

    /**
     * Add an item.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void addItem( YItem * item );

    /**
     * Add multiple items.
     *
     * Reimplemented for efficiency from YSelectionWidget.
     **/
    virtual void addItems( const YItemCollection & itemCollection );

    /**
     * Select or deselect an item.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void selectItem( YItem * item, bool selected = true );

    /**
     * Deselect all items.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void deselectAllItems();

    /**
     * Delete all items.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void deleteAllItems();

    /**
     * Notification that a cell (its text and/or its icon) was changed from the
     * outside. Applications are required to call this whenever a table cell is
     * changed after adding the corresponding table item (the row) to the table
     * widget.
     *
     * Reimplemented from YTable.
     **/
    virtual void cellChanged( const YTableCell * cell );

    /**
     * check/uncheck Item from application.
     * 
     * Note that item->check(true) does not update the table
     * 
     **/
    void checkItem( YItem * item, bool checked = true );
    
    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

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
     * Accept the keyboard focus.
     *
     * Reimplemented from YWidget.
     **/
    virtual bool setKeyboardFocus();

    /**
     * returns which column is managed by checkboxes, if any
     * -1 otherwise
     */
    int  checkboxItemColumn();

protected slots:

    /**
     * Notification that an item is selected (single click or keyboard).
     **/
    void slotSelected( QTreeWidgetItem * );

    /**
     * Notification that an item is activated (double click or keyboard).
     **/
    void slotActivated( QTreeWidgetItem* listViewItem, int column );

    /**
     * Propagate a context menu selection
     *
     * This will trigger an 'ContextMenuActivated' event if 'notifyContextMenu' is set.
     **/
    void slotContextMenu ( const QPoint & pos );


    void slotcolumnClicked(QTreeWidgetItem *     item,
                           int                   col);

protected:

    /**
     * Select the original item (the YCBTableItem) that corresponds to the
     * specified listViewItem.
     **/
    void selectOrigItem( QTreeWidgetItem * listViewItem );

    /**
     * Internal addItem() method that will not do expensive operations in batch
     * mode.
     **/
    void addItem( YItem * item, bool batchMode, bool resizeColumnsToContent );

    //
    // Data members
    //

    QY2ListView * _qt_listView;
    
//     void keyPressEvent ( QKeyEvent * event );

private:
    struct Private;
    Private *d;
};



/**
 * Visual representation of a YCBTableItem.
 **/
class YMGA_QCBTableListViewItem: public QY2ListViewItem
{
public:

    /**
     * Constructor.
     **/
    YMGA_QCBTableListViewItem( YMGA_QCBTable     * 	table,
			 QY2ListView * 	parent,
			 YCBTableItem  *	origItem );

    /**
     * Return the parent table widget.
     **/
    YMGA_QCBTable * table() const { return _table; }

    /**
     * Return the corresponding YCBTableItem.
     **/
    YCBTableItem * origItem() const { return _origItem; }

    /**
     * Update this item's display with the content of 'cell'.
     **/
    void updateCell( const YTableCell * cell );

protected:

    YMGA_QCBTable *	 _table;
    YCBTableItem * _origItem;
};



#endif // YQLabel_h
