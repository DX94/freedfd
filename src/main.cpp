#include <dfd.h>
#include <tokeniza.h>
#include <errores.h>
#include <entorno-ejecucion.h>
#include <evaluacion.h>

#include <stdio.h>
#include <stdlib.h>

/* Define HAVE_READLINE if you have GNU readline
 *   (libreadline5-dev in Debian or Ubuntu).
 * http://en.wikipedia.org/wiki/Readline
 * #define HAVE_READLINE
 */

#define BUF_SIZE 256

#ifdef HAVE_READLINE
  #include <readline/readline.h>
#else
  #include <iostream>
  using namespace std;
  char buffer[BUF_SIZE];
#endif

/* Counters that help us check we don't have (obvious) memory leaks */
int ContadorCajita = 0;
int ContadorCampoVariable = 0;
int ContadorVariable = 0;
int ContadorToken = 0;
int ContadorTabla = 0;
EstadoPrograma Estado;
class PilaDeTablas PilaDeTablas; /* entorno de ejecucion, stack, tabla de simbolos */

const char *program_name = "FreeDFD";

BuzonDeErrores Buzon;


void
print_counters_if_needed()
{
  if (ContadorCajita)
    fprintf(stderr, "Ops. ContadorCajita == %d\n", ContadorCajita);
  if (ContadorCampoVariable)
    fprintf(stderr, "Ops. ContadorCampoVariable == %d\n", ContadorCampoVariable);
  if (ContadorVariable)
    fprintf(stderr, "Ops. ContadorVariable == %d\n", ContadorVariable);
  if (ContadorToken)
    fprintf(stderr, "Ops. ContadorToken == %d\n", ContadorToken);
  if (ContadorTabla != 1)
    fprintf(stderr, "Ops. ContadorTabla == %d\n", ContadorTabla);
}

void
postfix_print(const char *line)
{
    Token * Post = GetPostfijo (line);

    if (Buzon.GetHuboError())
    {
      fprintf(stderr, "Error %s\n", Buzon.GetErrorInfo());
      Buzon.Vacear();
    }
    else
    {
      char tmp_buf[BUF_SIZE];

      if (Post)
        printf("Postfijo => ");

      for (Token *tok = Post; tok; tok = tok->GetSig())
        printf("%s ", tok->AsString(tmp_buf, BUF_SIZE));

      fflush(stdout);

      Token *Res = EvaluaPostfijo(Post);

      if (Res)
      {
        printf("%s ", Res->AsString(tmp_buf, BUF_SIZE));
        delete Res;
      }
      else
        fprintf(stderr, __FILE__":%d EvaluaPostfijo returned NULL\n", __LINE__);

      LiberarListaToken(Post);

      putchar('\n');
    }

    print_counters_if_needed();
}

int
main(int argc, char *argv[])
{

  PilaDeTablas.Apilar(new Tabla); /* new symbol table */

#ifdef HAVE_READLINE
  while(1)
  {
    char *line;
    line = readline("DFD> ");
    if (!line)
      break;
    postfix_print(line);
    free(line);
  }
#else
  printf("DFD> ");
  while(std::cin.getline(buffer, BUF_SIZE))
  {
    postfix_print(buffer);
    printf("DFD> ");
  }
#endif

  return EXIT_SUCCESS;
}