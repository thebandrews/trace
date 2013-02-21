#include <cmath>
#include <float.h>
#include "trimesh.h"

using namespace std;

Trimesh::~Trimesh()
{
    for( Materials::iterator i = materials.begin(); i != materials.end(); ++i )
        delete *i;
}

bool Trimesh::hasPerVertexNormals()
{
    return !(this->normals.empty());
}

// must add vertices, normals, and materials IN ORDER
void Trimesh::addVertex( const Vec3d &v )
{
    vertices.push_back( v );
}

void Trimesh::addMaterial( Material *m )
{
    materials.push_back( m );
}

void Trimesh::addNormal( const Vec3d &n )
{
    normals.push_back( n );
}

void Trimesh::addTextureUV( const Vec2d &n )
{
    textureuvs.push_back( n );
}

// Returns false if the vertices a,b,c don't all exist
bool Trimesh::addFace( int a, int b, int c )
{
    int vcnt = vertices.size();

    if( a >= vcnt || b >= vcnt || c >= vcnt )
        return false;

    TrimeshFace *newFace = new TrimeshFace( scene, new Material(*this->material), this, a, b, c );
    newFace->setTransform(this->transform);
    faces.push_back( newFace );
    scene->add(newFace);
    return true;
}

char *
    Trimesh::doubleCheck()
    // Check to make sure that if we have per-vertex materials or normals
    // they are the right number.
{
    if( !materials.empty() && materials.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of materials.";
    if( !normals.empty() && normals.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of normals.";

    return 0;
}

// Calculates and returns the normal of the triangle too.
bool TrimeshFace::intersectLocal( const ray& r, isect& i ) const
{
    // YOUR CODE HERE:
    // Add triangle intersection code here.
    // it currently ignores all triangles and just return false.
    //
    // Note that you are only intersecting a single triangle, and the vertices
    // of the triangle are supplied to you by the trimesh class.
    //
    // TrimeshFace::parent->hasPerVertexNormals tells you if the triangle has per-vertex normals.
    // If it does, you should compute and return the interpolated normal at the intersection point.
    // If it does not, you should return the normal of the triangle's supporting plane.
    // 

    const int x = 0, y = 1, z = 2;
    Vec3d normal;

    const Vec3d P0 = r.getPosition();
    const Vec3d Rd = r.getDirection();

    //
    // Retrieve vertices of the triangle
    //
    const Vec3d& a = parent->vertices[ids[0]];
    const Vec3d& b = parent->vertices[ids[1]];
    const Vec3d& c = parent->vertices[ids[2]];

    //
    // Compute normal for plane containing triangle
    //
    const Vec3d ba_vec = b-a;
    const Vec3d ca_vec = c-a;

    normal = ba_vec^ca_vec;
    normal.normalize();

    //
    // If n*d = 0 plane is parallel and ray does not intersect the plane
    //
    double nDotD = normal*Rd;
    if(nDotD == 0)
    {
        return false;
    }

    //
    // Compute coefficient d
    //
    double d = normal*a;

    //
    // Compute t
    //
    double t = (d-(normal*P0))/nDotD;

    if(t > RAY_EPSILON)
    {
        //
        // Find intersection Q
        //
        const Vec3d Q = P0 + t*Rd;

        //
        // Compute if Q is inside triangle
        //
        const Vec3d qa_vec = Q-a;
        const Vec3d cb_vec = c-b;
        const Vec3d qb_vec = Q-b;
        const Vec3d ac_vec = a-c;
        const Vec3d qc_vec = Q-c;

        //
        // If cross product of Q-a and b-a >= 0,
        // then Q is inside triangle.
        //
        if( (((ba_vec ^ qa_vec) * normal) >= 0) &&
            (((cb_vec ^ qb_vec) * normal) >= 0) &&
            (((ac_vec ^ qc_vec) * normal) >= 0))
        {

            if(parent->hasPerVertexNormals())
            {
                //
                // Compute barycentric coordinates for normal interpolation
                //
                double alpha = ((cb_vec ^ qb_vec)*normal) / ((ba_vec ^ ca_vec)*normal);
                double beta  = ((ac_vec ^ qc_vec)*normal) / ((ba_vec ^ ca_vec)*normal);
                double gamma  = ((ba_vec ^ qa_vec)*normal) / ((ba_vec ^ ca_vec)*normal);

                //
                // Get parent vertex normals
                //
                const Vec3d& Na = parent->normals[ids[0]];
                const Vec3d& Nb = parent->normals[ids[1]];
                const Vec3d& Nc = parent->normals[ids[2]];

                //
                // Compute weighted normal
                //
                Vec3d Nq = (alpha * Na) + (beta * Nb) + (gamma * Nc);
                Nq.normalize();

                //
                // Set the normal to Nq
                //
                i.setN(Nq);
            }
            else
            {
                //
                // Set the normal to the plane
                //
                i.setN(normal);
            }

            i.obj = this;
            i.setT(t);
            return true;
        }
    }

    return false;
}


// Once you've loaded all the verts and faces, we can generate per
// vertex normals by averaging the normals of the neighboring faces.
void Trimesh::generateNormals()
{
    int cnt = vertices.size();
    normals.resize( cnt );
    int *numFaces = new int[ cnt ]; // the number of faces assoc. with each vertex
    memset( numFaces, 0, sizeof(int)*cnt );

    for( Faces::iterator fi = faces.begin(); fi != faces.end(); ++fi )
    {
        Vec3d a = vertices[(**fi)[0]];
        Vec3d b = vertices[(**fi)[1]];
        Vec3d c = vertices[(**fi)[2]];

        Vec3d faceNormal = ((b-a) ^ (c-a));
        faceNormal.normalize();

        for( int i = 0; i < 3; ++i )
        {
            normals[(**fi)[i]] += faceNormal;
            ++numFaces[(**fi)[i]];
        }
    }

    for( int i = 0; i < cnt; ++i )
    {
        if( numFaces[i] )
        {
            normals[i]  /= numFaces[i];
            normals[i].normalize();
        }
    }

    delete [] numFaces;
}

