#version 460 core
out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	sampler2D specular;    
	float shininess;
}; 

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;

/* the location of the light source */
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
	/* ambient */
	/* take the light’s color, multiply it with a small constant ambient factor */
  vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	/* diffuse */
	/*  calculate is the direction vector between the light source and the fragment’s position */
	/*  calculate this difference by subtracting both vectors. We also want to make sure all the relevant
			vectors end up as unit vectors so we normalize both the normal and the resulting direction vector */
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	/* calculate the actual diffuse impact the light has on the current fragment by taking the
			dot product of the norm and lightDir vector. The resulting value is then multiplied with the light’s color
			to get the diffuse component, resulting in a darker diffuse component the greater the angle is between both
			vectors */
	float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
	
	/* Next we calculate the view direction vector and the corresponding reflect vector along the
			normal axis:*/
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 
	/* actually calculate the specular component. This is accomplished with the following formula */
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  

			
	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
} 