//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "YoursRunner.h"

#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "TinyWebBrowser.h"
#include "GeoDataParser.h"
#include "GeoDataFolder.h"
#include "routing/RouteRequest.h"

#include <QString>
#include <QVector>
#include <QUrl>
#include <QTime>
#include <QNetworkAccessManager>
#include <QDomDocument>
#include <QBuffer>
#include <QTimer>

namespace Marble
{

YoursRunner::YoursRunner( QObject *parent ) :
        RoutingRunner( parent ),
        m_reply( 0 )
{
}

YoursRunner::~YoursRunner()
{
    // nothing to do
}

void YoursRunner::retrieveRoute( const RouteRequest *route )
{
    if ( route->size() != 2 ) {
        return;
    }

    GeoDataCoordinates source = route->source();
    GeoDataCoordinates destination = route->destination();

    double fLon = source.longitude( GeoDataCoordinates::Degree );
    double fLat = source.latitude( GeoDataCoordinates::Degree );

    double tLon = destination.longitude( GeoDataCoordinates::Degree );
    double tLat = destination.latitude( GeoDataCoordinates::Degree );

    QString base = "http://www.yournavigation.org/api/1.0/gosmore.php";
    //QString base = "http://nroets.dev.openstreetmap.org/demo/gosmore.php";
    QString args = "?flat=%1&flon=%2&tlat=%3&tlon=%4";
    args = args.arg( fLat, 0, 'f', 6 ).arg( fLon, 0, 'f', 6 ).arg( tLat, 0, 'f', 6 ).arg( tLon, 0, 'f', 6 );
    QString preferences = "&v=motorcar&fast=1&layer=mapnik";
    QString request = base + args + preferences;
    // mDebug() << "GET: " << request;

    m_request = QNetworkRequest( QUrl( request ) );

    QEventLoop eventLoop;

    QTimer timer;
    timer.setSingleShot( true );
    timer.setInterval( 15000 );

    connect( &timer, SIGNAL(timeout()),
             &eventLoop, SLOT(quit()));
    connect( this, SIGNAL(routeCalculated(GeoDataDocument*)),
             &eventLoop, SLOT(quit()) );

    // @todo FIXME Must currently be done in the main thread, see bug 257376
    QTimer::singleShot( 0, this, SLOT(get()) );
    timer.start();

    eventLoop.exec();
}

void YoursRunner::get()
{
    m_reply = networkAccessManager()->get( m_request );
    connect( m_reply, SIGNAL(finished()),
             this, SLOT(retrieveData()) );
    connect( m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
             this, SLOT(handleError(QNetworkReply::NetworkError)) );
}

void YoursRunner::retrieveData()
{
    if ( m_reply->isFinished() ) {
        QByteArray data = m_reply->readAll();
        m_reply->deleteLater();
        m_reply = 0;
        //mDebug() << "Download completed: " << data;
        GeoDataDocument* result = parse( data );
        if ( result ) {
            QString name = "%1 %2 (Yours)";
            QString unit = QLatin1String( "m" );
            qreal length = distance( result );
            if ( length == 0.0 ) {
                delete result;
                emit routeCalculated( 0 );
                return;
            } else if ( length >= 1000 ) {
                length /= 1000.0;
                unit = "km";
            }
            result->setName( name.arg( length, 0, 'f', 1 ).arg( unit ) );
        }
        emit routeCalculated( result );
    }
}

void YoursRunner::handleError( QNetworkReply::NetworkError error )
{
    mDebug() << " Error when retrieving yournavigation.org route: " << error;
    emit routeCalculated( 0 );
}

GeoDataDocument* YoursRunner::parse( const QByteArray &content ) const
{
    GeoDataParser parser( GeoData_UNKNOWN );

    // Open file in right mode
    QBuffer buffer;
    buffer.setData( content );
    buffer.open( QIODevice::ReadOnly );

    if ( !parser.read( &buffer ) ) {
        mDebug() << "Cannot parse kml data! Input is " << content ;
        return 0;
    }
    GeoDataDocument* document = static_cast<GeoDataDocument*>( parser.releaseDocument() );
    return document;
}

qreal YoursRunner::distance( const GeoDataDocument* document ) const
{
    QVector<GeoDataFolder*> folders = document->folderList();
    foreach( const GeoDataFolder *folder, folders ) {
        foreach( const GeoDataPlacemark *placemark, folder->placemarkList() ) {
            GeoDataGeometry* geometry = placemark->geometry();
            if ( geometry->geometryId() == GeoDataLineStringId ) {
                GeoDataLineString* lineString = dynamic_cast<GeoDataLineString*>( geometry );
                Q_ASSERT( lineString && "Internal error: geometry ID does not match class type" );
                return lineString->length( EARTH_RADIUS );
            }
        }
    }

    return 0.0;
}

} // namespace Marble

#include "YoursRunner.moc"
