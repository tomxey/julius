#ifndef __USER_LM_H__
#define __USER_LM_H__

#include <julius/jfunc.h>

void init_user_lms();
LOGPROB my_uni(WORD_INFO *winfo, WORD_ID w, LOGPROB ngram_prob);
LOGPROB my_bi(WORD_INFO *winfo, WORD_ID context, WORD_ID w, LOGPROB ngram_prob);
LOGPROB my_lm(WORD_INFO *winfo, WORD_ID *contexts, int context_len, WORD_ID w, LOGPROB ngram_prob);
boolean opt_user_lm_py_location(Jconf *jconf, char *arg[], int argnum);

#endif // __USER_LM_H__
