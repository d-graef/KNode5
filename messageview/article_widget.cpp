/*
 * Copyright (c) 2015 Olivier Trichet <olivier@trichet.fr>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "article_widget.h"

#include <KI18n/KLocalizedString>
#include <KWidgetsAddons/KToggleAction>
#include <KXmlGui/KActionCollection>
#include <KXmlGui/KXMLGUIClient>
#include <KXmlGui/KXMLGUIFactory>
#include <Libkdepim/AddEmailAddressJob>
#include <Libkdepim/OpenEmailAddressJob>
#include <MessageViewer/Viewer>
#include <QtWidgets/QLayout>
#include <QtWidgets/QMenu>
#include <QtCore/QTimer>

#include "instances.h"
#include "knarticlemanager.h"
#include "knglobals.h"
#include "knode_debug.h"
#include "settings.h"

namespace KNode {
namespace MessageView {

ArticleWidget::ArticleWidget(QWidget* parent, KXMLGUIClient* guiClient, bool isMainViewer)
    : QWidget(parent),
      mGuiClient(guiClient), mPopupUrl()
{
    mViewer = new MessageViewer::Viewer(this, Q_NULLPTR, guiClient->actionCollection());
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(mViewer);

    initActions();

    Instances::mInstances.append(this);

    mReadTimer = new QTimer(this);
    mReadTimer->setSingleShot(true);
    connect(mReadTimer, SIGNAL(timeout()),
            this, SLOT(slotMarkAsRead()));

    readConfig();
}

ArticleWidget::~ArticleWidget()
{
    mReadTimer->stop();
    Instances::mInstances.removeAll(this);
    delete mViewer;
}


void ArticleWidget::readConfig()
{
    Settings* settings = KNGlobals::self()->settings();
    bool showHtml = settings->alwaysShowHTML();
    mViewer->setDisplayFormatMessageOverwrite(showHtml ? MessageViewer::Viewer::Html
                                                       : MessageViewer::Viewer::Text);
    mViewer->toggleFixFontAction()->setChecked(settings->useFixedFont());

    if(!settings->autoMark()) {
        mReadTimer->stop();
    }

    mViewer->readConfig();

    // TODO
}
void ArticleWidget::writeConfig()
{
    Settings* settings = KNGlobals::self()->settings();
    settings->setUseFixedFont(mViewer->toggleFixFontAction()->isChecked());

    mViewer->writeConfig();

    // TODO
}

void ArticleWidget::initActions()
{
    KActionCollection* ac = mGuiClient->actionCollection();

    KStandardAction::print(this, SLOT(slotPrint()), ac);

    QAction* action;
    action= ac->action("save_message");
    if(action) {
        ac->setDefaultShortcut(action, QKeySequence(Qt::CTRL + Qt::Key_S));
    }
    action = ac->action("mark_all_text");
    if(action) {
        ac->setDefaultShortcut(action, QKeySequence(Qt::CTRL + Qt::Key_A));
    }

    connect(mViewer, SIGNAL(popupMenu(Akonadi::Item,QUrl,QUrl,QPoint)),
            this, SLOT(slotPopup(Akonadi::Item,QUrl,QUrl,QPoint)));
    action = ac->addAction("add_addr_book");
    action->setText(i18n("&Add to Address Book"));
    connect(action, SIGNAL(triggered(bool)),
            this, SLOT(slotAddToAddressBook()));
    action = ac->addAction("openin_addr_book");
    action->setText(i18n("&Open in Address Book"));
    connect(action, SIGNAL(triggered(bool)),
            this, SLOT(slotOpenInAddressBook()));
}



void ArticleWidget::setArticle(KNArticle::Ptr article)
{
    mReadTimer->stop();

    mViewer->enableMessageDisplay();


    // Keep a reference on this object to pass it safely to the viewer
    mArticle = article;

    if(!mArticle || mArticle->hasContent()) {
        setViewMessage(article);
    } else {
        setViewMessage(nullptr);
        if(!KNGlobals::self()->articleManager()->loadArticle( mArticle )) {
            displayErrorMessage(i18n("Unable to load the article."));
            mArticle.reset();
        }
    }
}

void ArticleWidget::updateContents()
{
    if(mArticle.get() == mViewer->message().data()) {
        mViewer->update();
    } else {
        setViewMessage(mArticle);
    }
}

static void noop(void *)
{
}

void ArticleWidget::setViewMessage(KNArticle::Ptr article)
{
    // Use a no-op deleter: this QSharedPointer must not delete the obj guarded by a boost::shared_ptr.
    QSharedPointer<KMime::Message> msg(article.get(), noop);
    mViewer->setMessage(msg);

    Settings* settings = KNGlobals::self()->settings();
    if(mArticle && settings->autoMark()) {
        mReadTimer->start(settings->autoMarkSeconds() * 1000);
    }

}



void ArticleWidget::displayErrorMessage(const QString& error) const
{
    mViewer->displaySplashPage(QStringLiteral("knode_error.html"),
                               { { QStringLiteral("errorMessage"), error } });
}


void ArticleWidget::atBottom()
{
    mViewer->atBottom();
}
void ArticleWidget::scrollNext()
{
    mViewer->slotScrollNext();
}


void ArticleWidget::slotPrint()
{
    mViewer->print();
}

void ArticleWidget::slotPopup(const Akonadi::Item&, const QUrl& url, const QUrl& imageUrl, const QPoint& mousePos)
{
    qCDebug(KNODE_LOG) << url << imageUrl << mousePos;

    QString name;
    if(url.isEmpty()) {
        name = "body_popup";
    } else if(url.scheme() == "mailto") {
        name = "mailto_popup";
    } else {
        name = "url_popup";
    }

    mPopupUrl = url;

    QMenu *popup = static_cast<QMenu*>(mGuiClient->factory()->container(name , mGuiClient));
    if(popup) {
        popup->popup(mousePos);
    }
}

void ArticleWidget::slotAddToAddressBook()
{
    KPIM::AddEmailAddressJob* job = new KPIM::AddEmailAddressJob(mPopupUrl.path(), this, this);
    job->start();
}

void ArticleWidget::slotOpenInAddressBook()
{
    KPIM::OpenEmailAddressJob* job = new KPIM::OpenEmailAddressJob(mPopupUrl.path(), this, this);
    job->start();
}

void ArticleWidget::slotMarkAsRead()
{
    if(mArticle && mArticle->type() == KNArticle::ATremote) {
        KNRemoteArticle::List list;
        list.append(boost::static_pointer_cast<KNRemoteArticle>(mArticle));
        KNGlobals::self()->articleManager()->setRead(list, true);
    }
}


}
}
