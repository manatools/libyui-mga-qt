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

  File:	      YMGA_QCBTable.cc

  Author:     Angelo Naselli <anaselli@linux.it>

/-*/

#include <QHeaderView>
#include <QVBoxLayout>
#include <QString>
#define YUILogComponent "mga-qt-ui"
#include <yui/YUILog.h>

#include <yui/qt/utf8.h>

#include <yui/qt/YQUI.h>
#include <yui/YEvent.h>
#include <yui/qt/YQSignalBlocker.h>
#include <yui/YUIException.h>

#include <yui/qt/QY2ListView.h>
#include "YMGA_QCBTable.h"
#include <yui/qt/YQApplication.h>

struct YMGA_QCBTable::Private
{
  ///< offset to first YCell 
  ///  usually 1 if checkbox Enabled and mode is YTableCheckBoxOnFirstColumn
  ///  0 otherwise
  unsigned int firstColumnOffset;

  ///< quick way to know if YTable has checkbox enabled, instead of 
  ///  multiselection or single selection mode
  bool         checkboxEnabled;
};


YMGA_QCBTable::YMGA_QCBTable( YWidget * parent, YTableHeader * tableHeader, YTableMode selectionMode )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YMGA_CBTable( parent, tableHeader, selectionMode ), _qt_listView(0), d(new Private)
{
    YUI_CHECK_NEW( d );

    setWidgetRep( this );
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setSpacing( 0 );
    setLayout( layout );

    layout->setMargin( YQWidgetMargin );

    _qt_listView = new QY2ListView( this );
    YUI_CHECK_NEW( _qt_listView );
    layout->addWidget( _qt_listView );
    _qt_listView->setAllColumnsShowFocus( true );
    _qt_listView->header()->setStretchLastSection( false );

    setKeepSorting(  keepSorting() );

    d->firstColumnOffset = 0;
    d->checkboxEnabled   = false;

    yuiMilestone() << " Slection mode " << selectionMode <<  std::endl;
    
    if ( selectionMode == YTableMultiSelection )
      _qt_listView->setSelectionMode ( QAbstractItemView::ExtendedSelection );
    else if ( selectionMode == YTableCheckBoxOnFirstColumn )
    {
        d->checkboxEnabled = true;
        d->firstColumnOffset = 1;
    }
    else if ( selectionMode == YTableCheckBoxOnLastColumn )
    {
        d->checkboxEnabled = true;
    }
    
    _qt_listView->setContextMenuPolicy( Qt::CustomContextMenu );

    //
    // Add columns
    //

    QStringList headers;
    _qt_listView->setColumnCount( columns() + (d->checkboxEnabled ? 1 : 0) );

    //TODO anaselli remove next line if not needed any more
//     _qt_listView->setSelectionBehavior(QAbstractItemView::SelectItems);
    if ( d->firstColumnOffset == 1 )
    {
      headers << "";
    }
    for ( int i=0; i < columns(); i++ )
    {
        headers << fromUTF8( header(i) );
    }
    if (d->checkboxEnabled && d->firstColumnOffset == 0)
    {
      headers << "";
    }

    _qt_listView->setHeaderLabels( headers );
    _qt_listView->header()->setResizeMode( QHeaderView::Interactive );
    _qt_listView->sortItems( 0, Qt::AscendingOrder);

    //
    // Connect signals and slots
    //

    connect( _qt_listView,      SIGNAL( itemDoubleClicked ( QTreeWidgetItem *, int ) ),
	     this, 		SLOT  ( slotActivated	  ( QTreeWidgetItem * ) ) );

    connect( _qt_listView, 	SIGNAL( currentItemChanged ( QTreeWidgetItem *, QTreeWidgetItem * ) ),
	     this, 		SLOT  ( slotSelected	   ( QTreeWidgetItem * ) ) );

    connect( _qt_listView,      SIGNAL( customContextMenuRequested ( const QPoint & ) ),
             this,      	SLOT  ( slotContextMenu ( const QPoint & ) ) );
    
    connect( _qt_listView,      SIGNAL( columnClicked ( int , QTreeWidgetItem* , int , const QPoint &) ),
             this,              SLOT  ( slotcolumnClicked ( int , QTreeWidgetItem* , int, const QPoint &  ) ) );
    
    if ( selectionMode == YTableMultiSelection )
    {
	// This is the exceptional case - avoid performance drop in the normal case

	connect( _qt_listView, 	SIGNAL( itemSelectionChanged() ),
		 this,		SLOT  ( slotSelectionChanged() ) );

    }
}


YMGA_QCBTable::~YMGA_QCBTable()
{
    if (d)
      delete d;
}

bool YMGA_QCBTable::hasCheckboxItems()
{
  return d->checkboxEnabled;
}

int YMGA_QCBTable::checkboxItemColumn()
{
  int column = -1;
  
  if (d->checkboxEnabled)
  {
    if (d->firstColumnOffset == 1)
      column = 0;
    else
      column = columns();
  }
  
  return column;
}



void
YMGA_QCBTable::setKeepSorting( bool keepSorting )
{
    YMGA_CBTable::setKeepSorting( keepSorting );
    _qt_listView->setSortByInsertionSequence( keepSorting );
    _qt_listView->setSortingEnabled( ! keepSorting );
}


void
YMGA_QCBTable::addItem( YItem * yitem )
{
    addItem( yitem,
	     false, // batchMode
	     true); // resizeColumnsToContent
}


void
YMGA_QCBTable::addItem( YItem * yitem, bool batchMode, bool resizeColumnsToContent )
{
    YTableItem * item = dynamic_cast<YTableItem *> (yitem);
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


    //
    // Set column alignment
    //

    for ( int col=0; col < columns(); col++ )
    {
	switch ( alignment( col ) )
	{
	    case YAlignBegin:	clone->setTextAlignment( col, Qt::AlignLeft   | Qt::AlignVCenter );	break;
	    case YAlignCenter:	clone->setTextAlignment( col, Qt::AlignCenter | Qt::AlignVCenter );	break;
	    case YAlignEnd:	clone->setTextAlignment( col, Qt::AlignRight  | Qt::AlignVCenter );	break;

	    case YAlignUnchanged: break;
	}
    }

    if ( ! batchMode )
	_qt_listView->sortItems( 0, Qt::AscendingOrder);

    if ( resizeColumnsToContent )
    {
        for ( int i=0; i < columns(); i++ )
	    _qt_listView->resizeColumnToContents( i );
	/* NOTE: resizeColumnToContents(...) is performance-critical ! */
    }
}


void
YMGA_QCBTable::addItems( const YItemCollection & itemCollection )
{
    YQSignalBlocker sigBlocker( _qt_listView );

    for ( YItemConstIterator it = itemCollection.begin();
	  it != itemCollection.end();
	  ++it )
    {
	addItem( *it,
		 true,    // batchMode
		 false ); // resizeColumnsToContent
	/* NOTE: resizeToContents=true would cause a massive performance drop !
	         => resize columns to content only one time at the end of this
	            function                                                 */
    }

    YItem * sel = YSelectionWidget::selectedItem();

    if ( sel )
	YMGA_QCBTable::selectItem( sel, true );

    for ( int i=0; i < columns(); i++ )
	_qt_listView->resizeColumnToContents( i );
}


void
YMGA_QCBTable::selectItem( YItem * yitem, bool selected )
{
    YQSignalBlocker sigBlocker( _qt_listView );

    YTableItem * item = dynamic_cast<YTableItem *> (yitem);
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
      _qt_listView->setCurrentItem ( clone ); // This deselects all other items!
    if ( hasCheckboxItems() )
    {
      clone->setCheckState ( checkboxItemColumn(), selected ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
    }
    else
    {
      clone->setSelected ( true );
      YMGA_CBTable::selectItem ( item, selected );
    }
  }
}


void
YMGA_QCBTable::deselectAllItems()
{
    YQSignalBlocker sigBlocker( _qt_listView );

    YMGA_CBTable::deselectAllItems();
    _qt_listView->clearSelection();
}


void
YMGA_QCBTable::deleteAllItems()
{
    _qt_listView->clear();
    YMGA_CBTable::deleteAllItems();
}


void
YMGA_QCBTable::cellChanged( const YTableCell * cell )
{
    YTableItem * item = cell->parent();
    YUI_CHECK_PTR( item );

    YMGA_QCBTableListViewItem * clone = (YMGA_QCBTableListViewItem *) item->data();
    YUI_CHECK_PTR( clone );

    clone->updateCell( cell );
}


void
YMGA_QCBTable::selectOrigItem( QTreeWidgetItem * listViewItem )
{
    if ( listViewItem )
    {
	YMGA_QCBTableListViewItem * tableListViewItem = dynamic_cast<YMGA_QCBTableListViewItem *> (listViewItem);
	YUI_CHECK_PTR( tableListViewItem );

	YMGA_CBTable::selectItem( tableListViewItem->origItem(), true );
    }
}


void
YMGA_QCBTable::slotSelected( QTreeWidgetItem * listViewItem  )
{
    if ( listViewItem && _qt_listView->selectedItems().count() > 0 )
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

	    yuiDebug() << "Sending SelectionChanged event" << std::endl;
	    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
	}
    }
}


void
YMGA_QCBTable::slotSelectionChanged()
{
    YSelectionWidget::deselectAllItems();
    yuiDebug() << std::endl;

    QList<QTreeWidgetItem *> selItems = _qt_listView->selectedItems();

    for ( QList<QTreeWidgetItem *>::iterator it = selItems.begin();
	  it != selItems.end();
	  ++it )
    {
	YMGA_QCBTableListViewItem * tableListViewItem = dynamic_cast<YMGA_QCBTableListViewItem *> (*it);

	if ( tableListViewItem )
	{
	    tableListViewItem->origItem()->setSelected( true );

	    yuiDebug() << "Selected item: " << tableListViewItem->origItem()->label() << std::endl;
	}
    }

    if ( immediateMode() )
    {
	if ( ! YQUI::ui()->eventPendingFor( this ) )
	{
	    // Avoid overwriting a (more important) Activated event with a SelectionChanged event

	    yuiDebug() << "Sending SelectionChanged event" << std::endl;
	    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
	}
    }
}


void
YMGA_QCBTable::slotActivated( QTreeWidgetItem * listViewItem )
{
    selectOrigItem( listViewItem );

    if ( notify() )
    {
	yuiDebug() << "Sending Activated event" << std::endl;
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
    }
}


void
YMGA_QCBTable::setEnabled( bool enabled )
{
    _qt_listView->setEnabled( enabled );
    //FIXME _qt_listView->triggerUpdate();
    YWidget::setEnabled( enabled );
}



int
YMGA_QCBTable::preferredWidth()
{
    // Arbitrary value.
    // Use a MinSize widget to set a size that is useful for the application.

    return 30;
}


int
YMGA_QCBTable::preferredHeight()
{
    // Arbitrary value.
    // Use a MinSize widget to set a size that is useful for the application.

    return 30;
}


void
YMGA_QCBTable::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool
YMGA_QCBTable::setKeyboardFocus()
{
    _qt_listView->setFocus();

    return true;
}


void
YMGA_QCBTable::slotContextMenu ( const QPoint & pos )
{
    if  ( ! _qt_listView ||  ! _qt_listView->viewport() )
	return;

    YQUI::yqApp()->setContextMenuPos( _qt_listView->viewport()->mapToGlobal( pos ) );
    if ( notifyContextMenu() )
        YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ContextMenuActivated ) );
}


void
YMGA_QCBTable::slotcolumnClicked(int               button,
                           QTreeWidgetItem * item,
                           int               col,
                           const QPoint &    pos)
{
  if (d->checkboxEnabled && col == checkboxItemColumn())
  {
    YMGA_QCBTableListViewItem * it = dynamic_cast<YMGA_QCBTableListViewItem*>(item);
    YTableItem *pYTableItem = it->origItem();

    yuiDebug() << "Column is checked: " << (item->checkState(col)==Qt::CheckState::Unchecked?"yes":"no") <<  std::endl;

    // it seems items contains old value when signal is emitted
    pYTableItem->setSelected(item->checkState(col)==Qt::CheckState::Unchecked);
     if ( notify() )
     {
       YMGA_CBTable::setChangedItem(pYTableItem);
       YQUI::ui()->sendEvent(new YWidgetEvent(this, YEvent::ValueChanged));
     }
  } 
}

YMGA_QCBTableListViewItem::YMGA_QCBTableListViewItem( YMGA_QCBTable *	table,
					  QY2ListView * parent,
					  YTableItem *	origItem )
    : QY2ListViewItem( parent )
    , _table( table )
    , _origItem( origItem )
{
    YUI_CHECK_PTR( _table );
    YUI_CHECK_PTR( _origItem );

    _origItem->setData( this );
        
    yuiMilestone() << "item checkable? " << (table->hasCheckboxItems()?"yes":"no") 
                   << " checkable column is " << table->checkboxItemColumn() <<  std::endl;

    int table_columns = _table->columns();
    if (table->hasCheckboxItems())
    {
      table_columns -= 1;
      setCheckState(table->checkboxItemColumn(), _origItem->selected() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);      
    }
    
    for ( YTableCellIterator it = _origItem->cellsBegin();
	  it != _origItem->cellsEnd();
	  ++it )
    {
        YTableCell * cell = *it;
        // if someone decided to have more cells for this item
        if (cell->column() > table_columns)
        {
           yuiWarning() << "Item contains too many columns, current is " << cell->column() 
                        << " but only " << _table->columns() << " columns are configured" << std::endl;
        }
        else
          updateCell( *it );
    }
}


void
YMGA_QCBTableListViewItem::updateCell( const YTableCell * cell )
{
    if ( ! cell )
	return;

    int column = cell->column();
    YTableMode mode = table()->selectionMode();
    if (mode == YTableMode::YTableCheckBoxOnFirstColumn)
      column += 1;

    //
    // Set label text
    //

    setText( column, fromUTF8( cell->label() ) );

    //
    // Set icon (if specified)
    //
    if ( cell->hasIconName() )
    {
	// _table is checked against 0 in the constructor

	string	iconName = _table->iconFullPath( cell->iconName() );
	QPixmap	icon	 = QPixmap( iconName.c_str() );

	if ( icon.isNull() )
	    yuiWarning() << "Can't load icon " << iconName << std::endl;
	else
	    setData( column, Qt::DecorationRole, icon );
    }
    else // No pixmap name
    {
	if ( ! data( column, Qt::DecorationRole ).isNull() ) // Was there a pixmap before?
	{
	    setData( column, Qt::DecorationRole, QPixmap() ); // Set empty pixmap
	}
    }
}


#include "YMGA_QCBTable.moc"
