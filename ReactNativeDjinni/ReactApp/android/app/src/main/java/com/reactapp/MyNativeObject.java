package com.reactapp;

/**
 * Created by TekuConcept on 6/8/2017.
 */

import android.widget.Toast;

import com.facebook.react.bridge.NativeModule;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;

import com.mycompany.helloworld.HelloWorld;

import java.util.Map;
import java.util.HashMap;
import java.util.StringTokenizer;

public class MyNativeObject extends ReactContextBaseJavaModule {

  static {
    System.loadLibrary("native-lib");
  }

  private HelloWorld helloWorldInterface;
  private static final String DURATION_SHORT_KEY = "SHORT";
  private static final String DURATION_LONG_KEY = "LONG";

  public MyNativeObject(ReactApplicationContext reactContext) {
    super(reactContext);
    helloWorldInterface = HelloWorld.create();
  }

  @Override
  public String getName() {
    return "MyNativeObject";
  }

  @Override
  public Map<String, Object> getConstants() {
    final Map<String, Object> constants = new HashMap<>();
    constants.put(DURATION_SHORT_KEY, Toast.LENGTH_SHORT);
    constants.put(DURATION_LONG_KEY, Toast.LENGTH_LONG);
    return constants;
  }

  @ReactMethod
  public void show(String message, int duration) {
    String myString = helloWorldInterface.getHelloWorld() + " : " + message;
    Toast.makeText(getReactApplicationContext(), myString, duration).show();
  }
}
