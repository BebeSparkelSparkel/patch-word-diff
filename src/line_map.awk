#!/usr/bin/env awk
BEGIN {
	print "#!/usr/bin/env awk"
  print "function srcLocate(line) {"
  sum = 0
}

!/ total$/ {
  printf "  %sif (line > %d && line <= %d) { lineCorrection = %d; srcFile = \"%s\" }\n", elseStr, sum, sum + $1, sum, $2
  sum += $1
  elseStr = "else "
}

END {
  print "  else { lineCorrection = 0; srcFile = \"unknown\" }"
  print "  return sprintf(\"#line %d \\\"%s\\\"\", line - lineCorrection, srcFile)"
  print "}"

  print "/^#line [0-9]* \"parser.y\"/ { print srcLocate($2); next }"
  print "{print}"
}
