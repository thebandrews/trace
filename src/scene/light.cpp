#include <cmath>

#include "light.h"


using namespace std;

//#define SINGLE_SIDED

double DirectionalLight::distanceAttenuation( const Vec3d& P ) const
{
    // distance to light is infinite, so f(di) goes to 0.  Return 1.
    return 1.0;
}


Vec3d DirectionalLight::shadowAttenuation( const Vec3d& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
    // HINT: You can access the Scene using the getScene function inherited by Light object.

    //
    // Compute shadow ray
    //
    Vec3d d = getDirection(P);
    ray r( P, d, ray::SHADOW );

    isect i;
    getScene()->intersect(r,i);

    double tLight = 1.0e308; // For directional light tLight = infinity.
    double t = i.t;

    Vec3d atten;

    if(t < tLight)
    {
        atten = Vec3d(0,0,0); //i.getMaterial().kt(i);
    }
    else
    {
        atten = Vec3d(1,1,1);
    }

    return atten;
}

Vec3d DirectionalLight::getColor() const
{
    return color;
}

Vec3d DirectionalLight::getDirection( const Vec3d& P ) const
{
    return -orientation;
}

double PointLight::distanceAttenuation( const Vec3d& P ) const
{
    // YOUR CODE HERE

    // You'll need to modify this method to attenuate the intensity 
    // of the light based on the distance between the source and the 
    // point P.  For now, we assume no attenuation and just return 1.0
    double atten = 1.0;

    //
    // Compute Distance
    //
    double d = (position - P).length();

    atten = 1.0 / (constantTerm + (linearTerm*d) + (quadraticTerm*d*d));
    atten = min(1.0, atten);

    return atten;
}

Vec3d PointLight::getColor() const
{
    return color;
}

Vec3d PointLight::getDirection( const Vec3d& P ) const
{
    Vec3d ret = position - P;
    ret.normalize();
    return ret;
}


Vec3d PointLight::shadowAttenuation(const Vec3d& P) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
    // HINT: You can access the Scene using the getScene function inherited by Light object.

    //
    // Compute shadow ray
    //
    Vec3d d = getDirection(P);
    ray r( P, d, ray::SHADOW );

    isect i;
    getScene()->intersect(r,i);

    double tLight = (position - P).length();
    double t = i.t;

    Vec3d atten;

    if(t < tLight)
    {
        atten = Vec3d(0,0,0);//i.getMaterial().kt(i);
    }
    else
    {
        atten = Vec3d(1,1,1);
    }

    return atten;
}
