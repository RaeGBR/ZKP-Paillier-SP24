// Require WebAssembly Module & Binary
const Module = require('../../build/wasm/js/cryptoplus');

// Require Unit Test Framework Modules
const assert = require('assert');

describe('Crypto Library - Elliptic Curve', () => {
  let wasm, curve;

  before('Prepare WebAssembly Runtime.', (done) => {
    Module({ wasmBinaryFile: '../../build/wasm/js/cryptoplus.wasm' }).then((result) => {
      wasm = result;
      done();
    });
  });

  it('SECP256K1 Curve', () => {
    curve = wasm.EC.SECP256K1();

    assert.equal(curve.getP().toHex(), 'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F');
    assert.equal(curve.getA().toString(), '0');
    assert.equal(curve.getB().toString(), '7');
    assert.equal(curve.getN().toHex(), 'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141');
    assert.equal(wasm.Utils.binaryToHex(curve.encodePoint(curve.getG(), true)), '0279BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798');
    assert.equal(wasm.Utils.binaryToHex(curve.encodePoint(curve.getG(), false)), '0479BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8');
  });

  it('ECPoint Arithmetic', () => {
    curve = wasm.EC.SECP256K1();
    const G = curve.getG();

    const p1 = curve.add(curve.add(G, G), G);
    const p2 = curve.mul(wasm.Integer.createWithString('3'), G);
    assert.equal(p1.eq(p2), true);
    assert.equal(p1.eq(G), false);

    const p3 = G.add(curve, G).add(curve, G).add(curve, p1);
    const p4 = curve.mul(wasm.Integer.createWithString('2'), p1);
    assert.equal(p3.eq(p4), true);
    assert.equal(p3.eq(G), false);
  });

  it('ECPoint Encoding & Decoding', () => {
    curve = wasm.EC.SECP256K1();
    const g1 = curve.decodePoint(wasm.Utils.hexToBinary('0279BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798'));
    const g2 = curve.decodePoint(wasm.Utils.hexToBinary('0479BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8'));

    assert.equal(g1.eq(g2), true);
    assert.equal(g1.eq(curve.getG()), true);
    assert.equal(g2.eq(curve.getG()), true);

    assert.equal(wasm.Utils.binaryToHex(curve.encodePoint(g1, true)), wasm.Utils.binaryToHex(curve.encodePoint(g2, true)));
    assert.equal(wasm.Utils.binaryToHex(curve.encodePoint(g1, false)), wasm.Utils.binaryToHex(curve.encodePoint(g2, false)));
    assert.notEqual(wasm.Utils.binaryToHex(curve.encodePoint(g1, true)), wasm.Utils.binaryToHex(curve.encodePoint(g2, false)));
    assert.notEqual(wasm.Utils.binaryToHex(curve.encodePoint(g1, false)), wasm.Utils.binaryToHex(curve.encodePoint(g2, true)));

    assert.notEqual(curve.encodePoint(g1.add(curve, g2), true), curve.encodePoint(g2, true));
  });

  it('Generate ECPoint', () => {
    curve = wasm.EC.SECP256K1();
    const pair = wasm.KeyPair.createWithSeed(curve, wasm.Utils.stringToBinary('123'));

    // Check The Point is constant first
    const point = pair.getPublicElement();

    const valid = wasm.ECPoint.create(
      wasm.Integer.createWithString('D238072CA679C9C71410EDC3276DD099C9A09C83D692E540992D431FFB4158A3'),
      wasm.Integer.createWithString('7BBF04361C785266C4593B8CB29FB7A237D257401F8F26D632C1F81CAA64F494')
    );

    assert.equal(point.getX().toString(), valid.getX().toString());
    assert.equal(point.getY().toString(), valid.getY().toString());

    // Check the Encoding is Constant first
    const compressed = curve.encodePoint(point, true);
    const encoded = curve.encodePoint(point, false);

    assert.equal(wasm.Utils.binaryToHex(encoded), "04D238072CA679C9C71410EDC3276DD099C9A09C83D692E540992D431FFB4158A37BBF04361C785266C4593B8CB29FB7A237D257401F8F26D632C1F81CAA64F494");
    assert.equal(wasm.Utils.binaryToHex(compressed), "02D238072CA679C9C71410EDC3276DD099C9A09C83D692E540992D431FFB4158A3");

    // Decode Compressed Point
    const result = curve.decodePoint(compressed);
    assert.equal(point.getX().toString(), result.getX().toString());
    assert.equal(point.getY().toString(), result.getY().toString());

    // Decode Encoded Point
    const result2 = curve.decodePoint(encoded);
    assert.equal(point.getX().toString(), result2.getX().toString());
    assert.equal(point.getY().toString(), result2.getY().toString());
  });
});
