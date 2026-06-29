 
# Custom settings, as referred to as "extra_script" in platformio.ini
#
# See http://docs.platformio.org/en/latest/projectconf.html#extra-script

from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()

# General options that are passed to the C and C++ compilers
env.Append(CCFLAGS=["-Wno-implicit-function-declaration", "-Wno-error=implicit-function-declaration","-Wno-int-conversion"])
