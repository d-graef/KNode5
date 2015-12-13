/*
    KNode, the KDE newsreader
    Copyright (c) 1999-2006 the KNode authors.
    See file AUTHORS for details

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, US
*/

#include "knarticlemanager.h"

#include <krun.h>
#include <kmessagebox.h>
#include <kmimetypetrader.h>
#include <klocale.h>
#include <kwindowsystem.h>
#include <QDir>
#include <QTemporaryFile>

#include "knmainwidget.h"
#include "knglobals.h"
#include "knode_debug.h"
#include "messageview/instances.h"
#include "utilities.h"
#include "kngroupmanager.h"
#include "knsearchdialog.h"
#include "knfiltermanager.h"
#include "knfolder.h"
#include "knarticlefilter.h"
#include "scheduler.h"
#include "knnntpaccount.h"
#include "knmemorymanager.h"
#include "knarticlefactory.h"
#include "knarticlewindow.h"
#include "knfoldermanager.h"
#include "nntpjobs.h"
#include "settings.h"
#include "utils/scoped_cursor_override.h"

using namespace KNode;
using namespace KNode::Utilities;


KNArticleManager::KNArticleManager() : QObject(0)
{
  f_ilterMgr = knGlobals.filterManager();
  f_ilter = f_ilterMgr->currentFilter();
  s_earchDlg=0;

  connect(f_ilterMgr, SIGNAL(filterChanged(KNArticleFilter*)), this,
    SLOT(slotFilterChanged(KNArticleFilter*)));
}


KNArticleManager::~KNArticleManager()
{
  delete s_earchDlg;
}


void KNArticleManager::showHdrs()
{
  if(!g_roup && !f_older) return;

  ScopedCursorOverride cursor( Qt::WaitCursor );
  knGlobals.setStatusMsg(i18n(" Creating list..."));

  KNArticleCollection::Ptr col;
  if(g_roup) {
    col = g_roup;
  } else if (f_older) {
    col = f_older;
  }
  emit collectionChanged(col);

  knGlobals.setStatusMsg( QString() );
  updateStatusString();
}


void KNArticleManager::updateViewForCollection( KNArticleCollection::Ptr c )
{
  if(g_roup==c || f_older==c)
    showHdrs();
}


void KNArticleManager::search()
{
  if(s_earchDlg) {
    s_earchDlg->show();
#ifdef Q_OS_UNIX
    KWindowSystem::activateWindow(s_earchDlg->winId());
#endif
  } else {
    s_earchDlg = new SearchDialog( SearchDialog::STgroupSearch, 0 );
    connect(s_earchDlg, SIGNAL(doSearch(KNArticleFilter*)), this,
      SLOT(slotFilterChanged(KNArticleFilter*)));
    connect(s_earchDlg, SIGNAL(dialogDone()), this,
      SLOT(slotSearchDialogDone()));
    s_earchDlg->show();
  }
}


void KNArticleManager::setGroup( KNGroup::Ptr g )
{
  g_roup = g;
  if ( g )
    emit aboutToShowGroup();
}


void KNArticleManager::setFolder( KNFolder::Ptr f )
{
  f_older = f;
  if ( f )
    emit aboutToShowFolder();
}


KNArticleCollection::Ptr KNArticleManager::collection()
{
  if(g_roup)
    return g_roup;
  if(f_older)
   return f_older;

  return KNArticleCollection::Ptr();
}


bool KNArticleManager::loadArticle( KNArticle::Ptr a )
{
  if (!a)
    return false;

  if (a->hasContent())
    return true;

  if (a->isLocked()) {
    if ( a->type() == KNArticle::ATremote )
      return true;   // locked == we are already loading this article...
    else
      return false;
  }

  if ( a->type() == KNArticle::ATremote ) {
    KNGroup::Ptr g = boost::static_pointer_cast<KNGroup>( a->collection() );
    if(g)
      emitJob( new ArticleFetchJob( this, g->account(), a ) );
    else
      return false;
  }
  else { // local article
    KNFolder::Ptr f = boost::static_pointer_cast<KNFolder>( a->collection() );
   if( f && f->loadArticle( boost::static_pointer_cast<KNLocalArticle>( a ) ) )
      knGlobals.memoryManager()->updateCacheEntry(a);
    else
      return false;
  }
  return true;
}


bool KNArticleManager::unloadArticle( KNArticle::Ptr a, bool force )
{
  if(!a || a->isLocked() )
    return false;
  if(!a->hasContent())
    return true;

  if (!force && a->isNotUnloadable())
    return false;

  if ( !force && ( MessageView::Instances::articleVisible( a ) ) )
    return false;

  if (!force && ( a->type()== KNArticle::ATlocal ) &&
      ( KNGlobals::self()->articleFactory()->findComposer( boost::static_pointer_cast<KNLocalArticle>( a ) ) != 0 ) )
    return false;

  if ( !ArticleWindow::closeAllWindowsForArticle( a, force ) )
    if (!force)
      return false;

  MessageView::Instances::articleRemoved( a );
  if ( a->type() != KNArticle::ATlocal )
    KNGlobals::self()->articleFactory()->deleteComposerForArticle( boost::static_pointer_cast<KNLocalArticle>( a ) );

  knGlobals.memoryManager()->removeCacheEntry(a);

  return true;
}


void KNArticleManager::copyIntoFolder( KNArticle::List &l, KNFolder::Ptr f )
{
  if(!f) return;

  KNLocalArticle::Ptr loc;
  KNLocalArticle::List l2;

  for ( KNArticle::List::Iterator it = l.begin(); it != l.end(); ++it ) {
    if ( !(*it)->hasContent() )
      continue;
    loc = KNLocalArticle::Ptr( new KNLocalArticle( KNArticleCollection::Ptr() ) );
    loc->setEditDisabled(true);
    loc->setContent( (*it)->encodedContent() );
    loc->parse();
    l2.append(loc);
  }

  if ( !l2.isEmpty() ) {

    f->setNotUnloadable(true);

    if ( !f->isLoaded() && !knGlobals.folderManager()->loadHeaders( f ) ) {
      l2.clear();
      f->setNotUnloadable(false);
      return;
    }

    if( !f->saveArticles( l2 ) ) {
      for ( KNLocalArticle::List::Iterator it = l2.begin(); it != l2.end(); ++it ) {
        if ( (*it)->isOrphant() )
          (*it).reset(); // ok, this is ugly; we simply delete orphant articles
        else
          (*it)->KMime::Content::clear(); // no need to keep them in memory
      }
      KNHelper::displayInternalFileError();
    } else {
      for ( KNLocalArticle::List::Iterator it = l2.begin(); it != l2.end(); ++it )
        (*it)->KMime::Content::clear(); // no need to keep them in memory
      knGlobals.memoryManager()->updateCacheEntry( boost::static_pointer_cast<KNArticleCollection>( f ) );
    }

    f->setNotUnloadable(false);
  }
}


void KNArticleManager::moveIntoFolder( KNLocalArticle::List &l, KNFolder::Ptr f )
{
  if(!f) return;
  qCDebug(KNODE_LOG) <<" Target folder:" << f->name();

  f->setNotUnloadable(true);

  if (!f->isLoaded() && !knGlobals.folderManager()->loadHeaders(f)) {
    f->setNotUnloadable(false);
    return;
  }

  if ( f->saveArticles( l ) ) {
    for ( KNLocalArticle::List::Iterator it = l.begin(); it != l.end(); ++it )
      knGlobals.memoryManager()->updateCacheEntry( boost::static_pointer_cast<KNArticle>(*it) );
    knGlobals.memoryManager()->updateCacheEntry( boost::static_pointer_cast<KNArticleCollection>( f ) );
  } else {
    for ( KNLocalArticle::List::Iterator it = l.begin(); it != l.end(); ++it )
      if ( (*it)->isOrphant() )
        (*it).reset(); // ok, this is ugly; we simply delete orphant articles
    KNHelper::displayInternalFileError();
  }

  f->setNotUnloadable(false);
}


bool KNArticleManager::deleteArticles(KNLocalArticle::List &l, bool ask)
{
  if(ask) {
    QStringList lst;
    for ( KNLocalArticle::List::Iterator it = l.begin(); it != l.end(); ++it ) {
      if ( (*it)->isLocked() )
        continue;
      if ( (*it)->subject()->isEmpty() )
        lst << i18n("no subject");
      else
        lst << (*it)->subject()->asUnicodeString();
    }
    if( KMessageBox::Cancel == KMessageBox::warningContinueCancelList(
      knGlobals.topWidget, i18n("Do you really want to delete these articles?"), lst,
        i18n("Delete Articles"), KGuiItem(i18n("&Delete"),"edit-delete")) )
      return false;
  }

  for ( KNLocalArticle::List::Iterator it = l.begin(); it != l.end(); ++it )
    knGlobals.memoryManager()->removeCacheEntry( boost::static_pointer_cast<KNArticle>(*it) );

  KNFolder::Ptr f = boost::static_pointer_cast<KNFolder>( l.first()->collection() );
  if ( f ) {
    f->removeArticles( l, true );
    knGlobals.memoryManager()->updateCacheEntry( boost::static_pointer_cast<KNArticleCollection>( f ) );
    return false; // composers for those articles were already removed in removeArticles
  } else {
    l.clear();
  }

  return true;
}


void KNArticleManager::setAllRead( bool read, int lastcount )
{
  if ( !g_roup )
    return;

  int groupLength = g_roup->length();
  int newCount = g_roup->newCount();
  int readCount = g_roup->readCount();
  int offset = lastcount;

  if ( lastcount > groupLength || lastcount < 0 )
    offset = groupLength;

  KNRemoteArticle::Ptr a;
  for ( int i = groupLength - offset; i < groupLength; ++i ) {
    a = g_roup->at( i );
    if ( a->getReadFlag() != read && !a->isIgnored() ) {
      a->setRead( read );
      a->setChanged( true );
      if ( !read ) {
        readCount--;
        if ( a->isNew() )
          newCount++;
      } else {
        readCount++;
        if ( a->isNew() )
          newCount--;
      }
    }
  }

  g_roup->updateThreadInfo();
  if ( lastcount < 0 && read ) {
    // HACK: try to hide the effects of the ignore/filter new/unread count bug
    g_roup->setReadCount( groupLength );
    g_roup->setNewCount( 0 );
  } else {
    g_roup->setReadCount( readCount );
    g_roup->setNewCount( newCount );
  }

  g_roup->updateListItem();
  showHdrs();
}


void KNArticleManager::setRead(KNRemoteArticle::List &l, bool r, bool handleXPosts)
{
  if ( l.isEmpty() )
    return;

  KNArticle::List changed;
  KNGroup::Ptr g = boost::static_pointer_cast<KNGroup>( l.first()->collection() );

  for ( KNRemoteArticle::List::Iterator it = l.begin(); it != l.end(); ++it ) {
    if( r && knGlobals.settings()->markCrossposts() &&
        handleXPosts && (*it)->newsgroups()->isCrossposted() ) {

      QVector<QByteArray> groups = (*it)->newsgroups()->groups();
      KNGroup::Ptr targetGroup;
      KNRemoteArticle::Ptr xp;
      KNRemoteArticle::List al;
      QByteArray mid = (*it)->messageID()->as7BitString( false );

      Q_FOREACH(const QByteArray& group, groups) {
        targetGroup = knGlobals.groupManager()->group(group, g->account());
        if (targetGroup) {
          if (targetGroup->isLoaded() && (xp=targetGroup->byMessageId(mid)) ) {
            al.clear();
            al.append(xp);
            setRead(al, r, false);
          } else {
            targetGroup->appendXPostID(mid);
          }
        }
      }
    }
    else if ( (*it)->getReadFlag() != r ) {
      (*it)->setRead( r );
      (*it)->setChanged( true );
      changed << (*it);

      if ( !(*it)->isIgnored() ) {
        if(r) {
          g->incReadCount();
          if ( (*it)->isNew() )
            g->decNewCount();
        }
        else {
          g->decReadCount();
          if ( (*it)->isNew() )
            g->incNewCount();
        }
      }
    }
  }

  if(!changed.isEmpty()) {
    g->updateListItem();
    if(g==g_roup)
      updateStatusString();

    emit articlesChanged(changed);
  }
}


void KNArticleManager::setAllNotNew()
{
  if ( !g_roup )
    return;
  KNRemoteArticle::Ptr a;
  for ( int i = 0; i < g_roup->length(); ++i) {
    a = g_roup->at(i);
    if ( a->isNew() ) {
      a->setNew( false );
      a->setChanged( true );
    }
  }
  g_roup->setFirstNewIndex( -1 );
  g_roup->setNewCount( 0 );
  g_roup->updateThreadInfo();
}


bool KNArticleManager::toggleWatched(KNRemoteArticle::List &l)
{
  if(l.isEmpty())
    return true;

  KNArticle::List changed;
  KNRemoteArticle::Ptr a = l.first();
  bool watch = (!a->isWatched());
  KNGroup::Ptr g = boost::static_pointer_cast<KNGroup>( a->collection() );

  for ( KNRemoteArticle::List::Iterator it = l.begin(); it != l.end(); ++it ) {
    if ( (*it)->isIgnored() ) {
      (*it)->setIgnored(false);

      if ( !(*it)->getReadFlag() ) {
        g->decReadCount();
        if ( (*it)->isNew() )
          g->incNewCount();
      }
    }

    (*it)->setWatched( watch );
    (*it)->setChanged( true );
    changed << (*it);
  }

  if(!changed.isEmpty()) {
    g->updateListItem();
    if(g==g_roup)
      updateStatusString();

    emit articlesChanged(changed);
  }

  return watch;
}


bool KNArticleManager::toggleIgnored(KNRemoteArticle::List &l)
{
  if(l.isEmpty())
    return true;

  KNArticle::List changed;
  bool ignore = !l.first()->isIgnored();
  KNGroup::Ptr g = boost::static_pointer_cast<KNGroup>( l.first()->collection() );

  for ( KNRemoteArticle::List::Iterator it = l.begin(); it != l.end(); ++it ) {
    (*it)->setWatched(false);
    if ( (*it)->isIgnored() != ignore ) {
      (*it)->setIgnored( ignore );

      if ( !(*it)->getReadFlag() ) {
        if ( ignore ) {
          g->incReadCount();
          if ( (*it)->isNew() )
            g->decNewCount();
        } else {
          g->decReadCount();
          if ( (*it)->isNew() )
            g->incNewCount();
        }

      }
    }

    (*it)->setChanged(true);
    changed << (*it);
  }

  if(!changed.isEmpty()) {
    g->updateListItem();
    if(g==g_roup)
      updateStatusString();

    emit articlesChanged(changed);
  }

  return ignore;
}


void KNArticleManager::notifyArticleChanged(KNArticle::Ptr a, bool deleted)
{
    KNArticle::List l;
    l << a;
    emit articlesChanged(l, deleted);
}


void KNArticleManager::processJob(KNJobData *j)
{
  if(j->type()==KNJobData::JTfetchArticle && !j->canceled()) {
    KNRemoteArticle::Ptr a = boost::static_pointer_cast<KNRemoteArticle>( j->data() );
    if(j->success()) {
      MessageView::Instances::articleChanged( a );
      if(!a->isOrphant()) //orphant articles are deleted by the displaying widget
        knGlobals.memoryManager()->updateCacheEntry( boost::static_pointer_cast<KNArticle>( a ) );

      notifyArticleChanged(a);
    } else {
      if ( j->error() == KIO::ERR_DOES_NOT_EXIST ) {
        MessageView::Instances::articleLoadError( a, i18n("The article you requested is not available on your news server.") );
        // mark article as read
        if ( knGlobals.settings()->autoMark() && !a->isOrphant() ) {
          KNRemoteArticle::List l;
          l.append( a );
          setRead( l, true );
        }
      } else
        MessageView::Instances::articleLoadError( a, j->errorString() );
    }
  }

  delete j;
}


void KNArticleManager::updateStatusString()
{
  int displCnt=0;

  if(g_roup) {
    if(f_ilter)
      displCnt=f_ilter->count();
    else
      displCnt=g_roup->count();

    QString name = g_roup->name();
    if (g_roup->status()==KNGroup::moderated)
      name += i18n(" (moderated)");

    knGlobals.setStatusMsg(i18n(" %1: %2 new , %3 displayed",
                         name, g_roup->newCount(), displCnt),SB_GROUP);

    if(f_ilter)
      knGlobals.setStatusMsg(i18n(" Filter: %1", f_ilter->translatedName()), SB_FILTER);
    else
      knGlobals.setStatusMsg( QString(), SB_FILTER );
  }
  else if(f_older) {
    if(f_ilter)
      displCnt=f_ilter->count();
    else
      displCnt=f_older->count();
    knGlobals.setStatusMsg(i18n(" %1: %2 displayed",
       f_older->name(), displCnt), SB_GROUP);
    knGlobals.setStatusMsg( QString(), SB_FILTER );
  } else {
    knGlobals.setStatusMsg( QString(), SB_GROUP );
    knGlobals.setStatusMsg( QString(), SB_FILTER );
  }
}


void KNArticleManager::slotFilterChanged(KNArticleFilter *f)
{
  f_ilter=f;
  emit filterChanged(f);
  showHdrs();
}


void KNArticleManager::slotSearchDialogDone()
{
  s_earchDlg->hide();
  slotFilterChanged(f_ilterMgr->currentFilter());
}


//-----------------------------
