

#version 330 core
out vec4 FragColor;

in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec4 Position_lightspace;

uniform sampler2D textureSampler;
uniform sampler2D shadowMap;

uniform vec3 LightPosition_worldspace;
uniform vec3 CameraPosition_worldspace;


float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal_cameraspace);
    vec3 lightDir = normalize(LightPosition_worldspace - Position_worldspace);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main()
{
    vec3 LightColor = vec3(1,1,1);
	float LightPower = 100.0f;

    // Material properties
	vec3 MaterialDiffuseColor = texture( textureSampler, UV ).rgb;
	vec3 MaterialAmbientColor = vec3(0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3);

    // Distance to the light
	float distance = length( LightPosition_worldspace - Position_worldspace );

	// Normal of the computed fragment, in camera space
	vec3 n = normalize( Normal_cameraspace );

	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( LightDirection_cameraspace );
	// Cosine of the angle between the normal and the light direction,
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp( dot( n,l ), 0,1 );

	// Eye vector (towards the camera)
	vec3 E = normalize(EyeDirection_cameraspace);

	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);

    // Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp( dot( E,R ), 0,1 );


    // vec3 color = texture(textureSampler, fs_in.TexCoords).rgb;
    // vec3 normal = normalize(fs_in.Normal);

    // ambient
    // vec3 ambient = 0.2 * LightColor;
    // // diffuse
    // vec3 lightDir = normalize(LightPosition_worldspace - fs_in.FragPos);
    // float diff = max(dot(lightDir, normal), 0.0);
    // vec3 diffuse = diff * LightColor * LightPower;
    // // specular
    // vec3 viewDir = normalize(CameraPosition_worldspace - fs_in.FragPos);
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = 0.0;
    // vec3 halfwayDir = normalize(lightDir + CameraPosition_worldspace);
    // spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    // vec3 specular = spec * LightColor * LightPower;
    // calculate shadow
    float shadow = ShadowCalculation(Position_lightspace);
    
    vec3 ambient = MaterialAmbientColor;
    vec3 diffuse = MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance);
    vec3 specular = MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);

    vec3 lighting = (
        ambient +
        // (1.0 - shadow) * 
        (diffuse + specular)
    ) * MaterialDiffuseColor;
    // vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * MaterialDiffuseColor;

    FragColor = vec4(lighting, 1.0);
}

