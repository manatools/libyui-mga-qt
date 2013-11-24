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

  File:         YQWE.cc

  Author:       Angelo Naselli <anaselli@linux.it>

/-*/

#include "YQWE.h"
#include "YMGAQWidgetFactory.h"
#include <YUIException.h>

YQWE::YQWE() : YExternalWidgets()
{
}

YQWE::~YQWE()
{
}

YExternalWidgetFactory* YQWE::createExternalWidgetFactory()
{
  YMGAQWidgetFactory * factory = new YMGAQWidgetFactory();
  YUI_CHECK_NEW( factory );

  return factory;
}

YExternalWidgets * createExternalWidgets()
{
    if ( ! YExternalWidgets::externalWidgets() )
    {
        YQWE * ue = new YQWE( );
        YUI_CHECK_NEW ( ue );
    }

    return YExternalWidgets::externalWidgets();
}
