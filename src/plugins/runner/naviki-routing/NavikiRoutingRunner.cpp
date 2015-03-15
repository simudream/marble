//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "NavikiRoutingRunner.h"

#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataExtendedData.h"
#include "routing/Maneuver.h"
#include "routing/RouteRequest.h"
#include "TinyWebBrowser.h"

#include <QString>
#include <QVector>
#include <QUrl>
#include <QTime>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QScriptValue>
#include <QScriptEngine>
#include <QScriptValueIterator>

namespace Marble
{

NavikiRoutingRunner::NavikiRoutingRunner( QObject *parent ) :
    RoutingRunner( parent ),
    m_networkAccessManager()
{
    connect( &m_networkAccessManager, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(retrieveData(QNetworkReply*)) );
}

NavikiRoutingRunner::~NavikiRoutingRunner()
{
    // nothing to do
}

void NavikiRoutingRunner::retrieveRoute( const RouteRequest *route )
{
    if ( route->size() < 2 ) {
        return;
    }

    const QHash<QString, QVariant> settings = route->routingProfile().pluginSettings()["naviki routing"];

    const QString type = settings.value( "preference", "5030" ).toString();

    QString url = "http://r.naviki.org/" + type + "/viaroute?alt=false&instructions=true&output=json&jsonp=jQuery110206345355095017593_1426419843140";

    for ( int i = 0; i < route->size(); ++i ) {
        const qreal lat = route->at(i).latitude( GeoDataCoordinates::Degree );
        const qreal lon = route->at(i).longitude( GeoDataCoordinates::Degree );
        url += "&loc=" + QString::number( lat ) + ',' + QString::number( lon );
    }

    m_request = QNetworkRequest( QUrl( url ) );
    m_request.setRawHeader( "User-Agent", TinyWebBrowser::userAgent( "Browser", "NavikiRoutingRunner" ) );

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

void NavikiRoutingRunner::retrieveData( QNetworkReply *reply )
{
    if ( reply->isFinished() ) {
        QByteArray data = reply->readAll();
        reply->deleteLater();
        GeoDataDocument* document = parse( data );

        if ( !document ) {
            mDebug() << "Failed to parse the downloaded route data" << data;
        }

        emit routeCalculated( document );
    }
}

void NavikiRoutingRunner::handleError( QNetworkReply::NetworkError error )
{
    mDebug() << " Error when retrieving NavikiRouting route: " << error;
}

void NavikiRoutingRunner::get()
{
    QNetworkReply *reply = m_networkAccessManager.get( m_request );
    connect( reply, SIGNAL(error(QNetworkReply::NetworkError)),
             this, SLOT(handleError(QNetworkReply::NetworkError)), Qt::DirectConnection );
}

void NavikiRoutingRunner::append(QString *input, const QString &key, const QString &value)
{
    *input += '&' + key + '=' + value;
}

GeoDataLineString *NavikiRoutingRunner::decodePolyline( const QString &geometry )
{
    // See https://developers.google.com/maps/documentation/utilities/polylinealgorithm
    GeoDataLineString* lineString = new GeoDataLineString;
    int coordinates[2] = { 0, 0 };
    int const length = geometry.length();
    for( int i=0; i<length; /* increment happens below */ ) {
        for ( int j=0; j<2; ++j ) { // lat and lon
            int block( 0 ), shift( 0 ), result( 0 );
            do {
                block = geometry.at( i++ /* increment for outer loop */ ).toLatin1() - 63;
                result |= ( block & 0x1F ) << shift;
                shift += 5;
            } while ( block >= 0x20 );
            coordinates[j] += ( ( result & 1 ) != 0 ? ~( result >> 1 ) : ( result >> 1 ) );
        }
        lineString->append( GeoDataCoordinates( double( coordinates[1] ) / 1E5,
                                                double( coordinates[0] ) / 1E5,
                                                0.0, GeoDataCoordinates::Degree ) );
    }
    return lineString;
}

RoutingInstruction::TurnType NavikiRoutingRunner::parseTurnType( const QString &instruction )
{
    if ( instruction == "1" ) {
        return RoutingInstruction::Straight;
    } else if ( instruction == "2" ) {
        return RoutingInstruction::SlightRight;
    } else if ( instruction == "3" ) {
        return RoutingInstruction::Right;
    } else if ( instruction == "4" ) {
        return RoutingInstruction::SharpRight;
    } else if ( instruction == "5" ) {
        return RoutingInstruction::TurnAround;
    } else if ( instruction == "6" ) {
        return RoutingInstruction::SharpLeft;
    } else if ( instruction == "7" ) {
        return RoutingInstruction::Left;
    } else if ( instruction == "8" ) {
        return RoutingInstruction::SlightLeft;
    } else if ( instruction == "10" ) {
        return RoutingInstruction::Continue;
    } else if ( instruction.startsWith( QLatin1String( "11-" ) ) ) {
        int const exit = instruction.mid( 3 ).toInt();
        switch ( exit ) {
        case 1: return RoutingInstruction::RoundaboutFirstExit; break;
        case 2: return RoutingInstruction::RoundaboutSecondExit; break;
        case 3: return RoutingInstruction::RoundaboutThirdExit; break;
        default: return RoutingInstruction::RoundaboutExit;
        }
    } else if ( instruction == "12" ) {
        return RoutingInstruction::RoundaboutExit;
    }

    // ignoring ReachViaPoint = 9;
    // ignoring StayOnRoundAbout = 13;
    // ignoring StartAtEndOfStreet = 14;
    // ignoring ReachedYourDestination = 15;

    return RoutingInstruction::Unknown;
}

GeoDataDocument *NavikiRoutingRunner::parse( const QByteArray &input ) const
{
    QString strInput = QString::fromUtf8( input );
    strInput = strInput.mid( strInput.indexOf( '(' ) );
    QScriptEngine engine;
    // Qt requires parentheses around json code
    const QScriptValue data = engine.evaluate( strInput );

    GeoDataDocument* result = 0;
    GeoDataLineString* routeWaypoints = 0;
    if ( data.property( "route_geometry" ).isString() ) {
        result = new GeoDataDocument();
        result->setName( "Naviki" );
        GeoDataPlacemark* routePlacemark = new GeoDataPlacemark;
        routePlacemark->setName( "Route" );
        routeWaypoints = decodePolyline( data.property( "route_geometry" ).toString() );
        routePlacemark->setGeometry( routeWaypoints );

        QTime time;
        time = time.addSecs( data.property( "route_summary" ).property("total_time").toNumber() );
        qreal length = routeWaypoints->length( EARTH_RADIUS );
        const QString name = nameString( "Naviki", length, time );
        const GeoDataExtendedData extendedData = routeData( length, time );
        routePlacemark->setExtendedData( extendedData );
        result->setName( name );
        result->append( routePlacemark );
    }

    if ( result && routeWaypoints && data.property( "route_instructions" ).isArray() ) {
        bool first = true;
        QScriptValueIterator iterator( data.property( "route_instructions" ) );
        GeoDataPlacemark* instruction = new GeoDataPlacemark;
        int lastWaypointIndex = 0;
        while ( iterator.hasNext() ) {
            iterator.next();
            QVariantList details = iterator.value().toVariant().toList();
            if ( details.size() > 7 ) {
                QString const text = details.at( 0 ).toString();
                QString const road = details.at( 1 ).toString();
                int const waypointIndex = details.at( 3 ).toInt();

                if ( waypointIndex < routeWaypoints->size() ) {
                    if ( iterator.hasNext() ) {
                        GeoDataLineString *lineString = new GeoDataLineString;
                        for ( int i=lastWaypointIndex; i<=waypointIndex; ++i ) {
                            lineString->append(routeWaypoints->at( i ) );
                        }
                        instruction->setGeometry( lineString );
                        result->append( instruction );
                        instruction = new GeoDataPlacemark;
                    }
                    lastWaypointIndex = waypointIndex;
                    GeoDataExtendedData extendedData;
                    GeoDataData turnTypeData;
                    turnTypeData.setName( "turnType" );
                    RoutingInstruction::TurnType turnType = parseTurnType( text );
                    turnTypeData.setValue( turnType );
                    extendedData.addValue( turnTypeData );
                    if (!road.isEmpty()) {
                        GeoDataData roadName;
                        roadName.setName( "roadName" );
                        roadName.setValue( road );
                        extendedData.addValue( roadName );
                    }

                    if ( first ) {
                        turnType = RoutingInstruction::Continue;
                        first = false;
                    }

                    if ( turnType == RoutingInstruction::Unknown ) {
                        instruction->setName( text );
                    } else {
                        instruction->setName( RoutingInstruction::generateRoadInstruction( turnType, road ) );
                    }
                    instruction->setExtendedData( extendedData );

                    if ( !iterator.hasNext() && lastWaypointIndex > 0 ) {
                        GeoDataLineString *lineString = new GeoDataLineString;
                        for ( int i=lastWaypointIndex; i<waypointIndex; ++i ) {
                            lineString->append(routeWaypoints->at( i ) );
                        }
                        instruction->setGeometry( lineString );
                        result->append( instruction );
                    }
                }
            }
        }
    }

    return result;
}

} // namespace Marble

#include "NavikiRoutingRunner.moc"