// lighting.inc
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

// version 330 core

struct PointLight
{
	vec3 position;

	vec4 diffuseColour;
	vec4 specularColour;

	float intensity;
	float lightRadius;
};

#define MAX_POINT_LIGHTS 16
uniform int pointLightCount;
uniform PointLight pointLights[MAX_POINT_LIGHTS];


struct SpotLight
{
	vec3 position;
	vec3 direction;

	vec4 diffuseColour;
	vec4 specularColour;

	// for smoothing, anything inside the inner cone is max bright,
	// anything between the inner and outer cones is bright between 0 to 1
	// anything outside the outer cone is 0.

	// note 2: we store the cosine values instead of the raw angle, such that we don't have to calculate
	// any inverse cosines; we get a dot product in the calculations, so we can just compare cos() values
	// directly instead.
	float innerCutoffCosine;
	float outerCutoffCosine;

	float intensity;
	float lightRadius;
};

#define MAX_SPOT_LIGHTS 16
uniform int spotLightCount;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];




struct Material
{
	vec4 ambientColour;

	vec4 diffuseColour;
	vec4 specularColour;

	sampler2D diffuseTexture;
	sampler2D specularTexture;

	float shine;
};

uniform Material material;


// lighting nonsense.
// note: there's actually two separate "ambient" lightings that we're talking about here.
// the first, which is controlled by the 2 uniforms below, is the global background lighting that stops everything from being
// pitch black in a scene without lights -- it's the bare minimum lighting, essentially.

// the second, which is the ambientColour of the material, is the colour the material has under ambient light.
// it's multiplied with the background ambient light, which is usually white, to get the desired effect.

uniform vec4 ambientLightColour;
uniform float ambientLightIntensity;

#define SPECULAR_POWER 1.5



// point lighting
vec4 applyPointLights(vec3 normal, vec3 fragPosition, vec3 viewDirection, vec4 diffuseSample, vec4 specularSample)
{
	vec4 result = vec4(0, 0, 0, 1);
	for(int i = 0; i < pointLightCount; i++)
	{
		vec3 lightDir = normalize(pointLights[i].position - fragPosition);
		vec3 halfwayDir = normalize(lightDir + viewDirection);

		float radius = pointLights[i].lightRadius;

		// Diffuse lighting
		float diff = max(dot(normal, lightDir), 0.0);

		// Specular lighting
		// vec3 reflectDir = reflect(-1 * lightDir, normal);
		float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shine);

		// Attenuation
		float dist = max(length(pointLights[i].position - fragPosition), 0);
		dist -= radius;

		float _denom = (dist / radius) + 1;
		float _att = 1.0 / (_denom * _denom);

		// float _att = 1.0 / (pointLights[i].constantFactor + pointLights[i].linearFactor * dist + pointLights[i].quadFactor * (dist * dist));
		vec4 atten = vec4(_att, _att, _att, 1.0);

		// Combine results
		vec4 diffuse = pointLights[i].diffuseColour * diff * diffuseSample;
		vec4 specular = pointLights[i].specularColour * SPECULAR_POWER * spec * specularSample;

		result += (diffuse + specular) * pointLights[i].intensity * atten;
	}

	return result;
}

vec4 applySpotLights(vec3 normal, vec3 fragPosition, vec3 viewDirection, vec4 diffuseSample, vec4 specularSample)
{
	vec4 result = vec4(0, 0, 0, 1);
	for(int i = 0; i < spotLightCount; i++)
	{
		vec3 lightDir = normalize(spotLights[i].position - fragPosition);
		float radius = spotLights[i].lightRadius;

		float theta = dot(lightDir, normalize(-spotLights[i].direction));
		float epsilon = (spotLights[i].innerCutoffCosine - spotLights[i].outerCutoffCosine);


		{
			// Diffuse lighting
			float diff = max(dot(normal, lightDir), 0.0);

			// Specular lighting
			vec3 reflectDir = reflect(-1 * lightDir, normal);
			float spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.shine);


			float smoothIntensity = clamp((theta - spotLights[i].outerCutoffCosine) / epsilon, 0.0, 1.0);

			// Attenuation
			float dist = max(length(spotLights[i].position - fragPosition), 0);
			dist -= radius;

			float _denom = (dist / radius) + 1;
			float _att = 1.0 / (_denom * _denom);

			vec4 atten = vec4(_att, _att, _att, 1.0);


			vec4 diffuse = spotLights[i].diffuseColour * diff * diffuseSample;
			vec4 specular = spotLights[i].specularColour * SPECULAR_POWER * spec * specularSample;

			result += (diffuse + specular) * spotLights[i].intensity * smoothIntensity * atten;
		}
	}

	return result;
}








