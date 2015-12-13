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

#ifndef KNODE_HEADERS_WIDGET_H
#define KNODE_HEADERS_WIDGET_H

#include <QtCore/QModelIndex>
#include <QtCore/QPoint>
#include <QtWidgets/QWidget>

#include "knarticle.h"
#include "knarticlecollection.h"

class KFilterProxySearchLine;
class KNArticleFilter;

namespace KNode {
namespace MessageList {

class HeadersModel;
class HeadersView;

class HeadersWidget : public QWidget
{
    Q_OBJECT

    public:
        explicit HeadersWidget(QWidget* parent = 0);
        ~HeadersWidget();

        void readConfig();
        void writeConfig();

        /**
         * Indicate if the search line is shown.
         */
        bool isSearchShown() const;

        /**
         * Change the sorting column.
         * @param i Index of the column section.
         */
        void setSorting(int section);
        /**
         * Returns the section that is currently sorted.
         */
        int sortColumn() const;

        /**
         * Select the next unread article.
         * @return @code false if no unread article is found.
         */
        bool selectNextUnreadMessage();
        /**
         * Select the first unread article starting from the next thread.
         * @return @code false if no unread article is found.
         */
        bool selectNextUnreadThread();

        /**
         * Select the previous message.
         */
        void selectPreviousMessage();
        /**
         * Select the next message.
         */
        void selectNextMessage();

        /**
         * Returns the list of currently selected articles.
         */
        void getSelectedMessages(KNRemoteArticle::List& l);
        /**
         * Returns the list of currently selected articles.
         */
        void getSelectedMessages(KNLocalArticle::List& l);

        /**
         * Returns the list of threads containing selected articles.
         */
        KNRemoteArticle::List getSelectedThreads();

    public Q_SLOTS:
        void showCollection(const KNArticleCollection::Ptr collection);
        void setFilter(KNArticleFilter* filter);

        /**
         * Display/hide the search line.
         */
        void toggleSearch(bool show);

        /**
         * Collapse all threads. Keep the current article visible.
         */
        void collapseAllThreads();
        /**
         * Expand all threads.
         */
        void expandAllThreads();
        /**
         * Collapse/Expand the thread below the current article.
         */
        void toggleCurrentItemExpansion();
        /**
         * Collapse the current thread.
         */
        void collapseCurrentThread();

    private Q_SLOTS:
        void sortingChanged(int logicalIndex, Qt::SortOrder order);
        void viewDoubleClicked(const QModelIndex& index);

    Q_SIGNALS:
        void articlesSelected(const KNArticle::List article);
        void showThreads(bool b);
        void doubleClicked(KNArticle::Ptr article);
        void contextMenuRequest(KNArticle::Ptr article, const QPoint& point);
        void sortingChanged(int section);

    private:
      KFilterProxySearchLine* mSearch;
      HeadersView* mView;
      HeadersModel* mModel;
};

}
}

#endif
