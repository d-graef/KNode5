// -*- c-basic-offset: 2 -*-
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

#include "knarticlewindow.h"

#include <QDebug>

#include "articlewidget.h"
#include "knode_debug.h"
#include "utilities.h"
#include "knglobals.h"
#include "knmainwidget.h"

#include <kwindowsystem.h>
#include <kstandardaction.h>
#include <kconfig.h>
#include <QAction>
#include <kactioncollection.h>

using namespace KNode;

QList<KNode::ArticleWindow*> ArticleWindow::mInstances;

void ArticleWindow::closeAllWindows()
{
  ArticleWindow::List list = mInstances;
  Q_FOREACH(ArticleWindow* win, list) {
    win->close();
  }
}


bool ArticleWindow::closeAllWindowsForCollection( KNArticleCollection::Ptr col, bool force )
{
  ArticleWindow::List list = mInstances;
  for ( ArticleWindow::List::Iterator it = list.begin(); it != list.end(); ++it )
    if ( (*it)->mArticleWidget->article() && (*it)->mArticleWidget->article()->collection() == col ) {
      if ( force )
        (*it)->close();
      else
        return false;
    }
  return true;
}


bool ArticleWindow::closeAllWindowsForArticle( KNArticle::Ptr art, bool force )
{
  ArticleWindow::List list = mInstances;
  for ( ArticleWindow::List::Iterator it = list.begin(); it != list.end(); ++it )
    if ( (*it)->mArticleWidget->article() && (*it)->mArticleWidget->article() == art ) {
      if ( force )
        (*it)->close();
      else
        return false;
    }
  return true;
}


bool ArticleWindow::raiseWindowForArticle( KNArticle::Ptr art )
{
  for ( ArticleWindow::List::Iterator it = mInstances.begin(); it != mInstances.end(); ++it )
    if ( (*it)->mArticleWidget->article() && (*it)->mArticleWidget->article() == art ) {
#ifdef Q_OS_UNIX
      KWindowSystem::activateWindow( (*it)->winId() );
#endif
      return true;
    }
  return false;
}


bool ArticleWindow::raiseWindowForArticle( const QByteArray &mid )
{
  for ( ArticleWindow::List::Iterator it = mInstances.begin(); it != mInstances.end(); ++it )
    if ( (*it)->mArticleWidget->article() &&
           (*it)->mArticleWidget->article()->messageID()->as7BitString( false ) == mid ) {
#ifdef Q_OS_UNIX
      KWindowSystem::activateWindow( (*it)->winId() );
#endif
      return true;
    }

  return false;
}


//==================================================================================

ArticleWindow::ArticleWindow( KNArticle::Ptr art )
  : KXmlGuiWindow( 0 )
{
  setObjectName( "articleWindow" );
  qCDebug(KNODE_LOG) << "Port";
#if 0
  if ( knGlobals.componentData().isValid() )
    setComponentData( knGlobals.componentData() );
#endif

  if ( art )
    setCaption( art->subject()->asUnicodeString() );

  mArticleWidget = new ArticleWidget( this, this, actionCollection() );
  mArticleWidget->setArticle( art );
  setCentralWidget( mArticleWidget );

  mInstances.append( this );

  // file menu
  KStandardAction::close( this, SLOT(close()), actionCollection() );

  // settings menu
  KStandardAction::preferences(knGlobals.top, SLOT(slotSettings()), actionCollection());

  setupGUI( ToolBar|Keys|Create, "knreaderui.rc");

  resize(500,400);    // default optimized for 800x600
  applyMainWindowSettings(KConfigGroup( knGlobals.config(), "articleWindow_options") );

  // this will enable keyboard-only actions like that don't appear in any menu
  actionCollection()->addAssociatedWidget( this );
  foreach (QAction* action, actionCollection()->actions())
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
}


ArticleWindow::~ArticleWindow()
{
  mInstances.removeAll( this );
  qCDebug(KNODE_LOG) << "Port";
#if 0
  saveMainWindowSettings(knGlobals.config()->group( "articleWindow_options") );
#endif
}

//--------------------------------

