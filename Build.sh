OUT_PATH=$HOME/EagleOS

export PREFIX="${OUT_PATH}/Opt/Cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

make all
