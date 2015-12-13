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


#include "headers_model.h"

#include <KI18n/KLocalizedString>
#include <KIconThemes/KIconLoader>
#include <QDebug>
#include <QFont>

#include "knarticlefilter.h"
#include "knconfigmanager.h"
#include "knfiltermanager.h"
#include "knode_debug.h"
#include "settings.h"

namespace KNode
{
namespace MessageList
{

static const int INVALID_ID = -1;

struct Header
{
    Header(KNArticle::Ptr a)
        : article(a), parent(0), children(),
          subThreadDate( !a ? 0 : a->date()->dateTime().toMSecsSinceEpoch() ),
          unreadFollowup(false)
    {
    };
    ~Header()
    {
        article.reset();
        parent = 0;
        qDeleteAll(children);
        children.clear();
    };
    void addChild(Header* hdr)
    {
        hdr->parent = this;
        children.append(hdr);

        // Update subThreadDate
        Header* h = hdr;
        while(h->parent && h->parent->subThreadDate < h->subThreadDate) {
            h->parent->subThreadDate = h->subThreadDate;
            h = h->parent;
        }
        // Update unreadFollowup
        KNRemoteArticle::Ptr r = boost::dynamic_pointer_cast<KNRemoteArticle>(hdr->article);
        if(r && !r->isRead()) {
            h = hdr;
            while(h->parent) {
                h = h->parent;
                if(!h->unreadFollowup) {
                    h->unreadFollowup = true;
                } else {
                    break;
                }
            }
        }
    };

    KNArticle::Ptr article;
    Header* parent;
    QList<Header*> children;
    qint64 subThreadDate; // The most recent date below this header (included)
    bool unreadFollowup;
};

static QVariant extractFrom(const KNArticle::Ptr& art)
{
    KMime::Headers::From* from = art->from();
    if(from && !from->mailboxes().isEmpty()) {
        const KMime::Types::Mailbox mb = from->mailboxes().first();
        if(mb.hasName()) {
            return mb.name();
        } else {
            return mb.prettyAddress(KMime::Types::Mailbox::QuoteNever);
        }
    }
    return QVariant();
}


HeadersModel::HeadersModel(QObject* parent)
    : QAbstractItemModel(parent),
      mRoot(new Header(KNArticle::Ptr())),
      mCollection(),
      mSortByThreadChangeDate(false)
{
    mDateFormatter.setCustomFormat(KNGlobals::self()->settings()->customDateFormat());
    mDateFormatter.setFormat(KNGlobals::self()->settings()->dateFormat());

    mFilter = KNGlobals::self()->filterManager()->currentFilter();

    mNormlizeSubject = new QRegExp("^\\s*(?:tr|re[.f]?|fwd?)\\s*:\\s*", Qt::CaseInsensitive);
}

HeadersModel::~HeadersModel()
{
    delete mRoot;
    mCollection.reset();
}

void HeadersModel::setFilter(KNArticleFilter* filter)
{
    mFilter = filter;
    reload(mCollection);
}

void HeadersModel::setCollection(const KNArticleCollection::Ptr collection)
{
    if(collection != mCollection) {
        reload(collection);
    }
}

void HeadersModel::showThreads(bool b)
{
    KNGlobals::self()->settings()->setShowThreads(b);
    reload(mCollection);
}



/**
 * Recursively search articles from @p articles and
 * - emit #dataChanged(QModelIndex).
 * - or remove them from the internal model.
 */
void HeadersModel::modifyInternal(KNArticle::List& articles, Header* parent, bool deletion)
{
    if(articles.isEmpty()) {
        return;
    }

    for(int r = 0 ; r < parent->children.length() ; ++r) {
        Header* child = parent->children.at(r);
        int idx = articles.indexOf(child->article);
        if(idx != -1) {
            if(deletion) {
                const QModelIndex parentIdx = (parent->parent ? createIndex(parent->parent->children.indexOf(parent), 0, parent) : QModelIndex());
                beginRemoveRows(parentIdx, r, r);
                parent->children.removeAt(r);
                delete child;
                endRemoveRows();
            } else {
                refreshInternal(child, r);
            }

            articles.removeAt(idx);
            if(articles.isEmpty()) {
                return;
            }
        }
        if(!child->children.isEmpty()) {
            modifyInternal(articles, child, deletion);
        }
    }
}

/**
 * Update the internal model when a KNArticle has changed.
 */
void HeadersModel::refreshInternal(Header* hdr, int row)
{
    Header *parent = hdr->parent;
    if(!parent) {
        // Reach the root
        return;
    }

    if(row < 0) {
        row = parent->children.indexOf(hdr);
    }

    emit dataChanged(createIndex(row, COLUMN_SUBJECT, hdr),
                     createIndex(row, COLUMN_DATE, hdr));

    if(!hdr->unreadFollowup) { // if current header still has unread children, its parent too.
        if(hdr->article->type() == KNArticle::ATremote) {
            Q_FOREACH(Header* child, parent->children) {
                const KNRemoteArticle::Ptr remote = boost::dynamic_pointer_cast<KNRemoteArticle>(child->article);
                if(parent->unreadFollowup != (remote && !remote->isRead())) {
                    parent->unreadFollowup = !parent->unreadFollowup;
                    refreshInternal(parent);
                    break;
                }
            }
        }
    }
}

void HeadersModel::changedArticles(const KNArticle::List articles, bool deleted)
{
    if(articles.isEmpty() || articles.first()->collection() != mCollection) {
        return;
    }

    KNArticle::List arts = articles;
    modifyInternal(arts, mRoot, deleted);
}



void HeadersModel::reload(const KNArticleCollection::Ptr collection)
{
    Header* root = new Header(KNArticle::Ptr());
    QHash<QByteArray, Header*> msgIdIndex;

    if(collection) {
        if(mFilter) {
            switch(collection->type()) {
                case KNCollection::CTgroup:
                    mFilter->doFilter(boost::dynamic_pointer_cast<KNGroup>(collection));
                    break;
                case KNCollection::CTfolder:
                    mFilter->doFilter(boost::dynamic_pointer_cast<KNFolder>(collection));
                    break;
                default:
                    qCCritical(KNODE_LOG) << "Invalid collection type:" << collection->type();
                    break;
            }
        }

        bool showThreads = KNGlobals::self()->settings()->showThreads()
                            && (collection->type() == KNCollection::CTgroup);

        if(showThreads) {
            for(int i = 0 ; i < collection->length() ; ++i) {
                const KNArticle::Ptr art = collection->at(i);
                if(art->filterResult()) {
                    msgIdIndex.insert(art->messageID()->as7BitString(false), new Header(art));
                }
            }

            Q_FOREACH(Header* hdr, msgIdIndex) {
                Header* parent = root;
                KMime::Headers::References* refs = hdr->article->references();
                if(refs && !refs->identifiers().isEmpty()) {
                    const QByteArray parentMsgId = '<' + refs->identifiers().last() + '>';
                    parent = msgIdIndex.value(parentMsgId, root);
                }
                parent->addChild(hdr);
            }
        } else {
            for(int i = 0 ; i < collection->length() ; ++i) {
                const KNArticle::Ptr art = collection->at(i);
                if(art->filterResult()) {
                    Header* hdr = new Header(art);
                    root->addChild(hdr);
                }
            }
        }
    }

    Header* oldRoot = mRoot;
    beginResetModel();
    mRoot = root;
    mCollection = collection;
    endResetModel();
    delete oldRoot;
}


int HeadersModel::rowCount(const QModelIndex& parent) const
{
    Header* p = parent.isValid() ? static_cast<Header*>(parent.internalPointer()) : mRoot;
    return p->children.count();
}

int HeadersModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return COLUMN_COUNT;
}

QVariant HeadersModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }

    const Header* hdr = static_cast<Header*>(index.internalPointer());
    const KNArticle::Ptr art = hdr->article;
    const KNRemoteArticle::Ptr remoteArt = boost::dynamic_pointer_cast<KNRemoteArticle>(art);

    switch(role) {
    case Qt::DisplayRole:
        switch(index.column()) {
            case COLUMN_SUBJECT:
                if(art->subject()) {
                    return art->subject()->asUnicodeString();
                }
                break;
            case COLUMN_FROM:
                return extractFrom(art);
                break;
            case COLUMN_DATE:
                if(art->date()) {
                    return mDateFormatter.dateString(art->date()->dateTime());
                }
                break;
        }
        break;
    case Qt::FontRole:
        if(remoteArt && !remoteArt->isRead()) {
            QFont font;
            font.setBold(true);
            return font;
        }
        break;
    case Qt::DecorationRole:
        if(index.column() == COLUMN_SUBJECT) {
            switch(art->type()) {
                case KNArticle::ATlocal: {
                    const KNLocalArticle::Ptr localArt = boost::dynamic_pointer_cast<KNLocalArticle>(art);
                    if(localArt->isSavedRemoteArticle()) {
                        return SmallIcon("edit-copy");
                    } else if(localArt->doPost()) {
                        if(localArt->canceled()) {
                            return SmallIcon("edit-delete");
                        } else {
                            return UserIcon("article");
                        }
                    } else if(localArt->doMail()) {
                        return SmallIcon("mail-message");
                    }
                    break;
                }
                case KNArticle::ATremote:
                    if(remoteArt->isIgnored()) {
                        return UserIcon("ignore");
                    } else if(remoteArt->isWatched()) {
                        return UserIcon("eyes");
                    } else if(hdr->unreadFollowup > 0) {
                        return UserIcon("newsubs");
                    }
                    break;
            }
        }
        break;
    case ArticleRole:
        return QVariant::fromValue(art);
        break;
    case ReadRole:
        return QVariant::fromValue(remoteArt ? remoteArt->isRead() : true);
        break;
    case SortRole:
        switch(index.column()) {
            case COLUMN_SUBJECT:
                if(art->subject()) {
                    return art->subject()->asUnicodeString().replace(*mNormlizeSubject, "");
                }
                break;
            case COLUMN_FROM:
                return extractFrom(art);
                break;
            case COLUMN_DATE:
                if(!mSortByThreadChangeDate) {
                    return art->date()->dateTime();
                } else {
                    return hdr->subThreadDate;
                }
                break;
        }
        break;
    }

    return QVariant();
}

QVariant HeadersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal) {
        switch(role) {
            case Qt::DisplayRole:
                switch(section) {
                    case COLUMN_SUBJECT:
                        return i18n("Subject");
                    case COLUMN_FROM:
                        return i18n("From");
                    case COLUMN_DATE:
                        return mSortByThreadChangeDate ? i18n("Date (thread changed)") : i18n("Date");
                }
                break;
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}


QModelIndex HeadersModel::index(int row, int column, const QModelIndex& parent) const
{
    Header* p = parent.isValid() ? static_cast<Header*>(parent.internalPointer()) : mRoot;
    if(row >= p->children.count()) {
        return QModelIndex();
    }
    return createIndex(row, column, p->children.at(row));
}

QModelIndex HeadersModel::parent(const QModelIndex& child) const
{
    if(!child.isValid()) {
        return QModelIndex();
    }

    Header* c = static_cast<Header*>(child.internalPointer());
    Header* p = c->parent;

    // Parent is the root
    if(p->parent == 0) {
        return QModelIndex();
    }

    int row = p->parent->children.indexOf(p);
    return createIndex(row, 0, p);
}


}
}
