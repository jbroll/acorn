
critcl:ccode { 
    extern trmesh_(int *nrows, float *x, float *y, int *list, int *lptr, int *lend, int *lnew, int i*er);
    extern trlist_(int *ncc, int *lcc, int *n, int *list, int *lptr, int *lend, int *nrow, int *nt, int *ltri, int *lct, int *ier);
}

cproc trimesh { Tcl_Interp* ip Tcl_Obj* points } ok {
    float *x 
    float *y
    float *z

    int *list = malloc(sizeof(int) * n * 6);
    int *lptr = malloc(sizeof(int) * n * 6);
    int *lend = malloc(sizeof(int) * n *  );
    int lnew = 0;
    int ier  = 0;

    int ncc  = 0;
    int lcc  = 0;
    int ltc  = 0;
    int nrow = 6

    int nt   = 0;;

    TclGetListFromObj(ip, points, &plist, &n);

    for ( i = 0; i < n; i++ ) {	
	TclGetListFromObj(objv[0], &nitem, &items);

	x[i] = TclGetDoubleFromObj(items[0]);
	y[i] = TclGetDoubleFromObj(items[1]);
	z[i] = TclGetDoubleFromObj(items[2]);
    }
    

    trmesh_(&n, x, y, list, lptr, lend, &lnew, &ier);
    trlist_(&ncc, &lcc, &n, list, lptr, lend, &nrow, &nt, ltri, &lct, &ier);

    Tcl_Obj *reply = Tcl_GetResultObj(ip);

    for ( i = 0; i < nt; i++ ) {
	Tcl_Obj *tri = Tcl_NewListObj();

	for ( j = 0; i < 3; j++ ) {
	    Tcl_AppendListObj(tri, Tcl_NewDoubleObj(x[ltri[i*6+j])
		    		 , Tcl_NewDoubleObj(y[ltri[i*6+j])
				 , Tcl_NewDoubleObj(z[ltri[i*6+j]));
	}

	Tcl_AppendListObj(reply, tri);
    }

    free(list);
    free(lptr);
    free(lend);

    return TCL_OK;
}

