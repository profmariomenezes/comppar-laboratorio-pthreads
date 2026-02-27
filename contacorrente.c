#include<stdio.h>
#include<stdlib.h>


int main() {

  

  saldo = 1000.00;

  depositos(); /* realiza uma "infinidade" de depositos, e.g. 2147483000 depositos de 5.0 unidades monetárias */

  saques(); /* realiza uma "infinidade" de saques, e.g. 2147483000 saques de 2.0 unidades monetárias */

  printf("Saldo final: %.2lf\n", saldo);

  return 0;  
}
