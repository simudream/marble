//
// Copyright 2008 Simon Edwards <simon@simonzone.com>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.
//

namespace Marble
{

class GeoDataFeature : Marble::GeoDataObject
{
%TypeHeaderCode
#include <GeoDataFeature.h>
%End

public:
                            GeoDataFeature ();
                            GeoDataFeature (const QString& name);
                            GeoDataFeature (const Marble::GeoDataFeature& other);
    Marble::EnumFeatureId   featureId () const;

    enum GeoDataVisualCategory
    {
        None,
        Default,
        Unknown,
        SmallCity,
        SmallCountyCapital,
        SmallStateCapital,
        SmallNationCapital,
        MediumCity,
        MediumCountyCapital,
        MediumStateCapital,
        MediumNationCapital,
        BigCity,
        BigCountyCapital,
        BigStateCapital,
        BigNationCapital,
        LargeCity,
        LargeCountyCapital,
        LargeStateCapital,
        LargeNationCapital,
        Nation,
        Mountain,
        Volcano,
        Mons,
        Valley,
        Continent,
        Ocean,
        OtherTerrain,
        Crater,
        Mare,
        GeographicPole,
        MagneticPole,
        ShipWreck,
        AirPort,
        Observatory,
        Wikipedia,
        OsmSite,
        Coordinate,
        MannedLandingSite,
        RoboticRover,
        UnmannedSoftLandingSite,
        UnmannedHardLandingSite,
        Folder,
        Bookmark,
        NaturalWater,
        NaturalWood,
        HighwaySteps,
        HighwayUnknown,
        HighwayPath,
        HighwayTrack,
        HighwayPedestrian,
        HighwayService,
        HighwayRoad,
        HighwayTertiaryLink,
        HighwayTertiary,
        HighwaySecondaryLink,
        HighwaySecondary,
        HighwayPrimaryLink,
        HighwayPrimary,
        HighwayTrunkLink,
        HighwayTrunk,
        HighwayMotorwayLink,
        HighwayMotorway,
        Building,
        AccomodationCamping,
        AccomodationHostel,
        AccomodationHotel,
        AccomodationMotel,
        AccomodationYouthHostel,
        AmenityLibrary,
        EducationCollege,
        EducationSchool,
        EducationUniversity,
        FoodBar,
        FoodBiergarten,
        FoodCafe,
        FoodFastFood,
        FoodPub,
        FoodRestaurant,
        HealthDoctors,
        HealthHospital,
        HealthPharmacy,
        MoneyAtm,
        MoneyBank,
        ShoppingBeverages,
        ShoppingHifi,
        ShoppingSupermarket,
        TouristAttraction,
        TouristCastle,
        TouristCinema,
        TouristMonument,
        TouristMuseum,
        TouristRuin,
        TouristTheatre,
        TouristThemePark,
        TouristViewPoint,
        TouristZoo,
        TransportAerodrome,
        TransportAirportTerminal,
        TransportBusStation,
        TransportBusStop,
        TransportCarShare,
        TransportFuel,
        TransportParking,
        TransportRentalBicycle,
        TransportRentalCar,
        TransportTaxiRank,
        TransportTrainStation,
        TransportTramStop,
        ReligionPlaceOfWorship,
        ReligionBahai,
        ReligionBuddhist,
        ReligionChristian,
        ReligionHindu,
        ReligionJain,
        ReligionJewish,
        ReligionShinto,
        ReligionSikh,
        LeisurePark,
        LanduseAllotments,
        LanduseBasin,
        LanduseCemetery,
        LanduseCommercial,
        LanduseConstruction,
        LanduseFarmland,
        LanduseFarmyard,
        LanduseGarages,
        LanduseGrass,
        LanduseIndustrial,
        LanduseLandfill,
        LanduseMeadow,
        LanduseMilitary,
        LanduseQuarry,
        LanduseRailway,
        LanduseReservoir,
        LanduseResidential,
        LanduseRetail,
        RailwayRail,
        RailwayTram,
        RailwayLightRail,
        RailwayAbandoned,
        RailwaySubway,
        RailwayPreserved,
        RailwayMiniature,
        RailwayConstruction,
        RailwayMonorail,
        RailwayFunicular,
        Satellite,
        LastIndex
    };

    QString                 name () const;
    void                    setName (const QString& value);
    QString                 address () const;
    void                    setAddress (const QString& value);
    QString                 phoneNumber () const;
    void                    setPhoneNumber (const QString& value);
    QString                 description () const;
    void                    setDescription (const QString& value);
    QString                 styleUrl () const;
    void                    setStyleUrl (const QString& value);
    bool                    isVisible () const;
    void                    setVisible (bool value);
    const Marble::GeoDataStyle*  style () const;
    Marble::GeoDataFeature::GeoDataVisualCategory  visualCategory () const;
    void                    setVisualCategory (Marble::GeoDataFeature::GeoDataVisualCategory category);
//ig    const QChar             role () const;
    qint64                  popularity () const;
    void                    setPopularity (qint64 popularity);
    const Marble::GeoDataStyleMap*  styleMap () const;
    const QSize             symbolSize () const;
    static void             resetDefaultStyles ();
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    static QFont            defaultFont ();
    static void             setDefaultFont (const QFont& font);
    virtual void            detach ();

    virtual ~GeoDataFeature ();
    virtual const char*     nodeType () const;
    bool                    descriptionIsCDATA () const;
    void                    setDescriptionCDATA (bool cdata);
    Marble::GeoDataRegion&  region () const;
    void                    setRegion (const Marble::GeoDataRegion& region);
protected:
//ig                            GeoDataFeature (GeoDataFeaturePrivate* priv);
public:
//ig    Marble::GeoDataTimeSpan&  timeSpan () const;
//ig    GeoDataTimeStamp&       timeStamp () const;
//ig    GeoDataExtendedData&    extendedData () const;
//ig    void                    setExtendedData (const GeoDataExtendedData& extendedData);
    void                    setRole (const QString& role);
    static Marble::GeoDataFeature::GeoDataVisualCategory  OsmVisualCategory (const QString& keyValue);
    bool                    isGloballyVisible () const;
    const QImage            symbol () const;
    int                     zoomLevel () const;
    void                    setZoomLevel (int index);
    static QColor           defaultLabelColor ();
    static void             setDefaultLabelColor (const QColor& color);
    void                    setStyleMap (const Marble::GeoDataStyleMap* map);
    const Marble::GeoDataAbstractView*  abstractView () const;
//ig    Marble::GeoDataAbstractView*  abstractView ();
    void                    setAbstractView (Marble::GeoDataAbstractView* abstractView);
    void                    setTimeSpan (const Marble::GeoDataTimeSpan& timeSpan);
    void                    setTimeStamp (const Marble::GeoDataTimeStamp& timeStamp);
    void                    setStyle (Marble::GeoDataStyle* style);
};
// GeoDataFeature

};
// Marble


