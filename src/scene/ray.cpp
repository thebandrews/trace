#include "ray.h"
#include "material.h"
#include "scene.h"

const Material &
isect::getMaterial() const
{
    if(material){
        return *material;
    }
    else if(obj)
    {
        return obj->getMaterial();
    }
    else{
        return Material();
    }
}
