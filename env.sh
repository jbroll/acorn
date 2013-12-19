
FC=gfortran
CC=cc
CPP=c++

case $(hostname) in
 panic)
	export FC=gfortran-4.6
	export CC=gcc-4.6
	export CPP=g++-4.6
 ;;
 listener)
	export FC=gfortran-mp-4.7
	export CC=gcc-mp-4.8
	export CPP=g++
 ;;
esac
 
