#include "user_lm.h"
#include <Python.h>
#include <stdlib.h>

static char *user_lm_py_location = NULL;
static PyObject *main_module, *global_dict, *unigram_py, *bigram_py, *lm_py, *lm_init_py;

void init_user_lms()
{
    if (user_lm_py_location == NULL)
    {
      printf("Path to user_lm.py not supplied, running without python interpretter! Pass it with -user-lm-py the_path.py if needed.\n");
      return;
    }
    // https://www.linuxjournal.com/article/8497
    wchar_t *program = Py_DecodeLocale(user_lm_py_location, NULL);
    Py_SetProgramName(program);
    
    Py_Initialize();
    PyRun_SimpleString("import sys; print('Python interpreter up, version: {}'.format(sys.version_info))");

    FILE *lm_funs_file = fopen(user_lm_py_location, "r");
    PyRun_SimpleFile(lm_funs_file, user_lm_py_location);
    fclose(lm_funs_file);

    // Get a reference to the main module
    // and global dictionary
    main_module = PyImport_AddModule("__main__");
    global_dict = PyModule_GetDict(main_module);

    // Extract a reference to the function "func_name"
    // from the global dictionary
    unigram_py = PyDict_GetItemString(global_dict, "unigram");
    bigram_py = PyDict_GetItemString(global_dict, "bigram");
    lm_py = PyDict_GetItemString(global_dict, "language_model");
    lm_init_py = PyDict_GetItemString(global_dict, "language_model_init");

    PyObject *init_status_py = PyObject_CallObject(lm_init_py, NULL);
    if(!PyBool_Check(init_status_py))
    {
        printf("Did not get bool from language_model_init, exiting\n");
        exit(1);
    }
    
    int init_status = PyLong_AsLong(init_status_py);
    printf("Py LM init status: %d\n", init_status);
    if(init_status == 0)
    {
        printf("Init failed, exiting\n");
        exit(1);
    }
}

LOGPROB unigram(WORD_INFO *winfo, WORD_ID w, LOGPROB ngram_prob){
  // fprintf(stderr, "my_uni call, wordid: %d, logprob: %f\n", w, ngram_prob);
  PyObject * ret_proba_py = PyObject_CallFunction(unigram_py, "yif", winfo->woutput[w], w, ngram_prob);
  if (!PyFloat_CheckExact(ret_proba_py))
  {
    printf("Did not get float from unigram_py, exiting\n");
    exit(1);
  }
  float ret_proba = PyFloat_AS_DOUBLE(ret_proba_py);
  // fprintf(stderr, "got %f\n", ret_proba);
  return ret_proba;
}

LOGPROB bigram(WORD_INFO *winfo, WORD_ID context, WORD_ID w, LOGPROB ngram_prob){
  // fprintf(stderr, "my_bi call, wordid: %d, ncalls: %d, logprob: %f\n", w, my_bi_calls, ngram_prob);
  PyObject * ret_proba_py = PyObject_CallFunction(bigram_py, "yyiif", winfo->woutput[context], winfo->woutput[w], context, w, ngram_prob);
  if (!PyFloat_CheckExact(ret_proba_py))
  {
    printf("Did not get float from bigram_py, exiting\n");
    exit(1);
  }
  float ret_proba = PyFloat_AS_DOUBLE(ret_proba_py);
  // fprintf(stderr, "got %f\n", ret_proba);
  return ret_proba;
}

LOGPROB language_model(WORD_INFO *winfo, WORD_ID *contexts, int context_len, WORD_ID w, LOGPROB ngram_prob){
  
  PyObject *context_ids_py = PyList_New(context_len);
  for(int i=0; i < context_len; ++i)
  {
      PyList_SetItem(context_ids_py, i, Py_BuildValue("i", contexts[i]));
  }

  PyObject *context_words_py = PyList_New(context_len);
  for(int i=0; i < context_len; ++i)
  {
      PyList_SetItem(context_words_py, i, Py_BuildValue("y", winfo->woutput[contexts[i]]));
  }

  PyObject * ret_proba_py = PyObject_CallFunction(lm_py, "OOyif", context_ids_py, context_words_py, winfo->woutput[w], w, ngram_prob);
  if (!PyFloat_CheckExact(ret_proba_py))
  {
    printf("Did not get float from language_model_py, exiting\n");
    exit(1);
  }
  float ret_proba = PyFloat_AS_DOUBLE(ret_proba_py);

  return ret_proba;
}

boolean opt_user_lm_py_location(Jconf *jconf, char *arg[], int argnum)
{
  user_lm_py_location = (char *)malloc(strlen(arg[0]) + 1);
  strcpy(user_lm_py_location, arg[0]);
  return TRUE;
}

void free_user_lms()
{
  Py_Finalize();
}
