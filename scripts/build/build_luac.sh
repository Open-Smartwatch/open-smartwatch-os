#/bin/bash

echo "Generate osw_wrap.cxx" 

DEFINES="-DEXAMPLE_LUA -DGAME_BRICK_BREAKER=1 -DGAME_SNAKE=1 \
-DIMGUI_ENABLE_TEST_ENGINE=1 -DIMGUI_TEST_ENGINE_ENABLE_COROUTINE_STDTHREAD_IMPL=1 \
-DOSW_APPS_EXAMPLES -DOSW_EMULATOR=1 -DOSW_FEATURE_LUA -DOSW_FEATURE_STATS_STEPS \
-DOSW_FEATURE_WEATHER -DOSW_SERVICE_CONSOLE -DOSW_TARGET_PLATFORM_HEADER=\"platform/EMULATOR.h\" \
-DPIO_ENV_NAME=\"VIRTUAL\" -DTOOL_CALCULATOR=1 -DTOOL_CLOCK=1 -DTOOL_FLASHLIGHT=1 \
-DTOOL_STOPWATCH=1 -DTOOL_WATERLEVEL=1"



swig "-c++" "-lua" "-I./include" "-I./lib/Arduino_GFX" ${DEFINES} ./src/swig/osw.i
mv ./src/swig/osw_wrap.cxx ./src/swig/osw_wrap.tmp
echo "#ifdef OSW_FEATURE_LUA" > ./src/swig/osw_wrap.cxx
cat ./src/swig/osw_wrap.tmp >> ./src/swig/osw_wrap.cxx
echo "" >> ./src/swig/osw_wrap.cxx
echo "#endif" >> ./src/swig/osw_wrap.cxx
rm ./src/swig/osw_wrap.tmp

# And compile luac for compressing lua chunks
echo "Building luac ..."
make -C ./lib/LUA/ luac

if [ "$?" != "0" ]; then
    echo "Building luac failed with return code: $?"
    exit 1
fi

echo "Building luac successful."


echo "Compiling lua bytecode with luac ..."

cd ./src.lua
for i in $(find . -iname '*.lua'); do 
    echo compiling $i
    ../lib/LUA/luac -s -o ../data/lua/$i $i
done
echo "Compiling lua bytecode successful."
