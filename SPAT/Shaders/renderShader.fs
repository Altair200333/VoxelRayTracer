#version 460
out vec4 FragColor;
in vec2 texcoord;

#define PI 3.1415926f
uniform int textureWidth;
uniform int textureHeight;
uniform int backType;

uniform samplerBuffer VertexSampler0;
uniform vec3 origin;
uniform vec3 front;
uniform vec3 right;
uniform vec3 up;
uniform float fov;

uniform float scale;

uniform float width;
uniform float height;


struct DirectionalLight
{
	float strength;
	vec3 color;
	float lightDist;
	vec3 dir;
};
struct World
{
	vec3 skyColor;
	vec3 floorColor;
	vec3 horizonColor;
	float horizonAngle;
};

uniform DirectionalLight dirLight;
uniform World world;

struct Octree
{
    vec3 color;
    vec3 pos;
    float size;
    int children[8];
    vec3 bounds[2];
	bool isLeaf;
	bool isEmpty;

};
struct Ray
{
	vec3 orig, dir;
	vec3 invdir;
	int sign[3];
};

Octree load(int pos)
{
    int id = pos/4;
    vec4 v1 = texelFetch(VertexSampler0, id + 0);
    vec4 v2 = texelFetch(VertexSampler0, id + 1);
    vec4 v3 = texelFetch(VertexSampler0, id + 2);
    vec4 v4 = texelFetch(VertexSampler0, id + 3);
    vec4 v5 = texelFetch(VertexSampler0, id + 4);
    vec4 v6 = texelFetch(VertexSampler0, id + 5);

    Octree oc;
    oc.pos = v1.xyz;
    oc.size = v1.w;
    oc.color = vec3(v2.x, v2.y, v2.z);

    oc.bounds[0] = vec3(v2.w, v3.xy);
    oc.bounds[1] = vec3(v3.zw, v4.x);

    oc.children[0] = int(v4.y);
    oc.children[1] = int(v4.z);
    oc.children[2] = int(v4.w);
    oc.children[3] = int(v5.x);
    oc.children[4] = int(v5.y);
    oc.children[5] = int(v5.z);
    oc.children[6] = int(v5.w);
    oc.children[7] = int(v6.x);

	oc.isLeaf = v6.y > 0;
	oc.isEmpty = v6.z > 0;

    return oc;
}
bool AABBtest(Octree oc, Ray r)
{
	float t1 = (oc.bounds[0].x - r.orig.x) * r.invdir.x;
	float t2 = (oc.bounds[1].x - r.orig.x) * r.invdir.x;

	float tmin = min(t1, t2);
	float tmax = max(t1, t2);

	t1 = (oc.bounds[0].y - r.orig.y) * r.invdir.y;
	t2 = (oc.bounds[1].y - r.orig.y) * r.invdir.y;

	tmin = max(tmin, min(t1, t2));
	tmax = min(tmax, max(t1, t2));

	t1 = (oc.bounds[0].z - r.orig.z) * r.invdir.z;
	t2 = (oc.bounds[1].z - r.orig.z) * r.invdir.z;

	tmin = max(tmin, min(t1, t2));
	tmax = min(tmax, max(t1, t2));

	return tmax > max(tmin, 0.0);
}
float sqDist(vec3 v1, vec3 v2)
{
    return dot(v1-v2, v1-v2);
}

vec3 dirs[6] = {
			{ 1, 0, 0},
			{-1, 0, 0},
			{ 0, 1, 0},
			{ 0,-1, 0},
			{ 0, 0, 1},
			{ 0, 0,-1}
		};

vec3 intersectPoint(vec3 rayVector, vec3 rayPoint, vec3 planeNormal, vec3 planePoint)
{
	float denom = dot(planeNormal, rayVector);
	float t = dot((planePoint - rayPoint), planeNormal) / denom;
	return rayPoint + rayVector * t;
}	
bool isInBox(vec3 point, vec3 boxPos, float halfWidth)
{
	float epsilon = 0.0001f;
	return (abs(point.x - boxPos.x) <= halfWidth + epsilon) && (abs(point.y - boxPos.y) <= halfWidth + epsilon) && (abs(point.z - boxPos.z) <= halfWidth + epsilon);
}
struct Contact
{
	vec3 position;
	vec3 normal;
};
Contact cubeHit(vec3 rayVector, vec3 rayPoint, vec3 boxPos, float halfWidth)
{
	vec3 closest = vec3(100000, 100000, 10000);
	int normalId = -1;
	for (int i = 0; i < 6; ++i)
	{
		vec3 pos = intersectPoint(rayVector, rayPoint, dirs[i] * halfWidth, boxPos + dirs[i] * halfWidth);
		if (isInBox(pos, boxPos, halfWidth) && length(pos-rayPoint) < length(closest - rayPoint))
		{
			normalId = i;
			closest = pos;
		}
	}
	Contact c;
	c.position = closest;
	c.normal = dirs[normalId];

	return c;
}
int iterativeHitNode(Ray r)
{
	int buf[1000];
	int front = 0;
	int rear = 0;
	buf[front++] = 0;
	
	int closest = -1;

	while (front != rear)
	{
		if(front >= 1000)
			return -1;
		Octree node = load(buf[rear++]);
		Octree closestNode = load(closest);
		if(node.isLeaf && (closest == -1 || (sqDist(node.pos, r.orig) <= sqDist(closestNode.pos, r.orig))))
		{
			closest = buf[rear - 1];
		}
		
		for (int i = 0; i < 8; ++i)
		{
			
			Octree child = load(node.children[i]);
			if(node.children[i] != -1 && AABBtest(child, r))
			{
				buf[front++] = node.children[i];
			}
			
		}
	}
	return closest;
}
vec3 getSkyBackground(vec3 from, vec3 direction)
{
	float horizon = acos(direction.z);
	float azimuth = atan(direction.y / direction.x);


	if (horizon <= world.horizonAngle)
	{
		float ratio = pow(cos(horizon * PI / (2 * world.horizonAngle)), 0.8);
		float dotProduct = dot(direction, dirLight.dir);

		vec3 topColor = world.skyColor;
		if (dotProduct < 0)
		{
			vec3 lightColor = dirLight.color;
			float lightRatio = exp(-(dotProduct + 1) * (dotProduct + 1) * 800)*dirLight.strength;

			topColor = lightColor * lightRatio + world.skyColor * (1 - lightRatio);
		}
		return topColor * ratio + world.horizonColor * (1 - ratio);
	}
	else
	{
		float ratio = pow(cos(horizon - world.horizonAngle), 30);
		return world.horizonColor * ratio + world.floorColor * (1 - ratio);
	}
}

vec3 getBackground(vec3 from, vec3 direction)
{
	if(backType == 0)
		return getSkyBackground(from, direction);
	else if(backType == 1)
	{
		return vec3(0.1);
	}

	return vec3(0);
}
vec3 multiplyColors(vec3 color1, vec3 otherColor)
{
	return vec3(otherColor.x * color1.x, otherColor.y * color1.y, otherColor.z * color1.z);
}
Ray initRay(vec3 dir, vec3 orig)
{
	Ray r;
	r.dir = normalize(dir);
	r.orig = orig;

	r.invdir = vec3(1.0f/r.dir.x, 1.0f/r.dir.y, 1.0f/r.dir.z);
	r.sign[0] = int(r.invdir.x<0);
	r.sign[1] = int(r.invdir.y<0);
	r.sign[2] = int(r.invdir.z<0);

	return r;
}

void main()
{    
	float px = texcoord.x*width - width * 0.5f;
	float py = texcoord.y*height - height * 0.5f;
	Ray r = initRay(front + right * px * scale + up * py * scale, origin);

	int res = iterativeHitNode(r);

	if(res != -1)
	{
		Octree oc = load(res);
		
		Contact c = cubeHit(r.dir, r.orig, oc.pos, oc.size*0.5f);
		float slope = abs(dot(dirLight.dir, c.normal));
		vec3 color = multiplyColors(oc.color, dirLight.color) * slope * dirLight.strength;

		vec3 shadowPos = c.position + c.normal*0.001f;
		//vec3 lightPos = shadowPos - dirLight.dir*dirLight.lightDist;

		Ray shadowRay = initRay(-dirLight.dir, shadowPos);
		
		int shadowHit = iterativeHitNode(shadowRay);
		
		if(shadowHit != -1)
			color = color*0.1f;
		
		FragColor = vec4(color, 1);
	}
	else
	{
    	FragColor = vec4(getBackground(r.orig, r.dir), 1);
	}
}