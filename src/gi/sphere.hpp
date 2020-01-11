#pragma once

#include <cmath>
#include <glm/glm.hpp>

#include "constants.hpp"
#include "hit.hpp"
#include "onb.hpp"
#include "ray.hpp"
#include "util.hpp"

class Sphere : public Hittable {
public:
    Sphere(const glm::vec3 &center, const float radius, const P_Material &material) :
        m_Center(center), m_Radius(radius), m_Material(material) {}

    virtual bool Emits() const {
        return m_Material->Emits();
    }

    virtual bool Hit(
        const Ray &ray, const float tmin, const float tmax, HitInfo &hit) const
    {
        const glm::vec3 oc = ray.Origin() - m_Center;
        const float a = glm::dot(ray.Direction(), ray.Direction());
        const float b = glm::dot(oc, ray.Direction());
        const float c = glm::dot(oc, oc) - m_Radius * m_Radius;
        const float d = b * b - a * c;
        if (d > 0) {
            float t = (-b - std::sqrt(b * b - a * c)) / a;
            if (t < tmax && t > tmin) {
                hit.T = t;
                hit.Position = ray.At(t);
                hit.Normal = (hit.Position - m_Center) / m_Radius;
                hit.Material = m_Material;
                return true;
            }
            t = (-b + std::sqrt(b * b - a * c)) / a;
            if (t < tmax && t > tmin) {
                hit.T = t;
                hit.Position = ray.At(t);
                hit.Normal = (hit.Position - m_Center) / m_Radius;
                hit.Material = m_Material;
                return true;
            }
        }
        return false;
    }

    virtual Ray RandomRay(const glm::vec3 &o) const {
        const glm::vec3 dir = m_Center - o;
        const ONB onb(dir);
        const glm::vec3 p = m_Center + onb.LocalToWorld(
            RandomInUnitDisk() * m_Radius);
        return Ray(o, glm::normalize(p - o));
    }

    virtual float Pdf(const Ray &ray) const {
        HitInfo hit;
        if (!Hit(ray, EPS, INF, hit)) {
            return 0;
        }
        const float costhetamax = std::sqrt(
            1 - m_Radius * m_Radius / glm::length2(m_Center - ray.Origin()));
        const float solidangle = 2 * M_PI * (1 - costhetamax);
        return 1 / solidangle;
    }

private:
    glm::vec3 m_Center;
    float m_Radius;
    P_Material m_Material;
};
