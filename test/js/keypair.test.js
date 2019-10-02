// Require WebAssembly Module & Binary
const Module = require("../../build/wasm/js/cryptoplus");

// Require Unit Test Framework Modules
const assert = require("assert");

describe("Crypto Library - KeyPair", () => {
  let wasm, curve;

  before("Prepare WebAssembly Runtime.", done => {
    Module({ wasmBinaryFile: "../../build/wasm/js/cryptoplus.wasm" }).then(
      result => {
        wasm = result;
        curve = wasm.ECCurve.SECP256K1();
        done();
      }
    );
  });

  describe("KeyPair Generation", () => {
    it("Normal Seed", () => {
      const seed = wasm.Utils.stringToBinary("HAHAHA");

      const pair1 = wasm.KeyPair.createWithSeed(curve, seed);
      const pair2 = wasm.KeyPair.createWithSeed(curve, seed);

      assert.equal(
        wasm.Utils.binaryToHex(pair1.getPrivateKey()),
        wasm.Utils.binaryToHex(pair2.getPrivateKey())
      );
      assert.equal(
        wasm.Utils.binaryToHex(pair1.getPublicKey()),
        wasm.Utils.binaryToHex(pair2.getPublicKey())
      );
    });

    it("Long Seed (> 256 bits)", () => {
      const seed = wasm.Utils.stringToBinary(
        "I AM A VERY VERY VERY LONG SENTENCE VERY VERY VERY LONG SENTENCE VERY VERY VERY LONG SENTENCE VERY VERY VERY LONG SENTENCE VERY VERY VERY LONG SENTENCE VERY VERY VERY LONG SENTENCE VERY VERY VERY LONG SENTENCE VERY VERY VERY LONG SENTENCE"
      );

      const pair1 = wasm.KeyPair.createWithSeed(curve, seed);
      const pair2 = wasm.KeyPair.createWithSeed(curve, seed);

      assert.equal(
        wasm.Utils.binaryToHex(pair1.getPrivateKey()),
        wasm.Utils.binaryToHex(pair2.getPrivateKey())
      );
      assert.equal(
        wasm.Utils.binaryToHex(pair1.getPublicKey()),
        wasm.Utils.binaryToHex(pair2.getPublicKey())
      );
    });

    it("Seed's Fixed Block Length (32-bytes) Spoof Attack", () => {
      const origin = wasm.Utils.stringToBinary(
        "I AM A VERY VERY VERY LONG SENTE"
      );
      const spoof = wasm.Utils.stringToBinary(
        "I AM A VERY VERY VERY LONG SENTEN"
      );

      const pair1 = wasm.KeyPair.createWithSeed(curve, origin);
      const pair2 = wasm.KeyPair.createWithSeed(curve, spoof);

      assert.notEqual(
        wasm.Utils.binaryToHex(pair1.getPrivateKey()),
        wasm.Utils.binaryToHex(pair2.getPrivateKey())
      );
      assert.notEqual(
        wasm.Utils.binaryToHex(pair1.getPublicKey()),
        wasm.Utils.binaryToHex(pair2.getPublicKey())
      );
    });

    it("Seed & Random KeyPair", () => {
      curve = wasm.ECCurve.SECP256K1();
      const seed = wasm.Utils.stringToBinary("123");
      const longSeed = wasm.Utils.stringToBinary(
        "I'm a very long string, that very long very long very long very long very long very long very long very long very long"
      );
      const k1 = wasm.KeyPair.create(curve);
      const k2 = wasm.KeyPair.create(curve);
      const k3 = wasm.KeyPair.createWithSeed(curve, seed);
      const k4 = wasm.KeyPair.createWithSeed(curve, seed);
      const k5 = wasm.KeyPair.createWithSeed(curve, longSeed);
      const k6 = wasm.KeyPair.createWithSeed(curve, longSeed);

      assert.equal(k1.eq(k2), false);
      assert.equal(k2.eq(k3), false);
      assert.equal(k3.eq(k4), true);
      assert.equal(k4.eq(k5), false);
      assert.equal(k5.eq(k6), true);
    });
  });

  describe("KeyPair Predict & Process", () => {
    it("KeyPair with Seed is predictable", () => {
      curve = wasm.ECCurve.SECP256K1();
      const seed = wasm.Utils.stringToBinary("Hello, my name is Alex");

      const pair = wasm.KeyPair.createWithSeed(curve, seed);

      const valid = wasm.KeyPair.createWithPrivateKey(
        curve,
        pair.getPrivateKey()
      );
      //assert.equal(wasm.Utils.binaryToHex(valid.getPublicKey()), "3330353633303130303630373241383634384345334430323031303630353242383130343030304130333432303030344141413544363045323338443031343333454144434446453741464443344439463936393133303630374530353941363841453532413044314544333336303930383246464345364543443245413836394431313631334433433732363032334132393939443730423444324336304631314133454530343546443531443333");
      assert.equal(
        wasm.Utils.binaryToHex(valid.getPrivateKey()),
        wasm.Utils.binaryToHex(pair.getPrivateKey())
      );
      assert.equal(
        wasm.Utils.binaryToHex(valid.getPublicKey()),
        wasm.Utils.binaryToHex(pair.getPublicKey())
      );
    });

    it("KeyPair Mathematic", () => {
      curve = wasm.ECCurve.SECP256K1();
      const pair = wasm.KeyPair.create(curve);
      const x = pair.getPrivateElement();
      const y = pair.getPublicElement();
      const gx = curve.getG().mul(curve, x);

      assert.equal(y.eq(gx), true);
    });

    it("Generate Public Key from Private Key", () => {
      curve = wasm.ECCurve.SECP256K1();
      const seed = wasm.Utils.stringToBinary("Hello, my name is Alex :)");
      const pair = wasm.KeyPair.createWithSeed(curve, seed);
      const valid = wasm.KeyPair.createWithPrivateKey(
        curve,
        pair.getPrivateKey()
      );

      assert.equal(
        wasm.Utils.binaryToHex(pair.getPrivateKey()),
        wasm.Utils.binaryToHex(valid.getPrivateKey())
      );
      assert.equal(
        wasm.Utils.binaryToHex(pair.getPublicKey()),
        wasm.Utils.binaryToHex(valid.getPublicKey())
      );
    });
  });
});
