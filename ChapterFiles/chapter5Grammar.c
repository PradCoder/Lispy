/*#include <stdio.h>*/

/*A grammar is a set of rules imposed on a languages to make larger constructs*/
/*Parsers are programs that process particular
parts of languages using rules for the processing
(a grammar)*/

/*Builder a parser 'Adjective' to recognize descriptions*/
/*mpc_parser_t* Adjective = mpc_or(4,
  mpc_sym("wow"), mpc_sym("many"),
  mpc_symc("so"), mpc_sym("such")
);*/

/*Build a parser 'Noun' to recognize things*/
/*mpc_parser_t* Noun = mpc_or(5,
  mpc_sym("lisp"), mpc_sym("language"),
  mpc_sym("book"), mpc_sym("build"),
  mpc_sym("c")
);*/

/*define a phrase*/
/*mpc_parser_t* Phrase = mpc_and(2, mpcf_strfold,
Adjective, Noun, free);*/

/*defines a doge*/
/*mpc_parser_t* Doge = mpc_many(mpcf_strfold, Phrase);*/

/*Reimplemtend*/
/*#include "mpc.h"

int main(int argc,char** argv){*/

  /* Build a parser 'Adjective' to recongnize descriptions */
  /*mpc_parser_t* Adjective = mpc_or(4,
    mpc_sym("wow"), mpc_sym("many"),
    mpc_sym("so"), mpc_sym("such")
  );*/

  /* Build a parser 'Noun' to recognize things */
  /*mpc_parser_t* Noun = mpc_or(5,
    mpc_sym("lisp"), mpc_sym("language"),
    mpc_sym("book"), mpc_sym("build"),
    mpc_sym("c")
  );

  mpc_parser_t* Phrase = mpc(2, mpcf_strfold,
    Adjective, Noun, free);

  mpc_parser_t* Doge = mpc_many(mpcf_strfold, Phrase);*/

  /* Do some parsing here... */

  /*mpc_delete(Doge);

  return 0;
}*/

/*Reimplemtation using using a much better representation*/
#include "mpc.h"

int main(int argc,char** argv){

  mpc_parser_t* Adjective = mpc_new("adjective");
  mpc_parser_t* Noun = mpc_new("noun");
  mpc_parser_t* Phrase = mpc_new("phrase");
  mpc_parser_t* Doge = mpc_new("doge");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                           \
    adjective : \"wow\" | \"many\"              \
              | \"so\" | \"such\";              \
    noun      : \"lisp\" | \"language\"         \
              | \"book\" | \"build\" | \"c\";   \
    phrase    : <adjective> <noun>;             \
    doge      : <phrase>*;                      \
    ",
    Adjective, Noun, Phrase, Doge);

  /*Do some parsing here...*/
  mpc_cleanup(4, Adjective, Noun, Phrase, Doge);

  return 0;

  /*
  Examples of strings that the Doge language contains
  1."wow language so build"
  2."so language many lisp"
  3."such language wow book"
  4."so c many lisp"
  5."such lisp so language"
  */

  /*
  the back slashes (\) is the Escape character and \"
  is used to print the " as the " can then be processed as
  a character and not the start or end of a String
  */

  /*
  It is to show the compiler that the string will take multiple lines
  */

  /*
  sign: \"+\"|\"-\";
  wholeNumber:\"0\"|\"1\"|\"2\"|\"3\"|\"4\"\
              |\"5\"|\"6\"|\"7\"|\"8\"|\"9\"|\"\";
  fraction: \".\"+<wholeNumber>+
  number:<sign>*<wholeNumber>+<fraction>*;
  */

  /*
  "
  transferProtocol: \"http://\"; \
  subdomain: \"www.\";\
  domain:\"buildyourownlisp.\";\
  topleveldomain:\"com\"|\"net\"|\"org";\
  secondleveldomain:\"co\";\
  url:<transferProtocol><subdomain><domain><secondleveldomain>*\
      <topleveldomain>;\
  "
  */

  /*
  "The cat sat on the mat"
  verb: "sat"
  subject:"cat"
  predicate: <subject><verb>
  clause:<predicate>+
  phrase:<verb>+|<subject>+ <verb>+|<subject>+
  modifier:<adjective>*<adverb>*
  */

  /*
  Describe more foramlly the above grammars
  (already did but I should retry them in textual format: No need)
  */

  /*
  Textually describe a grammar for JSON
  JSON allows for objects to be defined as name/value pairs
  Data is separated in commas, curly braces hold objects,
  square brackets hold arrays. Data consists of name and values,
  objects consist of datum, and arrays consist of objects.
  data: <name><value>
  object: <data>+
  arrays: <object>+
  */
}
