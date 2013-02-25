#include "ray.h"
#include "material.h"
#include "scene.h"

const Material &
isect::getMaterial() const
{
    if(material){
        return *material;
    }
    
    if(obj ==  NULL)
    {
        // This should never happen, but to avoid
        // insidious and difficult to debug memory
        // bugs throw here.
        throw;
    }

    return obj->getMaterial();
}
