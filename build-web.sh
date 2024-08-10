
if [ ! -d "build-web" ]; then
  mkdir build-web
fi

cd build-web

emcmake cmake ../

cmake --build .
