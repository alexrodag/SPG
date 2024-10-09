#pragma once

#include <spg/sim/energy/energy.h>

namespace spg
{
// Bending energy corresponding to Baraff 1998.
// (ref: "Large Steps in Cloth Simulation")
class BaraffWitkinBendingEnergy : public StencilBlockEnergy<4>
{
public:
    BaraffWitkinBendingEnergy() { m_name = "BaraffWitkinBendingEnergy"; }
    void addStencil(const std::array<int, s_stencilSize> &stencil, Real restTheta, Real stiffness);
    virtual void preparePrecomputations(const SimObject &obj);
    const std::vector<Real> &restThetas() const { return m_restTheta; }

protected:
    virtual void dConstraints(int i, const SimObject &obj, DConstraintsFirstD &dC) const;
    virtual void dConstraints(int i, const SimObject &obj, DConstraintsSecondD &dC) const;
    std::vector<Real> m_restTheta;
};

}  // namespace spg