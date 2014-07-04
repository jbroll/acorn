
    ::critcl::argtype doubleList {
	    if ( getDoubleList(interp, @@, &@A) != TCL_OK ) { { return TCL_ERROR; } }
    } 

    ::critcl::argtypesupport doubleList {
	    typedef struct _doubleList {
		    double *list;
		    int	length;
	    } doubleList;

	    int getDoubleList(Tcl_Interp *interp, Tcl_Obj *obj, doubleList *list) {
		    int i, objc;
		    Tcl_Obj **objv;

		if ( Tcl_ListObjGetElements(interp, obj, &objc, &objv) != TCL_OK ) { return TCL_ERROR; }

		list->list   = (double *) malloc(objc*sizeof(double));
		list->length = objc;

		for ( i = 0; i < objc; i++ ) {
		    if ( Tcl_GetDoubleFromObj(interp, objv[i], &list->list[i]) != TCL_OK ) {
			free(list->list);
			return TCL_ERROR;
		    }
		}

		return TCL_OK;
	    }

    }
