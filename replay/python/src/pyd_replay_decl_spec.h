/*

    Sartorial Programming Interface (SPI) runtime libraries
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA

*/

#ifndef _PYD_REPLAY_DECL_SPEC_H_
#define _PYD_REPLAY_DECL_SPEC_H_

/**
****************************************************************************
* Header file: pyd_replay_decl_spec.h
****************************************************************************
*/

/*
****************************************************************************
** pyd_replay_decl_spec.h
**
** If PY_REPLAY_EXPORT is defined then PY_REPLAY_IMPORT is dllexport.
** Otherwise PY_REPLAY_IMPORT is dllimport.
****************************************************************************
*/

#include <spi_util/ImportExport.h>
#ifdef PY_REPLAY_EXPORT
#define PY_REPLAY_IMPORT SPI_UTIL_DECLSPEC_EXPORT
#else
#define PY_REPLAY_IMPORT SPI_UTIL_DECLSPEC_IMPORT
#endif

#endif /* _PYD_REPLAY_DECL_SPEC_H_*/

