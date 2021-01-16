/*
  Copyright 2013-2020 by Angelo Naselli <anaselli at linux dot it>

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

  File:         YMGA_QCBTable.h

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
class YMGA_QCBTableListViewItem;

class YMGA_QCBTable : public QFrame, public YMGA_CBTable
{
  Q_OBJECT

public:

  /**
   * Constructor.
   **/
    YMGA_QCBTable(YWidget* parent, YCBTableHeader* tableHeader);

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
   * check/uncheck Item from application.
   *
   * Reimplemented from YMGA_CBTable
   * Note that setting it from cell with setChecked(true) does not update
   * the table
   *
   **/
  virtual void setItemChecked(YItem* yitem, int column, bool checked = true);


protected slots:

  /**
   * Notification that an item is selected (single click or keyboard).
   **/
  void slotSelected( QTreeWidgetItem * );

  /**
   * Notification that the item selection changed
   * (relevant for multiSelection mode).
   **/
  // YMGA_CBTable does not work in multiSelection Mode
  //void slotSelectionChanged();

  /**
   * Notification that an item is activated (double click or keyboard).
   **/
  void slotActivated( QTreeWidgetItem * );

  /**
   * Propagate an "item expanded" event to the underlying YTableItem.
   **/
  void slotItemExpanded( QTreeWidgetItem * item );

  /**
   * Propagate an "item collapsed" event to the underlying YTableItem.
   **/
  void slotItemCollapsed( QTreeWidgetItem * item );


  /**
   * Propagate a context menu selection.
   *
   * This will trigger a 'ContextMenuActivated' event if 'notifyContextMenu' is set.
   **/
  void slotContextMenu ( const QPoint & pos );

  /**
   * Propagate an "item changed" or an "item clicked" event to the underlying YTableItem..
   *
   **/
  void slotColumnClicked(QTreeWidgetItem *     item,
                         int                   col);


protected:

  /**
   * Select the original item (the YTableItem) that corresponds to the
   * specified listViewItem.
   **/
  void selectOrigItem( QTreeWidgetItem * listViewItem );

  /**
   * Internal addItem() method that will not do expensive operations in batch
   * mode.
   **/
  void addItem( YItem * item, bool batchMode, bool resizeColumnsToContent );

  /**
   * Clone (create Qt item counterparts) for all child items of 'parentItem'.
   * Set their Qt item parent to 'parentItemClone'.
   **/
  void cloneChildItems( YCBTableItem              * parentItem,
                        YMGA_QCBTableListViewItem * parentItemClone );

  //
  // Data members
  //

  QY2ListView * _qt_listView;

};



/**
 * Visual representation of a YCBTableItem.
 **/
class YMGA_QCBTableListViewItem: public QY2ListViewItem
{
public:

  /**
   * Constructor for toplevel items.
   **/
  YMGA_QCBTableListViewItem( YMGA_QCBTable * table,
                             QY2ListView   * parent,
                             YCBTableItem  * origItem );

  /**
   * Constructor for nested items.
   **/
  YMGA_QCBTableListViewItem( YMGA_QCBTable             * table,
                             YMGA_QCBTableListViewItem * parentItemClone,
                             YCBTableItem              * origItem );


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
  void updateCell( const YCBTableCell * cell );

  /**
   * Update all columns of this item with the content of the original item.
   **/
  void updateCells();

  /**
   * The text of the table cell or the sort-key if available.
   **/
  virtual QString smartSortKey(int column) const override;

protected:
  /**
   * Common initializations for all constructors
   **/
  void init();

  /**
   * Set the alignment for each column according to the YTable parent's
   * alignment.
   **/
  void setColAlignment();

  YMGA_QCBTable * _table;
  YCBTableItem  * _origItem;
};



#endif // YQLabel_h
