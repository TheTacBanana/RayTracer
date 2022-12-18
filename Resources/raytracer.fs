#version 330 core

uniform vec3 camPos;
uniform vec2 viewportDimensions;
uniform float imageAspectRatio;

out vec4 FragColour;

struct Ray{
    vec3 pos;
    vec3 dir;
};

struct RayHit{
    bool hit;
    vec3 pos;
    int sphereIndex;
    Ray ray;
    float distance;
};

struct Material{
    vec3 colour;
};

struct Sphere{
    vec3 pos;
    float radius;
    Material mat;
};

uniform Ray nullRay = Ray(vec3(0.0), vec3(0.0));
uniform Sphere nullSphere = Sphere(vec3(0.0), 0.0, Material(vec3(0.0))); 
uniform RayHit nullRayHit = RayHit(false, vec3(0.0f), -1, Ray(vec3(0.0), vec3(0.0)), 10000.0f);

uniform vec3 SkyColour = vec3(0.529, 0.807, 0.922);

uniform Sphere spheres[3] = Sphere[3](
                    Sphere(vec3(-0.3f,  0.3f, -1.1f), 0.2f, Material(vec3(1,0,0))),
                    Sphere(vec3( 0.3f,  0.3f, -1.1f), 0.2f, Material(vec3(0,1,0))),
                    Sphere(vec3( 0.0f, -0.3f, -1.1f), 0.2f, Material(vec3(0,0,1)))
                );

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 GetSphereNormal(vec3 pos, int sphereIndex){
    return normalize(pos - spheres[sphereIndex].pos);
}

vec3 ReflectVector(vec3 dir, vec3 normal){
    return dir - (2 * (dot(dir, normal)) * normal);
}

RayHit Intersect(Ray r, int sphereIndex){
    Sphere s = spheres[sphereIndex];
    vec3 dif = r.pos - s.pos;
    float x = dot(dif, r.dir);
    float y = dot(dif, dif) - (s.radius * s.radius);

    float d = x * x - y;

    if (d > 0){
        float xy = sqrt(d);
        float root1 = -x - xy;
        if (root1 >= 0)
            return RayHit(true, r.pos + r.dir * root1, sphereIndex, r, root1);
        float root2 = -x + xy;
        if (root2 >= 0)
            return RayHit(true, r.pos + r.dir * root2, sphereIndex, r, root2);
    }
    return nullRayHit;
}

vec3 TraceIterative(int maxIterations, Ray ray){
    vec3 outColour = vec3(0.0);
    Ray currentRay = ray;
    float shadowAmmount = 1.0;
    float contributionAmmount = 1.0;

    for(int i=0; i < maxIterations; ++i){
        RayHit closest = nullRayHit;

        for(int j=0; j < spheres.length(); ++j){
            RayHit result = Intersect(currentRay, j);
            if (result.hit == true && result.distance < closest.distance)
                closest = result;
        }

        if (closest.hit == true){
            vec3 normal = GetSphereNormal(closest.pos, closest.sphereIndex);
            vec3 reflectedDir = ReflectVector(closest.ray.dir, normal); 
            shadowAmmount *= (0.5 + dot(normal, vec3(0.0, 1.0, 0.0)) * 0.5);
            outColour += spheres[closest.sphereIndex].mat.colour * contributionAmmount * shadowAmmount;
            currentRay = Ray(closest.pos + (0.001 * normal), reflectedDir);
            contributionAmmount *= 0.5;
        } else{
            outColour += SkyColour * contributionAmmount * shadowAmmount;
            break;
        }
    }
    return outColour;
}

Ray CalcRay(){
    float focal = 60;
    float angle = tan(focal * 0.5 * 3.1415926535897932384626433832795 / 180);
    float xx = (2 * (gl_FragCoord.x + 0.5) / viewportDimensions.x - 1) * angle * imageAspectRatio;
    float yy = (-1 + 2 * (gl_FragCoord.y + 0.5) / viewportDimensions.y) * angle;
    return Ray(camPos, normalize(vec3(xx, yy, -1)));
}

void main() {   
    vec3 totalColour = vec3(0.0);
    float samples = 32;
    int depth = 4;
    for (int i=0; i < samples; ++i){
        Ray ray = CalcRay();
        totalColour += TraceIterative(depth, ray);
    }
    FragColour = vec4(totalColour * (1/samples), 1.0);
}