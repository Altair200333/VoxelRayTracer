#version 460
out vec4 FragColor;
in vec2 texcoord;

#define PI 3.1415926

uniform samplerBuffer VertexSampler0;
uniform vec3 origin;
uniform vec3 front;
uniform vec3 right;
uniform vec3 up;
uniform float fov;

uniform float scale;

uniform float width;
uniform float height;

uniform float time;

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

int nodeMask = 0;
Octree rootNode;
Ray ray;
Ray original_ray;

struct NodeTraversalInfo
{
	int node;
	vec3 t0;
	vec3 t1;
	vec3 tm;
	int nextChild;
};
NodeTraversalInfo stack[400];
int top = 0;
int back = 0;

NodeTraversalInfo buildNodeInfo(float tx0, float ty0, float tz0, float tx1, float ty1, float tz1, int node)
{
	NodeTraversalInfo info;
	info.node = node;
	info.t0 = vec3(tx0, ty0, tz0);
	info.t1 = vec3(tx1, ty1, tz1);
	info.nextChild = -1;
	return info;
}
void initTraversal()
{
	nodeMask = 0;
	original_ray = ray;
	//This algorithm works only for rays with positive directions so we need to mirror ray if goes in negative direction 
	if (ray.dir[0] < 0.0f) 
	{
		ray.orig[0] = rootNode.pos[0] - ray.orig[0];
		ray.dir[0] = -ray.dir[0];
		ray.invdir[0] = -ray.invdir[0];
		nodeMask |= 4; //bitwise OR (latest bits are XYZ)
	}
	if (ray.dir[1] < 0.0f) 
	{
		ray.orig[1] = rootNode.pos[1]- ray.orig[1];
		ray.dir[1] = -ray.dir[1];
		ray.invdir[1] = -ray.invdir[1];
		nodeMask |= 2;
	}
	if (ray.dir[2] < 0.0f) 
	{
		ray.orig[2] = rootNode.pos[2]  - ray.orig[2];
		ray.dir[2] = -ray.dir[2];
		ray.invdir[2] = -ray.invdir[2];
		nodeMask |= 1;
	}
	float tx0 = (rootNode.bounds[0][0] - ray.orig[0]) * ray.invdir[0];
	float tx1 = (rootNode.bounds[1][0] - ray.orig[0]) * ray.invdir[0];
	float ty0 = (rootNode.bounds[0][1] - ray.orig[1]) * ray.invdir[1];
	float ty1 = (rootNode.bounds[1][1] - ray.orig[1]) * ray.invdir[1];
	float tz0 = (rootNode.bounds[0][2] - ray.orig[2]) * ray.invdir[2];
	float tz1 = (rootNode.bounds[1][2] - ray.orig[2]) * ray.invdir[2];
	//push node info on the stack if we intersect root node i.e. bounding box of the scene
	if (max(tx0, max(ty0, tz0)) < min(tx1,min(ty1, tz1))) 
	{
		stack[top++] = buildNodeInfo(tx0, ty0, tz0, tx1, ty1, tz1, 0);
		//stack[front++] = (buildNodeInfo(tx0, ty0, tz0, tx1, ty1, tz1, rootNode));
	}
}
bool isTerminated()
{
	return top<=back;
}
int getCurrentNode()
{
	return stack[top].node;
}
int firstNode(float tx0, float ty0, float tz0, float txm, float tym, float tzm)
{
	int answer = 0;	// initialize to 00000000
	// select the entry plane and set bits
	if (tx0 > ty0) 
	{
		if (tx0 > tz0) // PLANE YZ
		{ 
			if (tym < tx0) answer |= 2;	// set bit at position 1
			if (tzm < tx0) answer |= 1;	// set bit at position 0
			return (answer);
		}
	}
	else 
	{
		if (ty0 > tz0) // PLANE XZ
		{ 
			if (txm < ty0) answer |= 4;	// set bit at position 2
			if (tzm < ty0) answer |= 1;	// set bit at position 0
			return (answer);
		}
	}
	// PLANE XY
	if (txm < tz0) answer |= 4;	// set bit at position 2
	if (tym < tz0) answer |= 2;	// set bit at position 1
	return (answer);
}
int newNode(float txm, int x, float tym, int y, float tzm, int z)
{
	if (txm < tym) 
	{
		if (txm < tzm) { return x; }  // YZ plane
	}
	else 
	{
		if (tym < tzm) { return y; } // XZ plane
	}
	return z; // XY plane;
}
void step()
{
	// if the stack is empty, we're done
	if (isTerminated())
		return;
	vec3 t0 = stack[back].t0;
	vec3 t1 = stack[back].t1;
	vec3 tm = stack[back].tm;
	//POP
	// if we're in a terminal node or if we visited all children of that node (next child = 8), 
	// or if the t1 values have become negative, pop the current node from the stack
	Octree backNode1 = load(stack[back].node);
	if (stack[back].nextChild == 8 || backNode1.isLeaf) {
		//stack.pop_back();
		back++;
		return;
	}
	NodeTraversalInfo backNode = stack[back];
	// PUSH
	// we haven't looked at any child in this voxel yet: pick the first and push down
	if (backNode.nextChild == -1) {
		// calculate midpoint and save it in stack
		tm = (t0 + t1) * 0.5f;
		// calculate first node
		backNode.nextChild = firstNode(t0[0], t0[1], t0[2], tm[0], tm[1], tm[2]);
	}
	// ADVANCE
	// let's look at the next child in this voxel

	Octree backNodenode = load(backNode.node);
	switch (backNode.nextChild)
	{
	case 0: {
		backNode.nextChild = newNode(tm[0], 4, tm[1], 2, tm[2], 1);
		if (backNodenode.children[nodeMask] != -1) {
			NodeTraversalInfo info = buildNodeInfo(t0[0], t0[1], t0[2], tm[0], tm[1], tm[2], backNodenode.children[nodeMask]);
			stack[top++] = info;
		} return; }
	case 1: {
		backNode.nextChild = newNode(tm[0], 5, tm[1], 3, t1[2], 8);
		if (backNodenode.children[1 ^ nodeMask] != -1) {
			NodeTraversalInfo info = buildNodeInfo(t0[0], t0[1], tm[2], tm[0], tm[1], t1[2], backNodenode.children[1 ^ nodeMask]);
			stack[top++] = info;
		} return; }
	case 2: {
		backNode.nextChild = newNode(tm[0], 6, t1[1], 8, tm[2], 3);
		if (backNodenode.children[2 ^ nodeMask] != -1) {
			NodeTraversalInfo info = buildNodeInfo(t0[0], tm[1], t0[2], tm[0], t1[1], tm[2], backNodenode.children[2 ^ nodeMask]);
			stack[top++] = (info);
		} return; }
	case 3: {
		backNode.nextChild = newNode(tm[0], 7, t1[1], 8, t1[2], 8);
		if (backNodenode.children[3 ^ nodeMask] != -1) {
			NodeTraversalInfo info = buildNodeInfo(t0[0], tm[1], tm[2], tm[0], t1[1], t1[2], backNodenode.children[3 ^ nodeMask]);
			stack[top++] = (info);
		} return; }
	case 4: {
		backNode.nextChild = newNode(t1[0], 8, tm[1], 6, tm[2], 5);
		if (backNodenode.children[4 ^ nodeMask] != -1) {
			NodeTraversalInfo info = buildNodeInfo(tm[0], t0[1], t0[2], t1[0], tm[1], tm[2], backNodenode.children[4 ^ nodeMask]);
			stack[top++] = (info);
		} return; }
	case 5: {
		backNode.nextChild = newNode(t1[0], 8, tm[1], 7, t1[2], 8);
		if (backNodenode.children[5 ^ nodeMask] != -1) {
			NodeTraversalInfo info = buildNodeInfo(tm[0], t0[1], tm[2], t1[0], tm[1], t1[2], backNodenode.children[5 ^ nodeMask]);
			stack[top++] = (info);
		} return; }
	case 6: {
		backNode.nextChild = newNode(t1[0], 8, t1[1], 8, tm[2], 7);
		if (backNodenode.children[6 ^ nodeMask] != -1) {
			NodeTraversalInfo info = buildNodeInfo(tm[0], tm[1], t0[2], t1[0], t1[1], tm[2], backNodenode.children[6 ^ nodeMask]);
			stack[top++] = (info);
		} return; }
	case 7:
		{
			backNode.nextChild = 8;
			if (backNodenode.children[7 ^ nodeMask] != -1) 
			{
				NodeTraversalInfo info = buildNodeInfo(tm[0], tm[1], tm[2], t1[0], t1[1], t1[2], backNodenode.children[7 ^ nodeMask]);
				stack[top++] = (info);
			} return;
		}
	}
}
int iterativeHitNode(Ray r)
{
	rootNode = load(0);
	ray = r;

	initTraversal();

	while (!isTerminated())
	{
		int nodeid = getCurrentNode();
		Octree node = load(nodeid);
		//we need to test if the original ray intersects node we just found
		//otherwise rays can collide with mirrored octree
		if (node.isLeaf && AABBtest(node, original_ray))
		{
			return nodeid;
		}
		//just make a step
		step();
	}
	return -1;
}
vec3 getBackground(vec3 from, vec3 direction)
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
// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}



// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }



// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}



// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
void main()
{    

	vec3  inputs = vec3( texcoord.xy, time );
	float rand   = random( inputs );
	
	/*For some reason skipping pixels doesnot improve fps almoust at*/
	//if(rand < 0.9f)
	//{
	//	FragColor = vec4(0.1, 0.1, 0.1, 1);
	//	discard;
	//}
	float px = texcoord.x*width - width * 0.5f;
	float py = texcoord.y*height - height * 0.5f;
	Ray r = initRay(front + right * px * scale + up * py * scale, origin);

	int res = iterativeHitNode(r);

	if(res != -1)
	{
		Octree oc = load(res);

		Contact c = cubeHit(r.dir, r.orig, oc.pos, oc.size*0.5f);
		float slope = abs(dot(dirLight.dir, c.normal));
		vec3 color = multiplyColors(oc.color, dirLight.color)*slope*dirLight.strength;

		vec3 shadowPos = c.position + c.normal*0.01f;
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