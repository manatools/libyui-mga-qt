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
#include <QKeyEvent>
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

#include <QKeyEvent>

struct YMGA_QCBTable::Private
{
  ///< offset to first YCell 
  ///  usually 1 if checkbox Enabled and mode is YCBTableCheckBoxOnFirstColumn
  ///  0 otherwise
  unsigned int firstColumnOffset;
};

class YQ2ListView : public  QY2ListView
{
public:
  /**
     * Constructor
     **/
  YQ2ListView ( QWidget * parent ) : QY2ListView ( parent )
  {
    setFocusPolicy ( Qt::StrongFocus );
  }

  /**
   * Destructor
   **/
  virtual ~YQ2ListView() {}

  void keyPressEvent ( QKeyEvent * event )
  {
    int k = event->key();
    switch ( k )
    {
    case Qt::Key_Space:
      event->accept();
      emit itemClicked ( currentItem(), -1 );
      break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
      event->accept();
      emit itemDoubleClicked ( currentItem(), -1 );
      break;
    default:
      QY2ListView::keyPressEvent ( event );
      break;
    }
  }

};


YMGA_QCBTable::YMGA_QCBTable( YWidget * parent, YTableHeader * tableHeader, YCBTableMode tableMode )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YMGA_CBTable( parent, tableHeader, tableMode ), _qt_listView(0), d(new Private)
{
  YUI_CHECK_NEW ( d );

  setWidgetRep ( this );
  QVBoxLayout* layout = new QVBoxLayout ( this );
  layout->setSpacing ( 0 );
  setLayout ( layout );

  layout->setMargin ( YQWidgetMargin );

  _qt_listView = new YQ2ListView ( this );
  YUI_CHECK_NEW ( _qt_listView );
  layout->addWidget ( _qt_listView );
  _qt_listView->setAllColumnsShowFocus ( true );
  _qt_listView->header()->setStretchLastSection ( false );

  setKeepSorting ( keepSorting() );

  d->firstColumnOffset = 0;

  yuiMilestone() << " Slection mode " << tableMode <<  std::endl;

  if ( tableMode == YCBTableCheckBoxOnFirstColumn )
  {
    d->firstColumnOffset = 1;
  }

  _qt_listView->setContextMenuPolicy ( Qt::CustomContextMenu );

  //
  // Add columns
  //

  QStringList headers;
  _qt_listView->setColumnCount ( columns() );

  // YCBTable needs header also for the checkable column
  for ( int i=0; i < columns(); i++ )
  {
    headers << fromUTF8 ( header ( i ) );
  }

  _qt_listView->setHeaderLabels ( headers );
  _qt_listView->header()->setResizeMode ( QHeaderView::Interactive );
  _qt_listView->sortItems ( 0, Qt::AscendingOrder );

  //
  // Connect signals and slots
  //

  connect ( _qt_listView,      SIGNAL ( itemDoubleClicked ( QTreeWidgetItem *, int ) ),
            this, 		SLOT ( slotActivated ( QTreeWidgetItem *, int ) ) );

  connect ( _qt_listView,      SIGNAL ( itemClicked ( QTreeWidgetItem *, int ) ),
            this,               SLOT ( slotcolumnClicked ( QTreeWidgetItem *, int ) ) );

  connect ( _qt_listView, 	SIGNAL ( currentItemChanged ( QTreeWidgetItem *, QTreeWidgetItem * ) ),
            this, 		SLOT ( slotSelected ( QTreeWidgetItem * ) ) );

  connect ( _qt_listView,      SIGNAL ( customContextMenuRequested ( const QPoint & ) ),
            this,      	SLOT ( slotContextMenu ( const QPoint & ) ) );

}


YMGA_QCBTable::~YMGA_QCBTable()
{
    if (d)
      delete d;
}

int YMGA_QCBTable::checkboxItemColumn()
{
  int column = (d->firstColumnOffset == 1) ? 0 : columns() -1;

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
  addItem ( yitem,
            false, // batchMode
            true ); // resizeColumnsToContent
}


void
YMGA_QCBTable::addItem( YItem * yitem, bool batchMode, bool resizeColumnsToContent )
{
  YCBTableItem * item = dynamic_cast<YCBTableItem *> ( yitem );
  YUI_CHECK_PTR ( item );

  YMGA_CBTable::addItem ( item );

  YMGA_QCBTableListViewItem * clone = new YMGA_QCBTableListViewItem ( this, _qt_listView, item );
  YUI_CHECK_NEW ( clone );

  if ( ! batchMode && item->selected() )
  {
    // YTable enforces single selection, if appropriate

    YQSignalBlocker sigBlocker ( _qt_listView );
    YMGA_QCBTable::selectItem ( YSelectionWidget::selectedItem(), true );
  }


  //
  // Set column alignment
  //

  for ( int col=0; col < columns(); col++ )
  {
    switch ( alignment ( col ) )
    {
    case YAlignBegin:
      clone->setTextAlignment ( col, Qt::AlignLeft   | Qt::AlignVCenter );
      break;
    case YAlignCenter:
      clone->setTextAlignment ( col, Qt::AlignCenter | Qt::AlignVCenter );
      break;
    case YAlignEnd:
      clone->setTextAlignment ( col, Qt::AlignRight  | Qt::AlignVCenter );
      break;

    case YAlignUnchanged:
      break;
    }
  }

  if ( ! batchMode )
    _qt_listView->sortItems ( 0, Qt::AscendingOrder );

  if ( resizeColumnsToContent )
  {
    for ( int i=0; i < columns(); i++ )
      _qt_listView->resizeColumnToContents ( i );
    /* NOTE: resizeColumnToContents(...) is performance-critical ! */
  }
}


void
YMGA_QCBTable::addItems( const YItemCollection & itemCollection )
{
  YQSignalBlocker sigBlocker ( _qt_listView );

  for ( YItemConstIterator it = itemCollection.begin();
        it != itemCollection.end();
        ++it )
  {
    addItem ( *it,
              true,    // batchMode
              false ); // resizeColumnsToContent
    /* NOTE: resizeToContents=true would cause a massive performance drop !
             => resize columns to content only one time at the end of this
                function                                                 */
  }

  YItem * sel = YSelectionWidget::selectedItem();

  if ( sel )
    YMGA_QCBTable::selectItem ( sel, true );

  for ( int i=0; i < columns(); i++ )
    _qt_listView->resizeColumnToContents ( i );
}


void YMGA_QCBTable::selectItem ( YItem * yitem, bool selected )
{
  YQSignalBlocker sigBlocker ( _qt_listView );

  YCBTableItem * item = dynamic_cast<YCBTableItem *> ( yitem );
  YUI_CHECK_PTR ( item );

  YMGA_QCBTableListViewItem * clone = ( YMGA_QCBTableListViewItem * ) item->data();
  YUI_CHECK_PTR ( clone );

  if ( ! selected && clone == _qt_listView->currentItem() )
  {
    deselectAllItems();
  }
  else
  {
    if ( ! hasMultiSelection() )
      _qt_listView->setCurrentItem ( clone ); // This deselects all other items!

    clone->setSelected ( true );
    YMGA_CBTable::selectItem ( item, selected );
  }
}

void YMGA_QCBTable::checkItem ( YItem* yitem, bool checked )
{
  YQSignalBlocker sigBlocker ( _qt_listView );

  YCBTableItem * item = dynamic_cast<YCBTableItem *> ( yitem );
  YUI_CHECK_PTR ( item );

  YMGA_QCBTableListViewItem * clone = ( YMGA_QCBTableListViewItem * ) item->data();
  YUI_CHECK_PTR ( clone );

  item->check(checked);
  clone->setCheckState ( checkboxItemColumn(), checked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
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
    YCBTableItem * item = dynamic_cast<YCBTableItem*>(cell->parent());
    YUI_CHECK_PTR( item );

    YMGA_QCBTableListViewItem * clone = (YMGA_QCBTableListViewItem *) item->data();
    YUI_CHECK_PTR( clone );

    clone->updateCell( cell );
}


void YMGA_QCBTable::selectOrigItem( QTreeWidgetItem * listViewItem )
{
  if ( listViewItem )
  {
    YMGA_QCBTableListViewItem * tableListViewItem = dynamic_cast<YMGA_QCBTableListViewItem *> ( listViewItem );
    YUI_CHECK_PTR ( tableListViewItem );

    YMGA_CBTable::selectItem ( tableListViewItem->origItem(), true );
     if ( ! hasMultiSelection() )
            _qt_listView->setCurrentItem( listViewItem );
  }
}


void YMGA_QCBTable::slotSelected ( QTreeWidgetItem * listViewItem )
{
  if ( listViewItem )
    selectOrigItem ( listViewItem );
  else
  {
    // Qt might select nothing if a user clicks outside the items in the widget

    if ( hasItems() && YSelectionWidget::hasSelectedItem() )
      YMGA_QCBTable::selectItem ( YSelectionWidget::selectedItem(), true );
  }

  if ( immediateMode() )
  {
    if ( ! YQUI::ui()->eventPendingFor ( this ) )
    {
      // Avoid overwriting a (more important) Activated event with a SelectionChanged event

      yuiDebug() << "Sending SelectionChanged event" << std::endl;
      YQUI::ui()->sendEvent ( new YWidgetEvent ( this, YEvent::SelectionChanged ) );
    }
  }
}

void YMGA_QCBTable::slotActivated( QTreeWidgetItem * listViewItem, int column )
{
  selectOrigItem( listViewItem );
  if ( notify() )
  {
    yuiDebug() << "Sending Activated event" << std::endl;
    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
  }
}


void
YMGA_QCBTable::slotcolumnClicked(QTreeWidgetItem* item,
                                 int              col)
{
  selectOrigItem( item );

  if ( col == checkboxItemColumn() )
  {
    YMGA_QCBTableListViewItem * it = dynamic_cast<YMGA_QCBTableListViewItem*> ( item );
    Qt::CheckState checked = item->checkState ( col );
    YCBTableItem *pYCBTableItem = it->origItem();

    yuiDebug() << "Column is checked: " << (checked  == Qt::CheckState::Checked?"yes":"no" ) <<  std::endl;

    // it seems items contains old value when signal is emitted
    pYCBTableItem->check ( checked == Qt::CheckState::Checked );

    if ( notify() )
    {
      YMGA_CBTable::setChangedItem ( pYCBTableItem );
      YQUI::ui()->sendEvent ( new YWidgetEvent ( this, YEvent::ValueChanged ) );
    }
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


YMGA_QCBTableListViewItem::YMGA_QCBTableListViewItem( YMGA_QCBTable *	table,
					  QY2ListView * parent,
					  YCBTableItem *	origItem )
    : QY2ListViewItem( parent )
    , _table( table )
    , _origItem( origItem )
{
    YUI_CHECK_PTR( _table );
    YUI_CHECK_PTR( _origItem );

    _origItem->setData( this );
        
    yuiDebug() << "Checkable column is " << table->checkboxItemColumn() <<  std::endl;

    int table_columns = _table->columns()-2;
    setCheckState(table->checkboxItemColumn(), _origItem->checked() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);      
    
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
    YCBTableMode mode = table()->tableMode();
    if (mode == YCBTableMode::YCBTableCheckBoxOnFirstColumn)
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
