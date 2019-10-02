#include "PedersenCommitment.hpp"

shared_ptr<ECPoint> PedersenCommitment::commit(
    const shared_ptr<ECCurve> &curve,
    const shared_ptr<Integer> &x,
    const shared_ptr<ECPoint> &g,
    const shared_ptr<Integer> &r,
    const shared_ptr<ECPoint> &h)
{
  auto xG = g->mul(curve, x->mod(curve->getN()));
  auto rH = h->mul(curve, r->mod(curve->getN()));
  return xG->add(curve, rH);
}

bool PedersenCommitment::verify(
    const shared_ptr<ECCurve> &curve,
    const shared_ptr<ECPoint> &c,
    const shared_ptr<Integer> &x,
    const shared_ptr<ECPoint> &g,
    const shared_ptr<Integer> &r,
    const shared_ptr<ECPoint> &h)
{
  auto c1 = commit(curve, x, g, r, h);
  return c->eq(c1);
}
