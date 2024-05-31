#version 410 core

in vec3 localPos;
in vec3 worldPos;
in vec4 pos;

uniform samplerCube envMap;
uniform vec3 camPos;

out vec4 FragColor;

const float n1 = 1;         // Index of refraction for air
const float n2 = 4.f/3.f;   // Index of refraction for water


float FressnelCoeff(vec3 normal, vec3 fromObserver)
{
    //float tmp = (n2 - n1) / (n2 + n1);
    float f0 = 0.17f;

    float cosAlpha = max(dot(normal, fromObserver), 0.f);

    return f0 + (1-f0) * pow(1 - cosAlpha, 5.f);
}


vec3 intersectRay(vec3 pos, vec3 direction)
{
    float tx = max((1 - pos.x) / direction.x, (-1 - pos.x) / direction.x);
    float ty = max((1 - pos.y) / direction.y, (-1 - pos.y) / direction.y);
    float tz = max((1 - pos.z) / direction.z, (-1 - pos.z) / direction.z);

    float t = min(tx, min(ty, tz));
    return pos + direction * t;
}


void main()
{
    vec3 viewVec = normalize(camPos.xyz - worldPos);
    vec3 norm = vec3(0.0f, 1.0f, 0.0f);

    float refrCoeff = n1 / n2;

    if (dot(norm, viewVec) < 0)
    {
        refrCoeff = n2 / n1;
        norm = -norm;
    }

    vec3 reflectedVec = reflect(-viewVec, norm);
    vec3 refractedVec = refract(-viewVec, norm, refrCoeff);

    vec4 reflectedCol = texture(envMap, intersectRay(localPos, reflectedVec));

    if (refractedVec == vec3(0.f, 0.f, 0.f)) {
        FragColor = reflectedCol;
        return;
    }

    vec4 refractedCol = texture(envMap, intersectRay(localPos, refractedVec));

    float fressnel = FressnelCoeff(norm, viewVec);

    FragColor = mix(refractedCol, reflectedCol, fressnel);
}
