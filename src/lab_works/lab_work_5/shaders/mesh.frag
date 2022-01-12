#version 450

#define BLINN
#define POINTLIGHTS
#define BLOOM
#define FOG
#define NIGHT

layout( binding = 2 ) uniform sampler2D uAmbiantMap;
layout( binding = 1 ) uniform sampler2D uDiffuseMap;
layout( binding = 3 ) uniform sampler2D uSpecularMap;
layout( binding = 4 ) uniform sampler2D uShininessMap;
layout( binding = 5 ) uniform sampler2D uNormalMap;

uniform bool uHasAmbiantMap;
uniform bool uHasDiffuseMap;
uniform bool uHasSpecularMap;
uniform bool uHasShininessMap;
uniform bool uHasNormalMap;

uniform vec3 uAmbiant;
uniform vec3 uDiffuse;
uniform vec3 uSpecular;
uniform float uShininess;

layout( location = 0 ) out vec4 fragColor;
#ifdef BLOOM
layout (location = 1) out vec4 BrightColor;
#endif

uniform vec3 uViewLightPos;

in vec3 aVNormal;
in vec3 aFragViewPos;
in vec2 aUV;
in float aViewspaceDist;
in mat3 aTBN;

#ifdef POINTLIGHTS
struct PointLight {    
    vec3 position;
    float constant;
    float linear;
    float quadratic;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define LIGHTPOINTS_NMBR 4  
uniform PointLight uPointLights[LIGHTPOINTS_NMBR];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shininess)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float angle = max(dot(normal, lightDir), 0.0);
    // specular shading
#ifdef BLINN
	const vec3 reflectDir = normalize( viewDir + lightDir );
	const float spec = pow( max( dot( normal, reflectDir ), 0.0 ), shininess );
#else
	const vec3 reflectDir = reflect( -lightDir, normal );
	const float spec = pow( max( dot( reflectDir, viewDir ), 0.0 ), shininess );
#endif
    // attenuation
    float distance    = length(light.position - fragPos)/2;
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(uDiffuseMap, aUV));
    vec3 diffuse  = light.diffuse  * angle * vec3(texture(uDiffuseMap, aUV));
    vec3 specular = light.specular * spec * vec3(texture(uSpecularMap, aUV));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
#endif

void main()
{
	const vec3 viewDir = normalize(-aFragViewPos);
	const vec3 lightDir = normalize( uViewLightPos - aFragViewPos );

	vec3 fragNormal = vec3( 0.0 );
	if ( uHasNormalMap )
	{
		fragNormal = texture2D( uNormalMap, aUV ).rgb;
		fragNormal = fragNormal * 2.0 - 1.0;
		fragNormal = normalize( aTBN * fragNormal );
	}
	else
	{
		fragNormal = normalize( aVNormal );
		if ( dot( fragNormal, viewDir ) < 0.0 )
			fragNormal *= -1;
	}

	float shininessRes;
	if(uHasShininessMap) shininessRes = texture(uShininessMap, aUV).x;
	else shininessRes = uShininess;
	
#ifdef BLINN
	const vec3 halfDir = normalize( viewDir + lightDir );
	const float spec = pow( max( dot( fragNormal, halfDir ), 0.0 ), shininessRes );
#else
	const vec3 reflected = reflect( -lightDir, fragNormal );
	const float spec = pow( max( dot( reflected, viewDir ), 0.0 ), shininessRes );
#endif

	vec3 diffuseRES;
	vec3 ambiantRES;
	vec3 specularRES;

	const float angle = max( dot( fragNormal, lightDir ), 0.0 );
	if(uHasDiffuseMap) {
		vec4 texel = texture(uDiffuseMap, aUV);
		diffuseRES = angle * texel.rgb;
		if ( texel.a < 0.5 )
			discard;
	}
	else diffuseRES = uDiffuse * angle;

	if(uHasAmbiantMap) ambiantRES = uAmbiant * vec3(texture(uAmbiantMap, aUV));
	else ambiantRES = uAmbiant;

	if(uHasSpecularMap) specularRES = spec * vec3(texture(uSpecularMap, aUV).rrr);
	else specularRES = uSpecular * spec;

	vec3 lightRes = ambiantRES + diffuseRES.xyz + specularRES;

#ifdef NIGHT
	lightRes.r *= 0.08;
	lightRes.g *= 0.15;
	lightRes.b *= 0.3;
#endif

#ifdef POINTLIGHTS
	for(int i = 0; i < LIGHTPOINTS_NMBR; i++)
        lightRes += CalcPointLight(uPointLights[i], fragNormal, aFragViewPos, viewDir, shininessRes);
#endif

#ifdef BLOOM
	float brightness = dot(lightRes.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.5f) BrightColor = vec4(lightRes.rgb,1.f);
    else BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
#endif

	fragColor = vec4(lightRes, 1.f);

#ifdef FOG
	fragColor.x += aViewspaceDist * 0.002;
	fragColor.y += aViewspaceDist * 0.002;
	fragColor.z += aViewspaceDist * 0.002;
	fragColor.w += aViewspaceDist * 0.002;
#endif
}
