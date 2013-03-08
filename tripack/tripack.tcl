#!/Users/john/bin/tclkit8.6
#

#critcl::cheaders -I/Users/john/include -I/home/john/include

#critcl::tcl 8.6
critcl::clibraries tripack.o

package provide tripack 1.0

critcl::ccode { 
    #include <stdlib.h>

    extern void trmesh_(int *nrows, float *x, float *y, int *list, int *lptr, int *lend, int *lnew, int *ier);
    extern void trlist_(int *ncc, int *lcc, int *n, int *list, int *lptr, int *lend, int *nrow, int *nt, int *ltri, int *lct, int *ier);
}

critcl::cproc trimesh { Tcl_Interp* ip Tcl_Obj* points } ok {
    int    n;
    float *x;
    float *y;
    float *z;

    Tcl_Obj **plist;
    Tcl_Obj **items;
    int      nitem;

    Tcl_ListObjGetElements(ip, points, &n, &plist);

    int *list = malloc(sizeof(int) * n *  6);
    int *lptr = malloc(sizeof(int) * n *  6);
    int *lend = malloc(sizeof(int) * n);
    int *ltri = malloc(sizeof(int) * n * 12);
    int lnew = 0;
    int ier  = 0;

    int ncc  = 0;
    int lcc  = 0;
    int lct  = 0;
    int nrow = 6;

    x = malloc(sizeof(float) * n);
    y = malloc(sizeof(float) * n);
    z = malloc(sizeof(float) * n);

    int nt   = 0;;
    int i, j;

    Tcl_Obj *result = Tcl_GetObjResult(ip);


    for ( i = 0; i < n; i++ ) {	
	double val;

	Tcl_ListObjGetElements(ip, plist[i], &nitem, &items);

	if ( nitem != 3 ) {
	    Tcl_AppendStringsToObj(result, "point does not have 3 coords", NULL);
	    return TCL_ERROR;
	}

	if ( Tcl_GetDoubleFromObj(ip, items[0], &val) != TCL_OK ) { return TCL_ERROR; };  x[i] = val;
	if ( Tcl_GetDoubleFromObj(ip, items[1], &val) != TCL_OK ) { return TCL_ERROR; };  y[i] = val;
	if ( Tcl_GetDoubleFromObj(ip, items[2], &val) != TCL_OK ) { return TCL_ERROR; };  z[i] = val;
    }
    
    ier = 0;
    trmesh_(&n, x, y, list, lptr, lend, &lnew, &ier);

    if ( ier != 0 ) {
	Tcl_AppendStringsToObj(result, "cannot mesh:", NULL);
	switch ( ier ) {
	    case -1 : Tcl_AppendStringsToObj(result, "too few points", NULL);			break;
	    case -3 : Tcl_AppendStringsToObj(result, "first 3 points are colinear", NULL);	break;
	    default : Tcl_AppendStringsToObj(result, "points are coincident", NULL);		break;
	}
	return TCL_ERROR;
    }

    ier = 0;
    trlist_(&ncc, &lcc, &n, list, lptr, lend, &nrow, &nt, ltri, &lct, &ier);

    if ( ier != 0 ) {
	Tcl_AppendStringsToObj(result, "cannot list", NULL);
	return TCL_ERROR;
    }


    for ( i = 0; i < nt; i++ ) {
	Tcl_Obj *tri = Tcl_NewObj();

	for ( j = 0; j < 3; j++ ) {
	    Tcl_ListObjAppendElement(ip, tri, Tcl_NewDoubleObj(x[ltri[i*6+j]-1]));
	    Tcl_ListObjAppendElement(ip, tri, Tcl_NewDoubleObj(y[ltri[i*6+j]-1]));
	    Tcl_ListObjAppendElement(ip, tri, Tcl_NewDoubleObj(z[ltri[i*6+j]-1]));
	}

	Tcl_ListObjAppendElement(ip, result, tri);
    }

    free(list);
    free(lptr);
    free(lend);
    free(ltri);

    free(x);
    free(y);
    free(z);

    return TCL_OK;
}

