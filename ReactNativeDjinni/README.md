This is my attempt at using Djinni to generate native source files for Android and iOS. Then using those native files to extend React-Native.<br>

Note: Djinni is not supported on Windows yet. For Windows users, a Debian Linux VM can be setup to quickly generate source files.<br>

### A Special Thanks To The Following Articles Online

C++ iOS/Android Djinni: http://mobilecpptutorials.com/<br>
React Android-Native: http://facebook.github.io/react-native/docs/native-modules-android.html<br>
React iOS-Native: https://facebook.github.io/react-native/docs/native-modules-ios.html<br>

## Setting Up Your Environment
#### Windows Users:

1. Setup a Linux virtual machine: *[VirtualBox](https://www.virtualbox.org/wiki/Downloads) with the latest [Debian image](https://www.debian.org/distrib/).*
2. Back to Windows: Install [node](https://nodejs.org/en/): *v7.6.0.*
3. `npm install -g create-react-native-app`: v0.0.6
4. `npm install -g ignite-cli`: v2.0.0-rc.2
5. `npm install -g react-native-cli`: v2.0.1
6. `npm install -g yarn`: v0.24.5
5. Install [Android Studio](https://developer.android.com/studio/index.html): v2.3.3
    * Create an AVD by going into Tools > Android > AVD Manager (use Nougat v7 32-bit)
    * Go to Tools > Andoird > SDK Manager and install LLDB and Cmake (for C++)
    * File > Other Settings > Default Project Structure; Install Android NDK
6. iOS development needs to be done on a Mac with XCode do to its proprietary nature.

## Setting Up A Project
1. Create a new folder that will contain your project and enter it.
2. In a terminal, type the following:
```
git init
git submodule add https://github.com/dropbox/djinni.git ThirdParty/Djinni
```
3. Create an interface file &lt;your interface&gt;.djinni
4. Create a script to run djinni: *run_djinni.sh* (run this on Linux VM); Djinni will save the generated sources to the directory specified (here it is *djinni-src*).
5. Create a folder in the project directory to store your C++ implementation files `src/cpp`
6. In the project directory run the command `ignite new <YourAppName>` (here it is ReactApp)

Everything is now setup for development. React-Native native modules can be edited using respective IDEs for the mobile devices. For example, for Android, the Android Studio IDE can be used to modify and add native modules to the app.

In Android Studio, select *Import Project* on startup and select *"&lt;project_dir&gt;\\&lt;YourAppName&gt;\\android"*.

