#include <stdio.h>
#include <string.h>
#include "string_builder.h"

int main() {
  // Create a string builder
  StringBuilder builder;
  initStringBuilder(&builder);
  
  // Append various types of strings
  appendDataString(&builder, "Hello, ");
  appendHeapString(&builder, 5, strdup("world"));
  appendDataString(&builder, "! ");
  
  // Create a string list builder for interspersed strings
  StringListBuilder listBuilder;
  initStringListBuilder(&listBuilder);
  
  // Create strings to be joined with a separator
  StringBuilder item1;
  initStringBuilder(&item1);
  appendDataString(&item1, "This");
  appendStringListBuilder(&listBuilder, finalizeBuilder(&item1));
  
  StringBuilder item2;
  initStringBuilder(&item2);
  appendDataString(&item2, "is");
  appendStringListBuilder(&listBuilder, finalizeBuilder(&item2));
  
  StringBuilder item3;
  initStringBuilder(&item3);
  appendDataString(&item3, "an");
  appendStringListBuilder(&listBuilder, finalizeBuilder(&item3));
  
  StringBuilder item4;
  initStringBuilder(&item4);
  appendDataString(&item4, "example");
  appendStringListBuilder(&listBuilder, finalizeBuilder(&item4));
  
  // Create an interspersed string with space as separator
  StringWriter *joined = intersperseDataString(&listBuilder, " ");
  
  // Append the interspersed string to our main builder
  appendStringWriter(&builder, joined);
  
  // Finalize the builder to get a string writer
  StringWriter *w = finalizeBuilder(&builder);
  
  // Print the final string
  fprintStringWriter(stdout, w);
  printf("\n");
  
  return 0;
}
