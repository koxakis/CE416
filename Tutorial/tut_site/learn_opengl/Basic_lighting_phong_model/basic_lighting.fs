#version 460 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  

/* the location of the light source */
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
	/* ambient */
	/* take the light’s color, multiply it with a small constant ambient factor */
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;
	
	/* diffuse */
	/*  calculate is the direction vector between the light source and the fragment’s position */
	/*  calculate this difference by subtracting both vectors. We also want to make sure all the relevant
			vectors end up as unit vectors so we normalize both the normal and the resulting direction vector */
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	/* calculate the actual diffuse impact the light has on the current fragment by taking the
			dot product of the norm and lightDir vector. The resulting value is then multiplied with the light’s color
			to get the diffuse component, resulting in a darker diffuse component the greater the angle is between both
			vectors */
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	/* Next we calculate the view direction vector and the corresponding reflect vector along the
			normal axis:*/
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	/* actually calculate the specular component. This is accomplished with the following formula */
	/* first calculate the dot product between the view direction and the reflect direction (and make sure
			it’s not negative) and then raise it to the power of 32*/
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;  
			
	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 1.0);
} 