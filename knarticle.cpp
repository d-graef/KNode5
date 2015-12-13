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

#include "knarticle.h"

#include <kcodecs.h>
#include <KDE/KLocale>
#include <kmimetype.h>
#include <QDebug>

#include "kngroup.h"
#include "knode_debug.h"
#include "knglobals.h"
#include "knconfigmanager.h"
#include "settings.h"
#include "utilities.h"
#include "utils/locale.h"


using namespace KNode::Utilities;
using namespace KMime;


KNArticle::KNArticle( KNArticleCollection::Ptr c )
  : i_d( -1 ), c_ol( c ),
    f_lags( 13 )
{
}


KNArticle::~KNArticle()
{
}


void KNArticle::clear()
{
  f_lags.clear();
}

void KNArticle::setLocked(bool b)
{
  f_lags.setBit(0, b);
  if(c_ol) {  // local articles may have c_ol==0 !
    if(b)
      c_ol->articleLocked();
    else
      c_ol->articleUnlocked();
  }
}


//=========================================================================================


KNRemoteArticle::KNRemoteArticle( KNGroup::Ptr g )
 : KNArticle(g), a_rticleNumber(-1), i_dRef(-1), t_hrLevel(0),
   u_nreadFups(0), n_ewFups(0)
{
  qCDebug(KNODE_LOG) << "Port";
#if 0
  setDefaultCharset( Locale::defaultCharset( g ) );
#endif

  // Remote article are read-only
  setFrozen( true );
}


KNRemoteArticle::~KNRemoteArticle()
{}


void KNRemoteArticle::parse()
{
  KNArticle::parse();
  QByteArray raw;

  raw = KMime::extractHeader( head(), from()->type() );
  if ( !raw.isEmpty() ) {
    QByteArray f;
    Locale::encodeTo7Bit( raw, defaultCharset(), f );
    from()->from7BitString( f );
  }

  raw = KMime::extractHeader( head(), subject()->type() );
  if( !raw.isEmpty() ) {
    QByteArray subjectStr;
    Locale::encodeTo7Bit( raw, defaultCharset(), subjectStr );
    subject()->from7BitString( subjectStr );
  }
}

void KNRemoteArticle::setForceDefaultCharset(bool b)
{
  qCDebug(KNODE_LOG) << "Port";
#if 0
  if (!b) { // restore default
    KNGroup::Ptr g = boost::static_pointer_cast<KNGroup>( c_ol );
    setDefaultCharset( Locale::defaultCharset( g ) );
  }
  KNArticle::setForceDefaultCharset( b );
#endif
}



//=========================================================================================


KNLocalArticle::KNLocalArticle( KNArticleCollection::Ptr c )
  : KNArticle(c), s_Offset(0), e_Offset(0), s_erverId(-1)
{
  qCDebug(KNODE_LOG) << "Port";
#if 0
  setDefaultCharset( Locale::defaultCharset() );
#endif
}


KNLocalArticle::~KNLocalArticle()
{}


void KNLocalArticle::setForceDefaultCharset( bool b )
{
  qCDebug(KNODE_LOG) << "Port";
#if 0
  if (!b)  // restore default
    setDefaultCharset( Locale::defaultCharset() );
  KNArticle::setForceDefaultCharset( b );
#endif
}


//=========================================================================================


KNAttachment::KNAttachment(Content *c)
  : c_ontent(c), l_oadHelper(0), f_ile(0), i_sAttached(true)
{
  Headers::ContentType  *t=c->contentType();
  Headers::ContentTransferEncoding   *e=c->contentTransferEncoding();
  Headers::ContentDescription *d=c->contentDescription(false);

  n_ame=t->name();

  if(d)
    d_escription=d->asUnicodeString();


  setMimeType(t->mimeType());

  if(e->encoding()==Headers::CEuuenc) {
    setCte( Headers::CEbase64 );
    updateContentInfo();
  }
  else
    e_ncoding.setEncoding( e->encoding() );


  h_asChanged=false; // has been set to "true" in setMimeType()
}


KNAttachment::KNAttachment(KNLoadHelper *helper)
  : c_ontent(0), l_oadHelper(helper), f_ile(helper->getFile()), i_sAttached(false), h_asChanged(true)
{
  setMimeType(KMimeType::findByPath(f_ile->fileName())->name());
  n_ame=helper->getURL().fileName();
}


KNAttachment::~KNAttachment()
{
  if(!i_sAttached && c_ontent)
    delete c_ontent;
  delete l_oadHelper;
}


void KNAttachment::setMimeType(const QString &s)
{
  mMimeType = s;
  h_asChanged=true;

  if ( !mMimeType.contains( "text/", Qt::CaseInsensitive ) ) {
    f_b64=true;
    e_ncoding.setEncoding(Headers::CEbase64);
  }
  else {
    f_b64=false;
    if ( knGlobals.settings()->allow8BitBody() )
      setCte(Headers::CE8Bit);
    else
      setCte(Headers::CEquPr);
  }
}


QString KNAttachment::contentSize() const
{
  int s=0;

  if(c_ontent && c_ontent->hasContent())
    s=c_ontent->size();
  else {
    if (f_ile)
      s=f_ile->size();
  }

  return KLocale::global()->formatByteSize( s );
}


void KNAttachment::updateContentInfo()
{
  if(!h_asChanged || !c_ontent)
    return;

  //Content-Type
  Headers::ContentType *t=c_ontent->contentType();
  t->setMimeType( mMimeType.toLatin1() );
  t->setName(n_ame, "UTF-8");
  t->setCategory(Headers::CCmixedPart);

  //Content-Description
  if(d_escription.isEmpty())
    c_ontent->removeHeader("Content-Description");
  else
    c_ontent->contentDescription()->fromUnicodeString(d_escription, "UTF-8");

  //Content-Disposition
  Headers::ContentDisposition *d=c_ontent->contentDisposition();
  d->setDisposition(Headers::CDattachment);
  d->setFilename(n_ame);

  //Content-Transfer-Encoding
  if(i_sAttached)
    c_ontent->changeEncoding(e_ncoding.encoding());
  else
    c_ontent->contentTransferEncoding()->setEncoding(e_ncoding.encoding());

  c_ontent->assemble();

  h_asChanged=false;
}



void KNAttachment::attach(Content *c)
{
  if(i_sAttached || !f_ile)
    return;

  c_ontent=new Content();
  updateContentInfo();
  Headers::ContentType *type=c_ontent->contentType();
  Headers::ContentTransferEncoding *e=c_ontent->contentTransferEncoding();
  QByteArray data;

  data = f_ile->readAll();

  if ( data.size() < f_ile->size() && f_ile->error() != QFile::NoError ) {
    KNHelper::displayExternalFileError();
    delete c_ontent;
    c_ontent=0;
  } else {
    if (e_ncoding.encoding()==Headers::CEbase64 || !type->isText()) { //encode base64
      c_ontent->setBody( KCodecs::base64Encode(data, true) + '\n' );
      //      c_ontent->b_ody += '\n';
      e->setEncoding(Headers::CEbase64);
      e->setDecoded(false);
    } else  {
      c_ontent->setBody( data + '\n' );
      //      c_ontent->b_ody += '\n';
      e->setDecoded(true);
    }
  }

  if(c_ontent) {
    c->addContent(c_ontent);
    i_sAttached=true;
  }
}


void KNAttachment::detach(Content *c)
{
  if(i_sAttached) {
    c->removeContent(c_ontent, false);
    i_sAttached=false;
  }
}
