package hk.edu.polyu.comp.myapplication;

import android.R.*;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import polyu.cryptoplus.EC;
import polyu.cryptoplus.KeyPair;
import polyu.cryptoplus.Utils;


public class MainActivity extends AppCompatActivity {

  static {
    System.loadLibrary("cryptoplus");
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    TextView txt = (TextView) findViewById(R.id.txtResult);
    String result = "Hi\n\n";

    EC curve = EC.SECP256K1();
    byte[] seed = Utils.stringToBinary("Hello PolyU");
    KeyPair key = KeyPair.createWithSeed(curve, seed);
    result += "sk: " + Utils.binaryToHex(key.getPrivateKey()) + "\n\n";
    result += "pk: " + Utils.binaryToHex(key.getPublicKey()) + "\n\n";

    txt.setText(result);
  }
}
