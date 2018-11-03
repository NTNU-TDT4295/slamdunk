#pragma once

class BBox
{
 public:
	BBox() {}
	BBox(vec3 min_, vec3 max_) {
		bounds[0] = min_;
		bounds[1] = max_;
	}
	BBox& extendBy(const vec3 &p) {
		if(p.x < bounds[0].x) bounds[0].x = p.x;
		if(p.y < bounds[0].y) bounds[0].y = p.y;
		if(p.z < bounds[0].z) bounds[0].z = p.z;
		if(p.x > bounds[1].x) bounds[1].x = p.x;
		if(p.y > bounds[1].y) bounds[1].y = p.y;
		if(p.z > bounds[1].z) bounds[1].z = p.z;

		return *this;
	}

	float GetIntersection(const Ray &r) const {
		float tmin, tmax, tymin, tymax, tzmin, tzmax;

		tmin = (bounds[r.sign[0]].x - r.GetOrigin().x) * r.invDir.x;
		tmax = (bounds[1 - r.sign[0]].x - r.GetOrigin().x) * r.invDir.x;
		tymin = (bounds[r.sign[1]].y - r.GetOrigin().y) * r.invDir.y;
		tymax = (bounds[1 - r.sign[1]].y - r.GetOrigin().y) * r.invDir.y;

		if((tmin > tymax) || (tymin > tmax))
			return false;

		if(tymin > tmin)
			tmin = tymin;
		if(tymax < tmax)
			tmax = tymax;

		tzmin = (bounds[r.sign[2]].z - r.GetOrigin().z) * r.invDir.z;
		tzmax = (bounds[1 - r.sign[2]].z - r.GetOrigin().z) * r.invDir.z;

		if((tmin > tzmax) || (tzmin > tmax))
			return false;

		if(tzmin > tmin)
			tmin = tzmin;
		if(tzmax < tmax)
			tmax = tzmax;

		float t = tmin;

		if(t < 0) {
			t = tmax;
			if(t < 0) return false;
		}

		return t;
	}

	vec3 bounds[2];

 private:
	vec3 min;
	vec3 max;
};
