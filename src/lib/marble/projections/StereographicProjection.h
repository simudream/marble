//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Bernhard Beschow  <bbeschow@cs.tu-berlin.de>
//


#ifndef MARBLE_STEREOGRAPHICPROJECTION_H
#define MARBLE_STEREOGRAPHICPROJECTION_H


#include "AbstractProjection.h"
#include "AzimuthalProjection.h"

namespace Marble
{

class StereographicProjectionPrivate;

/**
 * @short A class to implement the spherical projection used by the "Globe" view.
 */

class StereographicProjection : public AzimuthalProjection
{
    // Not a QObject so far because we don't need to send signals.
 public:

    /**
     * @brief Construct a new StereographicProjection.
     */
    StereographicProjection();

    virtual ~StereographicProjection();

    /**
     * @brief Returns the user-visible name of the projection.
     */
    QString name() const;

    /**
     * @brief Returns a short user description of the projection
     * that can be used in tooltips or dialogs.
     */
    QString description() const;

    /**
     * @brief Returns an icon for the projection.
     */
    QIcon icon() const;

    virtual bool isClippedToSphere() const;

    /**
     * @brief Get the screen coordinates corresponding to geographical coordinates in the map.
     * @param lon    the lon coordinate of the requested pixel position
     * @param lat    the lat coordinate of the requested pixel position
     * @param x      the x coordinate of the pixel is returned through this parameter
     * @param y      the y coordinate of the pixel is returned through this parameter
     * @return @c true  if the geographical coordinates are visible on the screen
     *         @c false if the geographical coordinates are not visible on the screen
     */
    virtual bool screenCoordinates( const GeoDataCoordinates &coordinates,
                            const ViewportParams *params,
                            qreal &x, qreal &y, bool &globeHidesPoint ) const;

    virtual bool screenCoordinates( const GeoDataCoordinates &coordinates,
                            const ViewportParams * viewport,
                            qreal *x, qreal &y, int &pointRepeatNum,
                            const QSizeF& size,
                            bool &globeHidesPoint ) const;

    using AbstractProjection::screenCoordinates;

    /**
     * @brief Get the earth coordinates corresponding to a pixel in the map.
     * @param x      the x coordinate of the pixel
     * @param y      the y coordinate of the pixel
     * @param lon    the longitude angle is returned through this parameter
     * @param lat    the latitude angle is returned through this parameter
     * @return @c true  if the pixel (x, y) is within the globe
     *         @c false if the pixel (x, y) is outside the globe, i.e. in space.
     */
    bool geoCoordinates( const int x, const int y,
                         const ViewportParams *params,
                         qreal& lon, qreal& lat,
                         GeoDataCoordinates::Unit unit = GeoDataCoordinates::Degree ) const;

 protected:
    StereographicProjection(StereographicProjectionPrivate *dd );

 private:
    Q_DECLARE_PRIVATE(StereographicProjection)
    Q_DISABLE_COPY( StereographicProjection )
};

}

#endif
