#!/bin/bash

gen_dir="djinni-src"
base_dir=$(cd "`dirname "0"`" && pwd)
cpp_out="$base_dir/$gen_dir/cpp"
jni_out="$base_dir/$gen_dir/jni"
objc_out="$base_dir/$gen_dir/objc"
java_out="$base_dir/$gen_dir/java/com/mycompany/helloworld"
java_package="com.mycompany.helloworld"
namespace="helloworld"
objc_prefix="HW"
djinni_file="helloworld.djinni"

ThirdParty/Djinni/src/run \
   --java-out $java_out \
   --java-package $java_package \
   --ident-java-field mFooBar \
   \
   --cpp-out $cpp_out \
   --cpp-namespace $namespace \
   \
   --jni-out $jni_out \
   --ident-jni-class NativeFooBar \
   --ident-jni-file NativeFooBar \
   \
   --objc-out $objc_out \
   --objc-type-prefix $objc_prefix \
   \
   --objcpp-out $objc_out \
   \
   --idl $djinni_file
