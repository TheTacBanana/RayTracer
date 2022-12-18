#version 330 core

uniform vec3 camPos;
uniform vec2 viewportDimensions;
uniform float imageAspectRatio;

out vec4 FragColour;

struct Sphere {
    vec3 origin;
    float radius;
    vec3 colour;
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct HitRecord {
    bool hit;
    vec3 position;
    Sphere shape;
    Ray ray;
    float distance;
};

uniform Sphere spheres[3] = Sphere[3](
                    Sphere(vec3(-0.3f,  0.3f, -1.1f), 0.2f, vec3(1,0,0)),
                    Sphere(vec3( 0.3f,  0.3f, -1.1f), 0.2f, vec3(0,1,0)),
                    Sphere(vec3( 0.0f, -0.3f, -1.1f), 0.2f, vec3(0,0,1))
                );
uniform Ray nullRay = Ray(vec3(0.0), vec3(0.0));
uniform Sphere nullSphere = Sphere(vec3(0.0), 0.0f, vec3(0.0)); 
uniform HitRecord nullHitRecord = HitRecord(false, vec3(0.0f), Sphere(vec3(0.0), 0.0f, vec3(0.0)), Ray(vec3(0.0), vec3(0.0)), 10000.0f);

vec3 GetSphereNormal(vec3 pos, Sphere sphere){
    return normalize(pos - sphere.origin);
}

vec3 ReflectVector(vec3 dir, vec3 normal){
    return dir - (2 * (dot(dir, normal)) * normal);
}

HitRecord Intersect(Ray r, Sphere s){
    vec3 dif = r.origin - s.origin;
    float x = dot(dif, r.direction);
    float y = dot(dif, dif) - (s.radius * s.radius);

    float d = x * x - y;

    if (d > 0){
        float xy = sqrt(d);
        float root1 = -x - xy;
        if (root1 >= 0)
            return HitRecord(true, r.origin + r.direction * root1, s, r, root1);
        float root2 = -x + xy;
        if (root2 >= 0)
            return HitRecord(true, r.origin + r.direction * root2, s, r, root2);
    }
    return nullHitRecord;
}

vec3 TraceIterative(int maxIterations, Ray ray){
    vec3 outColour = vec3(0.0);
    Ray currentRay = ray;
    float shadowAmmount = 1.0;

    for(int i=0; i < maxIterations; ++i){
        HitRecord closest = nullHitRecord;

        for(int j=0; j < spheres.length(); ++j){
            HitRecord result = Intersect(currentRay, spheres[j]);
            if (result.hit == true && result.distance < closest.distance)
                closest = result;
        }

        if (closest.hit == true){
            vec3 normal = GetSphereNormal(closest.position, closest.shape);
            vec3 reflectedDir = ReflectVector(currentRay.direction, normal);
            shadowAmmount *= (0.5 + dot(normal, vec3(0.0, 1.0, 0.0)) * 0.5);
            outColour += closest.shape.colour * shadowAmmount;
            currentRay = Ray(closest.position + (0.001 * normal), reflectedDir);
        } else
            break;
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
    Ray ray = CalcRay();
    vec3 resultantColour = TraceIterative(10, ray);
    FragColour = vec4(resultantColour, 1.0);
}