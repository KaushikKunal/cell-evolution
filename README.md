# SFML Template

This is a super basic SFML template for Mac with VSCode. You shouldn't have to download anything.

## Running
Building and running the code is done with  `Makefile`. Therefore all you have to do is `cd src/` and run `make`.

## Debugging
If you run into linking errors, remap the libraries. For example, if you get the following error...
```bash
dyld: Library not loaded: @rpath/../Frameworks/freetype.framework/Versions/A/freetype
  Referenced from: /Users/kunalsingh/Desktop/Kunal/Stuff/Programs/cpp-tests/sfml_test/src/lib/libsfml-graphics.2.6.1.dylib
  Reason: image not found
zsh: abort      ./sfml_test
```

...you would remap the library with -
```bash
install_name_tool -change @rpath/../Frameworks/freetype.framework/Versions/A/freetype @loader_path/../frameworks/freetype.framework/Versions/A/freetype sfml/lib/libsfml-graphics.2.6.1.dylib
```
Note that if you think the `.dylib` is messed up, you can start from scratch by downloading SFML for Mac Clang 64-bit at https://www.sfml-dev.org/download/sfml/2.6.1/ and copy over the specific file from `lib/`.

You might also run into permission errors on Mac. Go to Settings->Security & Privacy and click 'Open Anyways' or equivalent.