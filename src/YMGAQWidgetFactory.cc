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

  File:         YMGAQWidgetFactory.cc

  Author:       Stefan Hundhammer <sh@suse.de>

/-*/

#include <QColorGroup>
#define YUILogComponent "mga-qt"
#include <yui/YUILog.h>

#include "YMGAQWidgetFactory.h"
#include <yui/qt/YQApplication.h>
#include <yui/qt/YQUI.h>
#include <yui/YUIException.h>
#include <YExternalWidgets.h>


#include <string>

#include "YMGA_QCBTable.h"

using std::string;


YMGAQWidgetFactory::YMGAQWidgetFactory()
    : YMGAWidgetFactory()
{
    // NOP
}


YMGAQWidgetFactory::~YMGAQWidgetFactory()
{
    // NOP
}


YMGA_CBTable *
YMGAQWidgetFactory::createCBTable( YWidget * parent, YTableHeader * header, YCBTableMode mode )
{
    YMGA_QCBTable * table = new YMGA_QCBTable( parent, header, mode );
    YUI_CHECK_NEW( table );

    return table;
}

