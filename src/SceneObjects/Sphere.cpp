#include <cmath>

#include "Sphere.h"

using namespace std;


bool Sphere::intersectLocal( const ray& r, isect& i ) const
{
    // YOUR CODE HERE:
    // Add sphere intersection code here.
    // it currently ignores all spheres and just return false.

    const int x = 0, y = 1, z = 2;  // For the dumb array indexes for the vectors

    Vec3d normal;

    Vec3d P0 = r.getPosition();
    Vec3d Rd = r.getDirection();

    //
    // Compute the a, b, c terms of the sphere equation
    //
    double a = Rd[x]*Rd[x] + Rd[y]*Rd[y] + Rd[z]*Rd[z];
    double b = 2*(P0[x]*Rd[x] + P0[y]*Rd[y] + P0[z]*Rd[z]);
    double c = P0[x]*P0[x] + P0[y]*P0[y] + P0[z]*P0[z] - 1;

    //
    // Compute discriminant
    //
    double discriminant = b * b - 4 * a * c;

    double BIG_NUMBER = 1e100;
    double t1, t2;

    //
    // If discriminant < 0 we have imaginary numbers for
    // our solution and no intersection - return false.
    //
    if(discriminant < 0)
    {
        return false;
    }

    //
    // Compute sphere intersection points
    //
    t2 = (-b + sqrt(discriminant)) / ( 2 * a );
    t1 = (-b - sqrt(discriminant)) / ( 2 * a );

    if(t2 <= RAY_EPSILON)
    {
        return false;
    }

    //
    // Two intersections
    //
    if(t1 > RAY_EPSILON)
    {
        //
        // Compute the normal which is just r at the nearRoot x,y,z coordinates
        //
        normal = Vec3d(r.at(t1));
        normal.normalize();

        i.setT(t1);
        i.setN(normal);
        i.obj = this;

        return true;
    }

    // TODO: Do we need to compute t2? Possibly for reflection/shadows
    return false;

}

