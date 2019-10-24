// Require WebAssembly Module & Binary
const Module = require("../../build/wasm/js/cryptoplus");

// Require Unit Test Framework Modules
const assert = require("assert");

describe("Crypto Library - Integer", () => {
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

  it("Default Test", () => {
    const a = wasm.Integer.create("", 10);
    const b = wasm.Integer.createWithString("0");
    assert.equal(a.eq(b), true);
    assert.equal(a.toString(), "0");

    const c = wasm.Integer.ZERO();
    const d = wasm.Integer.ONE();
    assert.equal(a.eq(c), true);
    assert.equal(a.eq(d), false);
    assert.equal(c.toString(), "0");
    assert.equal(d.toString(), "1");
  });

  it("Power Of Two", () => {
    const a = wasm.Integer.pow2(0);
    const b = wasm.Integer.pow2(1);
    const c = wasm.Integer.pow2(2);
    const d = wasm.Integer.pow2(8);
    assert.equal(a.toString(), "1");
    assert.equal(b.toString(), "2");
    assert.equal(c.toString(), "4");
    assert.equal(d.toString(), "256");
  });

  it("Decimal Equals To Heximal", () => {
    const a = wasm.Integer.create("26", 10);
    const b = wasm.Integer.create("1A", 16);
    const c = wasm.Integer.createWithString("1a");
    const d = wasm.Integer.createWithString("0x1a");

    assert.equal(a.eq(b), true);
    assert.equal(a.eq(c), true);
    assert.equal(a.eq(d), true);
    assert.equal(a.toString(), "26");
    assert.equal(a.toHex(), "1A");
  });

  describe("Number Arithmetic", () => {
    it("Add", () => {
      const a = wasm.Integer.create("1234", 10);
      const b = wasm.Integer.create("766", 10);
      const c = wasm.Integer.create("2000", 10);
      assert.equal(a.add(b).eq(c), true);
    });

    it("Subtract", () => {
      const a = wasm.Integer.create("2018", 10);
      const b = wasm.Integer.create("22", 10);
      const c = wasm.Integer.create("1996", 10);
      assert.equal(a.sub(b).eq(c), true);
    });

    it("Multiply", () => {
      const a = wasm.Integer.create("35", 10);
      const b = wasm.Integer.create("10", 10);
      const c = wasm.Integer.create("350", 10);
      assert.equal(a.mul(b).eq(c), true);
    });
  });

  describe("Modular Arithmetic", () => {
    it("Mod", () => {
      const a = wasm.Integer.create("130", 10);
      const b = wasm.Integer.create("11", 10);
      const c = wasm.Integer.create("9", 10);
      assert.equal(a.mod(b).eq(c), true);
    });

    it("Inverse", () => {
      const N = wasm.Integer.create("5", 10);

      const a = wasm.Integer.create("2", 10);
      const b = wasm.Integer.create("3", 10);

      const x = wasm.Integer.create("1", 10);

      assert.equal(
        a
          .mul(b)
          .mod(N)
          .eq(x),
        true
      );
      assert.equal(a.inv(N).eq(b), true);
    });

    it("Multiply", () => {
      const N = wasm.Integer.create("11", 10);
      const a = wasm.Integer.create("3", 10);
      const b = wasm.Integer.create("5", 10);
      const c = a.modMul(b, N);
      const d = b.modMul(a, N);

      assert.equal(c.toString(), "4");
      assert.equal(c.toString(), d.toString());
    });

    it("Power", () => {
      const N = wasm.Integer.create("11", 10);
      const a = wasm.Integer.create("5", 10);
      const b = wasm.Integer.create("2", 10);

      // 5^2 = 25 = 3 mod 11
      // 2^5 = 32 = 10 mod 11
      const c = a.modPow(b, N);
      const d = b.modPow(a, N);

      assert.equal(c.toString(), "3");
      assert.equal(d.toString(), "10");
    });

    it("Square Root", () => {
      const N = wasm.Integer.create("11", 10);
      const a = wasm.Integer.create("3", 10);

      // 5*5 = 25 = 3 mod 11
      // -5 = 11-5 = 6 mod 11
      // 6*6 = 36 = 3 mod 11
      const b = a.modSqrt(N);
      const c = N.sub(b);

      assert.equal(b.toString(), "5");
      assert.equal(c.toString(), "6");
      assert.equal(
        b
          .mul(b)
          .mod(N)
          .toString(),
        a.toString()
      );
      assert.equal(
        c
          .mul(c)
          .mod(N)
          .toString(),
        a.toString()
      );
    });
  });

  describe("MISC Functions", () => {
    it("To String", () => {
      const a = wasm.Integer.create("8216", 10);
      assert.equal(a.toString(), "8216");
      assert.equal(a.toHex(), "2018");
    });

    it("To Binary", () => {
      const a = wasm.Integer.create("8216", 10);
      const b = a.toBinary();
      assert.equal(a.toHex(), "2018");
      assert.equal(b.get(0), 0x20);
      assert.equal(b.get(1), 0x18);
    });

    it("Pure Function", () => {
      const a = wasm.Integer.create("2", 10);
      const b = wasm.Integer.create("3", 10);

      // (2 + 3) * 2 mod 3
      const c = a
        .add(b)
        .mul(a)
        .mod(b);

      assert.equal(a.toString(), "2");
      assert.equal(b.toString(), "3");
      assert.equal(c.toString(), "1");
    });
  });
});
