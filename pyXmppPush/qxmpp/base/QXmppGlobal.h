/*
 * Copyright (C) 2008-2019 The QXmpp developers
 *
 * Author:
 *  Manjeet Dahiya
 *  Niels Ole Salscheider
 *  Linus Jahn
 *
 * Source:
 *  https://github.com/qxmpp-project/qxmpp
 *
 * This file is a part of QXmpp library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 */


#ifndef QXMPPGLOBAL_H
#define QXMPPGLOBAL_H

#include <QString>

//#if defined(QXMPP_BUILD)
//#    define QXMPP_EXPORT Q_DECL_EXPORT
//#else
//#    define QXMPP_EXPORT Q_DECL_IMPORT
//#endif


#define QXMPP_EXPORT
#define VERSION_MAJOR 1
#define VERSION_MINOR 2
#define VERSION_PATCH 0
#define VERSION_STRING "1.2.0"
#define SO_VERSION 2

#define QXMPP_AUTOTEST_EXPORT

/// This macro expands a numeric value of the form 0xMMNNPP (MM =
/// major, NN = minor, PP = patch) that specifies QXmpp's version
/// number. For example, if you compile your application against
/// QXmpp 1.2.3, the QXMPP_VERSION macro will expand to 0x010203.
///
/// You can use QXMPP_VERSION to use the latest QXmpp features where
/// available.
///


#define QXMPP_VERSION (VERSION_MAJOR << 16) | (VERSION_MINOR << 8) | VERSION_PATCH

inline QLatin1String QXmppVersion()
{
    return QLatin1String("VERSION_STRING");
}

// This sets which deprecated functions should still be usable
// It works exactly like QT_DISABLE_DEPRECATED_BEFORE
#ifndef QXMPP_DISABLE_DEPRECATED_BEFORE
#   define QXMPP_DISABLE_DEPRECATED_BEFORE VERSION_MAJOR << 16
#endif

// This works exactly like QT_DEPRECATED_SINCE, but checks QXMPP_DISABLE_DEPRECATED_BEFORE instead.
#define QXMPP_DEPRECATED_SINCE(major, minor) (QT_VERSION_CHECK(major, minor, 0) > QXMPP_DISABLE_DEPRECATED_BEFORE)

#endif //QXMPPGLOBAL_H

