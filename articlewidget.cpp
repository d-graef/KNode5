
#include "articlewidget.h"

#include "knarticle.h"
#include "knarticlecollection.h"
#include "knarticlefactory.h"
#include "knarticlemanager.h"
#include "knconfig.h"
#include "knconfigmanager.h"
#include "kndisplayedheader.h"
#include "knfolder.h"
#include "knfoldermanager.h"
#include "knglobals.h"
#include "kngroup.h"
#include "knmainwidget.h"
#include "knnntpaccount.h"
#include "knode_debug.h"
#include "knsourceviewwindow.h"
#include "nntpjobs.h"
#include "settings.h"
#include "utils/locale.h"


using namespace KNode;

ArticleWidget::ArticleWidget( QWidget *parent,
                              KXMLGUIClient *guiClient,
                              KActionCollection *actionCollection, bool isMainViewer ) :
  QWidget( parent ),
  mViewer( 0 ),
  mForceCharset( false ),
  mOverrideCharset( "iso-8859-1" ),
  mIsMainViewer( isMainViewer ),
  mGuiClient( guiClient ),
  mActionCollection( actionCollection )
{
  QHBoxLayout *box = new QHBoxLayout( this );
  mViewer = new KHTMLPart( this );
  box->addWidget( mViewer->widget() );
  mViewer->setPluginsEnabled( false );
  mViewer->setJScriptEnabled( false );
  mViewer->setJavaEnabled( false );
  mViewer->setMetaRefreshEnabled( false );
  mViewer->setOnlyLocalReferences( true );
  mViewer->view()->setFocusPolicy( Qt::WheelFocus );
  connect( mViewer->browserExtension(), SIGNAL(openUrlRequestDelayed(KUrl,KParts::OpenUrlArguments,KParts::BrowserArguments)),
           SLOT(slotURLClicked(KUrl)) );

  initActions();
  readConfig();
}


ArticleWidget::~ArticleWidget()
{
  if ( mArticle && mArticle->isOrphant() ) {
    // if the article manager is still loading the current article,
    // cancel the job.
    knGlobals.articleManager()->cancelJobs( mArticle );
  }
}


void ArticleWidget::initActions()
{
  mReplyAction = mActionCollection->addAction("article_postReply");
  mReplyAction->setIcon(KIcon("mail-reply-all"));
  mReplyAction->setText(i18n("&Followup to Newsgroup..."));
  connect(mReplyAction, SIGNAL(triggered(bool)), SLOT(slotReply()));
  mReplyAction->setShortcut(QKeySequence(Qt::Key_R));
  mRemailAction = mActionCollection->addAction("article_mailReply" );
  mRemailAction->setIcon(KIcon("mail-reply-sender"));
  mRemailAction->setText(i18n("Reply by E&mail..."));
  connect(mRemailAction, SIGNAL(triggered(bool)), SLOT(slotRemail()));
  mRemailAction->setShortcut(QKeySequence(Qt::Key_A));
  mForwardAction = mActionCollection->addAction("article_forward");
  mForwardAction->setIcon(KIcon("mail-forward"));
  mForwardAction->setText(i18n("Forw&ard by Email..."));
  connect(mForwardAction, SIGNAL(triggered(bool)), SLOT(slotForward()));
  mForwardAction->setShortcut(QKeySequence(Qt::Key_F));
  mCancelAction = mActionCollection->addAction("article_cancel");
  mCancelAction->setText(i18nc("article", "&Cancel Article"));
  connect(mCancelAction, SIGNAL(triggered(bool)), SLOT(slotCancel()));
  mSupersedeAction = mActionCollection->addAction("article_supersede");
  mSupersedeAction->setText(i18n("S&upersede Article"));
  connect(mSupersedeAction, SIGNAL(triggered(bool)), SLOT(slotSupersede()));
  mFancyToggle = mActionCollection->add<KToggleAction>("view_fancyFormating");
  mFancyToggle->setText(i18n("Fancy Formatting"));
  connect(mFancyToggle, SIGNAL(triggered(bool)), SLOT(slotToggleFancyFormating()));
  mFancyToggle->setShortcut(QKeySequence(Qt::Key_Y));


  mCharsetSelect = mActionCollection->add<KSelectAction>("set_charset");
  mCharsetSelect->setText( i18n( "Set chars&et" ) );
  QStringList cs = Utilities::Locale::encodings();
  cs.prepend( i18nc( "@item default character set", "Default") );
  mCharsetSelect->setItems( cs );
  mCharsetSelect->setCurrentItem( 0 );
  connect( mCharsetSelect, SIGNAL(triggered(QString)),SLOT(slotSetCharset(QString)) );
  mCharsetSelectKeyb = mActionCollection->addAction("set_charset_keyboard");
  mCharsetSelectKeyb->setText( i18n( "Set charset" ) );
  connect(mCharsetSelectKeyb, SIGNAL(triggered(bool)), SLOT(slotSetCharsetKeyboard()));
  mCharsetSelectKeyb->setShortcut(QKeySequence(Qt::Key_C));
}



void ArticleWidget::enableActions()
{
  if ( !mArticle ) {0
    disableActions();
    return;
  }

  mCopySelectionAction->setEnabled( true );
  mSelectAllAction->setEnabled( true );
  mFindAction->setEnabled( true );
  mForwardAction->setEnabled( true );
  mCharsetSelect->setEnabled( true );
  mCharsetSelectKeyb->setEnabled( true );
  mFancyToggle->setEnabled( true );

  // only valid for remote articles
  bool enabled = ( mArticle->type() == KNArticle::ATremote );
  mReplyAction->setEnabled( enabled );
  mRemailAction->setEnabled( enabled );

  enabled = ( mArticle->type() == KNArticle::ATremote
    || mArticle->collection() == knGlobals.folderManager()->sent() );
  mCancelAction->setEnabled( enabled );
  mSupersedeAction->setEnabled( enabled );
}


void ArticleWidget::disableActions()
{
  mCopySelectionAction->setEnabled( false );
  mSelectAllAction->setEnabled( false );
  mFindAction->setEnabled( false );
  mReplyAction->setEnabled( false );
  mRemailAction->setEnabled( false );
  mForwardAction->setEnabled( false );
  mCancelAction->setEnabled( false );
  mSupersedeAction->setEnabled( false );
  mCharsetSelect->setEnabled( false );
  mCharsetSelectKeyb->setEnabled( false );
  mFancyToggle->setEnabled( false );
}



void ArticleWidget::readConfig()
{
  mFancyToggle->setChecked( knGlobals.settings()->interpretFormatTags() );

  mViewer->setOnlyLocalReferences( !knGlobals.settings()->allowExternalReferences() );

}


void ArticleWidget::writeConfig()
{
  // main viewer determines the settings
  if ( !mIsMainViewer ) {
    return;
  }

  knGlobals.settings()->setInterpretFormatTags( mFancyToggle->isChecked() );
}

void ArticleWidget::displayArticle()
{
  if ( !mArticle) {
    clear();
    return;
  }

  if ( mForceCharset != mArticle->forceDefaultCharset()
       || ( mForceCharset && mArticle->defaultCharset() != mOverrideCharset ) ) {
        mArticle->setDefaultCharset( mOverrideCharset );
        mArticle->setForceDefaultCharset( mForceCharset );
  }

  mViewer->begin();

  // headers
  displayHeader();

  // body
  QString html;
  KMime::Content *text = 0;
  if ( !text )
    text = mArticle->textContent();

  // check if codec is available
  if ( text && !canDecodeText( text->contentType()->charset() ) ) {
    html += QString("<table width=\"100%\" border=\"0\"><tr><td bgcolor=\"#FF0000\">%1</td></tr></table>")
      .arg( i18n("Unknown charset. Default charset is used instead.") );
    qCDebug(KNODE_LOG) <<"unknown charset =" << text->contentType()->charset();
  }

  mViewer->write ( html );
  html.clear();

  KMime::Headers::ContentType *ct = mArticle->contentType();

  // get attachments
  mAttachments.clear();
  if( !text || ct->isMultipart() )
    mAttachments = mArticle->attachments( knGlobals.settings()->showAlternativeContents() );

  // partial message
  if(ct->isPartial()) {
    mViewer->write( i18n("<br /><b>This article has the MIME type &quot;message/partial&quot;, which KNode cannot handle yet.<br />Meanwhile you can save the article as a text file and reassemble it by hand.</b>") );
  }

  // display body text
  if ( text && text->hasContent() && !ct->isPartial() ) {
      QString htmlTxt = text->decodedText( true, knGlobals.settings()->removeTrailingNewlines() );
        html += "<div class=\"htmlWarn\">\n";
        html += i18n("<b>Note:</b> This is an HTML message. For "
                     "security reasons, only the raw HTML code "
                     "is shown. If you trust the sender of this "
                     "message then you can activate formatted "
                     "HTML display for this message "
                     "<a href=\"knode:showHTML\">by clicking here</a>.");
        html += "</div><br><br>";
        html += toHtmlString( htmlTxt );
  }
  mViewer->write( html );

  enableActions();
}



void ArticleWidget::displayHeader()
{
  QString headerHtml;

  // standard & fancy header style
  KMime::Headers::Base *hb;
  KNDisplayedHeader::List dhs = knGlobals.configManager()->displayedHeaders()->headers();
  foreach ( KNDisplayedHeader *dh, dhs) {
    hb = mArticle->headerByType(dh->header().toLatin1());
    if ( !hb || hb->is("Subject") || hb->is("Organization") )
      continue;

    if ( dh->hasName() ) {
      headerHtml += "<tr>";
      headerHtml += "<th align=\"right\">";
      headerHtml += toHtmlString( dh->translatedName(), None );
      headerHtml += ":</th><td width=\"100%\">";
    }

    if ( hb->is("From") ) {
      headerHtml += QString( "<a href=\"mailto:%1\">%2</a>")
          .arg( KPIMUtils::extractEmailAddress( hb->asUnicodeString() ) )
          .arg( toHtmlString( hb->asUnicodeString(), None ) );
      KMime::Headers::Base *orgHdr = mArticle->headerByType( "Organization" );
      if ( orgHdr && !orgHdr->isEmpty() ) {
        headerHtml += "&nbsp;&nbsp;(";
        headerHtml += toHtmlString( orgHdr->asUnicodeString() );
        headerHtml += ')';
      }
    } else if ( hb->is("Date") ) {
      KMime::Headers::Date *date=static_cast<KMime::Headers::Date*>(hb);
      headerHtml += toHtmlString( KLocale::global()->formatDateTime(date->dateTime().toLocalZone().dateTime(), KLocale::LongDate, true), None );
    } else if ( hb->is("Newsgroups") ) {
      QString groups = hb->asUnicodeString();
      groups.replace( ',', ", " );
      headerHtml += toHtmlString( groups, ParseURL );
    } else
      headerHtml += toHtmlString( hb->asUnicodeString(), ParseURL );

  }

}


void ArticleWidget::displayBodyBlock( const QStringList &lines )
{

   // FIXME: A SUPPRIMER

  QString quoteChars = knGlobals.settings()->quoteCharacters().simplified();
  if (quoteChars.isEmpty())
    quoteChars = '>';

    if ( knGlobals.settings()->showSignature() ) {
        html += "<hr size=\"1\"/>";
    }
  }

}

QString ArticleWidget::toHtmlString( const QString &line, int flags )
{
  int llflags = KPIMUtils::LinkLocator::PreserveSpaces;
  if ( !(flags & ArticleWidget::ParseURL) )
    llflags |= KPIMUtils::LinkLocator::IgnoreUrls;
  if ( mFancyToggle->isChecked() && (flags & ArticleWidget::FancyFormatting) )
    llflags |= KPIMUtils::LinkLocator::ReplaceSmileys |
               KPIMUtils::LinkLocator::HighlightText;
  QString text = line;
  return KPIMUtils::LinkLocator::convertToHtml( text, llflags );
}


bool ArticleWidget::canDecodeText( const QByteArray &charset ) const
{
  qCDebug(KNODE_LOG) << charset;
  if ( charset.isEmpty() )
    return false;
  bool ok = true;
  KCharsets::charsets()->codecForName( charset, ok );
  return ok;
}

void ArticleWidget::processJob( KNJobData * job )
{
  if ( job->type() == KNJobData::JTfetchSource || job->type() == KNJobData::JTfetchArticle ) {
    if ( !job->canceled() ) {
      if ( !job->success() )
        KMessageBox::error( this, i18n("An error occurred while downloading the article source:\n%1",
            job->errorString() ) );
      else {
        KNRemoteArticle::Ptr a = boost::static_pointer_cast<KNRemoteArticle>( job->data() );
        new KNSourceViewWindow( a->head() + QLatin1Char('\n') + a->body() );
      }
    }
  }
  delete job;
}


void ArticleWidget::slotURLClicked( const KUrl &url, bool forceOpen)
{
  // handle mailto
  if ( url.protocol() == "mailto" ) {
    KMime::Types::Mailbox addr;
    addr.fromUnicodeString( url.path() );
    KNGlobals::self()->articleFactory()->createMail( &addr );
    return;
  }
  // handle news URL's
  if ( url.protocol() == "news" ) {
    qCDebug(KNODE_LOG) << url;
    knGlobals.top->openURL( url );
    return;
  }

        // let KDE take care of the remaining protocols (http, ftp, etc.)
  new KRun( url, this );
}

void ArticleWidget::slotViewSource()
{
  // local article can be shown directly
  if ( mArticle && mArticle->type() == KNArticle::ATlocal && mArticle->hasContent() ) {
    new KNSourceViewWindow( mArticle->encodedContent( false ) );
  } else {
    // download remote article
    if ( mArticle && mArticle->type() == KNArticle::ATremote ) {
      KNGroup::Ptr g = boost::static_pointer_cast<KNGroup>( mArticle->collection() );
      KNRemoteArticle::Ptr a = KNRemoteArticle::Ptr( new KNRemoteArticle( g ) ); //we need "g" to access the nntp-account
      a->messageID( true )->from7BitString( mArticle->messageID()->as7BitString( false ) );
      a->lines( true )->from7BitString( mArticle->lines( true )->as7BitString( false ) );
      a->setArticleNumber( boost::static_pointer_cast<KNRemoteArticle>( mArticle )->articleNumber() );
      emitJob( new ArticleFetchJob( this, g->account(), a, false ) );
    }
  }
}


void ArticleWidget::slotReply()
{
  if ( mArticle && mArticle->type() == KNArticle::ATremote )
    KNGlobals::self()->articleFactory()->createReply( boost::static_pointer_cast<KNRemoteArticle>( mArticle ),
                                                      mViewer->selectedText(), true, false );
}


void ArticleWidget::slotRemail()
{
  if ( mArticle && mArticle->type()==KNArticle::ATremote )
    KNGlobals::self()->articleFactory()->createReply( boost::static_pointer_cast<KNRemoteArticle>( mArticle ),
                                                      mViewer->selectedText(), false, true );
}


void ArticleWidget::slotForward()
{
  KNGlobals::self()->articleFactory()->createForward( mArticle );
}


void ArticleWidget::slotCancel()
{
  KNGlobals::self()->articleFactory()->createCancel( mArticle );
}


void ArticleWidget::slotSupersede()
{
  KNGlobals::self()->articleFactory()->createSupersede( mArticle );
}



void ArticleWidget::slotToggleFancyFormating( )
{
  writeConfig();
  updateContents();
}


void ArticleWidget::slotSetCharset( const QString &charset )
{
  if ( charset.isEmpty() )
    return;

  if ( charset == i18nc( "@item default character set", "Default") ) {
    mForceCharset = false;
    mOverrideCharset = "iso-8859-1";
  } else {
    mForceCharset = true;
    mOverrideCharset = KCharsets::charsets()->encodingForName( charset ).toLatin1();
  }

  if ( mArticle && mArticle->hasContent() ) {
    mArticle->setDefaultCharset( mOverrideCharset );  // the article will choose the correct default,
    mArticle->setForceDefaultCharset( mForceCharset );     // when we disable the overdrive
  }
}


void ArticleWidget::slotSetCharsetKeyboard( )
{
  int charset = KNHelper::selectDialog( this, i18n("Select Charset"),
    mCharsetSelect->items(), mCharsetSelect->currentItem() );
  if ( charset != -1 ) {
    mCharsetSelect->setCurrentItem( charset );
    slotSetCharset( mCharsetSelect->items()[charset] );
  }
}

