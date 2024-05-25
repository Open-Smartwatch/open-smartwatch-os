# A word - before you start... 

These examples shows you how to create a simple app that display a text and a counter.

**The v1 interface** for osw-apps is very similar to the Arduinos classic setup/loop approach.
Keep in mind, that this interface is not actively maintained anymore and may not allow you to use all the features of the watch - especially in the future, when we add more and more functions depending on v2 interface.
You can get around this by either migrating to v2 or using the compatibility layer (see OswAppV2Compat). Do not fear to start your journey with the v1 interface, as it is still very easy to use and won't be removed any time soon.

**The v2 interface** for osw-apps behaves more like the Android-activity approach, where you can override methods to react to certain events. This interface is the preferred way to create new apps, as we plan to add more and more features that depend on it. Additionally, apps using the v2 interface are more integrated into the OS and allow for e.g. lazy-loading, more efficient rendering, unified button-handling and much more.
While this interface is more complex than the v1 interface, it is still very easy to use as you only need to override the methods you want to implement.

The example apps (v1/v2) are also a small showcase of what you can do with this OS, but they are maybe not the smallest possible examples (I mean, why do these apps use images?!)... 😎
If you want to fiddle around with them - either on the real hardware or in the emulator - you should define the feature flag for them inside the `platformio.ini` file, so that they are compiled and linked into the firmware.

Happy Hacking!
