/*
    KNode, the KDE newsreader
    Copyright (c) 1999-2005 the KNode authors.
    See file AUTHORS for details

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, US
*/

#ifndef KNAPPLICATION_H
#define KNAPPLICATION_H

#include <QApplication>

#include "knode_export.h"

class KNMainWindow;

class KNODE_EXPORT KNApplication : public QApplication
{
    Q_OBJECT
  public:
    KNApplication(int &argc, char **argv);
    void launch(const QStringList& params);

  private:
    KNMainWindow* findPrimaryWindow();

};
#endif
