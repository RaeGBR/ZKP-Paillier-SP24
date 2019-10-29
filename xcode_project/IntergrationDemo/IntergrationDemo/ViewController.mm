//
//  ViewController.mm
//  IntergrationDemo
//
//  Created by Franky Lau on 17/1/2019.
//  Copyright © 2019 PolyU. All rights reserved.
//

#import "ViewController.h"

#import "CPECCurve.h"
#import "CPKeyPair.h"
#import "CPUtils.h"
#import "CPRandomUtils.h"

@interface ViewController ()

@end

@implementation ViewController {
    UITextView *_textView;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    _textView = [[UITextView alloc] init];
    _textView.frame = CGRectMake(20.0, 20.0, 380.0, 580.0);
    [self.view addSubview:_textView];
    _textView.text = @"";
    
    // Use the Library to random something
    CPECCurve *curve = [CPECCurve SECP256K1];
    NSString *seedStr = @"Hello Crypto";
    NSData *seed = [CPUtils stringToBinary: seedStr];
    CPKeyPair *keypair = [CPKeyPair createWithSeed:curve seed:seed];
    NSString *skStr = [CPUtils binaryToHex:[keypair getPrivateKey]];
    NSString *pkStr = [CPUtils binaryToHex:[keypair getPublicKey]];
    
    _textView.text = [NSString stringWithFormat:@"%@\n%@: %@\n", _textView.text, @"Private Key", skStr];
    _textView.text = [NSString stringWithFormat:@"%@\n%@: %@\n", _textView.text, @"Public Key", pkStr];
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
