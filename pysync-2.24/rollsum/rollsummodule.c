/* Rollsum module */

/* This module provides an interface to the librsync rolling checksum
   Message-Digest Algorithm refered to here as Rollsum.
   It requires the files rollsum.c and rollsum.h */


/* Rollsum objects */

#include "Python.h"
#include "rollsum.h"

typedef struct {
    PyObject_HEAD
    Rollsum sum;                       /* the context holder */
} rollsumobject;

staticforward PyTypeObject Rollsumtype;

#define is_rollsumobject(v) ((v)->ob_type == &Rollsumtype)

static rollsumobject *
newrollsumobject(void)
{
    rollsumobject *sump;

    sump = PyObject_New(rollsumobject, &Rollsumtype);
    if (sump == NULL)
        return NULL;

    RollsumInit(&sump->sum);           /* actual initialisation */

    return sump;
}


/* Rollsum methods */

static void
rollsum_dealloc(rollsumobject *sump)
{
    PyObject_Del(sump);
}


/* Rollsum methods-as-attributes */

static PyObject *
rollsum_update(rollsumobject *self, PyObject *args)
{
    unsigned char *cp;
    int len;

    if (!PyArg_ParseTuple(args, "s#", &cp, &len))
        return NULL;

    RollsumUpdate(&self->sum, cp, len);

    Py_INCREF(Py_None);
    return Py_None;
}

static char update_doc [] =
"update (arg)\n\
\n\
Update the rollsum object with the string arg. Repeated calls are\n\
equivalent to a single call with the concatenation of all the\n\
arguments.";


static PyObject *
rollsum_rollin(rollsumobject *self, PyObject *args)
{
    unsigned char c;

    if (!PyArg_ParseTuple(args, "c", &c))
        return NULL;

    RollsumRollin(&self->sum, c);

    Py_INCREF(Py_None);
    return Py_None;
}

static char rollin_doc [] =
"rollin (c)\n\
\n\
Update the rollsum object by rolling in a single byte. Repeated calls are\n\
equivalent to a single call of update() with the concatenation of all the\n\
arguments.";


static PyObject *
rollsum_rollout(rollsumobject *self, PyObject *args)
{
    unsigned char c;

    if (!PyArg_ParseTuple(args, "c", &c))
        return NULL;

    RollsumRollout(&self->sum, c);

    Py_INCREF(Py_None);
    return Py_None;
}

static char rollout_doc [] =
"rollout (c)\n\
\n\
Update the rollsum object by rolling out a single byte. This removes the\n\
first byte provided from the start of the digest.";


static PyObject *
rollsum_rotate(rollsumobject *self, PyObject *args)
{
    unsigned char out,in;

    if (!PyArg_ParseTuple(args, "cc", &out, &in))
        return NULL;
    
    RollsumRotate(&self->sum, out, in);

    Py_INCREF(Py_None);
    return Py_None;
}

static char rotate_doc [] =
"rotate (out,in)\n\
\n\
Update the rollsum object by rotating one byte in and another out. This\n\
removes the first byte from the start of the digest, and adds the second\n\
to the end.";


static PyObject *
rollsum_digest(rollsumobject *self, PyObject *args)
{
    unsigned long aDigest;

    if (!PyArg_ParseTuple(args,""))
        return NULL;

    aDigest=RollsumDigest(&(self->sum));

    return PyInt_FromLong(aDigest);
}

static char digest_doc [] =
"digest() -> int\n\
\n\
Return the digest of the strings and characters included. This is a 32-bit\n\
value returned as an int.";


static PyObject *
rollsum_copy(rollsumobject *self, PyObject *args)
{
    rollsumobject *sump;

    if (!PyArg_ParseTuple(args,""))
        return NULL;

    if ((sump = newrollsumobject()) == NULL)
        return NULL;

    sump->sum = self->sum;

    return (PyObject *)sump;
}

static char copy_doc [] =
"copy() -> rollsum object\n\
\n\
Return a copy (``clone'') of the rollsum object.";


static PyMethodDef rollsum_methods[] = {
    {"digest",    (PyCFunction)rollsum_digest,    METH_VARARGS, digest_doc},
    {"rotate",    (PyCFunction)rollsum_rotate,    METH_VARARGS, rotate_doc},
    {"rollin",    (PyCFunction)rollsum_rollin,    METH_VARARGS, rollin_doc},
    {"rollout",   (PyCFunction)rollsum_rollout,   METH_VARARGS, rollout_doc},
    {"update",    (PyCFunction)rollsum_update,    METH_VARARGS, update_doc},
    {"copy",      (PyCFunction)rollsum_copy,      METH_VARARGS, copy_doc},
    {NULL, NULL}                       /* sentinel */
};

static PyObject *
rollsum_getattr(rollsumobject *self, char *name)
{
    if (strcmp(name,"count") == 0)
        return PyInt_FromLong((long)self->sum.count);
    return Py_FindMethod(rollsum_methods, (PyObject *)self, name);
}

static char module_doc [] =
"This module implements the interface to the Rollsum implementation of the\n\
librsync rolling checksum message digest algorithm. Its use is quite\n\
straightforward: use the new() to create a rollsum object. You can now\n\
feed this object with arbitrary strings using the update() method, and\n\
at any point you can ask it for the digest (a weak kind of 32-bit\n\
checksum, a.k.a. ``fingerprint'') of the concatenation of the strings\n\
fed to it so far using the digest() method.\n\
\n\
Additionally you can ''rollin'', ''rollout'', or ''rotate'' individual bytes\n\
in and out of the digest. This allows you to rapidly ''roll'' the checksum\n\
of a moving window over a stream of data to find arbitarily aligned block\n\
matches.\n\
\n\
Functions:\n\
\n\
new([arg]) -- return a new rollsum object, initialized with arg if provided\n\
\n\
Special Objects:\n\
\n\
RollsumType -- type object for rollsum objects\n\
";

static char rollsumtype_doc [] =
"A rollsum represents the object used to calculate the Rollsum checksum of a\n\
string of information.\n\
\n\
Methods:\n\
\n\
update() -- updates the current digest with an additional string\n\
rotate() -- updates the digest, rolling single bytes in and out\n\
rollin() -- updates the current digest, rolling a single byte in\n\
rollout() -- updates the current digest, rolling a single byte out\n\
digest() -- return the current digest value\n\
copy() -- return a copy of the current rollsum object\n\
\n\
Variables:\n\
\n\
count -- the count of the bytes included in the digest\n\
";

statichere PyTypeObject Rollsumtype = {
    PyObject_HEAD_INIT(NULL)
    0,                                 /*ob_size*/
    "rollsum",                         /*tp_name*/
    sizeof(rollsumobject),             /*tp_size*/
    0,                                 /*tp_itemsize*/
    /* methods */
    (destructor)rollsum_dealloc,       /*tp_dealloc*/
    0,                                 /*tp_print*/
    (getattrfunc)rollsum_getattr,      /*tp_getattr*/
    0,                                 /*tp_setattr*/
    0,                                 /*tp_compare*/
    0,                                 /*tp_repr*/
    0,                                 /*tp_as_number*/
    0,                                 /*tp_as_sequence*/
    0,                                 /*tp_as_mapping*/
    0,                                 /*tp_hash*/
    0,                                 /*tp_call*/
    0,                                 /*tp_str*/
    0,                                 /*tp_getattro*/
    0,                                 /*tp_setattro*/
    0,                                 /*tp_as_buffer*/
    0,                                 /*tp_xxx4*/
    rollsumtype_doc,                   /*tp_doc*/
};


/* Rollsum functions */

static PyObject *
Rollsum_new(PyObject *self, PyObject *args)
{
    rollsumobject *sump;
    unsigned char *cp = NULL;
    int len = 0;

    if (!PyArg_ParseTuple(args, "|s#:new", &cp, &len))
        return NULL;

    if ((sump = newrollsumobject()) == NULL)
        return NULL;

    if (cp)
        RollsumUpdate(&sump->sum, cp, len);

    return (PyObject *)sump;
}

static char new_doc [] =
"new([arg]) -> rollsum object\n\
\n\
Return a new rollsum object. If arg is present, the method call update(arg)\n\
is made.";


/* List of functions exported by this module */

static PyMethodDef rollsum_functions[] = {
    {"new", (PyCFunction)Rollsum_new, METH_VARARGS, new_doc},
    {NULL,  NULL}                      /* Sentinel */
};


/* Initialize this module. */

DL_EXPORT(void)
initrollsum(void)
{
    PyObject *m, *d;

    Rollsumtype.ob_type = &PyType_Type;
    m = Py_InitModule3("rollsum", rollsum_functions, module_doc);
    d = PyModule_GetDict(m);
    PyDict_SetItemString(d, "RollsumType", (PyObject *)&Rollsumtype);
    /* No need to check the error here, the caller will do that */
}
