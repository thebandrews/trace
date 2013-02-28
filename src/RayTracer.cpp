// The main ray tracer.

#pragma warning (disable: 4786)

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"

#include "parser/Tokenizer.h"
#include "parser/Parser.h"

#include "ui/TraceUI.h"
#include <cmath>
#include <algorithm>

extern TraceUI* traceUI;

#include <iostream>
#include <fstream>

using namespace std;

// Use this variable to decide if you want to print out
// debugging messages.  Gets set in the "trace single ray" mode
// in TraceGLWindow, for example.
bool debugMode = false;

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
Vec3d RayTracer::trace( double x, double y )
{
    // Clear out the ray cache in the scene for debugging purposes,
    if (!traceUI->isMultithreading())
        scene->intersectCache.clear();

    ray r( Vec3d(0,0,0), Vec3d(0,0,0), ray::VISIBILITY );

    scene->getCamera().rayThrough( x,y,r );
    Vec3d ret = traceRay( r, Vec3d(1.0,1.0,1.0), 0 );
    ret.clamp();
    return ret;
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
Vec3d RayTracer::traceRay( const ray& r, 
                          const Vec3d& thresh, int depth )
{
    isect i;

    if( scene->intersect( r, i ) ) {
        // YOUR CODE HERE

        // An intersection occurred!  We've got work to do.  For now,
        // this code gets the material for the surface that was intersected,
        // and asks that material to provide a color for the ray.  

        // This is a great place to insert code for recursive ray tracing.
        // Instead of just returning the result of shade(), add some
        // more steps: add in the contributions from reflected and refracted
        // rays.

        

        const Material& m = i.getMaterial();
        

        const Vec3d P0 = r.getPosition();
        const Vec3d Rd = r.getDirection();
        Vec3d N = i.N;
        double t = i.t;
        Vec3d Q = P0 + t*Rd;
        Vec3d color = Vec3d(0,0,0);

        if(N*(-Rd) <= 0)
        {
            N = -N;
            i.setN(N);
            color = prod(m.kt(i), scene->ambient());
        }

        color += m.shade(scene, r, i);

        if(depth < m_depth){

            //
            // Compute reflected ray
            //
            if(m.kr(i) != Vec3d(0,0,0))
            {

                //
                // Reflection direction according to Shirley:
                // r = d-2(d · n)n
                //
                Vec3d R = Rd - (2*(Rd*N)*N);
                ray reflect( Q, R, ray::REFLECTION );

                color += (prod(m.kr(i),traceRay(reflect, thresh, depth+1)));
            }

            if(m.kt(i) != Vec3d(0,0,0))
            {
                double n_i;
                double n_t;

                //
                // Check to see if ray is entering object
                //
                if(N*(-Rd) > 0)
                {
                    n_i = 1.0003; //index_of_air
                    n_t = m.index(i);
                }
                else
                {
                    n_i = m.index(i);
                    n_t = 1.0003; ///index_of_air;
                }

                double tir_term = 1 - (((n_i*n_i)*(1 - ((Rd*N)*(Rd*N))))/(n_t*n_t));

                if(tir_term >= 0)
                {

                    Vec3d T = ((n_i*(Rd-(N*(Rd*N))))/(n_t)) - (N*(sqrt(tir_term)));
                    ray refract (Q, T, ray::REFRACTION );
                    color = color + (prod(m.kt(i), traceRay(refract, thresh, depth+1)));
                }

                /*double n = n_i / n_t;
                double n_squared = n * n;

                double cos_i = N*Rd;
                double tir_term = 1 - (n_squared*(1-(cos_i*cos_i)));
                double cos_t = sqrt(tir_term);

                if(tir_term >= 0)
                {
                    Vec3d T = (n*cos_i - cos_t)*N - n*N;
                    ray refract (Q, T, ray::REFRACTION );
                    color += (prod(m.kt(i), traceRay(refract, thresh, depth+1)));
                }*/

                ////
                //// Compute total internal reflection
                ////
                //double tir = 1-(((n_i*n_i)*(1-((Rd*N)*(Rd*N))))/(n_t*n_t));

                //if (tir >= 0)
                //{
                //    //
                //    // Compute T - Taken from Shirley
                //    //
                //    Vec3d T = ((n_i*(Rd - N*(Rd*N))) / (n_t)) - N*sqrt(tir);
                //    ray refract (Q, T, ray::REFRACTION );
                //
                //    color += (prod(m.kt(i), traceRay(refract, thresh, depth+1))); 
                //}



            }

            return color;
        }
        else
        {
            return color;
        }


    } else {
        // No intersection.  This ray travels to infinity, so we color
        // it according to the background color, which in this (simple) case
        // is just black.

        return Vec3d( 0.0, 0.0, 0.0 );
    }
}

RayTracer::RayTracer()
    : scene( 0 ), buffer( 0 ), buffer_width( 0 ), buffer_height( 0 ), m_bBufferReady( false )
{
}


RayTracer::~RayTracer()
{
    delete scene;
    delete [] buffer;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
    buf = buffer;
    w = buffer_width;
    h = buffer_height;
}

double RayTracer::aspectRatio()
{
    return sceneLoaded() ? scene->getCamera().getAspectRatio() : 1;
}

bool RayTracer::loadScene( const char* fn )
{
    ifstream ifs( fn );
    if( !ifs ) {
        string msg( "Error: couldn't read scene file " );
        msg.append( fn );
        traceUI->alert( msg );
        return false;
    }

    // Strip off filename, leaving only the path:
    string path( fn );
    if( path.find_last_of( "\\/" ) == string::npos )
        path = ".";
    else
        path = path.substr(0, path.find_last_of( "\\/" ));

    // Call this with 'true' for debug output from the tokenizer
    Tokenizer tokenizer( ifs, false );
    Parser parser( tokenizer, path );
    try {
        delete scene;
        scene = 0;
        scene = parser.parseScene();
    } 
    catch( SyntaxErrorException& pe ) {
        traceUI->alert( pe.formattedMessage() );
        return false;
    }
    catch( ParserException& pe ) {
        string msg( "Parser: fatal exception " );
        msg.append( pe.message() );
        traceUI->alert( msg );
        return false;
    }
    catch( TextureMapException e ) {
        string msg( "Texture mapping exception: " );
        msg.append( e.message() );
        traceUI->alert( msg );
        return false;
    }


    if( ! sceneLoaded() )
        return false;


    // Initialize the scene's BSP tree
    scene->initBSPTree();


    return true;
}

void RayTracer::traceSetup( int w, int h, int depth )
{
    if( buffer_width != w || buffer_height != h )
    {
        buffer_width = w;
        buffer_height = h;

        bufferSize = buffer_width * buffer_height * 3;
        delete [] buffer;
        buffer = new unsigned char[ bufferSize ];

    }
    m_depth = depth;
    memset( buffer, 0, w*h*3 );
    m_bBufferReady = true;
}

void RayTracer::tracePixel( int i, int j )
{
    Vec3d col;

    if( ! sceneLoaded() )
        return;

    double x = double(i)/double(buffer_width);
    double y = double(j)/double(buffer_height);

    col = trace( x, y);

    unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

    pixel[0] = (int)( 255.0 * col[0]);
    pixel[1] = (int)( 255.0 * col[1]);
    pixel[2] = (int)( 255.0 * col[2]);
}

