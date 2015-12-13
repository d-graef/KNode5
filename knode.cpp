/*
    KNode, the KDE newsreader
    Copyright (c) 1999-2010 the KNode authors.
    See file AUTHORS for details

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, US
*/
#include "knode.h"

#include <KDE/KGlobal>
#include <Libkdepim/BroadcastStatus>
#include <Libkdepim/ProgressDialog>
#include <Libkdepim/StatusbarProgressWidget>
#include <QApplication>
#include <QtWidgets/QStatusBar>

#include <kshortcutsdialog.h>
#include <kedittoolbar.h>
#include <kstandardaction.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kactioncollection.h>
#include <ksqueezedtextlabel.h>

#include "kncollectionviewitem.h"
#include "knglobals.h"
#include "knmainwidget.h"
#include "knode_debug.h"

KNMainWindow::KNMainWindow( QWidget* parent )
  : KXmlGuiWindow( parent )
{
  createStandardStatusBarAction();
  setStandardToolBarMenuEnabled(true);

  //config stuff
  KStandardAction::quit(qApp, SLOT(closeAllWindows()), actionCollection());
  KStandardAction::configureToolbars(this, SLOT(slotConfToolbar()), actionCollection());
  KStandardAction::keyBindings(this, SLOT(slotConfKeys()), actionCollection());

  m_mainWidget = new KNMainWidget( this, this );
  connect( m_mainWidget, SIGNAL(signalCaptionChangeRequest(QString)),
           SLOT(setCaption(QString)) );
  setCentralWidget( m_mainWidget );
  setupStatusBar();
  connect( KPIM::BroadcastStatus::instance(), SIGNAL(statusMsg(QString)),
    this, SLOT(slotShowStatusMsg(QString)) );
  createGUI( "knodeui.rc" );
  knGlobals.setComponentData( KComponentData() );

  applyMainWindowSettings(KGlobal::config()->group( "mainWindow_options") );
}

KNMainWindow::~KNMainWindow()
{
  qCDebug(KNODE_LOG) << "Port";
#if 0
  saveMainWindowSettings(knGlobals.config()->group( "mainWindow_options") );
#endif
}


void KNMainWindow::slotConfToolbar()
{
  qCDebug(KNODE_LOG) << "Port";
#if 0
  saveMainWindowSettings(knGlobals.config()->group( "mainWindow_options") );
#endif
  KEditToolBar dlg( actionCollection() );
  dlg.setResourceFile( "knodeui.rc" );
  connect(&dlg,SIGNAL(newToolBarConfig()), this, SLOT(slotNewToolbarConfig()));
  dlg.exec();
}

void KNMainWindow::slotNewToolbarConfig()
{
  createGUI("knodeui.rc");
  //initPopups();
  applyMainWindowSettings(knGlobals.config()->group( "mainWindow_options") );
}

void KNMainWindow::slotConfKeys()
{
  KShortcutsDialog::configure(actionCollection(), KShortcutsEditor::LetterShortcutsAllowed);
}

bool KNMainWindow::queryClose()
{
  return m_mainWidget->queryClose();
}

void KNMainWindow::setupStatusBar()
{
  mProgressDialog = new KPIM::ProgressDialog( statusBar(), this );
  mProgressDialog->hide();

  mLittleProgress = new StatusbarProgressWidget( mProgressDialog, statusBar() );
  mLittleProgress->show();

  statusBar()->addPermanentWidget( mLittleProgress, 0 );

  mStatusMsgLabel = new KSqueezedTextLabel( QString(), statusBar() );
  mStatusMsgLabel->setTextElideMode( Qt::ElideRight );
  mStatusMsgLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  statusBar()->addWidget( mStatusMsgLabel, 2 );
  statusBar()->addWidget(m_mainWidget->statusBarLabelFilter(), 2);
  statusBar()->addWidget(m_mainWidget->statusBarLabelGroup(), 3);
}

void KNMainWindow::slotShowStatusMsg( const QString &msg ) {
  mStatusMsgLabel->setText( msg );
}

