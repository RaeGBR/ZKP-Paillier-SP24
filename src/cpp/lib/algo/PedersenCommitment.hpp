#pragma once

#include "../namespace.hpp"

#include "Integer.hpp"
#include "ECCurve.hpp"
#include "ECPoint.hpp"

namespace cryptoplus
{

class PedersenCommitment
{

public:
    static shared_ptr<ECPoint> commit(
        const shared_ptr<ECCurve> &curve,
        const shared_ptr<Integer> &x,
        const shared_ptr<ECPoint> &g,
        const shared_ptr<Integer> &r,
        const shared_ptr<ECPoint> &h);

    static bool verify(
        const shared_ptr<ECCurve> &curve,
        const shared_ptr<ECPoint> &c,
        const shared_ptr<Integer> &x,
        const shared_ptr<ECPoint> &g,
        const shared_ptr<Integer> &r,
        const shared_ptr<ECPoint> &h);
};

} // namespace cryptoplus
