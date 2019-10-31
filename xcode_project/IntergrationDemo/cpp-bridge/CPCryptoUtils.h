// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from utils.djinni

#import <Foundation/Foundation.h>
@class CPECCurve;


@interface CPCryptoUtils : NSObject

+ (nonnull NSData *)ecGenerateSignature:(nullable CPECCurve *)curve
                             privateKey:(nonnull NSData *)privateKey
                                message:(nonnull NSData *)message;

+ (BOOL)ecVerifySignature:(nullable CPECCurve *)curve
                publicKey:(nonnull NSData *)publicKey
                  message:(nonnull NSData *)message
                signature:(nonnull NSData *)signature;

+ (nonnull NSData *)ecEncrypt:(nullable CPECCurve *)curve
                    publicKey:(nonnull NSData *)publicKey
                    plaintext:(nonnull NSData *)plaintext;

+ (nonnull NSData *)ecDecrypt:(nullable CPECCurve *)curve
                   privateKey:(nonnull NSData *)privateKey
                   cyphertext:(nonnull NSData *)cyphertext;

+ (nonnull NSData *)aesEncrypt:(nonnull NSData *)key
                            iv:(nonnull NSData *)iv
                     plainText:(nonnull NSData *)plainText;

+ (nonnull NSData *)aesDecrypt:(nonnull NSData *)key
                            iv:(nonnull NSData *)iv
                 encryptedText:(nonnull NSData *)encryptedText;

+ (nonnull NSData *)merkleTreeRootHash:(nonnull NSArray<NSData *> *)values;

@end