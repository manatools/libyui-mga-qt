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

  File:		YMGAQWidgetFactory.h

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YMGAQWidgetFactory_h
#define YMGAQWidgetFactory_h


#include <yui/YMGAWidgetExtensionFactory.h>

#include "YMGA_CBTable.h"


using std::string;


/**
 * Concrete widget factory for mandatory widgets.
 **/
class YMGAQWidgetFactory: public YMGAWidgetFactory
{
public:

  virtual YMGA_CBTable * createTable ( YWidget * parent, YTableHeader * header_disown, YTableMode mode = YTableSingleLineSelection );


protected:

    friend class YQWE;

    /**
     * Constructor.
     *
     * Use YUI::widgetFactory() to get the singleton for this class.
     **/
    YMGAQWidgetFactory();

    /**
     * Destructor.
     **/
    virtual ~YMGAQWidgetFactory();

}; // class YWidgetFactory


#endif // YMGAQWidgetFactory_h
