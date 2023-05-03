#version 330

in vec4 vCol;
in vec2 TexCoord;
out vec4 colour;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightColour;
uniform sampler2D texture2D;
uniform vec3 lightPos;
uniform vec3 viewPos;
vec3 diffuseLight()
{
	float diffuseStrength = 0.5f;
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 norm = normalize(Normal);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = lightColour * diff * diffuseStrength;
	return diffuse;
}


vec3 ambientLight()
{
	float ambientStrength = 0.3f;
	vec3 ambient = lightColour * ambientStrength;
	return ambient;
}

vec3 specularLight()
{
	float specularStrength = 0.8f;
	float shininess = 64.0f;
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 norm = normalize(Normal);
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 viewDir = normalize(viewPos - FragPos);
	//phong
	// float spec = pow(max(dot( reflectDir,viewDir), 0.0f), shininess);

	//bling
	vec3 halfDir = (lightDir+viewDir)/2.0f;
	float spec = pow(max(dot( halfDir,norm), 0.0f), shininess);

	vec3 specular = lightColour * spec * specularStrength;
	return specular;
}


void main()
{
	// float ambientStrength = 1f;
	// vec3 ambient = ambientStrength * lightColour;
	colour = texture(texture2D, TexCoord) * vec4(ambientLight()+diffuseLight()+specularLight(), 1.0f);
}