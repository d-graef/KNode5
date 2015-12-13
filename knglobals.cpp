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

#include "knglobals.h"

#include "knarticlefactory.h"
#include "knconfigmanager.h"
#include "knaccountmanager.h"
#include "kngroupmanager.h"
#include "knarticlemanager.h"
#include "knfiltermanager.h"
#include "knfoldermanager.h"
#include "knmemorymanager.h"
#include "knmainwidget.h"
#include "knode_debug.h"
#include "scheduler.h"
#include "settings.h"

#include <kconfig.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <KIdentityManagement/IdentityManager>


class KNGlobalsPrivate
{
  public:
    KNGlobals instance;
};

Q_GLOBAL_STATIC( KNGlobalsPrivate, kNGlobalsPrivate )


KNGlobals::KNGlobals() :
  mScheduler( 0 ),
  mCfgManager( 0 ),
  mAccManager( 0 ),
  mGrpManager( 0 ),
  mArtManager( 0 ),
  mFilManager( 0 ),
  mFolManager( 0 ),
  mMemManager( 0 ),
  mSettings( 0 ),
  mArticleFactory( 0 ),
  mIdentityManager( 0 )
{
  qCDebug(KNODE_LOG);
}

KNGlobals::~KNGlobals( )
{
  qCDebug(KNODE_LOG);
  mIdentityManager->deleteLater();
  delete mSettings;
}


KNGlobals * KNGlobals::self()
{
  Q_ASSERT ( !kNGlobalsPrivate.isDestroyed() );

  return &kNGlobalsPrivate->instance;
}


const KComponentData &KNGlobals::componentData() const
{
  if ( mInstance.isValid() )
    return mInstance;
  return KGlobal::mainComponent();
}


KConfig* KNGlobals::config()
{
  if (!c_onfig) {
      c_onfig = KSharedConfig::openConfig( "knoderc" );
  }
  return c_onfig.data();
}

KNConfigManager* KNGlobals::configManager()
{
  if (!mCfgManager)
    mCfgManager = new KNConfigManager();
  return mCfgManager;
}

KNode::Scheduler* KNGlobals::scheduler()
{
  if ( !mScheduler )
    mScheduler = new KNode::Scheduler();
  return mScheduler;
}

KNAccountManager* KNGlobals::accountManager()
{
  if(!mAccManager)
    mAccManager = new KNAccountManager(groupManager());
  return mAccManager;
}

KNGroupManager* KNGlobals::groupManager()
{
  if(!mGrpManager)
    mGrpManager = new KNGroupManager();
  return mGrpManager;
}

KNArticleManager* KNGlobals::articleManager()
{
  if(!mArtManager)
    mArtManager = new KNArticleManager();
  return mArtManager;
}

KNArticleFactory* KNGlobals::articleFactory()
{
  if ( !mArticleFactory ) {
    mArticleFactory = new KNArticleFactory();
  }
  return mArticleFactory;
}

KNFilterManager* KNGlobals::filterManager()
{
  if (!mFilManager)
    mFilManager = new KNFilterManager();
  return mFilManager;
}

KNFolderManager* KNGlobals::folderManager()
{
  if(!mFolManager)
    mFolManager = new KNFolderManager(articleManager());
  return mFolManager;
}

KNMemoryManager* KNGlobals::memoryManager()
{
  if(!mMemManager)
    mMemManager = new KNMemoryManager();
  return mMemManager;
}

KIdentityManagement::IdentityManager* KNGlobals::identityManager()
{
  if ( !mIdentityManager ) {
    mIdentityManager = new KIdentityManagement::IdentityManager( false, 0, "mIdentityManager" );
  }
  return mIdentityManager;
}


void KNGlobals::setStatusMsg(const QString &text, int id)
{
  if(top)
    top->setStatusMsg(text, id);
}

KNode::Settings * KNGlobals::settings( )
{
  if ( !mSettings ) {
    mSettings = new KNode::Settings();
    mSettings->readConfig();
  }
  return mSettings;
}

void KNGlobals::reset()
{
  delete mMemManager;
  mMemManager = 0;
  qCDebug(KNODE_LOG) << "Memory Manager deleted";

  delete mFolManager;
  mFolManager = 0;
  qCDebug(KNODE_LOG) << "Folder Manager deleted";

  delete mFilManager;
  mFilManager = 0;
  qCDebug(KNODE_LOG) << "Filter Manager deleted";

  delete mArtManager;
  mArtManager = 0;
  qCDebug(KNODE_LOG) << "Article Manager deleted";

  delete mGrpManager;
  mGrpManager = 0;
  qCDebug(KNODE_LOG) << "Group Manager deleted";

  delete mAccManager;
  mAccManager = 0;
  qCDebug(KNODE_LOG) << "Account Manager deleted";

  delete mCfgManager;
  mCfgManager = 0;
  qCDebug(KNODE_LOG) << "Config deleted";

  delete mScheduler;
  mScheduler = 0;
  qCDebug(KNODE_LOG) << "Scheduler deleted";

  delete mArticleFactory;
  mArticleFactory = 0;
  qCDebug(KNODE_LOG) <<" Article Factory deleted";
}
