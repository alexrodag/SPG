#pragma once

#include <spg/types.h>
#include <spg/constants.h>

#include <vector>
namespace spg
{
class SimObject;

namespace solver
{
class BaseSolver
{
public:
    virtual ~BaseSolver() = default;
    virtual void step() = 0;
    void addSimObject(const SimObject &object);
    std::vector<SimObject> &simObjects() { return m_simObjects; }
    const std::vector<SimObject> &simObjects() const { return m_simObjects; }
    void setDt(Real dt) { m_dtStep = dt; }
    Real dt() { return m_dtStep; }
    void setNumSubsteps(int nsubsteps) { m_nsubsteps = nsubsteps; }
    int numSubsteps() { return m_nsubsteps; }
    virtual void reset();
    void setVerbosity(Verbosity verbosity) { m_verbosity = verbosity; }
    void setGravity(const Vector3 &gravity) { m_gravity = gravity; }

protected:
    std::vector<SimObject> m_simObjects;
    Vector3 m_gravity{0, -9.8, 0};
    Real m_time{0};
    Real m_dtStep{0.01};
    int m_nsubsteps{1};
    Verbosity m_verbosity{Verbosity::None};
};
}  // namespace solver
}  // namespace spg