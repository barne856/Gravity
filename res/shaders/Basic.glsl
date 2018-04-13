#shader vertex
#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 velocity;

uniform mat4 view;

out vec3 newPosition;
out vec3 newVelocity;

out float c;

uniform float dt = 0.0001;
uniform int COUNT;

uniform vec3 center;
uniform samplerBuffer positions;

void detectCollision()
{
	if(newPosition.x > 1.0)
	{
		newVelocity.x = -newVelocity.x;
	}
	if(newPosition.x < -1.0)
	{
		newVelocity.x = -newVelocity.x;
	}
	if(newPosition.y > 1.0)
	{
		newVelocity.y = -newVelocity.y;
	}
	if(newPosition.y < -1.0)
	{
		newVelocity.y = -newVelocity.y;
	}
	if(newPosition.z > 1.0)
	{
		newVelocity.z = -newVelocity.z;
	}
	if(newPosition.z < -1.0)
	{
		newVelocity.z = -newVelocity.z;
	}
}

vec3 centralForce()
{
	vec3 force;
	float r = length(center - position);
	vec3 u = (center - position)/r;
	float kd = 0.01;
	if(r > 0.01)
	{
		force = (500.0)*u*(1.0/(pow(r, 2))) - (velocity) * kd;
	}
	return force;
}

void nBody()
{
	vec3 force = {0.0, 0.0, 0.0};
	for(int i = 0; i < COUNT; i++)
	{
		vec3 p2 = texelFetch(positions, i).xyz;
		float r = length(p2 - position);
		if(r > 0.01)
		{
			vec3 u = (p2 - position)/r;
			float kd = 0.01;
			force = force + (1000.0/COUNT)*u*(1.0/(pow(r, 2))) - (velocity) * kd;
		}
	}
	//force = force + centralForce();
	newPosition = position + velocity*dt;
	newVelocity = velocity + force*dt;
	detectCollision();
	
}

void main()
{
	nBody();
	gl_Position = view * vec4(newPosition, 1.0);
	c = length(newVelocity)/80;
};

#shader fragment
#version 450 core

layout(location = 0) out vec4 color;

uniform vec4 uColor;

in float c;

vec4 pink = {1.0, 0.078, 0.882, 1.0};
vec4 cyan = {0.078, 0.764, 1.0, 1.0};

void main()
{
	color = mix(cyan, pink, clamp(c, 0.0, 1.0));
};