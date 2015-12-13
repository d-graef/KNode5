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

#ifndef KNODE_HEADERS_VIEW_H
#define KNODE_HEADERS_VIEW_H

#include <QtWidgets/QTreeView>

#include "knarticle.h"

namespace KNode {
namespace MessageList {

class HeadersView : public QTreeView
{
    Q_OBJECT

    public:
        explicit HeadersView(QWidget* parent = 0);
        virtual ~HeadersView();

        void readConfig();
        void writeConfig();

        /**
         * Select the next unread article.
         * @return @code false if no unread article is found.
         */
        bool selectNextUnread();
        /**
         * Select the next unread article in the next threads.
         * @return @code false if no unread article is found.
         */
        bool selectNextUnreadThread();

        void selectPreviousMessage();
        void selectNextMessage();

    protected Q_SLOTS:
        virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

    private Q_SLOTS:
        void expandChildren(const QModelIndex& index);
        void contextMenu(const QPoint& point);

    Q_SIGNALS:
        void articlesSelected(const KNArticle::List articles);
        void contextMenuRequest(KNArticle::Ptr article, const QPoint& point);

    private:
        QModelIndex find(const QModelIndex& from, int role, const QVariant& value);
};

}
}

#endif
