#version 450

layout( location = 0 ) out vec4 fragColor;
uniform vec3 uAmbiant;
uniform vec3 uDiffuse;
uniform vec3 uSpecular;
uniform float uShininess;

in vec3 Norm;
in vec3 FragPosView;
in vec3 Lpos;

vec3 removeNegLight( vec3 normal, vec3 viewDir )
{
	if ( dot( normal, viewDir ) < 0.0)
		normal *= -1;
	return normal;
}

void main()
{
	const vec3 viewDir = normalize(-FragPosView);
	const vec3 fragNormal = removeNegLight( normalize( Norm ), viewDir );
	const float angle = max( dot( fragNormal, Lpos ), 0.0 );
	const float spec = pow( dot( fragNormal, normalize( viewDir + Lpos ) ), uShininess );

	fragColor = vec4(uAmbiant + angle * uDiffuse + spec * uSpecular,1.f);
}
