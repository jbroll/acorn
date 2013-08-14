
FC=gfortran
CC=cc
CPP=c++

case $(hostname) in
 panic)
	export FC=gfortran-4.6
	export CC=gcc-4.6
	export CPP=g++-4.6
 ;;
esac
 
