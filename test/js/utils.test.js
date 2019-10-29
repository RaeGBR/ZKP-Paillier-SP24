// Require WebAssembly Module & Binary
const Module = require("../../build/wasm/js/cryptoplus");

// Require Unit Test Framework Modules
const chai = require("chai");
const { assert } = chai;

describe("Crypto Library - Utils", () => {
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

  describe("Random Utils", () => {
    const length = 16;
    // Use Async / Await to ensure WebAssembly finished the process.
    it("Integer", async () => {
      const int1 = await wasm.RandomUtils.randomInt(length);
      const int2 = await wasm.RandomUtils.randomInt(length);
      assert.notEqual(int1.toHex(), int2.toHex());
      assert.isAtMost(int1.toHex().length / 2, length);
      assert.isAtMost(int2.toHex().length / 2, length);
    });

    it("Integer with Seed", async () => {
      const seed = wasm.Utils.stringToBinary("HAHAHA");
      const answer = "3E0EB07B03CEC22EC11529F5282980A9";

      const int1 = await wasm.RandomUtils.randomIntWithSeed(length, seed);
      assert.equal(int1.toHex(), answer);
      assert.isAtMost(int1.toHex().length / 2, length);
    });

    it("Hex", async () => {
      const h1 = await wasm.RandomUtils.randomHex(length);
      const h2 = await wasm.RandomUtils.randomHex(length);
      assert.notEqual(h1, h2);
      assert.isAtMost(h1.length / 2, length);
      assert.isAtMost(h2.length / 2, length);
    });

    it("Hex with Seed", async () => {
      const seed = wasm.Utils.stringToBinary("HAHAHA");
      const answer = "3E0EB07B03CEC22EC11529F5282980A9";

      const h1 = await wasm.RandomUtils.randomHexWithSeed(length, seed);
      assert.equal(h1, answer);
      assert.equal(h1.length / 2, length);
    });
  });

  describe("Hash Utils", () => {
    it("SHA256", () => {
      const message = wasm.Utils.stringToBinary("MESSAGE");
      const answer = wasm.Utils.hexToBinary(
        "B194D92018D6074234280C5F5B88649C8DB14EF4F2C3746D8A23896A0F6F3B66"
      );
      const result = wasm.HashUtils.sha256(message);
      assert.equal(
        wasm.Utils.binaryToHex(result),
        wasm.Utils.binaryToHex(answer)
      );

      const message2 = wasm.Utils.stringToBinary("MESSAGE2");
      const answer2 = wasm.Utils.hexToBinary(
        "ECBB828A6F634EED1E3E43EBB4378113A0832CD8CCAC8B0A74B511452FC8A35A"
      );
      const result2 = wasm.HashUtils.sha256(message2);
      assert.equal(
        wasm.Utils.binaryToHex(result2),
        wasm.Utils.binaryToHex(answer2)
      );
    });
  });

  describe("EC Utils", () => {
    it("ECDSA", () => {
      const message = wasm.Utils.stringToBinary("VEDA PROJECT !");
      const fakeMessage = wasm.Utils.stringToBinary("fake");
      const curve = wasm.ECCurve.SECP256K1();
      const p1 = wasm.KeyPair.createWithSeed(
        curve,
        wasm.Utils.stringToBinary("key pair 1")
      );
      const p2 = wasm.KeyPair.createWithSeed(
        curve,
        wasm.Utils.stringToBinary("key pair 2")
      );

      const s1 = wasm.CryptoUtils.ecGenerateSignature(
        curve,
        p1.getPrivateKey(),
        message
      );
      const s2 = wasm.CryptoUtils.ecGenerateSignature(
        curve,
        p2.getPrivateKey(),
        message
      );
      const s3 = wasm.CryptoUtils.ecGenerateSignature(
        curve,
        p1.getPrivateKey(),
        message
      );
      const s4 = wasm.CryptoUtils.ecGenerateSignature(
        curve,
        p2.getPrivateKey(),
        message
      );

      assert.notEqual(wasm.Utils.binaryToHex(s1), wasm.Utils.binaryToHex(s3));
      assert.notEqual(wasm.Utils.binaryToHex(s2), wasm.Utils.binaryToHex(s4));
      assert.notEqual(wasm.Utils.binaryToHex(s1), wasm.Utils.binaryToHex(s2));

      assert.equal(
        wasm.CryptoUtils.ecVerifySignature(
          curve,
          p1.getPublicKey(),
          message,
          s1
        ),
        true
      );
      assert.equal(
        wasm.CryptoUtils.ecVerifySignature(
          curve,
          p2.getPublicKey(),
          message,
          s2
        ),
        true
      );
      assert.equal(
        wasm.CryptoUtils.ecVerifySignature(
          curve,
          p1.getPublicKey(),
          message,
          s3
        ),
        true
      );
      assert.equal(
        wasm.CryptoUtils.ecVerifySignature(
          curve,
          p2.getPublicKey(),
          message,
          s4
        ),
        true
      );

      assert.equal(
        wasm.CryptoUtils.ecVerifySignature(
          curve,
          p1.getPublicKey(),
          message,
          s2
        ),
        false
      );
      assert.equal(
        wasm.CryptoUtils.ecVerifySignature(
          curve,
          p2.getPublicKey(),
          message,
          s1
        ),
        false
      );
      assert.equal(
        wasm.CryptoUtils.ecVerifySignature(
          curve,
          p1.getPublicKey(),
          fakeMessage,
          s1
        ),
        false
      );
      assert.equal(
        wasm.CryptoUtils.ecVerifySignature(
          curve,
          p2.getPublicKey(),
          fakeMessage,
          s2
        ),
        false
      );
    });
  });
});
