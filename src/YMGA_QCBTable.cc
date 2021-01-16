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

  File:       YMGA_QCBTable.cc

  Author:     Angelo Naselli <anaselli@linux.it>

/-*/

#define YUILogComponent "mga-qt-ui"
#include <yui/YUILog.h>

#include <QHeaderView>
#include <QVBoxLayout>
#include <QString>

#include <yui/qt/utf8.h>

#include <yui/qt/YQUI.h>
#include <yui/YEvent.h>
#include <yui/qt/YQSignalBlocker.h>
#include <yui/YUIException.h>

#include "YMGA_QCBTable.h"
#include <yui/qt/YQApplication.h>

#define INDENTATION_WIDTH 10
using std::endl;

// define_class YMGA_QCBTable

YMGA_QCBTable::YMGA_QCBTable( YWidget* parent,
                              YCBTableHeader* tableHeader)
    : QFrame( (QWidget *) parent->widgetRep() )
    , YMGA_CBTable( parent, tableHeader )
    , _qt_listView( 0 )
{
  setWidgetRep( this );
  QVBoxLayout* layout = new QVBoxLayout( this );
  layout->setSpacing( 0 );
  setLayout( layout );

  layout->setMargin( YQWidgetMargin );

  _qt_listView = new QY2ListView( this );
  YUI_CHECK_NEW( _qt_listView );
  layout->addWidget( _qt_listView );
  _qt_listView->setAllColumnsShowFocus( true );
  _qt_listView->setIndentation( INDENTATION_WIDTH );
  _qt_listView->header()->setStretchLastSection( false );

  setKeepSorting(  keepSorting() );

  _qt_listView->setContextMenuPolicy( Qt::CustomContextMenu );

  //
  // Add columns
  //

  QStringList headers;
  _qt_listView->setColumnCount( columns() );

  for ( int i=0; i < columns(); i++ )
  {
    headers << fromUTF8( header(i) );
  }

  _qt_listView->setHeaderLabels( headers );
  _qt_listView->header()->setSectionResizeMode( QHeaderView::Interactive );
  _qt_listView->sortItems( 0, Qt::AscendingOrder);


  //
  // Connect signals and slots
  //

  connect( _qt_listView,  &pclass(_qt_listView)::itemDoubleClicked,
           this,  &pclass(this)::slotActivated );

  connect( _qt_listView,  &pclass(_qt_listView)::customContextMenuRequested,
           this,  &pclass(this)::slotContextMenu );

  connect( _qt_listView,  &pclass(_qt_listView)::itemExpanded,
           this,  &pclass(this)::slotItemExpanded );

  connect( _qt_listView,  &pclass(_qt_listView)::itemCollapsed,
           this,  &pclass(this)::slotItemCollapsed );

  connect( _qt_listView,  &pclass(_qt_listView)::currentItemChanged,
             this,  &pclass(this)::slotSelected );

  connect ( _qt_listView, &pclass(_qt_listView)::itemClicked,
            this,   &pclass(this)::slotColumnClicked );

  connect ( _qt_listView,  &pclass(_qt_listView)::itemChanged,
            this,   &pclass(this)::slotColumnClicked );

}


YMGA_QCBTable::~YMGA_QCBTable()
{
    // NOP
}


void YMGA_QCBTable::setKeepSorting( bool keepSorting )
{
    YTable::setKeepSorting( keepSorting );
    _qt_listView->setSortByInsertionSequence( keepSorting );
    _qt_listView->setSortingEnabled( ! keepSorting );
}


void YMGA_QCBTable::addItem( YItem * yitem )
{
  addItem ( yitem,
            false, // batchMode
            true ); // resizeColumnsToContent
}

void YMGA_QCBTable::addItem( YItem * yitem, bool batchMode, bool resizeColumnsToContent )
{
  YCBTableItem * item = dynamic_cast<YCBTableItem *> (yitem);
  YUI_CHECK_PTR( item );

  YMGA_CBTable::addItem( item );

  YMGA_QCBTableListViewItem * clone = new YMGA_QCBTableListViewItem( this, _qt_listView, item );
  YUI_CHECK_NEW( clone );

  if ( ! batchMode && item->selected() )
  {
    // YTable enforces single selection, if appropriate

    YQSignalBlocker sigBlocker( _qt_listView );
    YMGA_QCBTable::selectItem( YSelectionWidget::selectedItem(), true );
  }

  if ( item->hasChildren() )
  {
    cloneChildItems( item, clone );
    _qt_listView->setRootIsDecorated( true );
  }

  if ( ! batchMode )
    _qt_listView->sortItems( 0, Qt::AscendingOrder);

  if ( resizeColumnsToContent )
  {
    for ( int i=0; i < columns(); i++ )
      _qt_listView->resizeColumnToContents( i );
    // NOTE: resizeColumnToContents() is performance-critical!
  }
}


void YMGA_QCBTable::cloneChildItems(YCBTableItem* parentItem, YMGA_QCBTableListViewItem* parentItemClone)
{
    for ( YItemIterator it = parentItem->childrenBegin();
          it != parentItem->childrenEnd();
          ++it )
    {
        YCBTableItem * childItem = dynamic_cast<YCBTableItem *>( *it );

        if ( childItem )
        {
            YMGA_QCBTableListViewItem * childClone = new YMGA_QCBTableListViewItem( this, parentItemClone, childItem );
            YUI_CHECK_NEW( childClone );

            cloneChildItems( childItem, childClone );
        }
    }
}


void YMGA_QCBTable::addItems( const YItemCollection & itemCollection )
{
  YQSignalBlocker sigBlocker( _qt_listView );

  for ( YItemConstIterator it = itemCollection.begin();
       it != itemCollection.end(); ++it )
  {
    addItem( *it,
            true,    // batchMode
            false ); // resizeColumnsToContent
    // NOTE: resizeToContents = true would cause a massive performance drop!
    // => resize columns to content only once at the end of this function
  }

  YItem * sel = YSelectionWidget::selectedItem();

  if ( sel )
    YMGA_QCBTable::selectItem( sel, true );

  // TODO FIXME allow resizing only if enabled in header
  for ( int i=0; i < columns(); i++ )
    _qt_listView->resizeColumnToContents( i );
}


void YMGA_QCBTable::selectItem( YItem * yitem, bool selected )
{
  YQSignalBlocker sigBlocker( _qt_listView );

  YCBTableItem * item = dynamic_cast<YCBTableItem *> (yitem);
  YUI_CHECK_PTR( item );

  YMGA_QCBTableListViewItem * clone = (YMGA_QCBTableListViewItem *) item->data();
  YUI_CHECK_PTR( clone );

  if ( ! selected && clone == _qt_listView->currentItem() )
  {
    deselectAllItems();
  }
  else
  {
    if ( ! hasMultiSelection() )
      _qt_listView->setCurrentItem( clone ); // This deselects all other items!

    clone->setSelected( true );
    YMGA_CBTable::selectItem( item, selected );
  }
}


void YMGA_QCBTable::slotItemExpanded( QTreeWidgetItem * qItem )
{
  YMGA_QCBTableListViewItem * item = dynamic_cast<YMGA_QCBTableListViewItem *> (qItem);

  if ( item )
    item->origItem()->setOpen( true );

  _qt_listView->resizeColumnToContents( 0 );
}


void YMGA_QCBTable::slotItemCollapsed( QTreeWidgetItem * qItem )
{
  YMGA_QCBTableListViewItem * item = dynamic_cast<YMGA_QCBTableListViewItem *> (qItem);

  if ( item )
    item->origItem()->setOpen( false );

  _qt_listView->resizeColumnToContents( 0 );
}


void YMGA_QCBTable::deselectAllItems()
{
  YQSignalBlocker sigBlocker( _qt_listView );

  YTable::deselectAllItems();
  _qt_listView->clearSelection();
}


void YMGA_QCBTable::deleteAllItems()
{
  _qt_listView->clear();
  YTable::deleteAllItems();
}


void YMGA_QCBTable::cellChanged( const YTableCell * cell )
{
    YCBTableItem * item = dynamic_cast<YCBTableItem*>(cell->parent());
    YUI_CHECK_PTR( item );

    YMGA_QCBTableListViewItem * clone = (YMGA_QCBTableListViewItem *) item->data();
    YUI_CHECK_PTR( clone );

    const YCBTableCell *pCell = dynamic_cast<const YCBTableCell*>(cell);
    YUI_CHECK_PTR( pCell );

    clone->updateCell( pCell );
}


void YMGA_QCBTable::selectOrigItem( QTreeWidgetItem * listViewItem )
{
  if ( listViewItem )
  {
    YMGA_QCBTableListViewItem * tableListViewItem = dynamic_cast<YMGA_QCBTableListViewItem *> (listViewItem);
    YUI_CHECK_PTR( tableListViewItem );

    YMGA_QCBTable::selectItem( tableListViewItem->origItem(), true );
  }
}


void YMGA_QCBTable::slotSelected( QTreeWidgetItem * listViewItem  )
{
  if ( listViewItem )
    selectOrigItem( listViewItem );
  else
  {
    // Qt might select nothing if a user clicks outside the items in the widget

    if ( hasItems() && YSelectionWidget::hasSelectedItem() )
      YMGA_QCBTable::selectItem( YSelectionWidget::selectedItem(), true );
  }

  if ( immediateMode() )
  {
    if ( ! YQUI::ui()->eventPendingFor( this ) )
    {
      // Avoid overwriting a (more important) Activated event with a SelectionChanged event

      yuiDebug() << "Sending SelectionChanged event" << endl;
      YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
    }
  }
}


void YMGA_QCBTable::slotActivated( QTreeWidgetItem * listViewItem )
{
  selectOrigItem( listViewItem );

  if ( notify() )
  {
    yuiDebug() << "Sending Activated event" << endl;
    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
  }
}


void YMGA_QCBTable::setEnabled( bool enabled )
{
  _qt_listView->setEnabled( enabled );
  // FIXME _qt_listView->triggerUpdate();
  YWidget::setEnabled( enabled );
}


int YMGA_QCBTable::preferredWidth()
{
    // Arbitrary value.
    // Use a MinSize widget to set a size that is useful for the application.

    return 30;
}


int YMGA_QCBTable::preferredHeight()
{
  // Arbitrary value.
  // Use a MinSize widget to set a size that is useful for the application.
  // Upstream was 30 let's show som line and not only header as default

    return 100;
}


void YMGA_QCBTable::setSize( int newWidth, int newHeight )
{
  resize( newWidth, newHeight );
}


bool YMGA_QCBTable::setKeyboardFocus()
{
  _qt_listView->setFocus();

  return true;
}

void YMGA_QCBTable::slotContextMenu ( const QPoint & pos )
{
  if  ( ! _qt_listView ||  ! _qt_listView->viewport() )
    return;

  YQUI::yqApp()->setContextMenuPos( _qt_listView->viewport()->mapToGlobal( pos ) );
  if ( notifyContextMenu() )
    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ContextMenuActivated ) );
}


void YMGA_QCBTable::slotColumnClicked(QTreeWidgetItem* item,
                                      int col)
{
  if (item)
  {
    selectOrigItem( item );

    YMGA_QCBTableListViewItem * it = dynamic_cast<YMGA_QCBTableListViewItem*> ( item );
    YUI_CHECK_PTR( it );

    if ( it->table()->isCheckBoxColumn(col) )
    {
      Qt::CheckState checked = item->checkState ( col );
      YCBTableItem *pYCBTableItem = it->origItem();

      yuiDebug() << "Column is checked: " << (checked  == Qt::CheckState::Checked?"yes":"no" ) <<  std::endl;

      // it seems items contains old value when signal is emitted
      YCBTableCell *cell = dynamic_cast<YCBTableCell *>(pYCBTableItem->cell(col)); // checkboxItemColumn is true so hasCell is as well
      YUI_CHECK_PTR( cell );
      cell->setChecked( checked == Qt::CheckState::Checked );

      if ( notify() )
      {
        YMGA_CBTable::setChangedItem ( pYCBTableItem );
        pYCBTableItem->setChangedColumn(col);
        YQUI::ui()->sendEvent ( new YWidgetEvent ( this, YEvent::ValueChanged ) );
      }
    }
  }
}



void YMGA_QCBTable::setItemChecked ( YItem* yitem, int column, bool checked )
{
  YQSignalBlocker sigBlocker ( _qt_listView );

  YCBTableItem * item = dynamic_cast<YCBTableItem *> ( yitem );
  YUI_CHECK_PTR ( item );

  YMGA_QCBTableListViewItem * clone = ( YMGA_QCBTableListViewItem * ) item->data();
  YUI_CHECK_PTR ( clone );
  if ( clone->table()->isCheckBoxColumn(column) )
  {
    YCBTableCell *cell = dynamic_cast<YCBTableCell *>(item->cell(column)); // checkboxItemColumn is true so hasCell is as well
    YUI_CHECK_PTR( cell );
    cell->setChecked( checked );

    clone->setCheckState ( column, checked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
  }
}


// define_class YMGA_QCBTableListViewItem


YMGA_QCBTableListViewItem::YMGA_QCBTableListViewItem( YMGA_QCBTable * table,
                                                      QY2ListView   * parent,
                                                      YCBTableItem  * origItem )
    : QY2ListViewItem( parent )
    , _table( table )
    , _origItem( origItem )
{
    init();
}


YMGA_QCBTableListViewItem::YMGA_QCBTableListViewItem( YMGA_QCBTable *          table,
                                                      YMGA_QCBTableListViewItem * parentItemClone,
                                                      YCBTableItem *          origItem )
: QY2ListViewItem( parentItemClone )
, _table( table )
, _origItem( origItem )
{
  init();
}



void YMGA_QCBTableListViewItem::init()
{
    YUI_CHECK_PTR( _table );
    YUI_CHECK_PTR( _origItem );

    _origItem->setData( this );

    int cols = _table->columns();
    for (int col=0; col< cols; ++col)
    {
      if (_table->isCheckBoxColumn(col))
      {
        setCheckState(col, _origItem->checked(col) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
      }
    }

    updateCells();
    setColAlignment();

    if ( _origItem->isOpen() && _origItem->hasChildren() )
        setExpanded( true );
}



void YMGA_QCBTableListViewItem::updateCells()
{
  for ( YTableCellIterator it = _origItem->cellsBegin();
        it != _origItem->cellsEnd();
        ++it )
  {
    YCBTableCell * cell = dynamic_cast<YCBTableCell *>(*it);
    if (cell)
      updateCell( cell );
  }
}



void YMGA_QCBTableListViewItem::updateCell( const YCBTableCell * cell )
{
  if ( ! cell )
    return;

  int column = cell->column();
  if (_table->isCheckBoxColumn(column))
  {
    setCheckState( column, cell->checked() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
  }
  else
  {
    //
    // Set label text
    //
    setText( column, fromUTF8( cell->label() ) );

    //
    // Set icon (if specified)
    //
    if ( cell->hasIconName() )
    {
      QIcon icon = YQUI::ui()->loadIcon( cell->iconName() );

      if ( ! icon.isNull() )
        setData( column, Qt::DecorationRole, icon );
    }
    else // No icon name
    {
      if ( ! data( column, Qt::DecorationRole ).isNull() ) // Was there an icon before?
      {
        setData( column, Qt::DecorationRole, QIcon() ); // Set empty icon
      }
    }
  }
}


void YMGA_QCBTableListViewItem::setColAlignment()
{
  YUI_CHECK_PTR( _table );

  for ( int col=0; col < _table->columns(); col++ )
  {
    switch ( _table->alignment( col ) )
    {
      case YAlignBegin:	setTextAlignment( col, Qt::AlignLeft   | Qt::AlignVCenter );	break;
      case YAlignCenter:	setTextAlignment( col, Qt::AlignCenter | Qt::AlignVCenter );	break;
      case YAlignEnd:	setTextAlignment( col, Qt::AlignRight  | Qt::AlignVCenter );	break;

      case YAlignUnchanged: break;
    }
  }
}


QString YMGA_QCBTableListViewItem::smartSortKey(int column) const
{
    if (!_table->isCheckBoxColumn(column))
    {
      const YCBTableCell* tableCell = dynamic_cast<const YCBTableCell*>(origItem()->cell(column));
      if (tableCell && tableCell->hasSortKey())
          return QString::fromUtf8(tableCell->sortKey().c_str());
      else
          return text(column).trimmed();
    }

    return QString();
}

#include "YMGA_QCBTable.moc"
