#version 460 core
out vec4 FragColor;
#define NR_POINT_LIGHTS 5

struct Material 
	{
		sampler2D diffuseMap_floor;
		sampler2D specularMap_floor;    
		float shininess;
	}; 
struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;
uniform DirLight dirLight;

// function prototypes
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{

	/* properties */
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);	
	/* phase 1: directional lighting */
	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	/* phase 2: point lights */
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
			result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
	
	FragColor = vec4(result, 1.0);
		
} 
/* General calculation comments */
  /* diffuse shading */
	/* diffuse */
	/*  calculate is the direction vector between the light source and the fragment’s position */
	/*  calculate this difference by subtracting both vectors. We also want to make sure all the relevant
			vectors end up as unit vectors so we normalize both the normal and the resulting direction vector */
	//vec3 norm = normalize(Normal);
	//vec3 lightDir = normalize(light.position - FragPos);

	/* calculate the actual diffuse impact the light has on the current fragment by taking the
			dot product of the norm and lightDir vector. The resulting value is then multiplied with the light’s color
			to get the diffuse component, resulting in a darker diffuse component the greater the angle is between both
			vectors */
	//float diff = max(dot(norm, lightDir), 0.0);
	//vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  

	/* specular shading */
	/* Next we calculate the view direction vector and the corresponding reflect vector along the
			normal axis:*/
	//vec3 viewDir = normalize(viewPos - FragPos);
	//vec3 reflectDir = reflect(-lightDir, norm); 
	/* actually calculate the specular component. This is accomplished with the following formula */
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	//vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  

	/* spotlight (soft edges) */
	//float theta = dot(lightDir, normalize(-light.direction)); 
	//float epsilon = (light.cutOff - light.outerCutOff);
	//float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	//diffuse  *= intensity;
	//specular *= intensity;


	/* ambient */
	/* take the light’s color, multiply it with a small constant ambient factor */
	//vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
/* calculates the color when using a directional light. */
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    /* diffuse shading */
    float diff = max(dot(normal, lightDir), 0.0);
    /* specular shading */
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    /* combine results */
    vec3 ambient = light.ambient * vec3(texture(material.diffuseMap_floor, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseMap_floor, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specularMap_floor, TexCoords));
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    /* diffuse shading */
    float diff = max(dot(normal, lightDir), 0.0);

    /* specular shading */
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    /* attenuation */
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    /* combine results */
    vec3 ambient = light.ambient * vec3(texture(material.diffuseMap_floor, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseMap_floor, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specularMap_floor, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}
