/*
    KNode, the KDE newsreader
    Copyright (c) 2005 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, US
*/

#ifndef KNODE_ARTICLEWIDGET_H
#define KNODE_ARTICLEWIDGET_H

#include "knarticle.h"
#include "knarticlecollection.h"
#include "knjobdata.h"

class QAction;
class KActionCollection;
class KActionMenu;
class KHTMLPart;
class KSelectAction;
class KToggleAction;
class KXMLGUIClient;

namespace KNode {

/**
  Widget to display a news article.
*/
class ArticleWidget : public QWidget, public KNJobConsumer {

  Q_OBJECT

  public:
    /// Construct a new article widget.
    ArticleWidget( QWidget *parent,
                   KXMLGUIClient *guiClient,
                   KActionCollection *actionCollection, bool isMainViewer = false );
    /// Destroy the article widget.
    ~ArticleWidget();

    /// read config settings
    void readConfig();
    /// write config settings (call only for the main viewer)
    void writeConfig();

  protected:
    /// process download jobs for view source action
    void processJob( KNJobData *j );

  private:
    void initActions();

    /// enable article dependent actions
    void enableActions();
    /// disable article dependent actions
    void disableActions();
//
    /// displays the current article or clears the view if no article is set
    void displayArticle();

    /// display the message header (should be replaced by KMail's HeaderStyle class)
    void displayHeader();

    /// HTML conversion flags for toHtmlString()
    enum ConversionFlags {
      None = 0,
      ParseURL = 1,
      FancyFormatting = 2
    };
    /// convert the given string into a HTML string
    QString toHtmlString( const QString &line, int flags = ParseURL );

    /** Checks if the given charset is supported.
     * @param charset The charset to check.
     */
    bool canDecodeText( const QByteArray &charset ) const;


  private slots:
    /// called if the user clicked on an URL
    void slotURLClicked( const KUrl &url, bool forceOpen = false );

    void slotReply();
    void slotRemail();
    void slotForward();
    void slotCancel();
    void slotSupersede();
    void slotToggleFancyFormating();

    void slotSetCharset( const QString &charset );
    void slotSetCharsetKeyboard();
  private:
    /// the currently shown article
    KNArticle::Ptr mArticle;

    KHTMLPart *mViewer;
    QString mAttachmentStyle;
    bool mForceCharset;
    QByteArray mOverrideCharset;

    /// the last RMB clicked URL
    KUrl mCurrentURL;

    /// list of all instances of this class
    static QList<ArticleWidget*> mInstances;
    /**
      Indicates if this ArticleWidget is the main one (displayed in KNMainWidget)
    */
    bool mIsMainViewer;

    KXMLGUIClient *mGuiClient;
    KActionCollection *mActionCollection;

    QAction *mCopySelectionAction;
    QAction *mSelectAllAction;
    QAction *mCharsetSelectKeyb;
    QAction *mReplyAction;
    QAction *mRemailAction;
    QAction *mForwardAction;
    QAction *mCancelAction;
    QAction *mSupersedeAction;
    KActionMenu *mAttachmentStyleMenu;
    KToggleAction *mFancyToggle;
    KSelectAction *mCharsetSelect;
};

}

#endif
