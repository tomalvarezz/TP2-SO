#ifndef _WRAPPERS_H_
#define _WRAPPERS_H_

void dummyWrapper(int argc, char** argv);
void printHelpWrapper(int argc, char** argv);
int exitUserlandWrapper(int argc, char** argv);
void clearWrapper(int argc, char** argv);
void printTimeWrapper(int argc, char** argv);
void printRegistersWrapper(int argc, char** argv);
void printMemoryWrapper(int argc, char** argv);
void zeroExceptionCommandWrapper(int argc, char** argv);
void invalidOpCodeWrapper(int argc, char** argv);
void processesStatusWrapper(int argc, char** argv);
void memoryDumpWrapper(int argc, char** argv);
void loopWrapper(int argc, char** argv);
void killProcessWrapper(int arg, char** argv);
void niceWrapper(int argc, char** argv);
void blockWrapper(int argc, char** argv);
void semStatusWrapper(int argc, char** argv);
void catWrapper(int argc, char** argv);
void wcWrapper(int argc, char** argv);
void filterWrapper(int argc, char** argv);
void pipeStatusWrapper(int argc, char** argv);
void phyloWrapper(int argc, char** argv);
void testMmWrapper(int argc, char** argv);
void testProcessesWrapper(int argc, char** argv);
void testPriorityWrapper(int argc, char** argv);
void testSynchroWrapper(int argc, char** argv);
void testNoSynchroWrapper(int argc, char** argv);

#endif