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

#include "headers_widget.h"

#include <KDE/KFilterProxySearchLine>
#include <QtCore/QSortFilterProxyModel>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QVBoxLayout>

#include "headers_model.h"
#include "headers_view.h"

#include "knarticlemanager.h"
#include "knglobals.h"
#include "settings.h"

namespace KNode {
namespace MessageList {

HeadersWidget::HeadersWidget(QWidget* parent)
  : QWidget(parent)
{
    mView = new HeadersView(this);

    mModel = new HeadersModel(mView);
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(mView);
    proxyModel->setSourceModel(mModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setSortLocaleAware(true);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setSortRole(HeadersModel::SortRole);
    mView->setModel(proxyModel);

    mSearch = new KFilterProxySearchLine(this);
    mSearch->setProxy(proxyModel);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(mSearch);
    layout->addWidget(mView);
    layout->setSpacing(0);
    layout->setMargin(0);

    mView->setSortingEnabled(true);

    connect(KNGlobals::self()->articleManager(), SIGNAL(collectionChanged(KNArticleCollection::Ptr)),
            this, SLOT(showCollection(KNArticleCollection::Ptr)));
    connect(KNGlobals::self()->articleManager(), SIGNAL(filterChanged(KNArticleFilter*)),
            this, SLOT(setFilter(KNArticleFilter*)));
    connect(KNGlobals::self()->articleManager(), SIGNAL(articlesChanged(KNArticle::List,bool)),
            mModel, SLOT(changedArticles(KNArticle::List,bool)));
    connect(mView, SIGNAL(articlesSelected(const KNArticle::List)),
            this, SIGNAL(articlesSelected(const KNArticle::List)));
    connect(mView->header(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
            this, SLOT(sortingChanged(int,Qt::SortOrder)));
    connect(this, SIGNAL(showThreads(bool)),
            mModel, SLOT(showThreads(bool)));
    connect(mView, SIGNAL(contextMenuRequest(KNArticle::Ptr,QPoint)),
            this, SIGNAL(contextMenuRequest(KNArticle::Ptr,QPoint)));

    mView->setExpandsOnDoubleClick(false);
    connect(mView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(viewDoubleClicked(QModelIndex)));
}

HeadersWidget::~HeadersWidget()
{
}

void HeadersWidget::readConfig()
{
    Settings* settings = KNGlobals::self()->settings();
    toggleSearch(settings->showHeadersSearchLine());
    mView->readConfig();
    // After "mView->readConfig()", otherwise it will call sortingChanged (via the sortIndicatorChanged() signal
    // when restoring the state of the view; thus changing the value of sortByThreadChangeDate..
    mModel->setSortedByThreadChangeDate(KNGlobals::self()->settings()->sortByThreadChangeDate());
}
void HeadersWidget::writeConfig()
{
    Settings* settings = KNGlobals::self()->settings();
    settings->setShowHeadersSearchLine(isSearchShown());
    settings->setSortByThreadChangeDate(mModel->sortedByThreadChangeDate());
    mView->writeConfig();
}


void HeadersWidget::toggleSearch(bool show)
{
    mSearch->setVisible(show);
}
bool HeadersWidget::isSearchShown() const
{
    return mSearch->isVisible();
}


void HeadersWidget::sortingChanged(int logicalIndex, Qt::SortOrder order)
{
    if(logicalIndex == HeadersModel::COLUMN_DATE && order == Qt::AscendingOrder) {
        mModel->setSortedByThreadChangeDate(!mModel->sortedByThreadChangeDate());
    }
    emit sortingChanged(logicalIndex);
}

int HeadersWidget::sortColumn() const
{
    return mView->header()->sortIndicatorSection();
}
void HeadersWidget::setSorting(int section)
{
    mView->header()->setSortIndicator(section, mView->header()->sortIndicatorOrder());
}



void HeadersWidget::showCollection(const KNArticleCollection::Ptr collection)
{
    mModel->setCollection(collection);
    if(KNGlobals::self()->settings()->defaultToExpandedThreads()) {
        expandAllThreads();
    }
}

void HeadersWidget::setFilter(KNArticleFilter* filter)
{
    mModel->setFilter(filter);
}

bool HeadersWidget::selectNextUnreadMessage()
{
    return mView->selectNextUnread();
}

bool HeadersWidget::selectNextUnreadThread()
{
    return mView->selectNextUnreadThread();
}

void HeadersWidget::selectPreviousMessage()
{
    mView->selectPreviousMessage();
}

void HeadersWidget::selectNextMessage()
{
    mView->selectNextMessage();
}


template<class A>
static void addRemoteArticle(QList<boost::shared_ptr<A> >& res, const QModelIndex& index)
{
    const boost::shared_ptr<A> art = boost::dynamic_pointer_cast<A>(
                                                index.data(HeadersModel::ArticleRole).value<KNArticle::Ptr>()
                                        );
    if(art) {
        res << art;
    }
}

template<class A>
static void selectedMessages(HeadersView* view, QList<boost::shared_ptr<A> >& res)
{
    const QModelIndexList selection = view->selectionModel()->selectedRows();
    Q_FOREACH(const QModelIndex& index, selection) {
        addRemoteArticle<A>(res, index);
    }
}

void HeadersWidget::getSelectedMessages(KNRemoteArticle::List& l)
{
    selectedMessages<KNRemoteArticle>(mView, l);
}
void HeadersWidget::getSelectedMessages(KNLocalArticle::List& l)
{
    selectedMessages<KNLocalArticle>(mView, l);
}


/* Helper method for #getSelectedThreads().
   Recursively add children of parent into res. */
static void childrenArticles(KNRemoteArticle::List& res, const QModelIndex& parent)
{
    addRemoteArticle<KNRemoteArticle>(res, parent);

    int row = 0;
    QModelIndex child;
    while(true) {
        child = parent.child(row, parent.column());
        if(child.isValid()) {
            childrenArticles(res, child);
        } else {
            break;
        }
        ++row;
    }
}

KNRemoteArticle::List HeadersWidget::getSelectedThreads()
{
    KNRemoteArticle::List res;

    const QModelIndexList selection = mView->selectionModel()->selectedRows();
    QModelIndexList topLevelParents;
    QModelIndex i;
    Q_FOREACH(const QModelIndex& index, selection) {
        i = index;
        while(i.parent().isValid()) {
            i = i.parent();
        }
        if(!topLevelParents.contains(i)) {
            topLevelParents << i;
        }
    }

    Q_FOREACH(const QModelIndex& index, topLevelParents) {
        childrenArticles(res, index);
    }

    return res;
}


void HeadersWidget::collapseAllThreads()
{
    mView->collapseAll();

    // Ensure the current index is visible.
    const QModelIndex index = mView->currentIndex();
    if(index.isValid()) {
        mView->scrollTo(index, QAbstractItemView::PositionAtCenter);
    }
}

void HeadersWidget::expandAllThreads()
{
    mView->expandAll();

    const QModelIndex index = mView->currentIndex();
    if(index.isValid()) {
        mView->scrollTo(index, QAbstractItemView::PositionAtCenter);
    }
}

void HeadersWidget::toggleCurrentItemExpansion()
{
    const QModelIndex index = mView->currentIndex();
    if(index.isValid()) {
        mView->setExpanded(index, !mView->isExpanded(index));
    }
}

void HeadersWidget::collapseCurrentThread()
{
    QModelIndex index = mView->currentIndex();
    if(index.isValid()) {
        while(index.parent().isValid()) {
            index = index.parent();
        }
        mView->setCurrentIndex(index);
        mView->collapse(index);
        mView->scrollTo(index, QAbstractItemView::EnsureVisible);
    }
}


void HeadersWidget::viewDoubleClicked(const QModelIndex& index)
{
    if(index.isValid()) {
        QVariant v = mView->model()->data(index, HeadersModel::ArticleRole);
        if(v.isValid()) {
            emit doubleClicked(v.value<KNArticle::Ptr>());
        }
    }
}


}
}
