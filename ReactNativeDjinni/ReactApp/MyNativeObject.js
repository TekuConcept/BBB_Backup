'use strict';
/**
 * This exposes the native MyNativeObject module as a JS module. This has a
 * function 'show' which takes the following parameters:
 *
 * 1. String message: A string with the text to toast
 * 2. int duration: The duration of the toast. May be MyNativeObject.SHORT or
 *    MyNativeObject.LONG
 */
import { NativeModules } from 'react-native';
module.exports = NativeModules.MyNativeObject;