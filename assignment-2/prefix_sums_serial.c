#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_n(int* n);
void read_values(double user_input_values[], int n);
void calc_partial_sums(double user_input_values[], int n);
void print_array(double partial_sums[], int n);

int main(void) {
  int n;
  double *user_input_values;
  
  read_n(&n); // prompt n

  user_input_values = malloc(n * sizeof(double) );
  
  read_values(user_input_values, n); // prompt user input values
  calc_partial_sums(user_input_values, n);
  print_array(user_input_values, n);

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

void calc_partial_sums(double user_input_values[], int n) {
  int i;

  for(i = 1; i < n; i++) {
    user_input_values[i] += user_input_values[i - 1];
  }
}

void print_array(double partial_sums[], int n) {
  int i;

  printf("\n[ ");
  for(i = 0; i < n; i++) {
    printf("%.2lf ", partial_sums[i] );
  }
  printf(" ]\n");
}
