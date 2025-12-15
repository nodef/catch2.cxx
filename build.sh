#!/usr/bin/env bash
# Fetch the latest version of the library
fetch() {
if [ -d "catch2" ]; then return; fi
URL="https://github.com/catchorg/Catch2/archive/refs/heads/devel.zip"
ZIP="${URL##*/}"
DIR="catch2-devel"
mkdir -p .build
cd .build

# Download the release
if [ ! -f "$ZIP" ]; then
  echo "Downloading $ZIP from $URL ..."
  curl -L "$URL" -o "$ZIP"
  echo ""
fi

# Unzip the release
if [ ! -d "$DIR" ]; then
  echo "Unzipping $ZIP to .build/$DIR ..."
  cp "$ZIP" "$ZIP.bak"
  unzip -q "$ZIP"
  rm "$ZIP"
  mv "$ZIP.bak" "$ZIP"
  echo ""
fi
cd ..

# Copy the libs to the package directory
echo "Copying libs to catch2/ ..."
rm -rf catch2
mkdir -p catch2
cp -rf ".build/$DIR/src/catch2" .
echo ""
}


# Change all #include directives to use relative paths
process() {
  node build.js
}


# Test the project
test() {
echo "Running 01-basic-window.c ..."
clang -I. -I"node_modules/glad.c" -I"node_modules/glfw3.c" -o 01.exe examples/01-basic-window.c  && ./01.exe && echo -e "\n"
# echo "Running 02-slider-panel.c ..."
# clang -I. -I"node_modules/glad.c" -I"node_modules/glfw3.c" -o 02.exe examples/02-slider-panel.c  && ./02.exe && echo -e "\n"
# echo "Running 03-popup-dialog.c ..."
# clang -I. -I"node_modules/glad.c" -I"node_modules/glfw3.c" -o 03.exe examples/03-popup-dialog.c  && ./03.exe && echo -e "\n"
}


# Main script
if [[ "$1" == "test" ]]; then test
elif [[ "$1" == "fetch" ]]; then fetch && process
elif [[ "$1" == "process" ]] || [[ "$1" == "" ]]; then process
else echo "Usage: $0 {fetch|process|test}"; fi
