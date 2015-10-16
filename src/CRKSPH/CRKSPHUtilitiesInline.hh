//---------------------------------Spheral++----------------------------------//
// CRKSPHUtilities
//
// Useful methods for using the CRKSPH formalism.
//
// Created by JMO, Fri Aug  8 16:16:33 PDT 2008
//----------------------------------------------------------------------------//
#include "Kernel/TableKernel.hh"

namespace Spheral {
namespace CRKSPHSpace {

//------------------------------------------------------------------------------
// Compute the corrected kernel value.
//------------------------------------------------------------------------------
template<typename Dimension>
inline
typename Dimension::Scalar
CRKSPHKernel(const KernelSpace::TableKernel<Dimension>& W,
           const int correctionOrder,
           const typename Dimension::Vector& rij,
           const typename Dimension::Vector& etai,
           const typename Dimension::Scalar& Hdeti,
           const typename Dimension::Vector& etaj,
           const typename Dimension::Scalar& Hdetj,
           const typename Dimension::Scalar& Ai,
           const typename Dimension::Vector& Bi,
           const typename Dimension::Tensor& Ci) {
  typedef typename Dimension::Tensor Tensor;
  if(correctionOrder == 0){
      return Ai*W(etaj.magnitude(), Hdetj);
  }else if(correctionOrder == 1){
      return Ai*(1.0 + Bi.dot(rij))*W(etaj.magnitude(), Hdetj);
  }else {//correctionOrder == 2
      Tensor CiP = Tensor::zero;
      for (size_t ii = 0; ii != Dimension::nDim; ++ii) {
        for (size_t jj = 0; jj != Dimension::nDim; ++jj) {
           if(ii <= jj)CiP(ii,jj)=Ci(ii,jj);
        }
      }
      return Ai*(1.0 + Bi.dot(rij)+CiP.dot(rij).dot(rij))*W(etaj.magnitude(), Hdetj);
  }
}

//------------------------------------------------------------------------------
// Compute the corrected kernel value and gradient.
//------------------------------------------------------------------------------
template<typename Dimension>
inline
void
CRKSPHKernelAndGradient(const KernelSpace::TableKernel<Dimension>& W,
                      const int correctionOrder,
                      const typename Dimension::Vector& rij,
                      const typename Dimension::Vector& etai,
                      const typename Dimension::SymTensor& Hi,
                      const typename Dimension::Scalar& Hdeti,
                      const typename Dimension::Vector& etaj,
                      const typename Dimension::SymTensor& Hj,
                      const typename Dimension::Scalar& Hdetj,
                      const typename Dimension::Scalar& Ai,
                      const typename Dimension::Vector& Bi,
                      const typename Dimension::Tensor& Ci,
                      const typename Dimension::Vector& gradAi,
                      const typename Dimension::Tensor& gradBi,
                      const typename Dimension::ThirdRankTensor& gradCi,
                      typename Dimension::Scalar& WCRKSPH,
                      typename Dimension::Scalar& gradWSPH,
                      typename Dimension::Vector& gradWCRKSPH) {
  typedef typename Dimension::Scalar Scalar;
  typedef typename Dimension::Vector Vector;
  typedef typename Dimension::Tensor Tensor;
  const std::pair<Scalar, Scalar> WWj = W.kernelAndGradValue(etaj.magnitude(), Hdetj);
  const Scalar Wj = WWj.first; 
  if(correctionOrder == 0){
     WCRKSPH = Ai*Wj;
     gradWSPH = WWj.second;
     const Vector gradWj = Hj*etaj.unitVector() * WWj.second;
     //gradWCRKSPH = Ai*(1.0 + Bi.dot(rij))*gradWj + Ai*(Bi + gradBi*rij)*Wj + gradAi*(1.0 + Bi.dot(rij))*Wj;
     gradWCRKSPH = Ai*gradWj + gradAi*Wj;
  }else if(correctionOrder == 1){
     WCRKSPH = Ai*(1.0 + Bi.dot(rij))*Wj;
     gradWSPH = WWj.second;
     const Vector gradWj = Hj*etaj.unitVector() * WWj.second;
     //gradWCRKSPH = Ai*(1.0 + Bi.dot(rij))*gradWj + Ai*(Bi + gradBi*rij)*Wj + gradAi*(1.0 + Bi.dot(rij))*Wj;
     gradWCRKSPH = Ai*(1.0 + Bi.dot(rij))*gradWj + Ai*Bi*Wj + gradAi*(1.0 + Bi.dot(rij))*Wj;
     for (size_t ii = 0; ii != Dimension::nDim; ++ii) {
       for (size_t jj = 0; jj != Dimension::nDim; ++jj) {
         gradWCRKSPH(ii) += Ai*Wj*gradBi(jj,ii)*rij(jj);
       }
     }
  }else {//correctionOrder == 2
      Tensor CiP = Tensor::zero;
      for (size_t ii = 0; ii != Dimension::nDim; ++ii) {
        for (size_t jj = 0; jj != Dimension::nDim; ++jj) {
           if(ii <= jj)CiP(ii,jj)=Ci(ii,jj);
        }
      }
     WCRKSPH = Ai*(1.0 + Bi.dot(rij) + CiP.dot(rij).dot(rij))*Wj;
     gradWSPH = WWj.second;
     const Vector gradWj = Hj*etaj.unitVector() * WWj.second;
     //gradWCRKSPH = Ai*(1.0 + Bi.dot(rij))*gradWj + Ai*(Bi + gradBi*rij)*Wj + gradAi*(1.0 + Bi.dot(rij))*Wj;
     gradWCRKSPH = Ai*(1.0 + Bi.dot(rij))*gradWj + Ai*Bi*Wj + gradAi*(1.0 + Bi.dot(rij))*Wj;
     gradWCRKSPH = Ai*(1.0 + Bi.dot(rij) + CiP.dot(rij).dot(rij))*gradWj + Ai*Bi*Wj + gradAi*(1.0 + Bi.dot(rij) + CiP.dot(rij).dot(rij))*Wj;
     for (size_t ii = 0; ii != Dimension::nDim; ++ii) {
       for (size_t jj = 0; jj != Dimension::nDim; ++jj) {
         gradWCRKSPH(ii) += Ai*Wj*gradBi(jj,ii)*rij(jj);
         if(ii <= jj)gradWCRKSPH(ii) += Ai*Wj*CiP(ii,jj)*rij(jj);
         if(jj <= ii)gradWCRKSPH(ii) += Ai*Wj*CiP(jj,ii)*rij(jj);
         for (size_t kk = 0; kk != Dimension::nDim; ++kk) {
            if(jj <= kk)gradWCRKSPH(ii) += Ai*Wj*gradCi(jj,kk,ii)*rij(jj)*rij(kk);
         }
    
       }
     }
  }
}

}
}
