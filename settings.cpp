
/*
    Copyright (c) 2005 by Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, US
*/

#include "settings.h"

#include <QStandardPaths>

#include "knconfig.h"
#include "knglobals.h"
#include "utilities.h"

#include <klocale.h>
#include <KIdentityManagement/Identity>
#include <KIdentityManagement/IdentityManager>
#include <QFile>
#include <QTextStream>


KNode::Settings::Settings() : SettingsBase()
{
  // KConfigXT doesn't seem to support labels for parameterized fields
  quoteColorItem( 0 )->setLabel( i18n("Quoted Text - First level") );
  quoteColorItem( 1 )->setLabel( i18n("Quoted Text - Second level") );
  quoteColorItem( 2 )->setLabel( i18n("Quoted Text - Third level") );
}

void KNode::Settings::usrReadConfig( )
{
  // read extra header configuration
  const QString dir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QString::fromLatin1("/knode/");
  if ( !dir.isNull() ) {
    QFile f( dir + "xheaders" );
    if ( f.open( QIODevice::ReadOnly ) ) {
      mXHeaders.clear();
      QTextStream ts( &f );
      while ( !ts.atEnd() )
        mXHeaders.append( XHeader( ts.readLine() ) );
      f.close();
    }
  }
}

bool KNode::Settings::usrWriteConfig( )
{
  // write extra header configuration
  const QString dir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QString::fromLatin1("/knode/");
  if ( dir.isNull() ) {
    KNHelper::displayInternalFileError();
    return false;
  } else {
    QFile f( dir + "xheaders" );
    if ( f.open( QIODevice::WriteOnly ) ) {
      QTextStream ts( &f );
      for ( XHeader::List::Iterator it = mXHeaders.begin(); it != mXHeaders.end(); ++it )
        ts << (*it).header() << "\n";
      ts.flush();
      f.close();
    } else {
      KNHelper::displayInternalFileError();
      return false;
    }
  }
  return true;
}

QColor KNode::Settings::effectiveColor( KConfigSkeleton::ItemColor * item ) const
{
  if ( useCustomColors() )
    return item->value();
  item->swapDefault();
  QColor rv = item->value();
  item->swapDefault();
  return rv;
}

QFont KNode::Settings::effectiveFont( KConfigSkeleton::ItemFont * item ) const
{
  if ( useCustomFonts() )
    return item->value();
  item->swapDefault();
  QFont rv = item->value();
  item->swapDefault();
  return rv;
}

const KIdentityManagement::Identity & KNode::Settings::identity() const
{
  return KNGlobals::self()->identityManager()->identityForUoidOrDefault( SettingsBase::identity() );
}

void KNode::Settings::setIdentity( const KIdentityManagement::Identity &identity )
{
  SettingsBase::setIdentity( identity.uoid() );
}
