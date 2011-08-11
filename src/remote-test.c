#include "blaupunkt-2011.h"
#include <stdio.h>

int main(int argc, char **argv) {
  
  RemoteState rs;

  remoteStart(&rs, &BLAUPUNKT_2011, BLAUPUNKT_2011_POWER);

  int rc = 100;
  while (rc-- > 0 || !remoteDone(&rs)) {
    int time = remoteSignal(&rs);
    if (time < 0) {
      fprintf(stdout, "Space: %4d  state:%d  codestate:%d\n", -time, rs.state, rs.codeState);
    } else {
	fprintf(stdout, "Mark:  %4d  state:%d  codestate:%d\n", time, rs.state, rs.codeState);
    }  
  }
}
