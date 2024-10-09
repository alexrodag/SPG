#include <spg/sim/energy/membraneBaraffWitkinEnergy.h>
#include <spg/sim/simObject.h>
#include <cmath>

namespace spg
{
namespace
{
auto l_baraffWitkinConstraint =
    [](const MembraneBaraffWitkinEnergy *energy, const int i, const SimObject &obj, auto &dC) {
        const auto &x0p{obj.positions()[energy->stencils()[i][0]]};
        const auto &x1p{obj.positions()[energy->stencils()[i][1]]};
        const auto &x2p{obj.positions()[energy->stencils()[i][2]]};

        using ADouble = std::decay_t<decltype(dC[0])>;
        const Vector3T<ADouble> x0(ADouble(x0p.x(), 0), ADouble(x0p.y(), 1), ADouble(x0p.z(), 2));
        const Vector3T<ADouble> x1(ADouble(x1p.x(), 3), ADouble(x1p.y(), 4), ADouble(x1p.z(), 5));
        const Vector3T<ADouble> x2(ADouble(x2p.x(), 6), ADouble(x2p.y(), 7), ADouble(x2p.z(), 8));
        // Compute deformation gradient
        const Vector3T<ADouble> u = x1 - x0;
        const Vector3T<ADouble> v = x2 - x0;
        const MatrixT<ADouble, 3, 2> deformedMatrix{{u[0], v[0]}, {u[1], v[1]}, {u[2], v[2]}};
        const MatrixT<ADouble, 3, 2> F = deformedMatrix * energy->inverseReferenceMats()[i];

        dC = {F.col(0).norm() - 1, F.col(1).norm() - 1, F.col(0).dot(F.col(1))};
    };
}

void MembraneBaraffWitkinEnergy::addStencil(const std::array<int, s_stencilSize> &stencil,
                                            const Real kWeft,
                                            const Real kWarp,
                                            const Real kBias)
{
    m_stencils.push_back(stencil);

    const Matrix3 C{{kWeft, 0, 0}, {0, kWarp, 0}, {0, 0, kBias}};
    m_modelStiffness.emplace_back(C);
    m_effectiveStiffness.emplace_back(C);
    m_modelCompliance.emplace_back(C.inverse());
    m_effectiveCompliance.emplace_back(C.inverse());
}

void MembraneBaraffWitkinEnergy::preparePrecomputations(const SimObject &obj)
{
    const int nstencils{static_cast<int>(m_stencils.size())};
    m_inverseReferenceMat.resize(nstencils);
    for (int i = 0; i < nstencils; ++i) {
        const auto &x0{obj.positions0()[m_stencils[i][0]]};
        const auto &x1{obj.positions0()[m_stencils[i][1]]};
        const auto &x2{obj.positions0()[m_stencils[i][2]]};
        if (x0.z() != 0.0 || x1.z() != 0.0 || x2.z() != 0.0) {
            throw std::runtime_error(
                "Rest positions of triangles for membrane BaraffWitkin energy must have coordinate z=0");
        }
        // compute rest material matrix
        const Vector2 u = (x1 - x0).segment<2>(0);
        const Vector2 v = (x2 - x0).segment<2>(0);

        m_inverseReferenceMat[i].col(0) = u;
        m_inverseReferenceMat[i].col(1) = v;
        m_inverseReferenceMat[i] = m_inverseReferenceMat[i].inverse().eval();
        const auto area = (u.x() * v.y() - v.x() * u.y()) * 0.5;
        if (area <= 0) {
            throw std::runtime_error("Rest area of membrane BaraffWitkin triangle is not positive");
        }
        m_effectiveCompliance[i] = m_modelCompliance[i] / area;
        m_effectiveStiffness[i] = m_modelStiffness[i] * area;
    }
    StencilBlockEnergy<3, 3>::preparePrecomputations(obj);
}

void MembraneBaraffWitkinEnergy::dConstraints(int i, const SimObject &obj, DConstraintsFirstD &dC) const
{
    l_baraffWitkinConstraint(this, i, obj, dC);
}

void MembraneBaraffWitkinEnergy::dConstraints(int i, const SimObject &obj, DConstraintsSecondD &dC) const
{
    l_baraffWitkinConstraint(this, i, obj, dC);
}
}  // namespace spg