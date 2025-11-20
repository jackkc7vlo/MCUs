@echo off
"C:\Program Files (x86)\PC-lint 9.0\Lint-nt"  +v  -i"C:\Program Files (x86)\PC-lint 9.0"  D:\projects\Micros\mcus\nhs3100\std.lnt  -os(_LINT.TMP) %1 %2 %3 %4 %5 %6 %7 %8 %9
type _LINT.TMP

