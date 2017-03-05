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

#ifndef KNODE_MESSAGEVIEW_ARTICLEWIDGET_H
#define KNODE_MESSAGEVIEW_ARTICLEWIDGET_H

#include <QtWidgets/QWidget>

#include "knarticle.h"

class KActionCollection;
class KXMLGUIClient;
namespace Akonadi {
    class Item;
}
namespace MessageViewer {
    class Viewer;
}

namespace KNode {
namespace MessageView {

class ArticleWidget : public QWidget
{
    Q_OBJECT

    public:
        ArticleWidget(QWidget* parent, KXMLGUIClient* guiClient, bool isMainViewer = false );
        ~ArticleWidget();

        void readConfig();
        void writeConfig();

        /**
         * Called when the article should be reloaded.
         */
        void updateContents();
        /**
         * Display an error message.
         */
        void displayErrorMessage(const QString& error) const;

        void atBottom();
        void scrollNext();

        /** Change the article to display. */
        void setArticle(KNArticle::Ptr article);
        /** The displayed article. */
        KNArticle::Ptr article() const
        {
            return mArticle;
        }

    private Q_SLOTS:
        void slotPrint();
        void slotPopup(const Akonadi::Item&, const QUrl& url, const QUrl& imageUrl, const QPoint& mousePos);
        void slotAddToAddressBook();
        void slotOpenInAddressBook();
        void slotMarkAsRead();

    private:
        KNArticle::Ptr mArticle;
        MessageViewer::Viewer* mViewer;
        KXMLGUIClient* mGuiClient;
        QUrl mPopupUrl;
        QTimer *mReadTimer;

        void initActions();
        void setViewMessage(KNArticle::Ptr article);
};


}
}

#endif
