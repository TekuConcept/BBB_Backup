// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from helloworld.djinni

package com.mycompany.helloworld;

import java.util.concurrent.atomic.AtomicBoolean;

/** http://mobilecpptutorials.com/your-first-cross-platform-djinni-app-part-1-cplusplus/ */
public abstract class HelloWorld {
    public abstract String getHelloWorld();

    public static native HelloWorld create();

    private static final class CppProxy extends HelloWorld
    {
        private final long nativeRef;
        private final AtomicBoolean destroyed = new AtomicBoolean(false);

        private CppProxy(long nativeRef)
        {
            if (nativeRef == 0) throw new RuntimeException("nativeRef is zero");
            this.nativeRef = nativeRef;
        }

        private native void nativeDestroy(long nativeRef);
        public void destroy()
        {
            boolean destroyed = this.destroyed.getAndSet(true);
            if (!destroyed) nativeDestroy(this.nativeRef);
        }
        protected void finalize() throws java.lang.Throwable
        {
            destroy();
            super.finalize();
        }

        @Override
        public String getHelloWorld()
        {
            assert !this.destroyed.get() : "trying to use a destroyed object";
            return native_getHelloWorld(this.nativeRef);
        }
        private native String native_getHelloWorld(long _nativeRef);
    }
}
