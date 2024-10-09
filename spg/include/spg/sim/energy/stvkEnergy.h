#pragma once

#include <spg/sim/energy/energy.h>

namespace spg
{
// FEM energy corresponding to StVK model, also in XPBD format
// (ref: https://en.wikipedia.org/wiki/Hyperelastic_material#Saint_Venant%E2%80%93Kirchhoff_model)
class StvkEnergy : public StencilBlockEnergy<4, 2>
{
public:
    StvkEnergy() { m_name = "StvkEnergy"; }
    void addStencil(const std::array<int, s_stencilSize> &stencil, Real young, Real poisson);
    virtual void preparePrecomputations(const SimObject &obj);
    const std::vector<Matrix3> &inverseReferenceMats() const { return m_inverseReferenceMat; }

protected:
    virtual void dConstraints(int i, const SimObject &obj, DConstraintsFirstD &dC) const;
    virtual void dConstraints(int i, const SimObject &obj, DConstraintsSecondD &dC) const;
    virtual void dEnergy(int i, const SimObject &obj, DScalarFirstD &dC) const;
    virtual void dEnergy(int i, const SimObject &obj, DScalarSecondD &dC) const;
    std::vector<Matrix3> m_inverseReferenceMat;
};

}  // namespace spg