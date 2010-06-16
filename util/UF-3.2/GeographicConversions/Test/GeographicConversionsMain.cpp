
#include "ufRedfearn.h"
#include "ufLocalRedfearn.h"
#include "ufVincenty.h"
#include "ufMapGrid.h"
#include "ufMapGrids.h"
#include "ufEllipsoids.h"
#include "ufECEF.h"
#include "ufLogger.h"
#include "ufSystemTime.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

using namespace UF;
using namespace UF::Log;

void DisplayMapGrids(ufLogger *pLog)
{

  GeographicConversions::MapGrids *mg = GeographicConversions::MapGrids::Instance();
  GeographicConversions::TMapGridKeyVector keys = mg->AvailableMapGrids();
  GeographicConversions::TMapGridParametersVector v;
  for ( GeographicConversions::TMapGridKeyVectorItr p = keys.begin(); p != keys.end(); ++p)
  {
    *pLog << *p << ": (";
    v = mg->GetMapGrid(*p);
    int i = 0;
    for ( GeographicConversions::TMapGridParametersVectorItr q = v.begin(); q != v.end(); ++q)
    {
      switch (i)
      {
      case 0:
        *pLog << std::fixed << std::setprecision(1) << *q;
        break;
      case 1:
        *pLog << " " << std::fixed << std::setprecision(1) << *q;
        break;
      case 2:
        *pLog << " " << std::fixed << std::setprecision(4) << *q;
        break;
      default:
        *pLog << " " << std::fixed << std::setprecision(0) << *q;
      }
      i++;
    }
    *pLog << ")" << std::endl;
  }
  *pLog << std::endl;
}

void DisplayEllipsoids(ufLogger *pLog)
{

  GeographicConversions::Ellipsoids *mg = GeographicConversions::Ellipsoids::Instance();
  GeographicConversions::TEllipsoidKeyVector keys = mg->AvailableEllipsoids();
  GeographicConversions::TEllipsoidParametersVector v;
  for ( GeographicConversions::TEllipsoidKeyVectorItr p = keys.begin(); p != keys.end(); ++p)
  {
    *pLog << *p << ": (";
    v = mg->GetEllipsoid(*p);
    int i = 0;
    for ( GeographicConversions::TEllipsoidParametersVectorItr q = v.begin(); q != v.end(); ++q)
    {
      switch (i)
      {
      case 0:
        *pLog << std::fixed << std::setprecision(3) << *q;
        break;
      default:
        *pLog << " " << std::fixed << std::setprecision(9) << *q;
      }
      i++;
    }
    *pLog << ")" << std::endl;
  }
  *pLog << std::endl;
}

int TestRedfearn(ufLogger *pLog)
{
  GeographicConversions::Redfearn Redfearn("GRS80","MGA");
  try {
    if ( !(Redfearn.EllipsoidIsValid() && Redfearn.MapGridIsValid()) )
    {
      *pLog << "Invalid for some reason, GRS80 and MGA should exist. " << std::endl;
      return 1;
    }
  }
  catch (std::exception *ex)
  {
    *pLog << "Unexpected, GRS80 and MGA should exist: " << ex->what() << std::endl;
    return 1;
  }

  double lat = -(37+(39+10.15610/60.0)/60.0);
  double lng = 143+(55+35.38390/60.0)/60.0;
  double E, N, GC, PS;
  std::string Z;

  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  Redfearn.GetGridCoordinates(lat, lng, Z, E, N, GC, PS);
  *pLog << "Grid:       " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << std::endl;
  lat = 0; lng = 0;
  Redfearn.GetGeographicCoordinates(Z, E, N, lat, lng, GC, PS);
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  lat = 0; lng = 0;
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  Redfearn.GetGridCoordinates(lat, lng, Z, E, N, GC, PS);
  *pLog << "Grid:       " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << std::endl;

  Z = "55H";
  E = 228854.052;
  N = 5828259.038;
  *pLog << "Grid      : " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << std::endl;
  Redfearn.GetGeographicCoordinates(Z, E, N, lat, lng, GC, PS);
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  Z = "55H";
  Redfearn.GetZoneGridCoordinates(lat, lng, "55H", E, N, GC, PS);
  *pLog << "Grid (55H): " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << std::endl;
  Z = "55J";
  Redfearn.GetZoneGridCoordinates(lat, lng, "55J", E, N, GC, PS);
  *pLog << "Grid (55J): " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << std::endl;
  Redfearn.GetGridCoordinates(lat, lng, Z, E, N, GC, PS);
  *pLog << "Should default to zone 54H" << std::endl;
  *pLog << "Grid (54H): " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << std::endl;

  *pLog << "Testing different hemispheres." << std::endl;
  lat = -33;
  lng = 151;
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  Redfearn.GetGridCoordinates(lat, lng, Z, E, N, GC, PS);
  *pLog << "Grid:       " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << std::endl;
  Redfearn.GetGeographicCoordinates(Z, E, N, lat, lng, GC, PS);
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  lat = 33;
  lng = 151;
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  Redfearn.GetGridCoordinates(lat, lng, Z, E, N, GC, PS);
  *pLog << "Grid:       " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << std::endl;
  Redfearn.GetGeographicCoordinates(Z, E, N, lat, lng, GC, PS);
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  lat = 33;
  lng = -151;
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  Redfearn.GetGridCoordinates(lat, lng, Z, E, N, GC, PS);
  *pLog << "Grid:       " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << std::endl;
  Redfearn.GetGeographicCoordinates(Z, E, N, lat, lng, GC, PS);
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  lat = -33;
  lng = -151;
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  Redfearn.GetGridCoordinates(lat, lng, Z, E, N, GC, PS);
  *pLog << "Grid:       " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << std::endl;
  Redfearn.GetGeographicCoordinates(Z, E, N, lat, lng, GC, PS);
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;

  *pLog << "Testing ECRF/LatLong" << std::endl;
  double x,y,z;
  double h = 1000000;

  GeographicConversions::ECEF ecef("GRS80");
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::setprecision(3) << ", " << h << std::endl;
  ecef.toECEF(lat,lng,h,x,y,z);
  *pLog << "Geodetic:   " << std::fixed << std::setprecision(3) << x << ", " << y << ", " << z << std::endl;
  ecef.fromECEF(x,y,z,lat,lng,h);
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::setprecision(3) << ", " << h << std::endl;

  lat = -lat;
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::setprecision(3) << ", " << h << std::endl;
  ecef.toECEF(lat,lng,h,x,y,z);
  *pLog << "Geodetic:   " << std::fixed << std::setprecision(3) << x << ", " << y << ", " << z << std::endl;
  ecef.fromECEF(x,y,z,lat,lng,h);
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::setprecision(3) << ", " << h << std::endl;

  lat = -lat; lng = -lng;
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::setprecision(3) << ", " << h << std::endl;
  ecef.toECEF(lat,lng,h,x,y,z);
  *pLog << "Geodetic:   " << std::fixed << std::setprecision(3) << x << ", " << y << ", " << z << std::endl;
  ecef.fromECEF(x,y,z,lat,lng,h);
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::setprecision(3) << ", " << h << std::endl;

  lng = -lng;
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::setprecision(3) << ", " << h << std::endl;
  ecef.toECEF(lat,lng,h,x,y,z);
  *pLog << "Geodetic:   " << std::fixed << std::setprecision(3) << x << ", " << y << ", " << z << std::endl;
  ecef.fromECEF(x,y,z,lat,lng,h);
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::setprecision(3) << ", " << h << std::endl;


  lat = 0;lng = -90; h = 1025.456;
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::setprecision(3) << ", " << h << std::endl;
  ecef.toECEF(lat,lng,h,x,y,z);
  *pLog << "Geodetic:   " << std::fixed << std::setprecision(3) << x << ", " << y << ", " << z << std::endl;
  ecef.fromECEF(x,y,z,lat,lng,h);
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::setprecision(3) << ", " << h << std::endl;
  ecef.fromECEF_Inexact(x,y,z,lat,lng,h);
  *pLog << "Cartesian to Geodetic (inexact) " << std::endl;
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::setprecision(3) << ", " << h << std::endl;

  x = 0; y = +6379162.456; z = 0;
  *pLog << "Geodetic:   " << std::fixed << std::setprecision(3) << x << ", " << y << ", " << z << std::endl;
  ecef.fromECEF(x,y,z,lat,lng,h);
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::setprecision(3) << ", " << h << std::endl;
  ecef.fromECEF_Inexact(x,y,z,lat,lng,h);
  *pLog << "Cartesian to Geodetic (inexact) " << std::endl;
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::setprecision(3) << ", " << h << std::endl;

  Redfearn.SetEllipsoid("GDA94");
  Redfearn.SetMapGrid("UTM");
  *pLog << "Fiddle with points on a boundary. Zone could be one of 50H, 51H, 50J, 51J" << std::endl;
  lat = -32;
  lng = 120;
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  Redfearn.GetGridCoordinates(lat, lng, Z, E, N, GC, PS);
  *pLog << "Grid:       " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << std::endl;
  Z = "50H";
  Redfearn.GetZoneGridCoordinates(lat, lng, Z, E, N, GC, PS);
  *pLog << "Grid:       " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << std::endl;
  Redfearn.GetGeographicCoordinates(Z, E, N, lat, lng, GC, PS);
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  Z = "50J";
  Redfearn.GetZoneGridCoordinates(lat, lng, Z, E, N, GC, PS);
  *pLog << "Grid:       " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << std::endl;
  Redfearn.GetGeographicCoordinates(Z, E, N, lat, lng, GC, PS);
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  Z = "51J";
  Redfearn.GetZoneGridCoordinates(lat, lng, Z, E, N, GC, PS);
  *pLog << "Grid:       " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << std::endl;
  Redfearn.GetGeographicCoordinates(Z, E, N, lat, lng, GC, PS);
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  Z = "51H";
  Redfearn.GetZoneGridCoordinates(lat, lng, Z, E, N, GC, PS);
  *pLog << "Grid:       " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << std::endl;
  Redfearn.GetGeographicCoordinates(Z, E, N, lat, lng, GC, PS);
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;

  Redfearn.SetMapGrid("UTM");
  Redfearn.SetEllipsoid("WGS84");

  lat = -32.5588;
  lng = 116.15883;
  *pLog << "Huntley Crusher" << std::endl;
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  Redfearn.GetGridCoordinates(lat, lng, Z, E, N, GC, PS);
  *pLog << "Grid:       " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << std::endl;

  lat = -32.84832;
  lng = 116.06004;
  *pLog << "Willowdale Crusher" << std::endl;
  *pLog << "Geographic: " << std::fixed << std::setprecision(6) << lat << ", " << lng << std::endl;
  Redfearn.GetGridCoordinates(lat, lng, Z, E, N, GC, PS);
  *pLog << "Grid:       " << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << std::endl;

  return 0;
}


int TestLocalRedfearn(ufLogger *pLog){

    double local_central_meridian = 150.12345;

    GeographicConversions::LocalRedfearn local("WGS84","UTM",local_central_meridian);

    double Lat, Lng, Easting, Northing, GridConv, PtScale;
    std::string Zone;
    
    try{

        *pLog << std::endl << "Testing LocalRedfearn:" << std::endl;

        GeographicConversions::LocalRedfearn local1(local);
        if ( local1 != local )
        {
          *pLog << "Copy constructor failed." << std::endl;
          return 1;
        }
        GeographicConversions::LocalRedfearn local2("WGS84","UTM",local_central_meridian - 0.12345);
        local1 = local2;
        if ( local1 != local2 )
        {
          *pLog << "Assugnment failed." << std::endl;
          return 1;
        }

        Lng = local_central_meridian;
        Lat = -50; 

        *pLog << "Points on the central meridian:- All should have the same easting value" << std::endl;

        local.GetGridCoordinates (Lat, Lng, Zone, Easting, Northing, GridConv, PtScale);
        *pLog << "Lat: "<< Lat << " Lng: " << Lng << " East: " << Easting << " North: " << Northing << " Zone: " << Zone << std::endl;

        Lat = Lat + 50;
        local.GetGridCoordinates (Lat, Lng, Zone, Easting, Northing, GridConv, PtScale);
        *pLog << "Lat: "<< Lat << " Lng: " << Lng << " East: " << Easting << " North: " << Northing << " Zone: " << Zone << std::endl;

        Lat = Lat + 50;
        local.GetGridCoordinates (Lat, Lng, Zone, Easting, Northing, GridConv, PtScale);
        *pLog << "Lat: "<< Lat << " Lng: " << Lng << " East: " << Easting << " North: " << Northing << " Zone: " << Zone << std::endl;

        *pLog << "Points across the zone:- All should be in zone 1" << std::endl;

        Lat = -50;
        Lng = local_central_meridian - 2.9;
        local.GetGridCoordinates (Lat, Lng, Zone, Easting, Northing, GridConv, PtScale);
        *pLog << "Lat: "<< Lat << " Lng: " << Lng << " East: " << Easting << " North: " << Northing << " Zone: " << Zone << std::endl;

        Lng = local_central_meridian;
        local.GetGridCoordinates (Lat, Lng, Zone, Easting, Northing, GridConv, PtScale);
        *pLog << "Lat: "<< Lat << " Lng: " << Lng << " East: " << Easting << " North: " << Northing << " Zone: " << Zone << std::endl;

        Lng = local_central_meridian + 2.9;
        local.GetGridCoordinates (Lat, Lng, Zone, Easting, Northing, GridConv, PtScale);
        *pLog << "Lat: "<< Lat << " Lng: " << Lng << " East: " << Easting << " North: " << Northing << " Zone: " << Zone << std::endl;
        

    }catch (std::runtime_error *ex){
        *pLog << "Unexpected, error when testing LocalRedfearn: " << ex->what() << std::endl;
        return 1;
    }

    return 0;
}

void TestVincenty(ufLogger *pLog)
{

  GeographicConversions::Vincenty v("GRS80");

  double Flinders_Peak_lat = -(37+(57+03.72030/60.0)/60.0);
  double Flinders_Peak_lng = 144+(25+29.52440/60.0)/60.0;
  double Buningyong_lat = -(37+(39+10.15610/60.0)/60.0);
  double Buningyong_lng = 143+(55+35.38390/60.0)/60.0;

  double EllipsoidalDistance;
  double ForwardAzimuth;
  double ReverseAzimuth;

  v.inverse(Flinders_Peak_lat,Flinders_Peak_lng,Buningyong_lat,Buningyong_lng,EllipsoidalDistance,ForwardAzimuth,ReverseAzimuth);

  *pLog << "Flinders Peak -> Buningyong: " << std::endl
      << "s: "<< std::fixed << std::setprecision(3) << EllipsoidalDistance
      << " az(1->2): " << std::setprecision(8) << ForwardAzimuth
      << " az(2->1): " << std::setprecision(8) << ReverseAzimuth << std::endl;

  v.inverse(Buningyong_lat,Buningyong_lng,Flinders_Peak_lat,Flinders_Peak_lng,EllipsoidalDistance,ForwardAzimuth,ReverseAzimuth);

  *pLog << "Buningyong -> Flinders Peak: " << std::endl
      << "s: " << std::fixed << std::setprecision(3) << EllipsoidalDistance
      << " az(1->2):  " << std::setprecision(8) << ForwardAzimuth
      << " az(2->1):  " << std::setprecision(8) << ReverseAzimuth << std::endl;

  double p1 = -30;
  double l1 = 157;
  double p2 = -10;
  double l2 = 157;
  v.inverse(p1,l1,p2,l2,EllipsoidalDistance,ForwardAzimuth,ReverseAzimuth);
  *pLog << "p1->p2: " << std::endl
      << "s: " << std::fixed << std::setprecision(3) << EllipsoidalDistance
      << " az(1->2):  " << std::setprecision(8) << ForwardAzimuth
      << " az(2->1):  " << std::setprecision(8) << ReverseAzimuth << std::endl;

  ForwardAzimuth = 306+(52+05.373/60.0)/60.0;
  EllipsoidalDistance = 54972.271;
  v.direct(Flinders_Peak_lat,Flinders_Peak_lng,EllipsoidalDistance,ForwardAzimuth,Buningyong_lat,Buningyong_lng,ReverseAzimuth);
  *pLog << "Flinders Peak -> Buningyong: " << std::endl
      << "lat: "<< std::fixed << std::setprecision(8) << Buningyong_lat
      << " long: " << std::setprecision(8) << Buningyong_lng
      << " az(2->1): " << std::setprecision(8) << ReverseAzimuth << std::endl;

  ForwardAzimuth = 180;
  EllipsoidalDistance = 54972.271;
  v.direct(Flinders_Peak_lat,Flinders_Peak_lng,EllipsoidalDistance,ForwardAzimuth,Buningyong_lat,Buningyong_lng,ReverseAzimuth);
  *pLog << "Flinders Peak -> p2: " << std::endl
      << "lat: "<< std::fixed << std::setprecision(8) << Buningyong_lat
      << " long: " << std::setprecision(8) << Buningyong_lng
      << " az(2->1): " << std::setprecision(8) << ReverseAzimuth << std::endl;

}

int TestRedfearnForSomePositons(ufLogger *pLog)
{
  GeographicConversions::Redfearn Redfearn("WGS84","UTM");
  try {
    if ( !(Redfearn.EllipsoidIsValid() && Redfearn.MapGridIsValid()) )
    {
      *pLog << "Invalid for some reason, WGS84 and UTM should exist. " << std::endl;
      return 1;
    }
  }
  catch (std::exception *ex)
  {
    *pLog << "Unexpected, GRS80 and MGA should exist: " << ex->what() << std::endl;
    return 1;
  }

  double lat;
  double lon;
  double lat1;
  double lon1;
  double E, N, GC, PS;
  std::string Z;

  int status = 0;

  //*pLog << "Calculation of Easting and Northing from Latitude and Longitude and conversion back to Latitude and Longitude." << std::endl;
  //*pLog << "Latitude, Longitude, Zone, Easting, Northing, Grid Convergence, Point Scale, Latitude, Longitude, Delta latitude, Delta longitude" << std::endl;
  for ( int i = -7; i < 8; ++i ) // Latitudes
  {
    lat = i * 22.5/2;
    if ( lat == 90 )
    {
      lat = 89.9;
    }
    if ( lat == -90 )
    {
      lat = -89.9;
    }
    for ( int j = -16; j < 17; ++j ) // Longitudes
    {
      lon = j * 22.5/2;
      if ( lon == 180 )
      {
        lon = 179.999999;
      }
      Redfearn.GetGridCoordinates(lat, lon, Z, E, N, GC, PS);
      Redfearn.GetGeographicCoordinates(Z, E, N, lat1, lon1, GC, PS);
      if ( std::abs(lat1 - lat) > 1.0e-8 || std::abs(lon1 - lon) > 1.0e-8 )
      {
        status = 0;
        *pLog << "Calculation of Easting and Northing from Latitude and Longitude and conversion back to Latitude and Longitude failed." << std::endl;
        *pLog << "Latitude, Longitude, Zone, Easting, Northing, Grid Convergence, Point Scale, Latitude, Longitude, Delta latitude, Delta longitude" << std::endl;
        *pLog << std::fixed << std::setprecision(6) << lat << ", " << lon << ", ";
        *pLog << Z << ", " << std::fixed << std::setprecision(3) << E << ", " << N << ", " << GC << ", " << PS << ", ";
        *pLog << std::fixed << std::setprecision(6) << lat1 << ", " << lon1 << ", ";
        *pLog << std::fixed << std::setprecision(9) << lat1 - lat << ", " << lon1 - lon << std::endl;
      }
    }
  }

  return status;
}

int GeographicConversionsMain(int argc, char* argv[])
{
  // Get rid of the argc, argv unreferenced formal parameter warnings.
  int numargs;
  std::string programName;
  numargs = argc;
  if ( argc != 0 )
  {
    programName = argv[0];
  }

  std::string sep = "------------------------------------------------------------------------";
  // Set up the log file.
  ufLogger *pLog = new ufLogger;
  pLog->open("TestGeographicConversions.log",std::ios::out);
  std::string time;
  Time::SystemTime *pClock = Time::SystemTime::Instance();
  pClock->ISOLocalTime(time);
  *pLog << sep << "\nStarted:  " << time << std::endl;

  bool fail = false;

  *pLog << "Testing Geographic Conversions" << std::endl;

  DisplayMapGrids(pLog);
  DisplayEllipsoids(pLog);
  fail = TestRedfearn(pLog)!= 0;
  TestVincenty(pLog);
  fail &= TestRedfearnForSomePositons(pLog) != 0;

  fail &= TestLocalRedfearn(pLog) != 0;

  pClock->ISOLocalTime(time);
  *pLog << ((fail)?"Failed.":"Passed") << "\n"
      << "Finished: " << time << "\n" << sep << std::endl;

  delete pLog;

  if ( fail )
    return 1;

  return 0;
}

