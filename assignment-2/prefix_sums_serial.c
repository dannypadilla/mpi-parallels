#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void read_n(int* n);
void read_values(double user_input_values[], int n );

int main(void) {
  int n;
  double *user_input_values;
  
  read_n(&n); // prompt n

  user_input_values = malloc(n * sizeof(double) );
  
  read_values(user_input_values, n); // prompt user input values
  

  free(user_input_values);
  
  return 0;
}

void read_n(int* n) {
  printf("Enter n:\n");
  scanf("%d", n);
}

void read_values(double user_input_values[], int n) {
  int i;
    
  printf("Enter the values:\n");
  for(i = 0; i < n; i++) {
    scanf("%lf", &user_input_values[i] );
  }

}
